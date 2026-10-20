/**************************************************************************
*	VectorModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "VectorModel.h"
#include "VectorView.h"
#include "MainWindow.h"
#include "EntityTreeItemData.h"
#include "EntityModel.h"
#include "EntityDraw.h"

const float fSNAP_AMT = 25.0f;

VectorModel::VectorModel(EditModeType eEditModeType) :
	IEditModeModel(eEditModeType),
	m_pPrimTreeItemData(nullptr),
	m_iPrimLayerIndex(-1),
	m_Color(HyColor::White),
	m_vOffset(0.0f, 0.0f),
	m_bVisible(true),
	m_fAlpha(1.0f),
	m_iDisplayOrder(0),
	m_eShapeType(SHAPE_None),
	m_bLoopClosed(false),
	m_fOutline(0.0f),
	m_GrabPointCenter(GRABPOINT_Center),
	m_bSelfIntersecting(false),
	m_ptSelfIntersection(0.0f, 0.0f),
	m_sMalformedReason("No Data"),
	m_eCurAction(VECTORACTION_None),
	m_iGrabPointIndex(-1),
	m_bOverrideTransformStart(false),
	m_ptTransformStartOverride(0.0f, 0.0f)
{
	if(IsFixture())
		m_WidgetColor = HyGlobal::GetEditorColor(EDITORCOLOR_Fixtures);
	else
		m_WidgetColor = HyGlobal::GetEditorColor(EDITORCOLOR_ShapeGrabPointFill);

	ClearAction();
}

/*virtual*/ VectorModel::~VectorModel()
{
	ClearFixtures();
}

void VectorModel::InitPrimTreeItemData(EntityTreeItemData *pPrimTreeItemData, int iLayerIndex)
{
	m_pPrimTreeItemData = pPrimTreeItemData;
	m_iPrimLayerIndex = iLayerIndex;
}

HyPrimitive2d *VectorModel::GetHyNode() const
{
	if(m_pPrimTreeItemData == nullptr)
		return nullptr;

	EntityDrawItem *pPrimNodeDrawItem = static_cast<EntityDraw *>(m_pPrimTreeItemData->GetEntityModel().GetItem().GetDraw())->FindDrawItem(m_pPrimTreeItemData);
	if(pPrimNodeDrawItem == nullptr)
		return nullptr;
	
	return static_cast<HyPrimitive2d *>(pPrimNodeDrawItem->GetHyNode());
}

void VectorModel::FlushPrimLayer()
{
	m_eShapeType = SHAPE_None;
	ClearFixtures();

	HyPrimitive2d *pHyPrimitive = GetHyNode();
	if(pHyPrimitive)
		pHyPrimitive->SetAsNothing(m_iPrimLayerIndex);

	if(IsPrimitive())
	{
		m_Color = HyColor::White;
		m_vOffset = { 0.0f, 0.0f };
		m_bVisible = true;
		m_fAlpha = 1.0f;
	}
	else
	{
		m_Color = HyGlobal::GetEditorColor(EDITORCOLOR_Fixtures);
		m_vOffset = { 0.0f, 0.0f };
		m_bVisible = true;
		m_fAlpha = 0.42f;
	}
}

/*virtual*/ QJsonObject VectorModel::Serialize() const /*override*/
{
	QJsonObject serializedObj;
	if(m_eEditModeType == EDITMODETYPE_PrimitiveLineChain || m_eEditModeType == EDITMODETYPE_FixtureChain)
		serializedObj.insert("type", HYLINECHAIN_Name);
	else if(m_eEditModeType == EDITMODETYPE_FixturePoint)
		serializedObj.insert("type", HYPOINT_Name);
	else
		serializedObj.insert("type", HyGlobal::ShapeName(m_eShapeType));

	std::vector<float> dataList = SerializeData();
	QJsonArray dataArray;
	for(float f : dataList)
		dataArray.push_back(f);
	serializedObj.insert("data", dataArray);

	serializedObj.insert("outline", m_fOutline);

	return serializedObj;
}

/*virtual*/ void VectorModel::Deserialize(bool bEnabled, const QJsonObject &serializedObj) /*override*/
{
	if(IsFixture())
		m_iDisplayOrder = bEnabled ? DISPLAYORDER_FixtureSelected : DISPLAYORDER_Fixture;

	m_sMalformedReason = DeserializeData(serializedObj);
	if(m_sMalformedReason.isEmpty() == false || m_FixtureList.empty()) // *this is a prim layer, update the parent primitive node
	{
		HyPrimitive2d *pHyPrimNode = GetHyNode();
		if(pHyPrimNode)
			pHyPrimNode->SetAsNothing(m_iPrimLayerIndex);
	}
}

/*virtual*/ Qt::CursorShape VectorModel::MouseMoveIdle() /*override*/
{
	VectorAction eResult = DoMouseMoveIdle();

	for(IEditModeView *pView : m_ViewList)
		static_cast<VectorView *>(pView)->DrawVectorAction(eResult);

	switch(eResult)
	{
	case VECTORACTION_Creation:
		MainWindow::SetStatus("Edit Mode - Click + Drag to Create", 0);
		return Qt::CrossCursor;

	case VECTORACTION_Outside:
		MainWindow::SetStatus("Edit Mode", 0);
		return Qt::ArrowCursor;

	case VECTORACTION_Inside:
		if(IsAllGrabPointsSelected())
		{
			MainWindow::SetStatus("Edit Mode - Translate ", 0);
			return Qt::SizeAllCursor;
		}
		else
		{
			MainWindow::SetStatus("Edit Mode - Mouse Over", 0);
			return Qt::ArrowCursor;
		}

	case VECTORACTION_AppendVertex:
		MainWindow::SetStatus("Edit Mode - Append Vertex", 0);
		return Qt::CrossCursor;

	case VECTORACTION_InsertVertex:
		MainWindow::SetStatus("Edit Mode - Insert Vertex", 0);
		return Qt::CrossCursor;

	case VECTORACTION_HoverGrabPoint:
		if(IsHoverGrabPointSelected())
		{
			MainWindow::SetStatus("Edit Mode - Translate Vertex", 0);
			return Qt::SizeAllCursor;
		}
		else if(m_iGrabPointIndex >= 0)
		{
			MainWindow::SetStatus("Edit Mode - Select Vertex", 0);
			return m_GrabPointList[m_iGrabPointIndex].GetHoverCursor(CalcRotation());
		}
		break;

	case VECTORACTION_HoverCenter:
		MainWindow::SetStatus("Edit Mode - Translate", 0);
		return Qt::SizeAllCursor;

	case VECTORACTION_CloseLoop:
		MainWindow::SetStatus("Edit Mode - Close Loop", 0);
		return Qt::CrossCursor;

	default:
		HyGuiLog("VectorModel::MouseMoveIdle - unsupported edit mode action!", LOGTYPE_Error);
		break;
	}

	MainWindow::SetStatus("Edit Mode", 0);
	return Qt::ArrowCursor;
}

/*virtual*/ void VectorModel::MouseIdleRightClick() /*override*/
{
	DeselectAllGrabPoints();
}

/*virtual*/ bool VectorModel::MousePressEvent(bool bCtrlHeld, bool bShiftHeld, EditModeState eEditModeState, glm::vec2 ptClickPos) /*override*/
{
	bool bStartTransform = false;

	VectorAction eResult = DoMouseMoveIdle();
	if(eResult == VECTORACTION_AppendVertex || eResult == VECTORACTION_InsertVertex)
	{
		if(m_iGrabPointIndex == -1)
		{
			HyGuiLog("GfxShapeModel::MousePressEvent - Insert vertex index was -1 on AddVertex/Crosshair", LOGTYPE_Error);
			return VECTORACTION_None;
		}
		m_GrabPointList.insert(m_GrabPointList.begin() + m_iGrabPointIndex, GfxGrabPointModel(GRABPOINT_VertexSelected, ptClickPos));
		for(int i = 0; i < m_GrabPointList.size(); ++i)
			m_GrabPointList[i].SetSelected(false);
		m_GrabPointList[m_iGrabPointIndex].SetSelected(true);

		bStartTransform = true;
	}
	else if(eResult == VECTORACTION_HoverGrabPoint)
	{
		if(m_iGrabPointIndex == -1)
		{
			HyGuiLog("GfxShapeModel::OnMousePressEvent - Hover vertex index was -1 on HoverVertex/HoverSelectedVertex", LOGTYPE_Error);
			return VECTORACTION_None;
		}

		if(m_GrabPointList[m_iGrabPointIndex].IsTypeSelectable())
		{
			if(bShiftHeld)
			{
				m_GrabPointList[m_iGrabPointIndex].SetSelected(!m_GrabPointList[m_iGrabPointIndex].IsSelected());
			}
			else
			{
				if(m_GrabPointList[m_iGrabPointIndex].IsSelected() == false)
				{
					for(int i = 0; i < m_GrabPointList.size(); ++i)
						m_GrabPointList[i].SetSelected(false);
					m_GrabPointList[m_iGrabPointIndex].SetSelected(true);
				}
			}
		}

		for(GfxGrabPointModel &grabPtRef : m_GrabPointList)
			grabPtRef.SetCachePos();
		m_GrabPointCenter.SetCachePos();
		bStartTransform = true;
	}
	else if(eResult == VECTORACTION_HoverCenter)
	{
		for(GfxGrabPointModel &grabPtRef : m_GrabPointList)
			grabPtRef.SetCachePos();
		m_GrabPointCenter.SetCachePos();
		bStartTransform = true;
	}
	else if(eResult == VECTORACTION_Creation)
		bStartTransform = true;
	else if(eResult == VECTORACTION_Inside)
		bStartTransform = IsAllGrabPointsSelected();
	else if(eResult == VECTORACTION_CloseLoop)
	{
		m_bLoopClosed = true;
		bStartTransform = true;
	}

	m_eCurAction = eResult;

	return bStartTransform;
}

/*virtual*/ void VectorModel::MouseTransform(bool bCtrlHeld, bool bShiftHeld, glm::vec2 ptStartPos, glm::vec2 ptDragPos) /*override*/
{
	if(m_bOverrideTransformStart)
		ptStartPos = m_ptTransformStartOverride;

	glm::vec2 vDragDelta = { 0.0f, 0.0f };
	if(false == (HyCompareFloat(ptStartPos.x, ptDragPos.y) && HyCompareFloat(ptStartPos.y, ptDragPos.y)))
		vDragDelta = { ptDragPos.x - ptStartPos.x, ptDragPos.y - ptStartPos.y };
	
	for(IEditModeView *pView : m_ViewList)
		static_cast<VectorView *>(pView)->DrawVectorAction(VECTORACTION_None);

	switch(m_eCurAction)
	{
	case VECTORACTION_Creation:
		DoTransformCreation(bCtrlHeld, bShiftHeld, fSNAP_AMT, ptStartPos, ptDragPos);
		break;


	case VECTORACTION_CloseLoop:
		// Convert CloseLoop to AppendVertex
		if(m_iGrabPointIndex == -1)
		{
			HyGuiLog("GfxShapeModel::MouseTransform - CloseLoop -> AppendVertex m_iGrabPointIndex was -1", LOGTYPE_Error);
			return;
		}

		if(bCtrlHeld)
			ptDragPos = { roundf(ptDragPos.x / fSNAP_AMT) * fSNAP_AMT, roundf(ptDragPos.y / fSNAP_AMT) * fSNAP_AMT };
		m_GrabPointList.insert(m_GrabPointList.begin() + m_iGrabPointIndex, GfxGrabPointModel(GRABPOINT_VertexSelected, ptDragPos));
		for(int i = 0; i < m_GrabPointList.size(); ++i)
			m_GrabPointList[i].SetSelected(false);
		m_GrabPointList[m_iGrabPointIndex].SetSelected(true);

		m_eCurAction = VECTORACTION_AppendVertex;
		[[fallthrough]];
	case VECTORACTION_AppendVertex:
		if(bCtrlHeld)
			ptDragPos = { roundf(ptDragPos.x / fSNAP_AMT) * fSNAP_AMT, roundf(ptDragPos.y / fSNAP_AMT) * fSNAP_AMT };
		m_GrabPointList[m_iGrabPointIndex].SetPos(ptDragPos);
		break;

	case VECTORACTION_HoverGrabPoint:
		if(m_GrabPointList[m_iGrabPointIndex].IsTypeSelectable())
		{
			glm::vec2 ptNewPos = m_GrabPointList[m_iGrabPointIndex].GetCachePos() + vDragDelta;
			if(bCtrlHeld) // Snap to fSNAP_AMT pixel increments if ctrl is held
				ptNewPos = { roundf(ptNewPos.x / fSNAP_AMT) * fSNAP_AMT, roundf(ptNewPos.y / fSNAP_AMT) * fSNAP_AMT };
			m_GrabPointList[m_iGrabPointIndex].SetPos(ptNewPos);

			glm::vec2 vOffsetAmt = ptNewPos - m_GrabPointList[m_iGrabPointIndex].GetCachePos();

			std::vector<glm::vec2> grabPointPosList;
			for(int i = 0; i < m_GrabPointList.size(); ++i)
			{
				GfxGrabPointModel &grabPtModel = m_GrabPointList[i];
				if(grabPtModel.IsSelected() && i != m_iGrabPointIndex)
					grabPtModel.SetPos(grabPtModel.GetCachePos() + vOffsetAmt);

				grabPointPosList.push_back(grabPtModel.GetPos());
			}

			m_GrabPointCenter.SetPos(HyMath::CalculateCentroid(grabPointPosList));
		}
		else // Not assembling vertices, this is a shape grab point
		{
			if(m_FixtureList.size() != 1)
				HyGuiLog("VectorModel::MouseTransform() - m_FixtureList isn't size 1 with a shape grab point", LOGTYPE_Error);

			switch(m_eShapeType)
			{
			case SHAPE_Box:
				DoTransformShapeCtrlGrabPoint_Box(bCtrlHeld, bShiftHeld, fSNAP_AMT, vDragDelta);
				break;

			case SHAPE_Circle: {
				glm::vec2 ptNewPos = m_GrabPointList[m_iGrabPointIndex].GetCachePos() + vDragDelta;
				if(bCtrlHeld) // Snap to fSNAP_AMT pixel increments if ctrl is held
					ptNewPos = { roundf(ptNewPos.x / fSNAP_AMT) * fSNAP_AMT, roundf(ptNewPos.y / fSNAP_AMT) * fSNAP_AMT };
				float fNewRadius = glm::length(ptNewPos - m_GrabPointCenter.GetCachePos());
				static_cast<HyShape2d *>(m_FixtureList[0])->SetAsCircle(m_GrabPointCenter.GetCachePos(), fNewRadius);
				std::vector<glm::vec2> grabList = static_cast<HyShape2d *>(m_FixtureList[0])->CalcGrabPoints();
				if(m_GrabPointList.size() != 4 || grabList.size() != 4)
					HyGuiLog("VectorModel::MouseTransform() - Circle shape ctrl was expecting 4 grab points", LOGTYPE_Error);
				for(int i = 0; i < 4; i++)
					m_GrabPointList[i].SetPos(grabList[i]);

				break; }

			case SHAPE_LineSegment: {
				glm::vec2 ptNewPos = m_GrabPointList[m_iGrabPointIndex].GetCachePos() + vDragDelta;
				if(bCtrlHeld) // Snap to fSNAP_AMT pixel increments if ctrl is held
					ptNewPos = { roundf(ptNewPos.x / fSNAP_AMT) * fSNAP_AMT, roundf(ptNewPos.y / fSNAP_AMT) * fSNAP_AMT };

				m_GrabPointList[m_iGrabPointIndex].SetPos(ptNewPos);

				static_cast<HyShape2d *>(m_FixtureList[0])->SetAsLineSegment(m_GrabPointList[0].GetPos(), m_GrabPointList[1].GetPos());

				glm::vec2 ptCenter;
				m_FixtureList[0]->GetCentroid(ptCenter);
				m_GrabPointCenter.SetPos(ptCenter);
				break; }

			case SHAPE_Capsule: {
				glm::vec2 ptNewPos = m_GrabPointList[m_iGrabPointIndex].GetCachePos() + vDragDelta;
				if(bCtrlHeld) // Snap to fSNAP_AMT pixel increments if ctrl is held
					ptNewPos = { roundf(ptNewPos.x / fSNAP_AMT) * fSNAP_AMT, roundf(ptNewPos.y / fSNAP_AMT) * fSNAP_AMT };
				
				float fRadius;
				if(m_iGrabPointIndex == 0 || m_iGrabPointIndex == 1) // Position grab point
				{
					fRadius = glm::distance(m_GrabPointList[0].GetCachePos(), m_GrabPointList[2].GetCachePos());
					m_GrabPointList[m_iGrabPointIndex].SetPos(ptNewPos);
				}
				else // A radius grab point
				{
					if(m_iGrabPointIndex == 2 || m_iGrabPointIndex == 3)
						fRadius = glm::length(ptNewPos - m_GrabPointList[0].GetCachePos());
					else
						fRadius = glm::length(ptNewPos - m_GrabPointList[1].GetCachePos());
				}
				static_cast<HyShape2d *>(m_FixtureList[0])->SetAsCapsule(m_GrabPointList[0].GetPos(), m_GrabPointList[1].GetPos(), fRadius);
				
				std::vector<glm::vec2> grabPtList = static_cast<HyShape2d *>(m_FixtureList[0])->CalcGrabPoints();
				if(m_GrabPointList.size() != 6 || grabPtList.size() != 6)
					HyGuiLog("VectorModel::MouseTransform() - Capsule shape ctrl was expecting 6 grab points", LOGTYPE_Error);
				for(int i = 0; i < 6; i++)
					m_GrabPointList[i].SetPos(grabPtList[i]);
				glm::vec2 ptCenter;
				m_FixtureList[0]->GetCentroid(ptCenter);
				m_GrabPointCenter.SetPos(ptCenter);
				break; }

			case SHAPE_Polygon:
			case SHAPE_None:
			default:
				HyGuiLog("VectorModel::MouseTransform() - Unhandled shape type with shape ctrl grab point", LOGTYPE_Error);
				break;
			}
		}
		break;

	case VECTORACTION_HoverCenter: {

		glm::vec2 vDragDelta = { 0.0f, 0.0f };
		if(bCtrlHeld) // Snap to fSNAP_AMT pixel increments if ctrl is held
			ptDragPos = { roundf(ptDragPos.x / fSNAP_AMT) * fSNAP_AMT, roundf(ptDragPos.y / fSNAP_AMT) * fSNAP_AMT };
		if(false == (HyCompareFloat(ptStartPos.x, ptDragPos.y) && HyCompareFloat(ptStartPos.y, ptDragPos.y)))
			vDragDelta = { ptDragPos.x - ptStartPos.x, ptDragPos.y - ptStartPos.y };

		for(GfxGrabPointModel &grabPtModel : m_GrabPointList)
			grabPtModel.SetPos(grabPtModel.GetCachePos() + vDragDelta);
		m_GrabPointCenter.SetPos(m_GrabPointCenter.GetCachePos() + vDragDelta);

		if(IsLineChain())
		{
			std::vector<glm::vec2> vertexList;
			for(GfxGrabPointModel &grabPtModel : m_GrabPointList)
				vertexList.push_back(grabPtModel.GetPos());
			
			HyChain2d *pChainFixture = static_cast<HyChain2d *>(m_FixtureList[0]);
			pChainFixture->SetData(vertexList, pChainFixture->GetChainData().bLoop);
		}
		else
		{
			switch(m_eShapeType)
			{
			case SHAPE_Box:
				static_cast<HyShape2d *>(m_FixtureList[0])->SetAsBox(HyRect(m_GrabPointList[0].GetPos(), m_GrabPointList[2].GetPos(), m_GrabPointList[4].GetPos(), m_GrabPointList[6].GetPos()));
				break;

			case SHAPE_Circle:
				static_cast<HyShape2d *>(m_FixtureList[0])->SetAsCircle(m_GrabPointCenter.GetPos(), 0.5f * glm::distance(m_GrabPointList[0].GetCachePos(), m_GrabPointList[2].GetCachePos()));
				break;

			case SHAPE_LineSegment:
				static_cast<HyShape2d *>(m_FixtureList[0])->SetAsLineSegment(m_GrabPointList[0].GetPos(), m_GrabPointList[1].GetPos());
				break;

			case SHAPE_Capsule:
				static_cast<HyShape2d *>(m_FixtureList[0])->SetAsCapsule(m_GrabPointList[0].GetPos(), m_GrabPointList[1].GetPos(), glm::length(m_GrabPointList[0].GetPos() - m_GrabPointList[2].GetPos()));
				break;

			default:
				HyGuiLog("VectorModel::MouseTransform() - HoverCenter dragged, Unhandled shape type", LOGTYPE_Error);
				break;
			}
		}
		break; }

	case VECTORACTION_InsertVertex:
		break;

	case VECTORACTION_Outside:
	case VECTORACTION_Inside:
	default:
		HyGuiLog("VectorModel::MouseTransform() - Unhandled VectorAction: " % QString::number(m_eCurAction), LOGTYPE_Error);
		break;
	}

	SyncViews(EDITMODE_MouseDragTransform);
}

/*virtual*/ void VectorModel::MouseClickTransformReleased(bool bCtrlHeld, bool bShiftHeld, glm::vec2 ptReleasePos) /*override*/
{
	if(m_eCurAction == VECTORACTION_Creation)
	{
		glm::vec2 ptStartPos = ptReleasePos + glm::vec2(-25.0f, -25.0f);
		glm::vec2 ptDragPos = ptReleasePos + glm::vec2(25.0f, 25.0f);
		DoTransformCreation(bCtrlHeld, bShiftHeld, fSNAP_AMT, ptStartPos, ptDragPos);
	}
	else if(m_eCurAction == VECTORACTION_HoverGrabPoint)
	{
		if(bShiftHeld == false)
		{
			for(int i = 0; i < m_GrabPointList.size(); ++i)
				m_GrabPointList[i].SetSelected(false);
		}
		m_GrabPointList[m_iGrabPointIndex].SetSelected(true);
	}
}

/*virtual*/ void VectorModel::MouseMarqueeReleased(EditModeState eEditModeState, bool bLeftClick, QPointF ptBotLeft, QPointF ptTopRight) /*override*/
{
	// Select grab points within marquee
	for(GfxGrabPointModel &grabPtModel : m_GrabPointList)
	{
		grabPtModel.SetSelected(false);

		QPointF ptGrabPos(grabPtModel.GetPos().x, grabPtModel.GetPos().y);
		if(ptGrabPos.x() >= ptBotLeft.x() && ptGrabPos.x() <= ptTopRight.x() &&
		   ptGrabPos.y() >= ptBotLeft.y() && ptGrabPos.y() <= ptTopRight.y())
		{
			if(bLeftClick)
				grabPtModel.SetSelected(true);
		}
	}

	SyncViews(eEditModeState);
}

bool VectorModel::IsPrimitive() const
{
	return m_eEditModeType == EDITMODETYPE_PrimitiveShape || m_eEditModeType == EDITMODETYPE_PrimitiveLineChain;
}

bool VectorModel::IsFixture() const
{
	return m_eEditModeType == EDITMODETYPE_FixtureShape || m_eEditModeType == EDITMODETYPE_FixtureChain || m_eEditModeType == EDITMODETYPE_FixturePoint;
}

bool VectorModel::IsLineChain() const
{
	return m_eEditModeType == EDITMODETYPE_PrimitiveLineChain || m_eEditModeType == EDITMODETYPE_FixtureChain;
}

void VectorModel::SetEditModeType(EditModeType eEditModeType)
{
	m_eEditModeType = eEditModeType;
}

EditorShape VectorModel::GetShapeType() const
{
	return m_eShapeType;
}

HyColor VectorModel::GetWidgetColor() const
{
	return m_WidgetColor;
}

void VectorModel::SetWidgetColor(HyColor widgetColor)
{
	m_WidgetColor = widgetColor;
}

HyColor VectorModel::GetColor() const
{
	return m_Color;
}

void VectorModel::SetColor(HyColor color)
{
	m_Color = color;
	for(IEditModeView *pView : m_ViewList)
		static_cast<VectorView *>(pView)->SyncColor();
}

glm::vec2 VectorModel::GetOffset() const
{
	return m_vOffset;
}

void VectorModel::SetOffset(glm::vec2 vOffset)
{
	m_vOffset = vOffset;
}

bool VectorModel::IsVisible() const
{
	return m_bVisible;
}

void VectorModel::SetVisible(bool bVisible)
{
	m_bVisible = bVisible;
}

float VectorModel::GetAlpha() const
{
	return m_fAlpha;
}

void VectorModel::SetAlpha(float fAlpha)
{
	m_fAlpha = fAlpha;
}

int VectorModel::GetDisplayOrder() const
{
	return m_iDisplayOrder;
}

void VectorModel::SetDisplayOrder(int iDisplayOrder)
{
	m_iDisplayOrder = iDisplayOrder;
}

float VectorModel::CalcRotation() const
{
	if(m_eShapeType == SHAPE_Box)
	{
		if(m_FixtureList.isEmpty())
			return 0.0f;

		std::vector<glm::vec2> ptCorners = m_FixtureList[0]->CalcGrabPoints();
		glm::vec2 dir = glm::normalize(ptCorners[1] - ptCorners[0]);
		return glm::degrees(glm::atan(dir.y, dir.x));
	}

	return 0.0f;
}

void VectorModel::ChangeToLineChain(bool bIsActiveEditModeItem, bool bAsFixture)
{
	std::vector<float> floatList = ConvertedPolygonOrLineChainData();

	if(bAsFixture)
		m_eEditModeType = EDITMODETYPE_FixtureChain;
	else
		m_eEditModeType = EDITMODETYPE_PrimitiveLineChain;

	m_eShapeType = SHAPE_None;

	// Reassemble model
	QJsonObject serializedObj;
	serializedObj.insert("type", HYLINECHAIN_Name);
	QJsonArray dataArray;
	for(float f : floatList)
		dataArray.push_back(f);
	serializedObj.insert("data", dataArray);
	serializedObj.insert("outline", m_fOutline <= 0.0f ? 1.0f : m_fOutline);
	Deserialize(bIsActiveEditModeItem, serializedObj);
}

void VectorModel::ChangeToPoint(bool bIsActiveEditModeItem)
{
	std::vector<float> floatList = ConvertedPointData();

	m_eEditModeType = EDITMODETYPE_FixturePoint;

	m_eShapeType = SHAPE_None;

	// Reassemble model
	QJsonObject serializedObj;
	serializedObj.insert("type", HYPOINT_Name);
	QJsonArray dataArray;
	for(float f : floatList)
		dataArray.push_back(f);
	serializedObj.insert("data", dataArray);
	serializedObj.insert("outline", m_fOutline);
	Deserialize(bIsActiveEditModeItem, serializedObj);
}

void VectorModel::ChangeToShape(bool bIsActiveEditModeItem, EditorShape eNewShapeType, bool bAsFixture)
{
	std::vector<float> floatList;
	switch(eNewShapeType)
	{
	case SHAPE_None:
		break;
	case SHAPE_Box:
		floatList = ConvertedBoxData();
		break;
	case SHAPE_Circle:
		floatList = ConvertedCircleData();
		break;
	case SHAPE_LineSegment:
		floatList = ConvertedLineSegmentData();
		break;
	case SHAPE_Capsule:
		floatList = ConvertedCapsuleData();
		break;
	case SHAPE_Polygon:
		floatList = ConvertedPolygonOrLineChainData();
		break;
	default:
		HyGuiLog("GfxShapeModel::SetType - Unknown shape type encountered", LOGTYPE_Error);
		break;
	}

	if(bAsFixture)
		m_eEditModeType = EDITMODETYPE_FixtureShape;
	else
		m_eEditModeType = EDITMODETYPE_PrimitiveShape;

	m_eShapeType = eNewShapeType;

	// Reassemble model
	QJsonObject serializedObj;
	serializedObj.insert("type", HyGlobal::ShapeName(m_eShapeType));
	QJsonArray dataArray;
	for(float f : floatList)
		dataArray.push_back(f);
	serializedObj.insert("data", dataArray);
	if(m_eShapeType == SHAPE_LineSegment)
		serializedObj.insert("outline", m_fOutline <= 0.0f ? 1.0f : m_fOutline);
	else
		serializedObj.insert("outline", m_fOutline);
	Deserialize(bIsActiveEditModeItem, serializedObj);
}

bool VectorModel::IsLoopClosed() const
{
	return m_bLoopClosed;
}

float VectorModel::GetOutline() const
{
	return m_fOutline;
}

void VectorModel::SetOutline(float fOutline)
{
	m_fOutline = fOutline;
}

bool VectorModel::IsValidModel() const
{
	return m_sMalformedReason.isEmpty();
}

int VectorModel::GetNumFixtures() const
{
	return m_FixtureList.size();
}

const IHyFixture2d *VectorModel::GetFixture(int iIndex) const
{
	if(iIndex < 0 || iIndex >= m_FixtureList.size())
	{
		HyGuiLog("VectorModel::GetFixture - Index out of range", LOGTYPE_Error);
		return nullptr;
	}

	return m_FixtureList[iIndex];
}

const QList<GfxGrabPointModel> &VectorModel::GetGrabPointList() const
{
	return m_GrabPointList;
}

const GfxGrabPointModel &VectorModel::GetGrabPoint(int iIndex) const
{
	if(iIndex < 0 || iIndex >= m_GrabPointList.size())
	{
		HyGuiLog("VectorModel::GetGrabPoint - Index out of range", LOGTYPE_Error);
		return m_GrabPointCenter;
	}
	return m_GrabPointList[iIndex];
}

const GfxGrabPointModel *VectorModel::GetActiveGrabPoint() const
{
	if(m_iGrabPointIndex < 0 || m_iGrabPointIndex >= m_GrabPointList.size())
		return nullptr;

	return &m_GrabPointList[m_iGrabPointIndex];
}

const GfxGrabPointModel &VectorModel::GetCenterGrabPoint() const
{
	return m_GrabPointCenter;
}

int VectorModel::GetActiveGrabPointIndex() const
{
	return m_iGrabPointIndex;
}

int VectorModel::GetNumGrabPointsSelected() const
{
	int iNumSelected = 0;
	for(const GfxGrabPointModel &grabPtModel : m_GrabPointList)
	{
		if(grabPtModel.IsSelected())
			++iNumSelected;
	}
	return iNumSelected;
}

bool VectorModel::IsAllGrabPointsSelected() const
{
	for(const GfxGrabPointModel &grabPtModel : m_GrabPointList)
	{
		if(grabPtModel.IsSelected() == false)
			return false;
	}
	return true;
}

bool VectorModel::IsHoverGrabPointSelected() const
{
	if(m_iGrabPointIndex < 0 || m_iGrabPointIndex >= m_GrabPointList.size())
		return false;

	return m_GrabPointList[m_iGrabPointIndex].IsSelected();
}

void VectorModel::DeselectAllGrabPoints()
{
	for(GfxGrabPointModel &grabPtModel : m_GrabPointList)
		grabPtModel.SetSelected(false);
}

VectorAction VectorModel::GetCurrentAction() const
{
	return m_eCurAction;
}

void VectorModel::OnDeleteKeyPressed()
{
	if((m_eEditModeType == EDITMODETYPE_PrimitiveLineChain || m_eEditModeType == EDITMODETYPE_FixtureChain) ||
		m_eShapeType == SHAPE_Polygon)
	{
		for(int i = m_GrabPointList.size() - 1; i >= 0; --i)
		{
			if(m_GrabPointList[i].IsSelected())
			{
				m_GrabPointList.erase(m_GrabPointList.begin() + i);
				if(i < m_iGrabPointIndex)
					--m_iGrabPointIndex;
			}
		}

		SyncViews(EDITMODE_Idle);
	}
}

QString VectorModel::GetActionText(EditModeState eEditModeState, QString sNodeCodeName) const
{
	QString sUndoText;
	switch(m_eCurAction)
	{
	case VECTORACTION_None:
	case VECTORACTION_Outside:
		break;
	case VECTORACTION_Creation:
		if(m_eEditModeType == EDITMODETYPE_PrimitiveLineChain || m_eEditModeType == EDITMODETYPE_FixtureChain)
			sUndoText = "Create new Line Chain " % sNodeCodeName;
		else if(m_eEditModeType == EDITMODETYPE_FixturePoint)
			sUndoText = "Create new Point " % sNodeCodeName;
		else
			sUndoText = "Create new " % HyGlobal::ShapeName(m_eShapeType) % " shape " % sNodeCodeName;
		break;
	case VECTORACTION_Inside:
	case VECTORACTION_HoverCenter:
		sUndoText = "Translate shape " % sNodeCodeName;
		break;
	case VECTORACTION_AppendVertex:
		sUndoText = "Append vertex on " % sNodeCodeName;
		break;
	case VECTORACTION_InsertVertex:
		sUndoText = "Insert vertex on " % sNodeCodeName;
		break;
	case VECTORACTION_HoverGrabPoint:
		if(eEditModeState == EDITMODE_MouseClickTransform) // Ensure the minimum drag distance has occured
			break;

		if((m_eEditModeType == EDITMODETYPE_PrimitiveLineChain || m_eEditModeType == EDITMODETYPE_FixtureChain) ||
			m_eShapeType == SHAPE_Polygon ||
			m_eShapeType == SHAPE_LineSegment)
		{
			sUndoText = "Translate vert(s) on " % sNodeCodeName;
		}
		else if(m_eEditModeType == EDITMODETYPE_FixturePoint)
			sUndoText = "Translate point position on " % sNodeCodeName;
		else if(m_eShapeType == SHAPE_Circle)
			sUndoText = "Adjust circle radius on " % sNodeCodeName;
		else if(m_eShapeType == SHAPE_Box)
			sUndoText = "Adjust box size on " % sNodeCodeName;
		else if(m_eShapeType == SHAPE_Capsule)
			sUndoText = "Adjust capsule size on " % sNodeCodeName;
		else
			HyGuiLog("VectorModel::MouseTransformReleased - Invalid shape type for VECTORACTION_HoverGrabPoint", LOGTYPE_Error);
		break;

	case VECTORACTION_CloseLoop:
		sUndoText = "Close loop on " % sNodeCodeName;
		break;

	default:
		HyGuiLog("VectorModel::MouseTransformReleased - Invalid m_eCurTransform", LOGTYPE_Error);
		break;
	}

	return sUndoText;
}

void VectorModel::ClearAction()
{
	m_eCurAction = VECTORACTION_None;
	m_iGrabPointIndex = -1;
	m_bOverrideTransformStart = false;
	m_ptTransformStartOverride = { 0.0f, 0.0f };
}

/*virtual*/ void VectorModel::OnSyncViews() const /*override*/
{
	HyPrimitive2d *pHyPrimNode = GetHyNode();
	if(pHyPrimNode)
	{
		pHyPrimNode->SetLayerOffset(m_iPrimLayerIndex, GetOffset());
		pHyPrimNode->SetLayerVisible(m_iPrimLayerIndex, IsVisible());
		pHyPrimNode->SetLayerColor(m_iPrimLayerIndex, GetColor());
		pHyPrimNode->SetLayerAlpha(m_iPrimLayerIndex, GetAlpha());

		if(GetNumFixtures() > 0)
			pHyPrimNode->SetAsFixture(m_iPrimLayerIndex, *GetFixture(0), m_fOutline);
	}
	
}

std::vector<float> VectorModel::ConvertedPointData() const
{
	std::vector<float> convertedDataList;
	convertedDataList.reserve(2);

	glm::vec2 ptPos = m_GrabPointCenter.GetPos();
	convertedDataList.emplace_back(ptPos.x);
	convertedDataList.emplace_back(ptPos.y);

	return convertedDataList;
}

std::vector<float> VectorModel::ConvertedBoxData() const
{
	HyShape2d tmpBoxShape;
	if((m_eEditModeType == EDITMODETYPE_PrimitiveLineChain || m_eEditModeType == EDITMODETYPE_FixtureChain) ||
	    m_eShapeType == SHAPE_Polygon)
	{
		std::vector<glm::vec2> vertexList;
		for(const GfxGrabPointModel &grabPt : m_GrabPointList)
			vertexList.push_back(grabPt.GetPos());
		
		b2AABB boundingBox = HyMath::ComputeAABB(vertexList.data(), static_cast<int>(vertexList.size()), 5.0f);

		glm::vec2 ptCenter = glm::vec2(0.5f * (boundingBox.lowerBound.x + boundingBox.upperBound.x), 0.5f * (boundingBox.lowerBound.y + boundingBox.upperBound.y));
		glm::vec2 vHalfExtents = glm::vec2(0.5f * (boundingBox.upperBound.x - boundingBox.lowerBound.x), 0.5f * (boundingBox.upperBound.y - boundingBox.lowerBound.y));
		tmpBoxShape.SetAsBox(HyRect(vHalfExtents.x, vHalfExtents.y, ptCenter, 0.0f));
		
		return tmpBoxShape.SerializeSelf();
	}
	else if(m_eEditModeType == EDITMODETYPE_FixturePoint)
	{
		tmpBoxShape.SetAsBox(HyRect(10.0f, 10.0f, m_GrabPointCenter.GetPos(), 0.0f));
		return tmpBoxShape.SerializeSelf();
	}

	switch(m_eShapeType)
	{
	case SHAPE_None:
		break;
	case SHAPE_Box:
		return SerializeData();
	case SHAPE_Circle:
		if(m_FixtureList[0]->IsValid() && m_FixtureList[0]->GetType() != HYFIXTURE_LineSegment)
		{
			b2Circle b2Circ = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsCircle();
			glm::vec2 ptCenter(b2Circ.center.x, b2Circ.center.y);
			float fRadius = b2Circ.radius;
			tmpBoxShape.SetAsBox(HyRect(fRadius, fRadius, ptCenter, 0.0f));
			return tmpBoxShape.SerializeSelf();
		}
		break;
	case SHAPE_LineSegment:
		if(m_FixtureList[0]->IsValid() && m_FixtureList[0]->GetType() != HYFIXTURE_LineSegment)
		{
			b2Segment b2LineSeg = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsSegment();
			glm::vec2 pt1(b2LineSeg.point1.x, b2LineSeg.point1.y);
			glm::vec2 pt2(b2LineSeg.point2.x, b2LineSeg.point2.y);
			glm::vec2 ptCenter = (pt1 + pt2) * 0.5f;
			glm::vec2 vHalfExtents = glm::abs(pt2 - pt1) * 0.5f;
			tmpBoxShape.SetAsBox(HyRect(vHalfExtents.x, vHalfExtents.y, ptCenter, 0.0f));
			return tmpBoxShape.SerializeSelf();
		}
		break;
	case SHAPE_Capsule:
		if(m_FixtureList[0]->IsValid() && m_FixtureList[0]->GetType() != HYFIXTURE_LineSegment)
		{
			b2Capsule b2Cap = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsCapsule();
			glm::vec2 pt1(b2Cap.center1.x, b2Cap.center1.y);
			glm::vec2 pt2(b2Cap.center2.x, b2Cap.center2.y);
			float fRadius = b2Cap.radius;
			glm::vec2 ptCenter = (pt1 + pt2) * 0.5f;
			glm::vec2 vHalfExtents = (glm::abs(pt2 - pt1) * 0.5f) + glm::vec2(fRadius, fRadius);
			tmpBoxShape.SetAsBox(HyRect(vHalfExtents.x, vHalfExtents.y, ptCenter, 0.0f));
			return tmpBoxShape.SerializeSelf();
		}
		break;
	case SHAPE_Polygon:
		HyGuiLog("VectorModel::ConvertedBoxData - Polygon handled with line chain.", LOGTYPE_Error);
		break;

	default:
		HyGuiLog("VectorModel::ConvertedBoxData - Unhandled conversion to Box", LOGTYPE_Error);
		break;
	}
	
	return std::vector<float>();
}

std::vector<float> VectorModel::ConvertedCircleData() const
{
	std::vector<float> convertedDataList;
	if((m_eEditModeType == EDITMODETYPE_PrimitiveLineChain || m_eEditModeType == EDITMODETYPE_FixtureChain) ||
		m_eShapeType == SHAPE_Polygon)
	{
		std::vector<glm::vec2> vertexList;
		for(const GfxGrabPointModel &grabPt : m_GrabPointList)
			vertexList.push_back(grabPt.GetPos());
		
		if(vertexList.size() > 1)
		{
			b2AABB boundingBox = HyMath::ComputeAABB(vertexList.data(), static_cast<int>(vertexList.size()), 5.0f);
			
			glm::vec2 ptCenter = glm::vec2(0.5f * (boundingBox.lowerBound.x + boundingBox.upperBound.x), 0.5f * (boundingBox.lowerBound.y + boundingBox.upperBound.y));
			glm::vec2 vHalfExtents = glm::vec2(0.5f * (boundingBox.upperBound.x - boundingBox.lowerBound.x), 0.5f * (boundingBox.upperBound.y - boundingBox.lowerBound.y));
			float fRadius = HyMath::Max(vHalfExtents.x, vHalfExtents.y);
			convertedDataList.push_back(ptCenter.x);
			convertedDataList.push_back(ptCenter.y);
			convertedDataList.push_back(fRadius);
		}
	}
	else if(m_eEditModeType == EDITMODETYPE_FixturePoint)
	{
		glm::vec2 ptPos = m_GrabPointCenter.GetPos();
		convertedDataList.push_back(ptPos.x);
		convertedDataList.push_back(ptPos.y);
		convertedDataList.push_back(10.0f);
	}
	else
	{
		switch(m_eShapeType)
		{
		case SHAPE_None:
			break;
		case SHAPE_Box:
			if(m_FixtureList[0]->IsValid() && m_FixtureList[0]->GetType() != HYFIXTURE_LineSegment)
			{
				b2Polygon b2Poly = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsPolygon();
				glm::vec2 ptCenter(0.0f, 0.0f);
				for(int i = 0; i < b2Poly.count; ++i)
					ptCenter += glm::vec2(b2Poly.vertices[i].x, b2Poly.vertices[i].y);
				ptCenter /= static_cast<float>(b2Poly.count);
				float fMaxDist = 0.0f;
				for(int i = 0; i < b2Poly.count; ++i)
				{
					float fDist = glm::distance(ptCenter, glm::vec2(b2Poly.vertices[i].x, b2Poly.vertices[i].y));
					if(fDist > fMaxDist)
						fMaxDist = fDist;
				}
				convertedDataList.push_back(ptCenter.x);
				convertedDataList.push_back(ptCenter.y);
				convertedDataList.push_back(fMaxDist);
			}
			break;
		case SHAPE_Circle:
			return SerializeData();
		case SHAPE_LineSegment:
			if(m_FixtureList[0]->IsValid() && m_FixtureList[0]->GetType() != HYFIXTURE_LineSegment)
			{
				b2Segment b2LineSeg = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsSegment();
				glm::vec2 pt1(b2LineSeg.point1.x, b2LineSeg.point1.y);
				glm::vec2 pt2(b2LineSeg.point2.x, b2LineSeg.point2.y);
				glm::vec2 ptCenter = (pt1 + pt2) * 0.5f;
				float fRadius = glm::distance(pt1, pt2) * 0.5f;
				convertedDataList.push_back(ptCenter.x);
				convertedDataList.push_back(ptCenter.y);
				convertedDataList.push_back(fRadius);
			}
			break;
		case SHAPE_Capsule:
			if(m_FixtureList[0]->IsValid() && m_FixtureList[0]->GetType() != HYFIXTURE_LineSegment)
			{
				b2Capsule b2Cap = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsCapsule();
				glm::vec2 pt1(b2Cap.center1.x, b2Cap.center1.y);
				glm::vec2 pt2(b2Cap.center2.x, b2Cap.center2.y);
				glm::vec2 ptCenter = (pt1 + pt2) * 0.5f;
				float fRadius = glm::distance(pt1, pt2) * 0.5f + b2Cap.radius;
				convertedDataList.push_back(ptCenter.x);
				convertedDataList.push_back(ptCenter.y);
				convertedDataList.push_back(fRadius);
			}
			break;

		case SHAPE_Polygon:
			HyGuiLog("VectorModel::ConvertedCircleData - Polygon handled with line chain.", LOGTYPE_Error);
			break;
	
		default:
			HyGuiLog("GfxShapeModel::ConvertedCircleData - Unhandled conversion to Circle", LOGTYPE_Error);
			break;
		}
	}
	
	return convertedDataList;
}

std::vector<float> VectorModel::ConvertedLineSegmentData() const
{
	std::vector<float> convertedDataList;
	if((m_eEditModeType == EDITMODETYPE_PrimitiveLineChain || m_eEditModeType == EDITMODETYPE_FixtureChain) ||
		m_eShapeType == SHAPE_Polygon)
	{
		std::vector<glm::vec2> vertexList;
		for(const GfxGrabPointModel &grabPt : m_GrabPointList)
			vertexList.push_back(grabPt.GetPos());
		
		if(vertexList.size() > 1)
		{
			b2AABB boundingBox = HyMath::ComputeAABB(vertexList.data(), static_cast<int>(vertexList.size()), 5.0f);
			
			convertedDataList.push_back(boundingBox.lowerBound.x);
			convertedDataList.push_back(boundingBox.lowerBound.y);
			convertedDataList.push_back(boundingBox.upperBound.x);
			convertedDataList.push_back(boundingBox.upperBound.y);
		}
	}
	else if(m_eEditModeType == EDITMODETYPE_FixturePoint)
	{
		glm::vec2 ptPos = m_GrabPointCenter.GetPos();
		convertedDataList.push_back(ptPos.x - 10.0f);
		convertedDataList.push_back(ptPos.y - 10.0f);
		convertedDataList.push_back(ptPos.x + 10.0f);
		convertedDataList.push_back(ptPos.y + 10.0f);
	}
	else
	{
		switch(m_eShapeType)
		{
		case SHAPE_None:
			break;
		case SHAPE_Box:
			if(m_FixtureList[0]->IsValid() && m_FixtureList[0]->GetType() != HYFIXTURE_LineSegment)
			{
				b2Polygon b2Poly = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsPolygon();
				convertedDataList.push_back(b2Poly.vertices[0].x);
				convertedDataList.push_back(b2Poly.vertices[0].y);
				convertedDataList.push_back(b2Poly.vertices[2].x);
				convertedDataList.push_back(b2Poly.vertices[2].y);
			}
			break;
		case SHAPE_Circle:
			if(m_FixtureList[0]->IsValid() && m_FixtureList[0]->GetType() != HYFIXTURE_LineSegment)
			{
				b2Circle b2Circ = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsCircle();
				glm::vec2 ptCenter(b2Circ.center.x, b2Circ.center.y);
				float fRadius = b2Circ.radius;
				convertedDataList.push_back(ptCenter.x - fRadius);
				convertedDataList.push_back(ptCenter.y);
				convertedDataList.push_back(ptCenter.x + fRadius);
				convertedDataList.push_back(ptCenter.y);
			}
			break;
		case SHAPE_LineSegment:
			return SerializeData();
		case SHAPE_Capsule:
			if(m_FixtureList[0]->IsValid() && m_FixtureList[0]->GetType() != HYFIXTURE_LineSegment)
			{
				b2Capsule b2Cap = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsCapsule();
				glm::vec2 pt1(b2Cap.center1.x, b2Cap.center1.y);
				glm::vec2 pt2(b2Cap.center2.x, b2Cap.center2.y);
				convertedDataList.push_back(pt1.x);
				convertedDataList.push_back(pt1.y);
				convertedDataList.push_back(pt2.x);
				convertedDataList.push_back(pt2.y);
			}
			break;
		case SHAPE_Polygon:
			HyGuiLog("VectorModel::ConvertedLineSegmentData - Polygon handled with line chain.", LOGTYPE_Error);
			break;

		default:
			HyGuiLog("GfxShapeModel::ConvertedLineSegmentData - Unhandled conversion to LineSegment", LOGTYPE_Error);
			break;
		}
	}
	
	return convertedDataList;
}

std::vector<float> VectorModel::ConvertedCapsuleData() const
{
	std::vector<float> convertedDataList;

	if((m_eEditModeType == EDITMODETYPE_PrimitiveLineChain || m_eEditModeType == EDITMODETYPE_FixtureChain) ||
		m_eShapeType == SHAPE_Polygon || m_eShapeType == SHAPE_LineSegment)
	{
		std::vector<glm::vec2> vertexList;
		for(const GfxGrabPointModel &grabPt : m_GrabPointList)
			vertexList.push_back(grabPt.GetPos());
		
		if(vertexList.size() > 1)
		{
			b2AABB boundingBox = HyMath::ComputeAABB(vertexList.data(), static_cast<int>(vertexList.size()), 5.0f);
			
			convertedDataList.push_back(boundingBox.lowerBound.x);
			convertedDataList.push_back(boundingBox.lowerBound.y);
			convertedDataList.push_back(boundingBox.upperBound.x);
			convertedDataList.push_back(boundingBox.upperBound.y);
			float fRadius = HyMath::Min(0.5f * (boundingBox.upperBound.x - boundingBox.lowerBound.x), 0.5f * (boundingBox.upperBound.y - boundingBox.lowerBound.y));
			convertedDataList.push_back(fRadius);
		}
	}
	else if(m_eEditModeType == EDITMODETYPE_FixturePoint)
	{
		glm::vec2 ptPos = m_GrabPointCenter.GetPos();
		convertedDataList.push_back(ptPos.x - 10.0f);
		convertedDataList.push_back(ptPos.y - 10.0f);
		convertedDataList.push_back(ptPos.x + 10.0f);
		convertedDataList.push_back(ptPos.y + 10.0f);
		convertedDataList.push_back(10.0f);
	}
	else
	{
		switch(m_eShapeType)
		{
		case SHAPE_None:
			break;
		case SHAPE_Box:
			if(m_FixtureList[0]->IsValid() && m_FixtureList[0]->GetType() != HYFIXTURE_LineSegment)
			{
				b2Polygon b2Poly = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsPolygon();
				glm::vec2 pt1(b2Poly.vertices[1].x, b2Poly.vertices[1].y);
				glm::vec2 pt2(b2Poly.vertices[3].x, b2Poly.vertices[3].y);
				float fRadius = glm::distance(pt1, pt2) * 0.5f;
				convertedDataList.push_back(pt1.x);
				convertedDataList.push_back(pt1.y);
				convertedDataList.push_back(pt2.x);
				convertedDataList.push_back(pt2.y);
				convertedDataList.push_back(fRadius);
			}
			break;
		case SHAPE_Circle:
			if(m_FixtureList[0]->IsValid() && m_FixtureList[0]->GetType() != HYFIXTURE_LineSegment)
			{
				b2Circle b2Circ = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsCircle();
				glm::vec2 ptCenter(b2Circ.center.x, b2Circ.center.y);
				float fRadius = b2Circ.radius;
				glm::vec2 pt1 = ptCenter + glm::vec2(0.0f, -fRadius);
				glm::vec2 pt2 = ptCenter + glm::vec2(0.0f, fRadius);
				convertedDataList.push_back(pt1.x);
				convertedDataList.push_back(pt1.y);
				convertedDataList.push_back(pt2.x);
				convertedDataList.push_back(pt2.y);
				convertedDataList.push_back(fRadius);
			}
			break;
		case SHAPE_Capsule:
			return SerializeData();

		case SHAPE_Polygon:
		case SHAPE_LineSegment:
			HyGuiLog("VectorModel::ConvertedCapsuleData - Polygon and LineSegment handled with line chain.", LOGTYPE_Error);
			break;
		default:
			HyGuiLog("VectorModel::ConvertedCapsuleData - Unhandled conversion to Capsule", LOGTYPE_Error);
			break;
		}
	}

	return convertedDataList;
}

std::vector<float> VectorModel::ConvertedPolygonOrLineChainData() const
{
	std::vector<float> convertedDataList;

	if(m_eEditModeType == EDITMODETYPE_PrimitiveLineChain || m_eEditModeType == EDITMODETYPE_FixtureChain)
	{
		convertedDataList = SerializeData();
		return convertedDataList;
	}
	else if(m_eEditModeType == EDITMODETYPE_FixturePoint)
	{
		glm::vec2 ptPos = m_GrabPointCenter.GetPos();
		convertedDataList.push_back(ptPos.x - 10.0f);
		convertedDataList.push_back(ptPos.y - 10.0f);
		convertedDataList.push_back(ptPos.x + 10.0f);
		convertedDataList.push_back(ptPos.y + 10.0f);
		convertedDataList.push_back(0.0f);

		return convertedDataList;
	}

	switch(m_eShapeType)
	{
	case SHAPE_None:
		break;
	case SHAPE_Box:
		if(m_FixtureList[0]->IsValid() && m_FixtureList[0]->GetType() != HYFIXTURE_LineSegment)
		{
			b2Polygon b2Poly = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsPolygon();
			for(int i = 0; i < b2Poly.count; ++i)
			{
				convertedDataList.push_back(b2Poly.vertices[i].x);
				convertedDataList.push_back(b2Poly.vertices[i].y);
			}
			convertedDataList.push_back(1.0f); // Closed loop
		}
		break;
	case SHAPE_Circle:
		if(m_FixtureList[0]->IsValid() && m_FixtureList[0]->GetType() != HYFIXTURE_LineSegment)
		{
			b2Circle b2Circ = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsCircle();
			glm::vec2 ptCenter(b2Circ.center.x, b2Circ.center.y);
			float fRadius = b2Circ.radius;

			glm::vec2 ptVertList[6];
			ptVertList[0] = ptCenter + glm::vec2(fRadius, 0.0f);
			ptVertList[1] = ptCenter + glm::vec2(fRadius * 0.5f, fRadius * sqrt(3.0f) * 0.5f);
			ptVertList[2] = ptCenter + glm::vec2(-fRadius * 0.5f, fRadius * sqrt(3.0f) * 0.5f);
			ptVertList[3] = ptCenter + glm::vec2(-fRadius, 0.0f);
			ptVertList[4] = ptCenter + glm::vec2(-fRadius * 0.5f, -fRadius * sqrt(3.0f) * 0.5f);
			ptVertList[5] = ptCenter + glm::vec2(fRadius * 0.5f, -fRadius * sqrt(3.0f) * 0.5f);
			for(int i = 0; i < 6; ++i)
			{
				convertedDataList.push_back(ptVertList[i].x);
				convertedDataList.push_back(ptVertList[i].y);
			}
			convertedDataList.push_back(1.0f); // Closed loop
		}
		break;
	case SHAPE_LineSegment:
		if(m_FixtureList[0]->IsValid() && m_FixtureList[0]->GetType() != HYFIXTURE_LineSegment)
		{
			b2Segment b2LineSeg = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsSegment();
			convertedDataList.push_back(b2LineSeg.point1.x);
			convertedDataList.push_back(b2LineSeg.point1.y);
			convertedDataList.push_back(b2LineSeg.point2.x);
			convertedDataList.push_back(b2LineSeg.point2.y);
			convertedDataList.push_back(0.0f); // Open loop
		}
		break;
	case SHAPE_Polygon:
		convertedDataList = SerializeData();
		break;
	case SHAPE_Capsule:
		if(m_FixtureList[0]->IsValid() && m_FixtureList[0]->GetType() != HYFIXTURE_LineSegment)
		{
			b2Capsule b2Cap = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsCapsule();
			glm::vec2 pt1(b2Cap.center1.x, b2Cap.center1.y);
			glm::vec2 pt2(b2Cap.center2.x, b2Cap.center2.y);
			float fRadius = b2Cap.radius;
			glm::vec2 vDir = glm::normalize(pt2 - pt1);
			glm::vec2 vPerp(-vDir.y, vDir.x);
			// Approximate capsule with 8 vertices
			convertedDataList.push_back(pt1.x + vPerp.x * fRadius);
			convertedDataList.push_back(pt1.y + vPerp.y * fRadius);
			convertedDataList.push_back(pt1.x + (vDir.x * fRadius * -1.0f));
			convertedDataList.push_back(pt1.y + (vDir.y * fRadius * -1.0f));
			convertedDataList.push_back(pt1.x - vPerp.x * fRadius);
			convertedDataList.push_back(pt1.y - vPerp.y * fRadius);
			convertedDataList.push_back(pt2.x - vPerp.x * fRadius);
			convertedDataList.push_back(pt2.y - vPerp.y * fRadius);
			convertedDataList.push_back(pt2.x + (vDir.x * fRadius));
			convertedDataList.push_back(pt2.y + (vDir.y * fRadius));
			convertedDataList.push_back(pt2.x + vPerp.x * fRadius);
			convertedDataList.push_back(pt2.y + vPerp.y * fRadius);
			convertedDataList.push_back(1.0f); // Closed loop
		}
		break;
	default:
		HyGuiLog("GfxShapeModel::ConvertedPolygonOrLineChainData - Unhandled conversion to Polygon/LineChain", LOGTYPE_Error);
		break;
	}
	
	return convertedDataList;
}

void VectorModel::DoTransformShapeCtrlGrabPoint_Box(bool bCtrlHeld, bool bShiftHeld, float fSnapAmt, glm::vec2 vDragDelta)
{
	std::function<GfxTransformCtrl::GrabPointType(int)> fpIndexToEnum =
		[](int iGrabPointIndex) -> GfxTransformCtrl::GrabPointType
		{
			switch(iGrabPointIndex)
			{
			case 0:	return GfxTransformCtrl::GRAB_BotLeft;
			case 1:	return GfxTransformCtrl::GRAB_BotMid;
			case 2:	return GfxTransformCtrl::GRAB_BotRight;
			case 3:	return GfxTransformCtrl::GRAB_MidRight;
			case 4:	return GfxTransformCtrl::GRAB_TopRight;
			case 5:	return GfxTransformCtrl::GRAB_TopMid;
			case 6:	return GfxTransformCtrl::GRAB_TopLeft;
			case 7:	return GfxTransformCtrl::GRAB_MidLeft;
			default:
				HyGuiLog("VectorModel::DoTransformShapeCtrlGrabPoint_Box() - Box shape had invalid iGrabPointIndex", LOGTYPE_Error);
				break;
			}
			return GfxTransformCtrl::GRAB_None;
		};
	std::function<int(GfxTransformCtrl::GrabPointType)> fpEnumToIndex =
		[](GfxTransformCtrl::GrabPointType eGrabPt) -> int
		{
			switch(eGrabPt)
			{
			case GfxTransformCtrl::GRAB_BotLeft: return 0;
			case GfxTransformCtrl::GRAB_BotMid: return 1;
			case GfxTransformCtrl::GRAB_BotRight: return 2;
			case GfxTransformCtrl::GRAB_MidRight: return 3;
			case GfxTransformCtrl::GRAB_TopRight: return 4;
			case GfxTransformCtrl::GRAB_TopMid: return 5;
			case GfxTransformCtrl::GRAB_TopLeft: return 6;
			case GfxTransformCtrl::GRAB_MidLeft: return 7;
			default:
				HyGuiLog("VectorModel::DoTransformShapeCtrlGrabPoint_Box() - Box shape had invalid GfxTransformCtrl::GrabPointType enum", LOGTYPE_Error);
				break;
			}
			return -1;
		};
	#define fpGetGrabPt(x) m_GrabPointList[fpEnumToIndex(x)]

	GfxTransformCtrl::GrabPointType eGrabbedPoint = fpIndexToEnum(m_iGrabPointIndex);
	bool bCornerGrabbed = false;
	switch(eGrabbedPoint)
	{
	case GfxTransformCtrl::GRAB_BotLeft:
	case GfxTransformCtrl::GRAB_BotRight:
	case GfxTransformCtrl::GRAB_TopRight:
	case GfxTransformCtrl::GRAB_TopLeft:
		bCornerGrabbed = true;
		break;
	case GfxTransformCtrl::GRAB_BotMid:
	case GfxTransformCtrl::GRAB_MidRight:
	case GfxTransformCtrl::GRAB_TopMid:
	case GfxTransformCtrl::GRAB_MidLeft:
		bCornerGrabbed = false;
		break;
	default:
		HyGuiLog("VectorModel::DoTransformShapeCtrlGrabPoint_Box() - Box shape had invalid eGrabSpot", LOGTYPE_Error);
		break;
	}

	if(bCornerGrabbed)
	{
		int iAnchorIndex = -1, iAdjacentIndex1 = -1, iAdjacentIndex2 = -1;
		GfxGrabPointModel *pGrabbed, *pAnchor, *pAdjacentMid1, *pAdjacentMid2, *pAdjacentCorner1, *pAdjacentCorner2, *pFarMid1, *pFarMid2;
		pGrabbed = &fpGetGrabPt(eGrabbedPoint);
		if(eGrabbedPoint == GfxTransformCtrl::GRAB_BotLeft)
		{
			iAnchorIndex = fpEnumToIndex(GfxTransformCtrl::GRAB_TopRight);
			pAnchor = &fpGetGrabPt(GfxTransformCtrl::GRAB_TopRight);
			pAdjacentMid1 = &fpGetGrabPt(GfxTransformCtrl::GRAB_BotMid);
			pAdjacentMid2 = &fpGetGrabPt(GfxTransformCtrl::GRAB_MidLeft);
			iAdjacentIndex1 = fpEnumToIndex(GfxTransformCtrl::GRAB_BotRight);
			pAdjacentCorner1 = &fpGetGrabPt(GfxTransformCtrl::GRAB_BotRight);
			iAdjacentIndex2 = fpEnumToIndex(GfxTransformCtrl::GRAB_TopLeft);
			pAdjacentCorner2 = &fpGetGrabPt(GfxTransformCtrl::GRAB_TopLeft);
			pFarMid1 = &fpGetGrabPt(GfxTransformCtrl::GRAB_MidRight);
			pFarMid2 = &fpGetGrabPt(GfxTransformCtrl::GRAB_TopMid);
		}
		else if(eGrabbedPoint == GfxTransformCtrl::GRAB_BotRight)
		{
			iAnchorIndex = fpEnumToIndex(GfxTransformCtrl::GRAB_TopLeft);
			pAnchor = &fpGetGrabPt(GfxTransformCtrl::GRAB_TopLeft);
			pAdjacentMid1 = &fpGetGrabPt(GfxTransformCtrl::GRAB_MidRight);
			pAdjacentMid2 = &fpGetGrabPt(GfxTransformCtrl::GRAB_BotMid);
			iAdjacentIndex1 = fpEnumToIndex(GfxTransformCtrl::GRAB_TopRight);
			pAdjacentCorner1 = &fpGetGrabPt(GfxTransformCtrl::GRAB_TopRight);
			iAdjacentIndex2 = fpEnumToIndex(GfxTransformCtrl::GRAB_BotLeft);
			pAdjacentCorner2 = &fpGetGrabPt(GfxTransformCtrl::GRAB_BotLeft);
			pFarMid1 = &fpGetGrabPt(GfxTransformCtrl::GRAB_TopMid);
			pFarMid2 = &fpGetGrabPt(GfxTransformCtrl::GRAB_MidLeft);
		}
		else if(eGrabbedPoint == GfxTransformCtrl::GRAB_TopRight)
		{
			iAnchorIndex = fpEnumToIndex(GfxTransformCtrl::GRAB_BotLeft);
			pAnchor = &fpGetGrabPt(GfxTransformCtrl::GRAB_BotLeft);
			pAdjacentMid1 = &fpGetGrabPt(GfxTransformCtrl::GRAB_TopMid);
			pAdjacentMid2 = &fpGetGrabPt(GfxTransformCtrl::GRAB_MidRight);
			iAdjacentIndex1 = fpEnumToIndex(GfxTransformCtrl::GRAB_TopLeft);
			pAdjacentCorner1 = &fpGetGrabPt(GfxTransformCtrl::GRAB_TopLeft);
			iAdjacentIndex2 = fpEnumToIndex(GfxTransformCtrl::GRAB_BotRight);
			pAdjacentCorner2 = &fpGetGrabPt(GfxTransformCtrl::GRAB_BotRight);
			pFarMid1 = &fpGetGrabPt(GfxTransformCtrl::GRAB_MidLeft);
			pFarMid2 = &fpGetGrabPt(GfxTransformCtrl::GRAB_BotMid);
		}
		else if(eGrabbedPoint == GfxTransformCtrl::GRAB_TopLeft)
		{
			iAnchorIndex = fpEnumToIndex(GfxTransformCtrl::GRAB_BotRight);
			pAnchor = &fpGetGrabPt(GfxTransformCtrl::GRAB_BotRight);
			pAdjacentMid1 = &fpGetGrabPt(GfxTransformCtrl::GRAB_MidLeft);
			pAdjacentMid2 = &fpGetGrabPt(GfxTransformCtrl::GRAB_TopMid);
			iAdjacentIndex1 = fpEnumToIndex(GfxTransformCtrl::GRAB_BotLeft);
			pAdjacentCorner1 = &fpGetGrabPt(GfxTransformCtrl::GRAB_BotLeft);
			iAdjacentIndex2 = fpEnumToIndex(GfxTransformCtrl::GRAB_TopRight);
			pAdjacentCorner2 = &fpGetGrabPt(GfxTransformCtrl::GRAB_TopRight);
			pFarMid1 = &fpGetGrabPt(GfxTransformCtrl::GRAB_BotMid);
			pFarMid2 = &fpGetGrabPt(GfxTransformCtrl::GRAB_MidRight);
		}

		glm::vec2 ptNewPos = pGrabbed->GetCachePos() + vDragDelta;
		if(bCtrlHeld) // Snap to fSnapAmt pixel increments if ctrl is held
			ptNewPos = { roundf(ptNewPos.x / fSnapAmt) * fSnapAmt, roundf(ptNewPos.y / fSnapAmt) * fSnapAmt };

		glm::vec2 vDir1 = pAdjacentCorner1->GetCachePos() - pGrabbed->GetCachePos();
		glm::vec2 vDir2 = pAdjacentCorner2->GetCachePos() - pGrabbed->GetCachePos();
		glm::vec2 vAnchorDir1 = pAdjacentCorner1->GetCachePos() - pAnchor->GetCachePos();
		glm::vec2 vAnchorDir2 = pAdjacentCorner2->GetCachePos() - pAnchor->GetCachePos();

		// Do half-space tests to see if the grabbed point was dragged past the anchor point
		bool bPast1 = false;
		bool bPast2 = false;
		if(1 == HyMath::HalfSpaceTest(ptNewPos, glm::normalize(vDir1), pAnchor->GetCachePos()))
			bPast1 = true;
		if(1 == HyMath::HalfSpaceTest(ptNewPos, glm::normalize(vDir2), pAnchor->GetCachePos()))
			bPast2 = true;
		if(bPast1 || bPast2)
		{
			if(bPast1 && bPast2)
			{
				// TODO: This conditional seems to get stuck in the editor
				pGrabbed->SetPos(pAnchor->GetPos());
				pAnchor->SetPos(ptNewPos);

				glm::vec2 ptIntersection1, ptIntersection2;
				if(HyMath::TestLinesOverlap(ptNewPos, glm::normalize(vAnchorDir1), pAnchor->GetCachePos(), glm::normalize(vDir1), ptIntersection1) == false)
					HyGuiLog("VectorModel::DoTransformShapeCtrlGrabPoint_Box() - HyMath::TestLinesOverlap failed [bPast1 && bPast2, ptIntersection1]", LOGTYPE_Error);
				if(HyMath::TestLinesOverlap(ptNewPos, glm::normalize(vAnchorDir2), pAnchor->GetCachePos(), glm::normalize(vDir2), ptIntersection2) == false)
					HyGuiLog("VectorModel::DoTransformShapeCtrlGrabPoint_Box() - HyMath::TestLinesOverlap failed [bPast1 && bPast2, ptIntersection2]", LOGTYPE_Error);
				pAdjacentCorner1->SetPos(ptIntersection1);
				pAdjacentCorner2->SetPos(ptIntersection2);

				m_iGrabPointIndex = iAnchorIndex;
			}
			else if(bPast1)
			{
				pAdjacentCorner2->SetPos(pAnchor->GetPos());
				pAdjacentCorner1->SetPos(ptNewPos);

				glm::vec2 ptIntersection1, ptIntersection2;
				if(HyMath::TestLinesOverlap(ptNewPos, glm::normalize(vDir1 * -1.0f), pAnchor->GetCachePos(), glm::normalize(vAnchorDir1), ptIntersection1) == false)
					HyGuiLog("VectorModel::DoTransformShapeCtrlGrabPoint_Box() - HyMath::TestLinesOverlap failed [bPast1, ptIntersection1]", LOGTYPE_Error);
				if(HyMath::TestLinesOverlap(ptNewPos, glm::normalize(vDir2), pAnchor->GetCachePos(), glm::normalize(vAnchorDir2 * -1.0f), ptIntersection2) == false)
					HyGuiLog("VectorModel::DoTransformShapeCtrlGrabPoint_Box() - HyMath::TestLinesOverlap failed [bPast1, ptIntersection2]", LOGTYPE_Error);
				pGrabbed->SetPos(ptIntersection1);
				pAnchor->SetPos(ptIntersection2);

				m_iGrabPointIndex = iAdjacentIndex1;
			}
			else
			{
				pAdjacentCorner1->SetPos(pAnchor->GetPos());
				pAdjacentCorner2->SetPos(ptNewPos);

				glm::vec2 ptIntersection1, ptIntersection2;
				if(HyMath::TestLinesOverlap(ptNewPos, glm::normalize(vDir2 * -1.0f), pAnchor->GetCachePos(), glm::normalize(vAnchorDir2), ptIntersection1) == false)
					HyGuiLog("VectorModel::DoTransformShapeCtrlGrabPoint_Box() - HyMath::TestLinesOverlap failed [bPast2, ptIntersection1]", LOGTYPE_Error);
				if(HyMath::TestLinesOverlap(ptNewPos, glm::normalize(vDir1), pAnchor->GetCachePos(), glm::normalize(vAnchorDir1 * -1.0f), ptIntersection2) == false)
					HyGuiLog("VectorModel::DoTransformShapeCtrlGrabPoint_Box() - HyMath::TestLinesOverlap failed [bPast2, ptIntersection2]", LOGTYPE_Error);
				pGrabbed->SetPos(ptIntersection1);
				pAnchor->SetPos(ptIntersection2);

				m_iGrabPointIndex = iAdjacentIndex2;
			}

			for(GfxGrabPointModel &grabPtRef : m_GrabPointList)
				grabPtRef.SetCachePos();

			m_bOverrideTransformStart = true;
			m_ptTransformStartOverride = ptNewPos;

			DoTransformShapeCtrlGrabPoint_Box(bCtrlHeld, bShiftHeld, fSnapAmt, glm::vec2(0.0f, 0.0f));
			return;
		}
		
		glm::vec2 ptIntersection1, ptIntersection2;
		if(HyMath::TestLinesOverlap(ptNewPos, glm::normalize(vDir1), pAnchor->GetCachePos(), glm::normalize(vAnchorDir1), ptIntersection1) == false)
			HyGuiLog("VectorModel::DoTransformShapeCtrlGrabPoint_Box() - HyMath::TestLinesOverlap failed [regular, 1]", LOGTYPE_Error);
		if(HyMath::TestLinesOverlap(ptNewPos, glm::normalize(vDir2), pAnchor->GetCachePos(), glm::normalize(vAnchorDir2), ptIntersection2) == false)
			HyGuiLog("VectorModel::DoTransformShapeCtrlGrabPoint_Box() - HyMath::TestLinesOverlap failed [regular, 2]", LOGTYPE_Error);
		
		pGrabbed->SetPos(ptNewPos);
		pAdjacentCorner1->SetPos(ptIntersection1);
		pAdjacentCorner2->SetPos(ptIntersection2);
		pAdjacentMid1->SetPos(ptNewPos + ((ptIntersection1 - ptNewPos) * 0.5f));
		pAdjacentMid2->SetPos(ptNewPos + ((ptIntersection2 - ptNewPos) * 0.5f));
		pFarMid1->SetPos(pAnchor->GetCachePos() + ((ptIntersection1 - pAnchor->GetCachePos()) * 0.5f));
		pFarMid2->SetPos(pAnchor->GetCachePos() + ((ptIntersection2 - pAnchor->GetCachePos()) * 0.5f));
	}
	else // Mid-edge point grabbed
	{
		GfxGrabPointModel *pGrabbed, *pAnchorCorner1, *pAnchorCorner2, *pAnchorMid, *pAdjacentCorner1, *pAdjacentCorner2, *pAdjacentMid1, *pAdjacentMid2;
		pGrabbed = &fpGetGrabPt(eGrabbedPoint);
		int iAnchorIndex = -1;
		if(eGrabbedPoint == GfxTransformCtrl::GRAB_BotMid)
		{
			pAnchorCorner1 = &fpGetGrabPt(GfxTransformCtrl::GRAB_TopRight);
			pAnchorCorner2 = &fpGetGrabPt(GfxTransformCtrl::GRAB_TopLeft);
			iAnchorIndex = fpEnumToIndex(GfxTransformCtrl::GRAB_TopMid);
			pAnchorMid = &fpGetGrabPt(GfxTransformCtrl::GRAB_TopMid);
			pAdjacentCorner1 = &fpGetGrabPt(GfxTransformCtrl::GRAB_BotRight);
			pAdjacentCorner2 = &fpGetGrabPt(GfxTransformCtrl::GRAB_BotLeft);
			pAdjacentMid1 = &fpGetGrabPt(GfxTransformCtrl::GRAB_MidRight);
			pAdjacentMid2 = &fpGetGrabPt(GfxTransformCtrl::GRAB_MidLeft);
		}
		else if(eGrabbedPoint == GfxTransformCtrl::GRAB_MidRight)
		{
			pAnchorCorner1 = &fpGetGrabPt(GfxTransformCtrl::GRAB_TopLeft);
			pAnchorCorner2 = &fpGetGrabPt(GfxTransformCtrl::GRAB_BotLeft);
			iAnchorIndex = fpEnumToIndex(GfxTransformCtrl::GRAB_MidLeft);
			pAnchorMid = &fpGetGrabPt(GfxTransformCtrl::GRAB_MidLeft);
			pAdjacentCorner1 = &fpGetGrabPt(GfxTransformCtrl::GRAB_TopRight);
			pAdjacentCorner2 = &fpGetGrabPt(GfxTransformCtrl::GRAB_BotRight);
			pAdjacentMid1 = &fpGetGrabPt(GfxTransformCtrl::GRAB_TopMid);
			pAdjacentMid2 = &fpGetGrabPt(GfxTransformCtrl::GRAB_BotMid);
		}
		else if(eGrabbedPoint == GfxTransformCtrl::GRAB_TopMid)
		{
			pAnchorCorner1 = &fpGetGrabPt(GfxTransformCtrl::GRAB_BotLeft);
			pAnchorCorner2 = &fpGetGrabPt(GfxTransformCtrl::GRAB_BotRight);
			iAnchorIndex = fpEnumToIndex(GfxTransformCtrl::GRAB_BotMid);
			pAnchorMid = &fpGetGrabPt(GfxTransformCtrl::GRAB_BotMid);
			pAdjacentCorner1 = &fpGetGrabPt(GfxTransformCtrl::GRAB_TopLeft);
			pAdjacentCorner2 = &fpGetGrabPt(GfxTransformCtrl::GRAB_TopRight);
			pAdjacentMid1 = &fpGetGrabPt(GfxTransformCtrl::GRAB_MidLeft);
			pAdjacentMid2 = &fpGetGrabPt(GfxTransformCtrl::GRAB_MidRight);
		}
		else if(eGrabbedPoint == GfxTransformCtrl::GRAB_MidLeft)
		{
			pAnchorCorner1 = &fpGetGrabPt(GfxTransformCtrl::GRAB_BotRight);
			pAnchorCorner2 = &fpGetGrabPt(GfxTransformCtrl::GRAB_TopRight);
			iAnchorIndex = fpEnumToIndex(GfxTransformCtrl::GRAB_MidRight);
			pAnchorMid = &fpGetGrabPt(GfxTransformCtrl::GRAB_MidRight);
			pAdjacentCorner1 = &fpGetGrabPt(GfxTransformCtrl::GRAB_BotLeft);
			pAdjacentCorner2 = &fpGetGrabPt(GfxTransformCtrl::GRAB_TopLeft);
			pAdjacentMid1 = &fpGetGrabPt(GfxTransformCtrl::GRAB_BotMid);
			pAdjacentMid2 = &fpGetGrabPt(GfxTransformCtrl::GRAB_TopMid);
		}

		glm::vec2 ptNewPos = pGrabbed->GetCachePos() + vDragDelta;
		if(bCtrlHeld) // Snap to fSnapAmt pixel increments if ctrl is held
			ptNewPos = { roundf(ptNewPos.x / fSnapAmt) * fSnapAmt, roundf(ptNewPos.y / fSnapAmt) * fSnapAmt };

		glm::vec2 vDir1 = pAdjacentCorner1->GetCachePos() - pGrabbed->GetCachePos();
		glm::vec2 vDir2 = pAdjacentCorner2->GetCachePos() - pGrabbed->GetCachePos();
		glm::vec2 vAnchorDir1 = pAdjacentCorner1->GetCachePos() - pAnchorCorner1->GetCachePos();
		glm::vec2 vAnchorDir2 = pAdjacentCorner2->GetCachePos() - pAnchorCorner2->GetCachePos();

		glm::vec2 vMainDir = pAnchorMid->GetCachePos() - pGrabbed->GetCachePos();
		if(1 == HyMath::HalfSpaceTest(ptNewPos, glm::normalize(vMainDir), pAnchorMid->GetCachePos()))
		{
			pGrabbed->SetPos(pAnchorMid->GetPos());
			pAdjacentCorner1->SetPos(pAnchorCorner1->GetPos());
			pAdjacentCorner2->SetPos(pAnchorCorner2->GetPos());

			glm::vec2 ptIntersection1, ptIntersection2;
			if(HyMath::TestLinesOverlap(ptNewPos, glm::normalize(vDir1), pAnchorCorner1->GetCachePos(), glm::normalize(vAnchorDir1 * -1.0f), ptIntersection1) == false)
				HyGuiLog("VectorModel::DoTransformShapeCtrlGrabPoint_Box() - HyMath::TestLinesOverlap failed [flip, mid grab point 1]", LOGTYPE_Error);
			
			if(HyMath::TestLinesOverlap(ptNewPos, glm::normalize(vDir2), pAnchorCorner2->GetCachePos(), glm::normalize(vAnchorDir2 * -1.0f), ptIntersection2) == false)
				HyGuiLog("VectorModel::DoTransformShapeCtrlGrabPoint_Box() - HyMath::TestLinesOverlap failed [flip, mid grab point 2]", LOGTYPE_Error);
			
			pAnchorCorner1->SetPos(ptIntersection1);
			pAnchorCorner2->SetPos(ptIntersection2);
			pAnchorMid->SetPos(ptIntersection1 + (ptIntersection2 - ptIntersection1) * 0.5f);

			m_iGrabPointIndex = iAnchorIndex;

			for(GfxGrabPointModel &grabPtRef : m_GrabPointList)
				grabPtRef.SetCachePos();

			m_bOverrideTransformStart = true;
			m_ptTransformStartOverride = ptNewPos;

			DoTransformShapeCtrlGrabPoint_Box(bCtrlHeld, bShiftHeld, fSnapAmt, glm::vec2(0.0f, 0.0f));
			return;
		}

		glm::vec2 ptIntersection1, ptIntersection2;
		if(HyMath::TestLinesOverlap(ptNewPos, glm::normalize(vDir1), pAnchorCorner1->GetCachePos(), glm::normalize(vAnchorDir1), ptIntersection1) == false)
			HyGuiLog("VectorModel::DoTransformShapeCtrlGrabPoint_Box() - HyMath::TestLinesOverlap failed [regular, mid grab point 1]", LOGTYPE_Error);
		
		if(HyMath::TestLinesOverlap(ptNewPos, glm::normalize(vDir2), pAnchorCorner2->GetCachePos(), glm::normalize(vAnchorDir2), ptIntersection2) == false)
			HyGuiLog("VectorModel::DoTransformShapeCtrlGrabPoint_Box() - HyMath::TestLinesOverlap failed [regular, mid grab point 2]", LOGTYPE_Error);
		
		pAdjacentCorner1->SetPos(ptIntersection1);
		pAdjacentCorner2->SetPos(ptIntersection2);
		pGrabbed->SetPos(ptIntersection1 + (ptIntersection2 - ptIntersection1) * 0.5f);
		pAdjacentMid1->SetPos(pAdjacentCorner1->GetPos() + (pAnchorCorner1->GetCachePos() - pAdjacentCorner1->GetPos()) * 0.5f);
		pAdjacentMid2->SetPos(pAdjacentCorner2->GetPos() + (pAnchorCorner2->GetCachePos() - pAdjacentCorner2->GetPos()) * 0.5f);
	} // end of corner or mid-point logic

	std::vector<glm::vec2> ptVertexList = { m_GrabPointList[0].GetPos(), m_GrabPointList[2].GetPos(), m_GrabPointList[4].GetPos(), m_GrabPointList[6].GetPos() };
	static_cast<HyShape2d *>(m_FixtureList[0])->SetAsPolygon(ptVertexList);
	glm::vec2 ptCentroid;
	m_FixtureList[0]->GetCentroid(ptCentroid);
	m_GrabPointCenter.SetPos(ptCentroid);
}

VectorAction VectorModel::DoMouseMoveIdle()
{
	m_iGrabPointIndex = -1;

	for(int i = 0; i < m_GrabPointList.size(); ++i)
	{
		if(m_GrabPointList[i].IsMouseHover())
		{
			m_iGrabPointIndex = i;

			// Test for close loop
			if(m_bLoopClosed == false &&
			   m_GrabPointList.size() > 2 &&
			   m_GrabPointList[i].IsSelected() == false &&
			   (i == 0 || i == m_GrabPointList.size() - 1) &&
			   GetNumGrabPointsSelected() == 1 &&
				(i == 0 ? m_GrabPointList.back().IsSelected() : m_GrabPointList.front().IsSelected()))
			{
				return VECTORACTION_CloseLoop;
			}

			return VECTORACTION_HoverGrabPoint;
		}
	}
	if(m_GrabPointCenter.IsMouseHover())
		return VECTORACTION_HoverCenter;

	if((m_eEditModeType == EDITMODETYPE_PrimitiveLineChain || m_eEditModeType == EDITMODETYPE_FixtureChain) ||
		m_eShapeType == SHAPE_Polygon)
	{
		if(m_GrabPointList.empty())
			return VECTORACTION_Creation;

		if(CheckIfAddVertexOnEdge())
			return VECTORACTION_InsertVertex;

		if(m_bLoopClosed == false)
		{
			// If only the first or last vertex in chain is selected, return VECTORACTION_AppendVertex to indicate appending verts to the selected end
			int iNumSelected = 0;
			for(const GfxGrabPointModel &grabPtModel : m_GrabPointList)
			{
				if(grabPtModel.IsSelected())
					++iNumSelected;
			}
			if(iNumSelected == 1)
			{
				if(m_GrabPointList.front().IsSelected())
				{
					m_iGrabPointIndex = 0;
					return VECTORACTION_AppendVertex;
				}
				else if(m_GrabPointList.back().IsSelected())
				{
					m_iGrabPointIndex = m_GrabPointList.size();
					return VECTORACTION_AppendVertex;
				}
			}
		}
	}
	else if(IsValidModel() == false) // Any shape besides SHAPE_Polygon
		return VECTORACTION_Creation;

	if(m_eEditModeType != EDITMODETYPE_PrimitiveLineChain &&
	   m_eEditModeType != EDITMODETYPE_FixtureChain &&
	   m_eEditModeType != EDITMODETYPE_FixturePoint)
	{
		glm::vec2 ptWorldMousePos;
		if(HyEngine::Input().GetWorldMousePos(ptWorldMousePos) == false)
			HyGuiLog("GfxShapeModel::DoMouseMoveIdle - Failed to get world mouse position for hit testing", LOGTYPE_Error);

		for(IHyFixture2d *pFixture : m_FixtureList)
		{
			if(pFixture->TestPoint(ptWorldMousePos, glm::identity<glm::mat4>()))
				return VECTORACTION_Inside;
		}
	}
	
	return VECTORACTION_Outside;
}

void VectorModel::TransformData(glm::mat4 mtxTransform)
{
	for(IHyFixture2d *pFixture : m_FixtureList)
		pFixture->TransformSelf(mtxTransform);

	SyncViews(EDITMODE_Idle);
}

void VectorModel::DoTransformCreation(bool bCtrlHeld, bool bShiftHeld, float fSnapAmt, glm::vec2 ptStartPos, glm::vec2 ptDragPos)
{
	if(bCtrlHeld)
	{
		ptStartPos = { roundf(ptStartPos.x / fSnapAmt) * fSnapAmt, roundf(ptStartPos.y / fSnapAmt) * fSnapAmt };
		ptDragPos = { roundf(ptDragPos.x / fSnapAmt) * fSnapAmt, roundf(ptDragPos.y / fSnapAmt) * fSnapAmt };
	}

	glm::vec2 ptLowerBound, ptUpperBound, ptCenter;
	if(bShiftHeld)
	{
		ptCenter = ptStartPos;

		glm::vec2 vRadius = ptCenter - ptDragPos;
		vRadius.x = abs(vRadius.x);
		vRadius.y = abs(vRadius.y);
		ptUpperBound = (ptCenter + vRadius);
		ptLowerBound = (ptCenter + (vRadius * -1.0f));
	}
	else
	{
		HySetVec(ptLowerBound, ptStartPos.x < ptDragPos.x ? ptStartPos.x : ptDragPos.x, ptStartPos.y < ptDragPos.y ? ptStartPos.y : ptDragPos.y);
		HySetVec(ptUpperBound, ptStartPos.x >= ptDragPos.x ? ptStartPos.x : ptDragPos.x, ptStartPos.y >= ptDragPos.y ? ptStartPos.y : ptDragPos.y);
		ptCenter = ptLowerBound + ((ptUpperBound - ptLowerBound) * 0.5f);
	}

	if((m_eEditModeType == EDITMODETYPE_PrimitiveLineChain || m_eEditModeType == EDITMODETYPE_FixtureChain) ||
		m_eShapeType == SHAPE_Polygon)
	{
		if(m_FixtureList.empty())
		{
			if(m_eEditModeType == EDITMODETYPE_PrimitiveLineChain || m_eEditModeType == EDITMODETYPE_FixtureChain)
				m_FixtureList.push_back(new HyChain2d());
			else
				m_FixtureList.push_back(new HyShape2d());
		}
		m_fOutline = 1.0f;

		m_GrabPointList.clear();
		m_GrabPointList.append(GfxGrabPointModel(GRABPOINT_Endpoint, ptStartPos));
		m_GrabPointList.append(GfxGrabPointModel(GRABPOINT_Endpoint, ptDragPos));
		
		m_GrabPointList[0].SetSelected(false);
		m_GrabPointList[1].Set(GRABPOINT_EndpointSelected, ptDragPos);
		m_sMalformedReason = "Incomplete shape";

		std::vector<glm::vec2> vertexList;
		vertexList.push_back(ptStartPos);
		vertexList.push_back(ptDragPos);

		if(m_eEditModeType == EDITMODETYPE_PrimitiveLineChain || m_eEditModeType == EDITMODETYPE_FixtureChain)
			static_cast<HyChain2d *>(m_FixtureList[0])->SetData(vertexList, false);
		else
			static_cast<HyShape2d *>(m_FixtureList[0])->SetAsPolygon(vertexList);
	}
	else if(m_eEditModeType == EDITMODETYPE_FixturePoint)
	{
		ClearFixtures();
		m_GrabPointList.clear();
		m_GrabPointCenter.Set(GRABPOINT_Center, ptCenter);
	}
	else
	{
		if(m_FixtureList.empty())
			m_FixtureList.push_back(new HyShape2d());

		switch(m_eShapeType)
		{
		case SHAPE_None:
			break;
			//m_eShapeType = SHAPE_Box;
			//[[fallthrough]];
		case SHAPE_Box: {
			static_cast<HyShape2d *>(m_FixtureList[0])->SetAsBox(HyRect((ptUpperBound.x - ptLowerBound.x) * 0.5f, (ptUpperBound.y - ptLowerBound.y) * 0.5f, ptCenter, 0.0f));
			m_GrabPointList.clear();
			std::vector<glm::vec2> fixtureGrabPoints = m_FixtureList[0]->CalcGrabPoints();

			std::vector<glm::vec2> boxGrabPoints;
			boxGrabPoints.reserve(8);
			boxGrabPoints.emplace_back(fixtureGrabPoints[0]);
			boxGrabPoints.emplace_back((fixtureGrabPoints[0] + fixtureGrabPoints[1]) * 0.5f);
			boxGrabPoints.emplace_back(fixtureGrabPoints[1]);
			boxGrabPoints.emplace_back((fixtureGrabPoints[1] + fixtureGrabPoints[2]) * 0.5f);
			boxGrabPoints.emplace_back(fixtureGrabPoints[2]);
			boxGrabPoints.emplace_back((fixtureGrabPoints[2] + fixtureGrabPoints[3]) * 0.5f);
			boxGrabPoints.emplace_back(fixtureGrabPoints[3]);
			boxGrabPoints.emplace_back((fixtureGrabPoints[3] + fixtureGrabPoints[0]) * 0.5f);
			for(glm::vec2 &ptGrab : boxGrabPoints)
				m_GrabPointList.append(GfxGrabPointModel(GRABPOINT_Endpoint, ptGrab));
			break; }

		case SHAPE_Circle: {
			static_cast<HyShape2d *>(m_FixtureList[0])->SetAsCircle(ptCenter, glm::distance(ptCenter, ptUpperBound));
			m_GrabPointList.clear();
			std::vector<glm::vec2> lineGrabPoints = m_FixtureList[0]->CalcGrabPoints();
			for(glm::vec2 &ptGrab : lineGrabPoints)
				m_GrabPointList.append(GfxGrabPointModel(GRABPOINT_Endpoint, ptGrab));
			break; }

		case SHAPE_Capsule: {
			glm::vec2 pt1, pt2;
			pt1.x = ptCenter.x;
			pt1.y = ptLowerBound.y;
			pt2.x = ptCenter.x;
			pt2.y = ptUpperBound.y;
			float fRadius = 0.5f * glm::distance(pt1, pt2);
			static_cast<HyShape2d *>(m_FixtureList[0])->SetAsCapsule(ptStartPos, ptDragPos, fRadius);
			m_GrabPointList.clear();
			std::vector<glm::vec2> lineGrabPoints = m_FixtureList[0]->CalcGrabPoints();
			for(glm::vec2 &ptGrab : lineGrabPoints)
				m_GrabPointList.append(GfxGrabPointModel(GRABPOINT_Endpoint, ptGrab));
			break; }

		case SHAPE_LineSegment: {
			m_fOutline = 1.0f;
			static_cast<HyShape2d *>(m_FixtureList[0])->SetAsLineSegment(ptStartPos, ptDragPos);
			m_GrabPointList.clear();
			std::vector<glm::vec2> lineGrabPoints = m_FixtureList[0]->CalcGrabPoints();
			for(glm::vec2 &ptGrab : lineGrabPoints)
				m_GrabPointList.append(GfxGrabPointModel(GRABPOINT_Endpoint, ptGrab));
			break; }

		case SHAPE_Polygon:
			HyGuiLog("VectorModel::DoTransformCreation - Handled with Line Chain", LOGTYPE_Error);
			break;

		default:
			HyGuiLog("GfxShapeModel::DoTransformCreation - Initial transform called with unsupported shape type: " % QString::number(m_eShapeType), LOGTYPE_Error);
			break;
		}
	}
}

void VectorModel::ClearFixtures()
{
	for(IHyFixture2d *pFixture : m_FixtureList)
		delete pFixture;
	m_FixtureList.clear();
}

std::vector<float> VectorModel::SerializeData() const
{
	if(m_eEditModeType == EDITMODETYPE_FixturePoint)
	{
		std::vector<float> returnList;
		glm::vec2 ptPos = m_GrabPointCenter.GetPos();
		returnList.push_back(ptPos.x);
		returnList.push_back(ptPos.y);

		return returnList;
	}

	if(m_FixtureList.empty())
		return std::vector<float>();
	if((m_eEditModeType != EDITMODETYPE_PrimitiveLineChain && m_eEditModeType != EDITMODETYPE_FixtureChain) &&
		m_eShapeType != SHAPE_Polygon)
	{
		return m_FixtureList[0]->SerializeSelf();
	}

	// Line Chain or SHAPE_Polygon
	std::vector<float> returnList;
	for(const GfxGrabPointModel &grabPt : m_GrabPointList)
	{
		glm::vec2 ptVertex = grabPt.GetPos();
		returnList.push_back(ptVertex.x);
		returnList.push_back(ptVertex.y);
	}
	returnList.push_back(m_bLoopClosed ? 1.0f : 0.0f); // Final float indicates whether loop is closed
	return returnList;
}

QString VectorModel::DeserializeData(const QJsonObject &serializedObj)
{
	m_bSelfIntersecting = false;
	HySetVec(m_ptSelfIntersection, 0.0f, 0.0f);

	QString sType = serializedObj["type"].toString();
	if(sType.isEmpty())
		return "No data provided";

	if(sType == HYLINECHAIN_Name)
	{
		if(IsFixture())
			m_eEditModeType = EDITMODETYPE_FixtureChain;
		else
			m_eEditModeType = EDITMODETYPE_PrimitiveLineChain;
		m_eShapeType = SHAPE_None;
	}
	else if(sType == HYPOINT_Name)
	{
		m_eEditModeType = EDITMODETYPE_FixturePoint;
		m_eShapeType = SHAPE_None;
	}
	else
	{
		if(IsFixture())
			m_eEditModeType = EDITMODETYPE_FixtureShape;
		else
			m_eEditModeType = EDITMODETYPE_PrimitiveShape;
		m_eShapeType = HyGlobal::GetShapeFromString(sType);
	}

	QJsonArray dataArray = serializedObj["data"].toArray();
	std::vector<float> floatList;
	for(const QJsonValue &val : dataArray)
		floatList.push_back(static_cast<float>(val.toDouble()));

	m_fOutline = static_cast<float>(serializedObj["outline"].toDouble());

	if(floatList.empty())
	{
		ClearFixtures();
		m_GrabPointList.clear();
		return "No data provided";
	}

	HyFixtureType eFixtureType = HYFIXTURE_Nothing;
	if(IsLineChain())
	{
		if(m_FixtureList.size() > 1 || (m_FixtureList.size() == 1 && m_FixtureList[0]->GetType() != HYFIXTURE_LineChain))
			ClearFixtures();

		if(m_FixtureList.empty())
			m_FixtureList.push_back(new HyChain2d());

		eFixtureType = HYFIXTURE_LineChain;
	}
	else if(m_eEditModeType == EDITMODETYPE_FixturePoint)
	{
		ClearFixtures();
	}
	else
	{
		if(m_FixtureList.size() == 1 && m_FixtureList[0]->GetType() == HYFIXTURE_LineChain)
			ClearFixtures();
		if(m_FixtureList.empty())
			m_FixtureList.push_back(new HyShape2d());
		
		eFixtureType = HyGlobal::ConvertShapeToFixtureType(m_eShapeType);
	}

	if(m_FixtureList.empty() == false)
	{
		HyPrimitive2d *pHyPrimNode = GetHyNode();
		if(pHyPrimNode)
			pHyPrimNode->SetAsFixture(m_iPrimLayerIndex, *m_FixtureList[0], m_fOutline);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Acquire `m_GrabPointList` and `m_GrabPointCenter`
	std::vector<glm::vec2> grabPointList;
	glm::vec2 ptCentroid;

	if(IsLineChain() == false && m_eShapeType == SHAPE_Polygon)
	{
		// SHAPE_Polygon deserialization
		if((floatList.size() & 1) == 0)
			HyGuiLog("VectorModel::DeserializeData for polygon had an even number of floats (final, odd float indcates loop)", LOGTYPE_Error);
		else
		{
			int iNumVertFloats = static_cast<int>(floatList.size()) - 1;
			grabPointList.reserve(iNumVertFloats / 2);
			for(int i = 0; i < iNumVertFloats; i += 2)
				grabPointList.emplace_back(glm::vec2(floatList[i], floatList[i + 1]));

			m_bLoopClosed = floatList.back() != 0.0f;
		}

		ptCentroid = HyMath::CalculateCentroid(grabPointList);
	}
	else if(m_eEditModeType == EDITMODETYPE_FixturePoint)
	{
		if(floatList.size() != 2)
			HyGuiLog("VectorModel::DeserializeData did not have 2 serialized floats for a fixture point", LOGTYPE_Error);
		else
		{
			grabPointList.reserve(1);
			grabPointList.emplace_back(glm::vec2(floatList[0], floatList[1]));

			m_bLoopClosed = false;
		}
	}
	else
	{
		if(m_FixtureList.empty())
			return "Fixture list is empty";

		QString sResult = m_FixtureList[0]->DeserializeSelf(eFixtureType, floatList).c_str();
		if(sResult.isEmpty() == false)
			return sResult;

		if(m_eShapeType == SHAPE_Box)
		{
			std::vector<glm::vec2> ptCorners = m_FixtureList[0]->CalcGrabPoints();

			grabPointList.reserve(8);
			grabPointList.emplace_back(ptCorners[0]);
			grabPointList.emplace_back((ptCorners[0] + ptCorners[1]) * 0.5f);
			grabPointList.emplace_back(ptCorners[1]);
			grabPointList.emplace_back((ptCorners[1] + ptCorners[2]) * 0.5f);
			grabPointList.emplace_back(ptCorners[2]);
			grabPointList.emplace_back((ptCorners[2] + ptCorners[3]) * 0.5f);
			grabPointList.emplace_back(ptCorners[3]);
			grabPointList.emplace_back((ptCorners[3] + ptCorners[0]) * 0.5f);
		}
		else
			grabPointList = m_FixtureList[0]->CalcGrabPoints();

		m_FixtureList[0]->GetCentroid(ptCentroid);
		m_bLoopClosed = floatList.back() != 0.0f;
	}

	if(grabPointList.empty())
	{
		m_GrabPointList.clear();
		return "Failed to calculate grab points";
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Assemble `m_GrabPointList` and `m_GrabPointCenter`
	m_GrabPointCenter.SetPos(ptCentroid);

	if(IsLineChain() || m_eShapeType == SHAPE_Polygon)
	{
		// Preserve existing grab points where possible (keeps selection)
		for(int i = 0; i < grabPointList.size(); ++i)
		{
			if(static_cast<int>(m_GrabPointList.size()) - 1 < i)
				m_GrabPointList.push_back(GfxGrabPointModel(GRABPOINT_Vertex, grabPointList[i]));
			else
				m_GrabPointList[i].Set(m_GrabPointList[i].IsSelected() ? GRABPOINT_VertexSelected : GRABPOINT_Vertex, grabPointList[i]);
		}
		if(static_cast<int>(m_GrabPointList.size()) > static_cast<int>(grabPointList.size())) // Truncate to new size
			m_GrabPointList.resize(grabPointList.size());

		if(m_bLoopClosed == false && m_GrabPointList.size() > 1)
		{
			m_GrabPointList.front().SetType(m_GrabPointList.front().IsSelected() ? GRABPOINT_EndpointSelected : GRABPOINT_Endpoint);
			m_GrabPointList.back().SetType(m_GrabPointList.back().IsSelected() ? GRABPOINT_EndpointSelected : GRABPOINT_Endpoint);
		}
	}
	else if(m_eEditModeType == EDITMODETYPE_FixturePoint)
	{
		if(grabPointList.size() == 1)
		{
			m_GrabPointList.clear();
			m_GrabPointCenter.Set(GRABPOINT_Center, grabPointList[0]);

			return QString();
		}
	}
	else
	{
		switch(m_eShapeType)
		{
		case SHAPE_None:
			m_GrabPointList.clear();
			break;

		case SHAPE_Box:
			if(grabPointList.size() != 8)
				return "Invalid box shape data";

			m_GrabPointList.resize(8);
			m_GrabPointList[0].Set(GRABPOINT_ShapeCtrlDiagUp, grabPointList[0]);
			m_GrabPointList[1].Set(GRABPOINT_ShapeCtrlVert, grabPointList[1]);
			m_GrabPointList[2].Set(GRABPOINT_ShapeCtrlDiagDown, grabPointList[2]);
			m_GrabPointList[3].Set(GRABPOINT_ShapeCtrlHorz, grabPointList[3]);
			m_GrabPointList[4].Set(GRABPOINT_ShapeCtrlDiagUp, grabPointList[4]);
			m_GrabPointList[5].Set(GRABPOINT_ShapeCtrlVert, grabPointList[5]);
			m_GrabPointList[6].Set(GRABPOINT_ShapeCtrlDiagDown, grabPointList[6]);
			m_GrabPointList[7].Set(GRABPOINT_ShapeCtrlHorz, grabPointList[7]);
			break;

		case SHAPE_Circle:
			if(grabPointList.size() != 4)
				return "Invalid circle shape data";

			m_GrabPointList.resize(4);
			m_GrabPointList[0].Set(GRABPOINT_ShapeCtrlVert, grabPointList[0]); // Top
			m_GrabPointList[1].Set(GRABPOINT_ShapeCtrlHorz, grabPointList[1]); // Right
			m_GrabPointList[2].Set(GRABPOINT_ShapeCtrlVert, grabPointList[2]); // Bottom
			m_GrabPointList[3].Set(GRABPOINT_ShapeCtrlHorz, grabPointList[3]); // Left
			break;

		case SHAPE_LineSegment:
			if(grabPointList.size() != 2)
				return "Invalid line segment shape data";

			m_GrabPointList.resize(2);
			m_GrabPointList[0].Set(GRABPOINT_ShapeCtrlAll, grabPointList[0]);
			m_GrabPointList[1].Set(GRABPOINT_ShapeCtrlAll, grabPointList[1]);
			break;

		case SHAPE_Polygon:
			HyGuiLog("VectorModel::DeserializeData - Polygon shape type should have been handled by line chain deserialization!", LOGTYPE_Error);
			break;

		case SHAPE_Capsule:
			if(grabPointList.size() != 6)
				return "Invalid capsule shape data";

			m_GrabPointList.resize(6);
			m_GrabPointList[0].Set(GRABPOINT_ShapeCtrlVert, grabPointList[0]); // Pt1
			m_GrabPointList[1].Set(GRABPOINT_ShapeCtrlVert, grabPointList[1]); // Pt2
			m_GrabPointList[2].Set(GRABPOINT_ShapeCtrlHorz, grabPointList[2]); // Center1 Right
			m_GrabPointList[3].Set(GRABPOINT_ShapeCtrlHorz, grabPointList[3]); // Center1 Left
			m_GrabPointList[4].Set(GRABPOINT_ShapeCtrlHorz, grabPointList[4]); // Center2 Right
			m_GrabPointList[5].Set(GRABPOINT_ShapeCtrlHorz, grabPointList[5]); // Center2 Left
			break;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	if(m_GrabPointList.empty() &&
		((m_eEditModeType == EDITMODETYPE_PrimitiveLineChain || m_eEditModeType == EDITMODETYPE_FixtureChain || m_eEditModeType == EDITMODETYPE_FixturePoint) || m_eShapeType != SHAPE_None))
	{
		return "Grab points not provided";
	}

	if((m_eEditModeType == EDITMODETYPE_PrimitiveLineChain || m_eEditModeType == EDITMODETYPE_FixtureChain) ||
		m_eShapeType == SHAPE_Polygon)
	{
		std::vector<glm::vec2> vertexList;
		vertexList.reserve(m_GrabPointList.size());
		for(const GfxGrabPointModel &grabPt : m_GrabPointList)
			vertexList.emplace_back(grabPt.GetPos());

		// Validate no self-intersections
		glm::vec2 pt1, pt2, pt3, pt4;
		for(int i = 0; i < vertexList.size(); ++i)
		{
			const glm::vec2 &pt1 = vertexList[i];
			const glm::vec2 &pt2 = vertexList[(i + 1) % vertexList.size()];
			for(int j = i + 2; j < vertexList.size(); ++j)
			{
				// Don't check adjacent edges
				if(j == (i + 1) % vertexList.size() || (i == 0 && j == vertexList.size() - 1))
					continue;

				const glm::vec2 &pt3 = vertexList[j];
				const glm::vec2 &pt4 = vertexList[(j + 1) % vertexList.size()];
				m_bSelfIntersecting = HyMath::TestSegmentsOverlap(pt1, pt2, pt3, pt4, m_ptSelfIntersection);
				if(m_bSelfIntersecting)
					return "Has self-intersecting edges";
			}
		}

		if(m_eShapeType == SHAPE_Polygon) // Assemble `m_FixtureList` with valid sub-polygons (convex and <= 8 vertices)
		{
			// Ensure counter-clockwise winding on `vertexList`
			float fArea = 0.0f;
			for(int i = 0; i < vertexList.size(); ++i)
			{
				const glm::vec2 &ptCurrent = vertexList[i];
				const glm::vec2 &ptNext = vertexList[(i + 1) % m_GrabPointList.size()];
				fArea += (ptNext.x - ptCurrent.x) * (ptNext.y + ptCurrent.y);
			}
			if(fArea > 0.0f) // Clockwise winding, needs reverse
			{
				std::reverse(vertexList.begin(), vertexList.end());
				std::reverse(m_GrabPointList.begin(), m_GrabPointList.end());
			}

			// Decompose into convex polygons with <= 8 vertices
			if(vertexList.size() <= 8 && HyMath::IsConvexPolygon(vertexList))
				AssemblePolygonFixtures({ vertexList });
			else
				AssemblePolygonFixtures(MergeTriangles(HyMath::Triangulate(vertexList)));
		}
	}

	return QString();
}

bool VectorModel::CheckIfAddVertexOnEdge()
{
	if((m_eEditModeType != EDITMODETYPE_PrimitiveLineChain && m_eEditModeType != EDITMODETYPE_FixtureChain) &&
		m_eShapeType != SHAPE_Polygon)
	{
		HyGuiLog("VectorModel::CheckIfAddVertexOnEdge invoked with shape that isn't a linechain or polygon", LOGTYPE_Error);
		return false;
	}
	if(m_GrabPointList.size() < 2)
		return false;

	HyShape2d tmpEdgeShape;
	for(int i = 0; i < m_GrabPointList.size(); ++i)
	{
		if(i == (m_GrabPointList.size() - 1) && m_bLoopClosed == false)
			break;

		// TODO: Test whether to convert all this to camera space
		glm::vec2 pt1 = m_GrabPointList[i].GetPos();
		glm::vec2 pt2 = m_GrabPointList[(i + 1) % m_GrabPointList.size()].GetPos();

		glm::vec2 ptWorldMousePos;
		if(HyEngine::Input().GetWorldMousePos(ptWorldMousePos) == false)
			return false;

		tmpEdgeShape.SetAsLineSegment(pt1, pt2);
		if(tmpEdgeShape.TestPoint(ptWorldMousePos, glm::identity<glm::mat4>()))
		{
			m_iGrabPointIndex = i + 1;
			return true;
		}
	}

	return false;
}

void VectorModel::AssemblePolygonFixtures(std::vector<std::vector<glm::vec2>> subPolygonList)
{
	ClearFixtures();
	for(std::vector<glm::vec2> &subPoly : subPolygonList)
	{
		HyShape2d *pShape2d = new HyShape2d();
		pShape2d->SetAsPolygon(subPoly);
		m_FixtureList.push_back(pShape2d);
	}
}

std::vector<glm::vec2> VectorModel::MergePolygons(const std::vector<glm::vec2> &ptA, const std::vector<glm::vec2> &ptB, int a0, int a1, int b0, int b1)
{
	std::vector<glm::vec2> out;

	// ptA: from a1 -> a0
	for(int i = a1; i != a0; i = (i + 1) % ptA.size())
		out.push_back(ptA[i]);
	out.push_back(ptA[a0]);

	// ptB: from b1 -> b0
	for(int i = b1; i != b0; i = (i + 1) % ptB.size())
		out.push_back(ptB[i]);
	out.push_back(ptB[b0]);

	return out;
}

std::vector<std::vector<glm::vec2>> VectorModel::MergeTriangles(const std::vector<HyTriangle2d> &triangleList)
{
	std::vector<std::vector<glm::vec2>> polys;
	for(const HyTriangle2d &t : triangleList)
		polys.push_back({ t.m_ptA, t.m_ptB, t.m_ptC });

	bool bMerged;
	do
	{
		bMerged = false;

		for(size_t i = 0; i < polys.size() && !bMerged; ++i)
		{
			for(size_t j = i + 1; j < polys.size(); ++j)
			{
				int a0, a1, b0, b1;
				if(!IsShareEdge(polys[i], polys[j], a0, a1, b0, b1))
					continue;

				std::vector<glm::vec2> mergedPoly = MergePolygons(polys[i], polys[j], a0, a1, b0, b1);
				if(mergedPoly.size() <= 8 && HyMath::IsConvexPolygon(mergedPoly))
				{
					polys[i] = mergedPoly;
					polys.erase(polys.begin() + j);
					bMerged = true;
					break;
				}
			}
		}
	} while(bMerged);

	return polys;
}

bool VectorModel::IsShareEdge(const std::vector<glm::vec2> &a, const std::vector<glm::vec2> &b, int &a0, int &a1, int &b0, int &b1)
{
	for(int i = 0; i < (int)a.size(); ++i)
	{
		int ni = (i + 1) % a.size();
		for(int j = 0; j < (int)b.size(); ++j)
		{
			int nj = (j + 1) % b.size();

			if(a[i] == b[nj] && a[ni] == b[j])
			{
				a0 = i; a1 = ni;
				b0 = j; b1 = nj;
				return true;
			}
		}
	}
	return false;
}
