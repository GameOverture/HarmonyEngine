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

VectorModel::VectorModel(EditModeType eEditModeType, HyColor color) :
	IEditModeModel(eEditModeType),
	m_Color(color),
	m_iDisplayOrder(0),
	m_eShapeType(SHAPE_None),
	m_bLoopClosed(false),
	m_fOutline(0.0f),
	m_GrabPointCenter(GRABPOINT_Center),
	m_bSelfIntersecting(false),
	m_ptSelfIntersection(0.0f, 0.0f),
	m_sMalformedReason("No Data"),
	m_eCurAction(VECTORACTION_None),
	m_vDragDelta(0.0f, 0.0f),
	m_iGrabPointIndex(-1),
	m_ptGrabPointPos(0.0f, 0.0f)
{
	ClearAction();
}

/*virtual*/ VectorModel::~VectorModel()
{
	ClearFixtures();
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
	SyncViews(bEnabled ? EDITMODE_Idle : EDITMODE_Off);
}

/*virtual*/ Qt::CursorShape VectorModel::MouseMoveIdle() /*override*/
{
	VectorAction eResult = DoMouseMoveIdle();

	switch(eResult)
	{
	case VECTORACTION_Creation:
		MainWindow::SetStatus("Edit Mode - Creation", 0);
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
			return m_GrabPointList[m_iGrabPointIndex].GetHoverCursor();
		}

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

/*virtual*/ bool VectorModel::MousePressEvent(EditModeState eEditModeState, bool bShiftHeld) /*override*/
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
		m_GrabPointList.insert(m_GrabPointList.begin() + m_iGrabPointIndex, GfxGrabPointModel(GRABPOINT_VertexSelected, m_ptGrabPointPos));
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

		if(bShiftHeld)
		{
			m_GrabPointList[m_iGrabPointIndex].SetSelected(!m_GrabPointList[m_iGrabPointIndex].IsSelected());
		}
		else
		{
			for(GfxGrabPointModel &grabPtRef : m_GrabPointList)
				grabPtRef.SetCachePos();

			if(m_GrabPointList[m_iGrabPointIndex].IsSelected() == false)
			{
				for(int i = 0; i < m_GrabPointList.size(); ++i)
					m_GrabPointList[i].SetSelected(false);
				m_GrabPointList[m_iGrabPointIndex].SetSelected(true);
			}

			bStartTransform = true;
		}
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

/*virtual*/ void VectorModel::MouseTransform(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos) /*override*/
{
	HySetVec(m_vDragDelta, 0.0f, 0.0f);
	if(false == (HyCompareFloat(ptStartPos.x, ptDragPos.y) && HyCompareFloat(ptStartPos.y, ptDragPos.y)))
		HySetVec(m_vDragDelta, ptDragPos.x - ptStartPos.x, ptDragPos.y - ptStartPos.y);

	if(m_eCurAction == VECTORACTION_Creation)
		DoTransformCreation(bShiftMod, ptStartPos, ptDragPos);

	if(m_eCurAction == VECTORACTION_AppendVertex)
		m_GrabPointList[m_iGrabPointIndex].SetPos(ptDragPos);

	if(m_eCurAction == VECTORACTION_HoverGrabPoint)
	{
		for(GfxGrabPointModel &grabPtModel : m_GrabPointList)
		{
			if(grabPtModel.IsSelected())
			{
				glm::vec2 ptNewPos = grabPtModel.GetCachePos() + m_vDragDelta;
				if(bShiftMod) // Snap to 25 pixel increments if shift is held
					HySetVec(ptNewPos, roundf(ptNewPos.x / 25.0f) * 25.0f, roundf(ptNewPos.y / 25.0f) * 25.0f);
				grabPtModel.SetPos(ptNewPos);
			}
		}
	}
	if(m_eCurAction == VECTORACTION_HoverCenter)
	{
		for(GfxGrabPointModel &grabPtModel : m_GrabPointList)
		{
			glm::vec2 ptNewPos = grabPtModel.GetCachePos() + m_vDragDelta;
			if(bShiftMod) // Snap to 25 pixel increments if shift is held
				HySetVec(ptNewPos, roundf(ptNewPos.x / 25.0f) * 25.0f, roundf(ptNewPos.y / 25.0f) * 25.0f);
			grabPtModel.SetPos(ptNewPos);
		}

		glm::vec2 ptNewPos = m_GrabPointCenter.GetCachePos() + m_vDragDelta;
		if(bShiftMod) // Snap to 25 pixel increments if shift is held
			HySetVec(ptNewPos, roundf(ptNewPos.x / 25.0f) * 25.0f, roundf(ptNewPos.y / 25.0f) * 25.0f);
		m_GrabPointCenter.SetPos(ptNewPos);
	}

	SyncViews(EDITMODE_MouseDragTransform);
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

/*virtual*/ void VectorModel::MouseClickTransformReleased(glm::vec2 ptClickPos) /*override*/
{
	if(m_eCurAction == VECTORACTION_Creation)
	{
		glm::vec2 ptStartPos = ptClickPos + glm::vec2(-25.0f, -25.0f);
		glm::vec2 ptDragPos = ptClickPos + glm::vec2(25.0f, 25.0f);
		DoTransformCreation(false, ptStartPos, ptDragPos);
	}
	else if(m_eCurAction == VECTORACTION_HoverGrabPoint)
	{
		for(int i = 0; i < m_GrabPointList.size(); ++i)
			m_GrabPointList[i].SetSelected(false);
		m_GrabPointList[m_iGrabPointIndex].SetSelected(true);
	}
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
	SyncViews(EDITMODE_Idle);
}

bool VectorModel::IsVisible() const
{
	return m_bVisible;
}

void VectorModel::SetVisible(bool bVisible)
{
	m_bVisible = bVisible;
	SyncViews(EDITMODE_Idle);
}

float VectorModel::GetAlpha() const
{
	return m_fAlpha;
}

void VectorModel::SetAlpha(float fAlpha)
{
	m_fAlpha = fAlpha;
	SyncViews(EDITMODE_Idle);
}

int VectorModel::GetDisplayOrder() const
{
	return m_iDisplayOrder;
}

void VectorModel::SetDisplayOrder(int iDisplayOrder)
{
	m_iDisplayOrder = iDisplayOrder;
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
	serializedObj.insert("outline", m_fOutline);
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

glm::vec2 VectorModel::GetDragDelta() const
{
	return m_vDragDelta;
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
	m_vDragDelta = glm::vec2(0.0f, 0.0f);
	m_iGrabPointIndex = -1;
	m_ptGrabPointPos = glm::vec2(0.0f, 0.0f);
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
		
		b2AABB boundingBox;
		HyMath::ComputeAABB(boundingBox, vertexList.data(), static_cast<int>(vertexList.size()), 5.0f);

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
			b2AABB boundingBox;
			HyMath::ComputeAABB(boundingBox, vertexList.data(), static_cast<int>(vertexList.size()), 5.0f);
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
			b2AABB boundingBox;
			HyMath::ComputeAABB(boundingBox, vertexList.data(), static_cast<int>(vertexList.size()), 5.0f);
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
			b2AABB boundingBox;
			HyMath::ComputeAABB(boundingBox, vertexList.data(), static_cast<int>(vertexList.size()), 5.0f);
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
			convertedDataList.push_back(pt1.x - vPerp.x * fRadius);
			convertedDataList.push_back(pt1.y - vPerp.y * fRadius);
			convertedDataList.push_back(pt2.x - vPerp.x * fRadius);
			convertedDataList.push_back(pt2.y - vPerp.y * fRadius);
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

VectorAction VectorModel::DoMouseMoveIdle()
{
	m_iGrabPointIndex = -1;
	m_ptGrabPointPos = glm::vec2(0.0f, 0.0f);

	for(int i = 0; i < m_GrabPointList.size(); ++i)
	{
		if(m_GrabPointList[i].IsMouseHover())
		{
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

			m_iGrabPointIndex = i;
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
					if(HyEngine::Input().GetWorldMousePos(m_ptGrabPointPos) == false)
						HyGuiLog("GfxShapeModel::DoMouseMoveIdle - Failed to get world mouse position for append vertex", LOGTYPE_Error);
					
					return VECTORACTION_AppendVertex;
				}
				else if(m_GrabPointList.back().IsSelected())
				{
					m_iGrabPointIndex = m_GrabPointList.size();
					if(HyEngine::Input().GetWorldMousePos(m_ptGrabPointPos) == false)
						HyGuiLog("GfxShapeModel::DoMouseMoveIdle - Failed to get world mouse position for append vertex", LOGTYPE_Error);

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
		for(IHyFixture2d *pFixture : m_FixtureList)
		{
			glm::vec2 ptWorldMousePos;
			if(HyEngine::Input().GetWorldMousePos(ptWorldMousePos) == false)
				HyGuiLog("GfxShapeModel::DoMouseMoveIdle - Failed to get world mouse position for hit testing", LOGTYPE_Error);

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

void VectorModel::DoTransformCreation(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos)
{
	glm::vec2 ptLowerBound, ptUpperBound, ptCenter;
	if(bShiftMod)
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
			m_eShapeType = SHAPE_Box;
			[[fallthrough]];
		case SHAPE_Box: {
			static_cast<HyShape2d *>(m_FixtureList[0])->SetAsBox(HyRect((ptUpperBound.x - ptLowerBound.x) * 0.5f, (ptUpperBound.y - ptLowerBound.y) * 0.5f, ptCenter, 0.0f));
			m_GrabPointList.clear();
			std::vector<glm::vec2> lineGrabPoints = m_FixtureList[0]->CalcGrabPoints();
			for(glm::vec2 &ptGrab : lineGrabPoints)
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
	if(m_eEditModeType == EDITMODETYPE_PrimitiveLineChain || m_eEditModeType == EDITMODETYPE_FixtureChain)
	{
		if(m_FixtureList.size() > 1)
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

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Acquire `m_GrabPointList` and `m_GrabPointCenter`
	std::vector<glm::vec2> grabPointList;
	glm::vec2 ptCentroid;

	if((m_eEditModeType != EDITMODETYPE_PrimitiveLineChain && m_eEditModeType != EDITMODETYPE_FixtureChain) &&
		m_eShapeType == SHAPE_Polygon)
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
		QString sResult = m_FixtureList[0]->DeserializeSelf(eFixtureType, floatList).c_str();
		if(sResult.isEmpty() == false)
			return sResult;

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

	if((m_eEditModeType == EDITMODETYPE_PrimitiveLineChain || m_eEditModeType == EDITMODETYPE_FixtureChain) ||
		m_eShapeType == SHAPE_Polygon)
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
			if(grabPointList.size() != 4)
				return "Invalid box shape data";

			m_GrabPointList.resize(4);
			for(int i = 0; i < 4; ++i)
				m_GrabPointList[i].Set(GRABPOINT_ShapeCtrlAll, grabPointList[i]);
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
		{
			HyGuiLog("VectorModel::CheckIfAddVertexOnEdge - Failed to get world mouse position for hit testing", LOGTYPE_Error);
			return false;
		}

		tmpEdgeShape.SetAsLineSegment(pt1, pt2);
		if(tmpEdgeShape.TestPoint(ptWorldMousePos, glm::identity<glm::mat4>()))
		{
			m_iGrabPointIndex = i + 1;
			m_ptGrabPointPos = HyMath::ClosestPointOnSegment(pt1, pt2, ptWorldMousePos);
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
