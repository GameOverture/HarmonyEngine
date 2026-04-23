/**************************************************************************
*	IGfxEditModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "IGfxEditModel.h"
#include "IGfxEditView.h"
#include "MainWindow.h"

IGfxEditModel::IGfxEditModel() :
	m_bIsLineChain(false),
	m_eShapeType(SHAPE_None),
	m_bLoopClosed(false),
	m_fOutline(0.0f),
	m_GrabPointCenter(GRABPOINT_Center),
	m_bSelfIntersecting(false),
	m_ptSelfIntersection(0.0f, 0.0f),
	m_sMalformedReason("No Data"),
	m_eCurAction(EDITMODEACTION_None),
	m_vDragDelta(0.0f, 0.0f),
	m_iGrabPointIndex(-1),
	m_ptGrabPointPos(0.0f, 0.0f)
{
	ClearAction();
}

/*virtual*/ IGfxEditModel::~IGfxEditModel()
{
	ClearFixtures();
}

bool IGfxEditModel::IsLineChain() const
{
	return m_bIsLineChain;
}

EditorShape IGfxEditModel::GetShapeType() const
{
	return m_eShapeType;
}

void IGfxEditModel::ChangeToLineChain()
{
	m_bIsLineChain = true;
	m_eShapeType = SHAPE_None;

	std::vector<float> floatList = ConvertedPolygonOrLineChainData();

	// Reassemble model
	QJsonObject serializedObj;
	serializedObj.insert("type", HYLINECHAIN_Name);
	QJsonArray dataArray;
	for(float f : floatList)
		dataArray.push_back(f);
	serializedObj.insert("data", dataArray);
	serializedObj.insert("outline", m_fOutline);
	Deserialize(serializedObj);
}

void IGfxEditModel::ChangeToShape(EditorShape eNewShapeType)
{
	m_bIsLineChain = false;
	m_eShapeType = eNewShapeType;

	std::vector<float> floatList;
	switch(m_eShapeType)
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

	// Reassemble model
	QJsonObject serializedObj;
	serializedObj.insert("type", HyGlobal::ShapeName(m_eShapeType));
	QJsonArray dataArray;
	for(float f : floatList)
		dataArray.push_back(f);
	serializedObj.insert("data", dataArray);
	serializedObj.insert("outline", m_fOutline);
	Deserialize(serializedObj);
}

bool IGfxEditModel::IsLoopClosed() const
{
	return m_bLoopClosed;
}

float IGfxEditModel::GetOutline() const
{
	return m_fOutline;
}

bool IGfxEditModel::IsValidModel() const
{
	return m_sMalformedReason.isEmpty();
}

QJsonObject IGfxEditModel::Serialize() const
{
	QJsonObject serializedObj;
	if(m_bIsLineChain)
		serializedObj.insert("type", HYLINECHAIN_Name);
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

void IGfxEditModel::Deserialize(const QJsonObject &serializedObj)
{
	m_sMalformedReason = DeserializeData(serializedObj);
	SyncViews(EDITMODE_Idle, EDITMODEACTION_None);
}

void IGfxEditModel::AddView(IGfxEditView *pView)
{
	if(m_ViewList.contains(pView))
		return;
	m_ViewList.push_back(pView);
}

bool IGfxEditModel::RemoveView(IGfxEditView *pView)
{
	return m_ViewList.removeOne(pView);
}

void IGfxEditModel::SyncViews(EditModeState eEditModeState, EditModeAction eResult) const
{
	for(IGfxEditView *pView : m_ViewList)
		pView->SyncWithModel(eEditModeState, eResult);
}

int IGfxEditModel::GetNumFixtures() const
{
	return m_FixtureList.size();
}

const QList<GfxGrabPointModel> &IGfxEditModel::GetGrabPointList() const
{
	return m_GrabPointList;
}

const GfxGrabPointModel &IGfxEditModel::GetGrabPoint(int iIndex) const
{
	if(iIndex < 0 || iIndex >= m_GrabPointList.size())
	{
		HyGuiLog("IGfxEditModel::GetGrabPoint - Index out of range", LOGTYPE_Error);
		return m_GrabPointCenter;
	}
	return m_GrabPointList[iIndex];
}

const GfxGrabPointModel *IGfxEditModel::GetActiveGrabPoint() const
{
	if(m_iGrabPointIndex < 0 || m_iGrabPointIndex >= m_GrabPointList.size())
		return nullptr;

	return &m_GrabPointList[m_iGrabPointIndex];
}

const GfxGrabPointModel &IGfxEditModel::GetCenterGrabPoint() const
{
	return m_GrabPointCenter;
}

int IGfxEditModel::GetActiveGrabPointIndex() const
{
	return m_iGrabPointIndex;
}

int IGfxEditModel::GetNumGrabPointsSelected() const
{
	int iNumSelected = 0;
	for(const GfxGrabPointModel &grabPtModel : m_GrabPointList)
	{
		if(grabPtModel.IsSelected())
			++iNumSelected;
	}
	return iNumSelected;
}

bool IGfxEditModel::IsAllGrabPointsSelected() const
{
	for(const GfxGrabPointModel &grabPtModel : m_GrabPointList)
	{
		if(grabPtModel.IsSelected() == false)
			return false;
	}
	return true;
}

bool IGfxEditModel::IsHoverGrabPointSelected() const
{
	if(m_iGrabPointIndex < 0 || m_iGrabPointIndex >= m_GrabPointList.size())
		return false;

	return m_GrabPointList[m_iGrabPointIndex].IsSelected();
}

void IGfxEditModel::DeselectAllGrabPoints()
{
	for(GfxGrabPointModel &grabPtModel : m_GrabPointList)
		grabPtModel.SetSelected(false);
}

Qt::CursorShape IGfxEditModel::MouseMoveIdle()
{
	EditModeAction eResult = DoMouseMoveIdle();

	switch(eResult)
	{
	case EDITMODEACTION_Creation:
		MainWindow::SetStatus("Edit Mode - Creation", 0);
		return Qt::CrossCursor;

	case EDITMODEACTION_Outside:
		MainWindow::SetStatus("Edit Mode", 0);
		return Qt::ArrowCursor;

	case EDITMODEACTION_Inside:
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

	case EDITMODEACTION_AppendVertex:
		MainWindow::SetStatus("Edit Mode - Append Vertex", 0);
		return Qt::CrossCursor;

	case EDITMODEACTION_InsertVertex:
		MainWindow::SetStatus("Edit Mode - Insert Vertex", 0);
		return Qt::CrossCursor;

	case EDITMODEACTION_HoverGrabPoint:
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

	case EDITMODEACTION_HoverCenter:
		MainWindow::SetStatus("Edit Mode - Translate", 0);
		return Qt::SizeAllCursor;

	default:
		HyGuiLog("IGfxEditModel::MouseMoveIdle - unsupported edit mode action!", LOGTYPE_Error);
		break;
	}

	MainWindow::SetStatus("Edit Mode", 0);
	return Qt::ArrowCursor;
}

bool IGfxEditModel::MousePressEvent(EditModeState eEditModeState, bool bShiftHeld, Qt::MouseButtons uiButtonFlags)
{
	bool bStartTransform = false;

	EditModeAction eResult = DoMouseMoveIdle();
	if(eResult == EDITMODEACTION_AppendVertex || eResult == EDITMODEACTION_InsertVertex)
	{
		if(m_iGrabPointIndex == -1)
		{
			HyGuiLog("GfxShapeModel::MousePressEvent - Insert vertex index was -1 on AddVertex/Crosshair", LOGTYPE_Error);
			return EDITMODEACTION_None;
		}
		m_GrabPointList.insert(m_GrabPointList.begin() + m_iGrabPointIndex, GfxGrabPointModel(GRABPOINT_VertexSelected, m_ptGrabPointPos));
		for(int i = 0; i < m_GrabPointList.size(); ++i)
			m_GrabPointList[i].SetSelected(false);
		m_GrabPointList[m_iGrabPointIndex].SetSelected(true);

		bStartTransform = true;
	}
	else if(eResult == EDITMODEACTION_HoverGrabPoint)
	{
		if(m_iGrabPointIndex == -1)
		{
			HyGuiLog("GfxShapeModel::OnMousePressEvent - Hover vertex index was -1 on HoverVertex/HoverSelectedVertex", LOGTYPE_Error);
			return EDITMODEACTION_None;
		}

		if(bShiftHeld)
		{
			if(uiButtonFlags & Qt::LeftButton)
				m_GrabPointList[m_iGrabPointIndex].SetSelected(!m_GrabPointList[m_iGrabPointIndex].IsSelected());
			else if(uiButtonFlags & Qt::RightButton)
				m_GrabPointList[m_iGrabPointIndex].SetSelected(false);
		}
		else
		{
			if(uiButtonFlags & Qt::LeftButton)
			{
				for(int i = 0; i < m_GrabPointList.size(); ++i)
					m_GrabPointList[i].SetSelected(false);
				m_GrabPointList[m_iGrabPointIndex].SetSelected(true);
			}
			else if(uiButtonFlags & Qt::RightButton)
				m_GrabPointList[m_iGrabPointIndex].SetSelected(false);
		}

		bStartTransform = true;
	}
	else if(eResult == EDITMODEACTION_Creation)
		bStartTransform = true;
	else if(eResult == EDITMODEACTION_Inside)
		bStartTransform = IsAllGrabPointsSelected();

	m_eCurAction = eResult;

	return bStartTransform;
}

void IGfxEditModel::MouseMarqueeReleased(EditModeState eEditModeState, bool bLeftClick, QPointF ptBotLeft, QPointF ptTopRight)
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

	SyncViews(eEditModeState, EDITMODEACTION_None);
}

void IGfxEditModel::MouseTransform(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos)
{
	HySetVec(m_vDragDelta, 0.0f, 0.0f);
	if(false == (HyCompareFloat(ptStartPos.x, ptDragPos.y) && HyCompareFloat(ptStartPos.y, ptDragPos.y)))
		HySetVec(m_vDragDelta, ptDragPos.x - ptStartPos.x, ptDragPos.y - ptStartPos.y);

	if(m_eCurAction == EDITMODEACTION_Creation)
		DoTransformCreation(bShiftMod, ptStartPos, ptDragPos);

	if(m_eCurAction == EDITMODEACTION_AppendVertex)
		m_GrabPointList[m_iGrabPointIndex].SetPos(ptDragPos);

	SyncViews(EDITMODE_MouseDragTransform, m_eCurAction);
}

glm::vec2 IGfxEditModel::GetDragDelta() const
{
	return m_vDragDelta;
}

QString IGfxEditModel::GetActionText(QString sNodeCodeName) const
{
	QString sUndoText;
	switch(m_eCurAction)
	{
	case EDITMODEACTION_None:
	case EDITMODEACTION_Outside:
		break;
	case EDITMODEACTION_Creation:
		if(m_bIsLineChain)
			sUndoText = "Create new Line Chain " % sNodeCodeName;
		else
			sUndoText = "Create new " % HyGlobal::ShapeName(m_eShapeType) % " shape " % sNodeCodeName;
		break;
	case EDITMODEACTION_Inside:
	case EDITMODEACTION_HoverCenter:
		sUndoText = "Translate shape " % sNodeCodeName;
		break;
	case EDITMODEACTION_AppendVertex:
		sUndoText = "Append vertex on " % sNodeCodeName;
		break;
	case EDITMODEACTION_InsertVertex:
		sUndoText = "Insert vertex on " % sNodeCodeName;
		break;
	case EDITMODEACTION_HoverGrabPoint:
		if(m_bIsLineChain || m_eShapeType == SHAPE_Polygon || m_eShapeType == SHAPE_LineSegment)
			sUndoText = "Translate vert(s) on " % sNodeCodeName;
		else if(m_eShapeType == SHAPE_Circle)
			sUndoText = "Adjust circle radius on " % sNodeCodeName;
		else if(m_eShapeType == SHAPE_Box)
			sUndoText = "Adjust box size on " % sNodeCodeName;
		else if(m_eShapeType == SHAPE_Capsule)
			sUndoText = "Adjust capsule size on " % sNodeCodeName;
		else
			HyGuiLog("IGfxEditModel::MouseTransformReleased - Invalid shape type for EDITMODEACTION_HoverGrabPoint", LOGTYPE_Error);
		break;

	default:
		HyGuiLog("IGfxEditModel::MouseTransformReleased - Invalid m_eCurTransform", LOGTYPE_Error);
		break;
	}

	return sUndoText;
}

void IGfxEditModel::ClearAction()
{
	m_eCurAction = EDITMODEACTION_None;
	m_vDragDelta = glm::vec2(0.0f, 0.0f);
	m_iGrabPointIndex = -1;
	m_ptGrabPointPos = glm::vec2(0.0f, 0.0f);
}

std::vector<float> IGfxEditModel::ConvertedBoxData() const
{
	HyShape2d tmpBoxShape;
	if(m_bIsLineChain || m_eShapeType == SHAPE_Polygon)
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
		HyGuiLog("IGfxEditModel::ConvertedBoxData - Polygon handled with line chain.", LOGTYPE_Error);
		break;

	default:
		HyGuiLog("IGfxEditModel::ConvertedBoxData - Unhandled conversion to Box", LOGTYPE_Error);
		break;
	}
	
	return std::vector<float>();
}

std::vector<float> IGfxEditModel::ConvertedCircleData() const
{
	std::vector<float> convertedDataList(3);
	if(m_bIsLineChain || m_eShapeType == SHAPE_Polygon)
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
			HyGuiLog("IGfxEditModel::ConvertedCircleData - Polygon handled with line chain.", LOGTYPE_Error);
			break;
	
		default:
			HyGuiLog("GfxShapeModel::ConvertedCircleData - Unhandled conversion to Circle", LOGTYPE_Error);
			break;
		}
	}
	
	return convertedDataList;
}

std::vector<float> IGfxEditModel::ConvertedLineSegmentData() const
{
	std::vector<float> convertedDataList(4);
	if(m_bIsLineChain || m_eShapeType == SHAPE_Polygon)
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
			HyGuiLog("IGfxEditModel::ConvertedLineSegmentData - Polygon handled with line chain.", LOGTYPE_Error);
			break;

		default:
			HyGuiLog("GfxShapeModel::ConvertedLineSegmentData - Unhandled conversion to LineSegment", LOGTYPE_Error);
			break;
		}
	}
	
	return convertedDataList;
}

std::vector<float> IGfxEditModel::ConvertedCapsuleData() const
{
	std::vector<float> convertedDataList(5);

	if(m_bIsLineChain || m_eShapeType == SHAPE_Polygon || m_eShapeType == SHAPE_LineSegment)
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
			HyGuiLog("IGfxEditModel::ConvertedCapsuleData - Polygon and LineSegment handled with line chain.", LOGTYPE_Error);
			break;
		default:
			HyGuiLog("IGfxEditModel::ConvertedCapsuleData - Unhandled conversion to Capsule", LOGTYPE_Error);
			break;
		}
	}

	return convertedDataList;
}

std::vector<float> IGfxEditModel::ConvertedPolygonOrLineChainData() const
{
	std::vector<float> convertedDataList;


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

EditModeAction IGfxEditModel::DoMouseMoveIdle()
{
	m_iGrabPointIndex = -1;
	m_ptGrabPointPos = glm::vec2(0.0f, 0.0f);

	for(int i = 0; i < m_GrabPointList.size(); ++i)
	{
		if(m_GrabPointList[i].IsMouseHover())
		{
			m_iGrabPointIndex = i;
			return EDITMODEACTION_HoverGrabPoint;
		}
	}
	if(m_GrabPointCenter.IsMouseHover())
		return EDITMODEACTION_HoverCenter;

	if(m_bIsLineChain || m_eShapeType == SHAPE_Polygon)
	{
		if(m_GrabPointList.empty())
			return EDITMODEACTION_Creation;

		if(CheckIfAddVertexOnEdge())
			return EDITMODEACTION_InsertVertex;

		if(m_bLoopClosed == false)
		{
			// If only the first or last vertex in chain is selected, return EDITMODEACTION_AppendVertex to indicate appending verts to the selected end
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
					
					return EDITMODEACTION_AppendVertex;
				}
				else if(m_GrabPointList.back().IsSelected())
				{
					m_iGrabPointIndex = m_GrabPointList.size();
					if(HyEngine::Input().GetWorldMousePos(m_ptGrabPointPos) == false)
						HyGuiLog("GfxShapeModel::DoMouseMoveIdle - Failed to get world mouse position for append vertex", LOGTYPE_Error);

					return EDITMODEACTION_AppendVertex;
				}
			}
		}
	}
	else if(IsValidModel() == false) // Any shape besides SHAPE_Polygon
		return EDITMODEACTION_Creation;

	if(m_bIsLineChain == false)
	{
		for(IHyFixture2d *pFixture : m_FixtureList)
		{
			glm::vec2 ptWorldMousePos;
			if(HyEngine::Input().GetWorldMousePos(ptWorldMousePos) == false)
				HyGuiLog("GfxShapeModel::DoMouseMoveIdle - Failed to get world mouse position for hit testing", LOGTYPE_Error);

			if(pFixture->TestPoint(ptWorldMousePos, glm::identity<glm::mat4>()))
				return EDITMODEACTION_Inside;
		}
	}
	
	return EDITMODEACTION_Outside;
}

void IGfxEditModel::TransformData(glm::mat4 mtxTransform)
{
	for(IHyFixture2d *pFixture : m_FixtureList)
		pFixture->TransformSelf(mtxTransform);

	SyncViews(EDITMODE_Idle, EDITMODEACTION_None);
}

void IGfxEditModel::DoTransformCreation(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos)
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

	if(m_bIsLineChain || m_eShapeType == SHAPE_Polygon)
	{
		m_GrabPointList.clear();
		m_GrabPointList.append(GfxGrabPointModel(GRABPOINT_Endpoint, ptDragPos));
		m_GrabPointList.append(GfxGrabPointModel(GRABPOINT_Endpoint, ptDragPos));
		
		m_GrabPointList[0].SetSelected(false);
		m_GrabPointList[1].Set(GRABPOINT_EndpointSelected, ptDragPos);
		m_sMalformedReason = "Incomplete shape";
	}
	else
	{
		switch(m_eShapeType)
		{
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
			HyGuiLog("IGfxEditModel::DoTransformCreation - Handled with Line Chain", LOGTYPE_Error);
			break;

		default:
			HyGuiLog("GfxShapeModel::DoTransformCreation - Initial transform called with unsupported shape type: " % QString::number(m_eShapeType), LOGTYPE_Error);
			break;
		}
	}
}

void IGfxEditModel::ClearFixtures()
{
	for(IHyFixture2d *pFixture : m_FixtureList)
		delete pFixture;
	m_FixtureList.clear();
}

std::vector<float> IGfxEditModel::SerializeData() const
{
	if(m_FixtureList.empty())
		return std::vector<float>();
	if(m_eShapeType != SHAPE_Polygon)
		return m_FixtureList[0]->SerializeSelf();

	// SHAPE_Polygon
	std::vector<float> returnList;
	for(const GfxGrabPointModel &grabPt : m_GrabPointList)
	{
		glm::vec2 ptVertex = grabPt.GetPos();
		returnList.push_back(static_cast<float>(ptVertex.x));
		returnList.push_back(static_cast<float>(ptVertex.y));
	}
	returnList.push_back(m_bLoopClosed ? 1.0f : 0.0f); // Final float indicates whether loop is closed
	return returnList;
}

QString IGfxEditModel::DeserializeData(const QJsonObject &serializedObj)
{
	m_bSelfIntersecting = false;
	HySetVec(m_ptSelfIntersection, 0.0f, 0.0f);

	QString sType = serializedObj["type"].toString();
	if(sType == HYLINECHAIN_Name)
	{
		m_bIsLineChain = true;
		m_eShapeType = SHAPE_None;
	}
	else
	{
		m_bIsLineChain = false;
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
	if(m_bIsLineChain)
	{
		if(m_FixtureList.size() > 1)
			ClearFixtures();
		if(m_FixtureList.empty())
			m_FixtureList.push_back(new HyChain2d());

		eFixtureType = HYFIXTURE_LineChain;
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

	if(m_bIsLineChain == false && m_eShapeType == SHAPE_Polygon)
	{
		// SHAPE_Polygon deserialization
		if((floatList.size() & 1) == 0)
			HyGuiLog("GfxShapeModel::SetData for polygon had an even number of floats (final, odd float indcates loop)", LOGTYPE_Error);
		else
		{
			int iNumVertFloats = floatList.size() - 1;
			grabPointList.reserve(iNumVertFloats / 2);
			for(int i = 0; i < iNumVertFloats; i += 2)
				grabPointList.emplace_back(glm::vec2(floatList[i], floatList[i + 1]));

			m_bLoopClosed = floatList.back() != 0.0f;
		}

		ptCentroid = HyGlobal::CalculateCentroid(grabPointList);
	}
	else
	{
		QString sResult = m_FixtureList[0]->DeserializeSelf(eFixtureType, floatList).c_str();
		if(sResult.isEmpty() == false)
			return sResult;

		grabPointList = m_FixtureList[0]->CalcGrabPoints();
		m_FixtureList[0]->GetCentroid(ptCentroid);
	}

	if(grabPointList.empty())
	{
		m_GrabPointList.clear();
		return "Failed to calculate grab points";
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Assemble `m_GrabPointList` and `m_GrabPointCenter`
	m_GrabPointCenter.SetPos(ptCentroid);

	if(m_bIsLineChain || m_eShapeType == SHAPE_Polygon)
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
			HyGuiLog("IGfxEditModel::DeserializeData - Polygon shape type should have been handled by line chain deserialization!", LOGTYPE_Error);
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
	
	if(m_GrabPointList.empty() && (m_bIsLineChain == true || m_eShapeType != SHAPE_None))
		return "Grab points not provided";

	if(m_bIsLineChain || m_eShapeType == SHAPE_Polygon)
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

bool IGfxEditModel::CheckIfAddVertexOnEdge()
{
	if(m_bIsLineChain == false && m_eShapeType != SHAPE_Polygon)
	{
		HyGuiLog("IGfxEditModel::CheckIfAddVertexOnEdge invoked with shape that isn't a linechain or polygon", LOGTYPE_Error);
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
			HyGuiLog("IGfxEditModel::CheckIfAddVertexOnEdge - Failed to get world mouse position for hit testing", LOGTYPE_Error);
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

void IGfxEditModel::AssemblePolygonFixtures(std::vector<std::vector<glm::vec2>> subPolygonList)
{
	ClearFixtures();
	for(std::vector<glm::vec2> &subPoly : subPolygonList)
	{
		HyShape2d *pShape2d = new HyShape2d();
		pShape2d->SetAsPolygon(subPoly);
		m_FixtureList.push_back(pShape2d);
	}
}

std::vector<glm::vec2> IGfxEditModel::MergePolygons(const std::vector<glm::vec2> &ptA, const std::vector<glm::vec2> &ptB, int a0, int a1, int b0, int b1)
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

std::vector<std::vector<glm::vec2>> IGfxEditModel::MergeTriangles(const std::vector<HyTriangle2d> &triangleList)
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

bool IGfxEditModel::IsShareEdge(const std::vector<glm::vec2> &a, const std::vector<glm::vec2> &b, int &a0, int &a1, int &b0, int &b1)
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
