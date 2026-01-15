/**************************************************************************
*	Polygon2dModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2025 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "Polygon2dModel.h"
#include "IPolygon2dView.h"

Polygon2dModel::Polygon2dModel(HyColor color, EditorShape eShape /*= SHAPE_None*/, const QList<float> &floatList /*= QList<float>()*/) :
	m_eType(SHAPE_None),
	m_bSelfIntersecting(false),
	m_ptSelfIntersection(0.0f, 0.0f),
	m_bReverseWindingOrder(false),
	m_bLoopClosed(false),
	m_iGrabPointHoverIndex(-1),
	m_iInsertVertexIndex(-1),
	m_bTransformShiftMod(false),
	m_eTransformType(TRANSFORM_None)
{
	SetData(color, eShape, floatList);
}

/*virtual*/ Polygon2dModel::~Polygon2dModel()
{
	ClearFixtures();
}

bool Polygon2dModel::IsValidShape() const
{
	if(m_eType == SHAPE_LineChain)
	{
		HyChain2d *pChain2d = static_cast<HyChain2d *>(m_FixtureList[0]);
		if(pChain2d->GetChainData().iCount < 4)
			return false;

		return m_bSelfIntersecting == false;
	}
	
	switch(m_eType)
	{
	case SHAPE_None:
		return true;

	case SHAPE_Polygon:
		if(m_bSelfIntersecting || m_GrabPointList.size() < 3)
			return false;
		for(IHyFixture2d *pFixture : m_FixtureList)
		{
			if(pFixture->IsValid() == false)
				return false;
		}
		return true;

	case SHAPE_Box:
	case SHAPE_Circle:
	case SHAPE_LineSegment:
	case SHAPE_Capsule:
		return static_cast<HyShape2d *>(m_FixtureList[0])->IsValid();

	default:
		HyGuiLog("Polygon2dModel::IsValidShape: Unknown shape type encountered", LOGTYPE_Error);
		break;
	}

	return false;
}

HyColor Polygon2dModel::GetColor() const
{
	return m_Color;
}


void Polygon2dModel::SetColor(HyColor color)
{
	m_Color = color;

	for(IPolygon2dView *pView : m_ViewList)
		pView->RefreshColor();
}

EditorShape Polygon2dModel::GetType() const
{
	return m_eType;
}

void Polygon2dModel::SetType(EditorShape eNewShape)
{
	// Convert existing data to new shape type
	if(m_eType != eNewShape)
	{
		QList<float> convertedDataList;
		switch(eNewShape)
		{
		case SHAPE_None:
			break;
		case SHAPE_Box:
			convertedDataList = ConvertedBoxData();
			break;
		case SHAPE_Circle:
			convertedDataList = ConvertedCircleData();
			break;
		case SHAPE_LineSegment:
			convertedDataList = ConvertedLineSegmentData();
			break;
		case SHAPE_Capsule:
			convertedDataList = ConvertedCapsuleData();
			break;
		case SHAPE_Polygon:
		case SHAPE_LineChain:
			convertedDataList = ConvertedPolygonOrLineChainData();
			break;

		default:
			HyGuiLog("Polygon2dModel::SetType - Unknown shape type encountered", LOGTYPE_Error);
			break;
		}

		SetData(m_Color, eNewShape, convertedDataList);
	}

	for(IPolygon2dView *pView : m_ViewList)
		pView->RefreshView(false);
}

QList<float> Polygon2dModel::GetData() const
{
	if(m_FixtureList.empty())
		return QList<float>();
	if(m_eType != SHAPE_Polygon)
	{
		std::vector<float> serializedData = m_FixtureList[0]->SerializeSelf();
		QList<float> returnList(serializedData.begin(), serializedData.end());
		return returnList;
	}

	// SHAPE_Polygon
	QList<float> returnList;
	for(const GrabPointModel &grabPt : m_GrabPointList)
	{
		QPointF ptVertex = grabPt.GetPosition();
		returnList.push_back(static_cast<float>(ptVertex.x()));
		returnList.push_back(static_cast<float>(ptVertex.y()));
	}
	returnList.push_back(m_bLoopClosed ? 1.0f : 0.0f); // Final float indicates whether loop is closed
	return returnList;
}

void Polygon2dModel::SetData(HyColor color, EditorShape eShape, const QList<float> &floatList)
{
	m_Color = color;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	EditorShape ePreviouslyWasShape = m_eType;
	if(m_eType != eShape || m_FixtureList.empty())
	{
		m_eType = eShape;

		ClearFixtures();
		if(m_eType == SHAPE_LineChain)
			m_FixtureList.push_back(new HyChain2d());
		else if(m_eType != SHAPE_Polygon)
			m_FixtureList.push_back(new HyShape2d());
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Assemble `m_GrabPointList`
	std::vector<glm::vec2> grabPointList;
	if(m_eType != SHAPE_Polygon)
	{
		grabPointList = m_FixtureList[0]->DeserializeSelf(HyGlobal::ConvertShapeToFixtureType(m_eType), std::vector<float>(floatList.begin(), floatList.end()));
		
		if(m_eType != SHAPE_LineChain)
		{
			glm::vec2 ptCentroid;
			static_cast<HyShape2d *>(m_FixtureList[0])->GetCentroid(ptCentroid);
			m_GrabPointCenter.SetPosition(QPointF(ptCentroid.x, ptCentroid.y));
		}
	}
	else // SHAPE_Polygon deserialization
	{	
		if((floatList.size() & 1) == 0)
			HyGuiLog("Polygon2dModel::SetData for polygon had an even number of floats (final, odd float indcates loop)", LOGTYPE_Error);
		else
		{
			int iNumVertFloats = floatList.size() - 1;
			grabPointList.reserve(iNumVertFloats / 2);
			for(int i = 0; i < iNumVertFloats; i += 2)
				grabPointList.emplace_back(glm::vec2(floatList[i], floatList[i + 1]));

			m_bLoopClosed = floatList.back() != 0.0f;
		}
	}
	// Preserve existing grab points where possible (keeps selection)
	if(m_eType == ePreviouslyWasShape) 
	{
		for(int i = 0; i < grabPointList.size(); ++i)
		{
			if(static_cast<int>(m_GrabPointList.size()) - 1 < i)
				m_GrabPointList.push_back(GrabPointModel(QPointF(grabPointList[i].x, grabPointList[i].y)));
			else
				m_GrabPointList[i].SetPosition(QPointF(grabPointList[i].x, grabPointList[i].y));
		}
		if(m_GrabPointList.size() > grabPointList.size()) // Truncate to new size
			m_GrabPointList.resize(grabPointList.size());
	}
	else
	{
		m_GrabPointList.clear();
		for(const glm::vec2 &ptGrab : grabPointList)
			m_GrabPointList.push_back(GrabPointModel(QPointF(ptGrab.x, ptGrab.y)));
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	if(m_eType == SHAPE_Polygon) // Assemble `m_FixtureList` with valid sub-polygons (convex and <= 8 vertices)
	{
		std::vector<glm::vec2> ccwOrderedVertexList;
		ccwOrderedVertexList.reserve(m_GrabPointList.size());
		for(const GrabPointModel &grabPt : m_GrabPointList)
			ccwOrderedVertexList.emplace_back(static_cast<float>(grabPt.GetPosition().x()), static_cast<float>(grabPt.GetPosition().y()));

		// Validate simple polygon (no self-intersections)
		m_bSelfIntersecting = false;
		m_ptSelfIntersection.setX(0.0f);
		m_ptSelfIntersection.setY(0.0f);
		glm::vec2 pt1, pt2, pt3, pt4;
		for(int i = 0; i < ccwOrderedVertexList.size(); ++i)
		{
			const glm::vec2 &pt1 = ccwOrderedVertexList[i];
			const glm::vec2 &pt2 = ccwOrderedVertexList[(i + 1) % ccwOrderedVertexList.size()];
			for(int j = i + 2; j < ccwOrderedVertexList.size(); ++j)
			{
				// Don't check adjacent edges
				if(j == (i + 1) % ccwOrderedVertexList.size() || (i == 0 && j == ccwOrderedVertexList.size() - 1))
					continue;

				const glm::vec2 &pt3 = ccwOrderedVertexList[j];
				const glm::vec2 &pt4 = ccwOrderedVertexList[(j + 1) % ccwOrderedVertexList.size()];
				glm::vec2 ptIntersection;
				m_bSelfIntersecting = HyMath::TestSegmentsOverlap(pt1, pt2, pt3, pt4, ptIntersection);
				m_ptSelfIntersection = QPointF(ptIntersection.x, ptIntersection.y);
				if(m_bSelfIntersecting)
					return;
			}
		}

		// Ensure counter-clockwise winding on `ccwOrderedVertexList`
		float fArea = 0.0f;
		for(int i = 0; i < ccwOrderedVertexList.size(); ++i)
		{
			const glm::vec2 &ptCurrent = ccwOrderedVertexList[i];
			const glm::vec2 &ptNext = ccwOrderedVertexList[(i + 1) % m_GrabPointList.size()];
			fArea += (ptNext.x - ptCurrent.x) * (ptNext.y + ptCurrent.y);
		}
		m_bReverseWindingOrder = fArea > 0.0f; // Clockwise winding, needs reverse
		if(m_bReverseWindingOrder)
			std::reverse(ccwOrderedVertexList.begin(), ccwOrderedVertexList.end());

		// Decompose into convex polygons with <= 8 vertices
		if(ccwOrderedVertexList.size() <= 8 && HyMath::IsConvexPolygon(ccwOrderedVertexList))
			AssemblePolygonFixtures({ ccwOrderedVertexList });
		else
			AssemblePolygonFixtures(MergeTriangles(HyMath::Triangulate(ccwOrderedVertexList)));
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	for(IPolygon2dView *pView : m_ViewList)
		pView->RefreshView(false);
}

void Polygon2dModel::TransformSelf(glm::mat4 mtxTransform)
{
	for(IHyFixture2d *pFixture : m_FixtureList)
		pFixture->TransformSelf(mtxTransform);

	for(IPolygon2dView *pView : m_ViewList)
		pView->RefreshView(false);
}

int Polygon2dModel::GetNumFixtures() const
{
	return m_FixtureList.size();
}

IHyFixture2d *Polygon2dModel::GetFixture(int iIndex) const
{
	if(iIndex < 0 || iIndex >= m_FixtureList.size())
		return nullptr;
	return m_FixtureList[iIndex];
}

const QList<GrabPointModel> &Polygon2dModel::GetGrabPointList() const
{
	return m_GrabPointList;
}

const GrabPointModel &Polygon2dModel::GetCenterGrabPoint() const
{
	return m_GrabPointCenter;
}

ShapeMouseMoveResult Polygon2dModel::MouseMoveEvent(QPointF ptWorldMousePos)
{
	ShapeMouseMoveResult eResult = OnMouseMoveEvent(ptWorldMousePos);

	switch(eResult)
	{
	case SHAPEMOUSEMOVE_Outside:
	case SHAPEMOUSEMOVE_Inside:
		for(IPolygon2dView *pView : m_ViewList)
			pView->OnHoverClear();
		break;

	case SHAPEMOUSEMOVE_Crosshair:
		for(IPolygon2dView *pView : m_ViewList)
			pView->OnHoverCrosshair();
		break;

	case SHAPEMOUSEMOVE_AddVertex:
		for(IPolygon2dView *pView : m_ViewList)
			pView->OnHoverAddVertex();
		break;

	case SHAPEMOUSEMOVE_HoverVertex:
	case SHAPEMOUSEMOVE_HoverSelectedVertex:
		for(IPolygon2dView *pView : m_ViewList)
			pView->OnHoverVertex();
		break;

	case SHAPEMOUSEMOVE_HoverCenter:
		for(IPolygon2dView *pView : m_ViewList)
			pView->OnHoverCenter();
		break;

	default:
		HyGuiLog("Polygon2dModel::MouseMoveEvent - Unknown ShapeMouseMoveResult encountered", LOGTYPE_Error);
		break;
	}

	return eResult;
}

bool Polygon2dModel::MousePressEvent(bool bShiftHeld, Qt::MouseButtons uiButtonFlags, QPointF ptWorldMousePos)
{
	bool bTransformStarted = OnMousePressEvent(bShiftHeld, uiButtonFlags, ptWorldMousePos);
	if(bTransformStarted)
	{


		for(IPolygon2dView *pView : m_ViewList)
			pView->OnTransformStarted();
	}
	return bTransformStarted;
}

void Polygon2dModel::MouseMarqueeReleased(Qt::MouseButtons uiButtonFlags, QPointF ptBotLeft, QPointF ptTopRight)
{
	// Select grab points within marquee
	for(GrabPointModel &grabPtModel : m_GrabPointList)
	{
		QPointF ptGrabPos = grabPtModel.GetPosition();
		if(ptGrabPos.x() >= ptBotLeft.x() && ptGrabPos.x() <= ptTopRight.x() &&
		   ptGrabPos.y() >= ptBotLeft.y() && ptGrabPos.y() <= ptTopRight.y())
		{
			if(uiButtonFlags & Qt::LeftButton)
				grabPtModel.SetSelected(true);
			else if(uiButtonFlags & Qt::RightButton)
				grabPtModel.SetSelected(false);
		}
	}

	for(IPolygon2dView *pView : m_ViewList)
		pView->RefreshView(false);
}

void Polygon2dModel::MouseTransformDrag(bool bShiftMod, QPointF ptDragPos)
{
	m_bTransformShiftMod = bShiftMod;
	m_ptTransformDragPos = ptDragPos;

	switch(m_eTransformType)
	{
	case TRANSFORM_None:
		HyGuiLog("Polygon2dModel::MouseTransformDrag - Drag called with TRANSFORM_None", LOGTYPE_Error);
		break;

	case TRANSFORM_Initial:
		DoInitialTransformDrag();
		break;

	case TRANSFORM_TranslateShape:

		break;

	case TRANSFORM_InsertNewVertex:
		break;

	case TRANSFORM_TranslateVerts:
		break;
	}

	for(IPolygon2dView *pView : m_ViewList)
		pView->RefreshView(true);
}

QString Polygon2dModel::MouseTransformReleased(QPointF ptWorldMousePos)
{
		//	QString sUndoText;
	//	switch(GetCurAction())
	//	{
	//	case HYACTION_EntitySemTranslating:
	//	case HYACTION_EntitySemTranslateVertex:
	//		sUndoText = "Translate vert(s) on " % pTreeItemData->GetCodeName();
	//		break;
	//	
	//	case HYACTION_EntitySemRadiusHorizontal:
	//	case HYACTION_EntitySemRadiusVertical:
	//		sUndoText = "Adjust circle radius on " % pTreeItemData->GetCodeName();
	//		break;

	//	case HYACTION_EntitySemAddingVertex:
	//		sUndoText = "Add vertex to " % pTreeItemData->GetCodeName();
	//		break;

	//	default:
	//		HyGuiLog("EntityDraw::OnMouseReleaseEvent - Invalid GetCurAction()", LOGTYPE_Error);
	//		break;
	//	}
	return QString();
}

ShapeMouseMoveResult Polygon2dModel::OnMouseMoveEvent(QPointF ptWorldMousePos)
{
	m_iGrabPointHoverIndex = m_iInsertVertexIndex = -1;
	m_ptInsertVertexPos = QPointF(0.0f, 0.0f);

	for(int i = 0; i < m_GrabPointList.size(); ++i)
	{
		QPointF ptGrabPos = m_GrabPointList[i].GetPosition();

		QRectF grabRect(ptGrabPos, QSizeF(1.0f, 1.0f));
		grabRect = grabRect.adjusted(-GRABPOINT_SELECT_RADIUS, -GRABPOINT_SELECT_RADIUS, GRABPOINT_SELECT_RADIUS, GRABPOINT_SELECT_RADIUS);
		if(grabRect.contains(ptWorldMousePos))
		{
			m_iGrabPointHoverIndex = i;
			if(m_GrabPointList[i].IsSelected())
				return SHAPEMOUSEMOVE_HoverSelectedVertex;
			else
				return SHAPEMOUSEMOVE_HoverVertex;
		}
	}
	QRectF grabRect(m_GrabPointCenter.GetPosition(), QSizeF(1.0f, 1.0f));
	grabRect = grabRect.adjusted(-GRABPOINT_SELECT_RADIUS, -GRABPOINT_SELECT_RADIUS, GRABPOINT_SELECT_RADIUS, GRABPOINT_SELECT_RADIUS);
	if(grabRect.contains(ptWorldMousePos))
		return SHAPEMOUSEMOVE_HoverCenter;

	if(m_eType == SHAPE_LineChain || m_eType == SHAPE_Polygon)
	{
		if(CheckIfAddVertexOnEdge(ptWorldMousePos))
			return SHAPEMOUSEMOVE_AddVertex;

		if(m_GrabPointList.size() < 3 || m_bLoopClosed == false)
		{
			// If only the first or last vertex in chain is selected, return SHAPEMOUSEMOVE_Crosshair to indicate appending verts
			int iNumSelected = 0;
			for(const GrabPointModel &grabPtModel : m_GrabPointList)
			{
				if(grabPtModel.IsSelected())
					++iNumSelected;
			}
			if(iNumSelected == 1)
			{
				if(m_GrabPointList.front().IsSelected())
				{
					m_iInsertVertexIndex = 0;
					m_ptInsertVertexPos = QPointF(ptWorldMousePos.x(), ptWorldMousePos.y());
					return SHAPEMOUSEMOVE_Crosshair;
				}
				else if(m_GrabPointList.back().IsSelected())
				{
					m_iInsertVertexIndex = m_GrabPointList.size();
					m_ptInsertVertexPos = QPointF(ptWorldMousePos.x(), ptWorldMousePos.y());
					return SHAPEMOUSEMOVE_Crosshair;
				}
			}
		}
	}
	else if(IsValidShape() == false) // Any shape besides SHAPE_LineChain || SHAPE_Polygon
		return SHAPEMOUSEMOVE_Crosshair;

	for(IHyFixture2d *pFixture : m_FixtureList)
	{
		if(pFixture->TestPoint(glm::vec2(static_cast<float>(ptWorldMousePos.x()), static_cast<float>(ptWorldMousePos.y())), glm::identity<glm::mat4>()))
			return SHAPEMOUSEMOVE_Inside;
	}
	return SHAPEMOUSEMOVE_Outside;
}

bool Polygon2dModel::OnMousePressEvent(bool bShiftHeld, Qt::MouseButtons uiButtonFlags, QPointF ptWorldMousePos)
{
	m_ptTransformStartPos.setX(0.0f);
	m_ptTransformStartPos.setY(0.0f);
	m_eTransformType = TRANSFORM_None;

	switch(OnMouseMoveEvent(ptWorldMousePos))
	{
	case SHAPEMOUSEMOVE_Outside:
		return false; // Start a marquee select

	case SHAPEMOUSEMOVE_Inside:
		for(const GrabPointModel &grabPtModel : m_GrabPointList)
		{
			if(grabPtModel.IsSelected() == false)
				return false; // Start a marquee select
		}
		m_ptTransformStartPos = ptWorldMousePos;
		m_eTransformType = TRANSFORM_TranslateShape;
		return true;

	case SHAPEMOUSEMOVE_Crosshair:
	case SHAPEMOUSEMOVE_AddVertex:
		if(m_iInsertVertexIndex == -1)
		{
			HyGuiLog("Polygon2dModel::OnMousePressEvent - Insert vertex index was -1 on AddVertex/Crosshair", LOGTYPE_Error);
			return false;
		}
		m_GrabPointList.insert(m_GrabPointList.begin() + m_iInsertVertexIndex, GrabPointModel(m_ptInsertVertexPos));

		m_ptTransformStartPos = ptWorldMousePos;
		m_eTransformType = TRANSFORM_InsertNewVertex;
		return true;

	case SHAPEMOUSEMOVE_HoverVertex:
	case SHAPEMOUSEMOVE_HoverSelectedVertex:
		if(m_iGrabPointHoverIndex == -1)
		{
			HyGuiLog("Polygon2dModel::OnMousePressEvent - Hover vertex index was -1 on HoverVertex/HoverSelectedVertex", LOGTYPE_Error);
			return false;
		}

		if(bShiftHeld)
		{
			if(uiButtonFlags & Qt::LeftButton)
			{
				m_GrabPointList[m_iGrabPointHoverIndex].SetSelected(!m_GrabPointList[m_iGrabPointHoverIndex].IsSelected());
				m_ptTransformStartPos = ptWorldMousePos;
				m_eTransformType = TRANSFORM_TranslateVerts;
				return true;
			}
			else if(uiButtonFlags & Qt::RightButton)
			{
				m_GrabPointList[m_iGrabPointHoverIndex].SetSelected(false);
				return false;
			}
		}
		else
		{
			if(uiButtonFlags & Qt::LeftButton)
			{
				m_GrabPointList[m_iGrabPointHoverIndex].SetSelected(true);
				m_ptTransformStartPos = ptWorldMousePos;
				m_eTransformType = TRANSFORM_TranslateVerts;
				return true;
			}
			else if(uiButtonFlags & Qt::RightButton)
			{
				m_GrabPointList[m_iGrabPointHoverIndex].SetSelected(false);
				return false;
			}
		}
		return false;

	case SHAPEMOUSEMOVE_HoverCenter:
		m_ptTransformStartPos = ptWorldMousePos;
		m_eTransformType = TRANSFORM_TranslateShape;
		return true;
		break;

	default:
		HyGuiLog("Polygon2dModel::OnMousePressEvent - Unknown shape mouse move result encountered", LOGTYPE_Error);
		break;
	}

	return false;
}

bool Polygon2dModel::CheckIfAddVertexOnEdge(QPointF ptWorldMousePos)
{
	if(m_eType != SHAPE_LineChain || m_eType != SHAPE_Polygon)
	{
		HyGuiLog("Polygon2dModel::CheckIfAddVertexOnEdge invoked with shape that isn't a linechain or polygon", LOGTYPE_Error);
		return false;
	}
	if(m_GrabPointList.size() < 2)
		return false;

	glm::vec2 ptTest(static_cast<float>(ptWorldMousePos.x()), static_cast<float>(ptWorldMousePos.y()));
	HyShape2d tmpEdgeShape;
	for(int i = 0; i < m_GrabPointList.size(); ++i)
	{
		if(i == (m_GrabPointList.size() - 1) && m_bLoopClosed == false)
			break;

		glm::vec2 pt1(m_GrabPointList[i].GetPosition().x(), m_GrabPointList[i].GetPosition().y());
		glm::vec2 pt2(m_GrabPointList[(i + 1) % m_GrabPointList.size()].GetPosition().x(), m_GrabPointList[(i + 1) % m_GrabPointList.size()].GetPosition().y());
		tmpEdgeShape.SetAsLineSegment(pt1, pt2);
		if(tmpEdgeShape.TestPoint(ptTest, glm::identity<glm::mat4>()));
		{
			m_iInsertVertexIndex = i + 1;
			glm::vec2 ptClosestPoint = HyMath::ClosestPointOnSegment(pt1, pt2, ptTest);
			m_ptInsertVertexPos = QPointF(ptClosestPoint.x, ptClosestPoint.y);
			return true;
		}
	}

	return false;
}

void Polygon2dModel::DoInitialTransformDrag()
{
	glm::vec2 ptStartPos(m_ptTransformStartPos.x(), m_ptTransformStartPos.y());
	glm::vec2 ptDragPos(m_ptTransformDragPos.x(), m_ptTransformDragPos.y());
	glm::vec2 ptLowerBound, ptUpperBound, ptCenter;
	if(m_bTransformShiftMod)
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

	switch(m_eType)
	{
	case SHAPE_Box:
		static_cast<HyShape2d *>(m_FixtureList[0])->SetAsBox(HyRect((ptUpperBound.x - ptLowerBound.x) * 0.5f, (ptUpperBound.y - ptLowerBound.y) * 0.5f, ptCenter, 0.0f));
		break;
	case SHAPE_Circle:
		static_cast<HyShape2d *>(m_FixtureList[0])->SetAsCircle(ptCenter, glm::distance(ptCenter, ptUpperBound));
		break;
	case SHAPE_Capsule: {
		glm::vec2 pt1, pt2;
		pt1.x = ptCenter.x;
		pt1.y = ptLowerBound.y;
		pt2.x = ptCenter.x;
		pt2.y = ptUpperBound.y;
		float fRadius = 0.5f * glm::distance(pt1, pt2);
		static_cast<HyShape2d *>(m_FixtureList[0])->SetAsCapsule(ptStartPos, ptDragPos, fRadius);
		break; }
	case SHAPE_LineSegment:
		static_cast<HyShape2d *>(m_FixtureList[0])->SetAsLineSegment(ptStartPos, ptDragPos);
		break;
	case SHAPE_Polygon:
	case SHAPE_LineChain:
		if(m_GrabPointList.size() == 1)
			m_GrabPointList.append(GrabPointModel(m_ptTransformDragPos));
		if(m_GrabPointList.size() != 2)
			HyGuiLog("Polygon2dModel::MouseTransformDrag - Polygon or LineChain initial dragging with != 2 verts", LOGTYPE_Error);

		m_GrabPointList[0].SetSelected(false);
		m_GrabPointList[1].SetSelected(true);
		m_GrabPointList[1].SetPosition(m_ptTransformDragPos);
		break;

	default:
		HyGuiLog("Polygon2dModel::MouseTransformDrag - Initial transform called with unsupported shape type: " % QString::number(m_eType), LOGTYPE_Error);
		break;
	}
}

bool Polygon2dModel::IsShareEdge(const std::vector<glm::vec2> &a, const std::vector<glm::vec2> &b, int &a0, int &a1, int &b0, int &b1)
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

std::vector<glm::vec2> Polygon2dModel::MergePolygons(const std::vector<glm::vec2> &ptA, const std::vector<glm::vec2> &ptB, int a0, int a1, int b0, int b1)
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

std::vector<std::vector<glm::vec2>> Polygon2dModel::MergeTriangles(const std::vector<HyTriangle2d> &triangleList)
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

void Polygon2dModel::ClearFixtures()
{
	for(IHyFixture2d *pFixture : m_FixtureList)
		delete pFixture;
	m_FixtureList.clear();
}

void Polygon2dModel::AssemblePolygonFixtures(std::vector<std::vector<glm::vec2>> subPolygonList)
{
	ClearFixtures();
	for(std::vector<glm::vec2> &subPoly : subPolygonList)
	{
		HyShape2d *pShape2d = new HyShape2d();
		pShape2d->SetAsPolygon(subPoly);
		m_FixtureList.push_back(pShape2d);
	}
}

QList<float> Polygon2dModel::ConvertedBoxData() const
{
	HyShape2d tmpBoxShape;
	switch(m_eType)
	{
	case SHAPE_None:
		break;
	case SHAPE_Box:
		return GetData();
	case SHAPE_Circle:
		if(m_FixtureList[0]->IsValid())
		{
			b2Circle b2Circ = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsCircle();
			glm::vec2 ptCenter(b2Circ.center.x, b2Circ.center.y);
			float fRadius = b2Circ.radius;
			tmpBoxShape.SetAsBox(HyRect(fRadius, fRadius, ptCenter, 0.0f));
			std::vector<float> serializedTmpBox = tmpBoxShape.SerializeSelf();
			return QList<float>(serializedTmpBox.begin(), serializedTmpBox.end());
		}
		break;
	case SHAPE_LineSegment:
		if(m_FixtureList[0]->IsValid())
		{
			b2Segment b2LineSeg = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsSegment();
			glm::vec2 pt1(b2LineSeg.point1.x, b2LineSeg.point1.y);
			glm::vec2 pt2(b2LineSeg.point2.x, b2LineSeg.point2.y);
			glm::vec2 ptCenter = (pt1 + pt2) * 0.5f;
			glm::vec2 vHalfExtents = glm::abs(pt2 - pt1) * 0.5f;
			tmpBoxShape.SetAsBox(HyRect(vHalfExtents.x, vHalfExtents.y, ptCenter, 0.0f));
			std::vector<float> serializedTmpBox = tmpBoxShape.SerializeSelf();
			return QList<float>(serializedTmpBox.begin(), serializedTmpBox.end());
		}
		break;
	case SHAPE_Capsule:
		if(m_FixtureList[0]->IsValid())
		{
			b2Capsule b2Cap = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsCapsule();
			glm::vec2 pt1(b2Cap.center1.x, b2Cap.center1.y);
			glm::vec2 pt2(b2Cap.center2.x, b2Cap.center2.y);
			float fRadius = b2Cap.radius;
			glm::vec2 ptCenter = (pt1 + pt2) * 0.5f;
			glm::vec2 vHalfExtents = (glm::abs(pt2 - pt1) * 0.5f) + glm::vec2(fRadius, fRadius);
			tmpBoxShape.SetAsBox(HyRect(vHalfExtents.x, vHalfExtents.y, ptCenter, 0.0f));
			std::vector<float> serializedTmpBox = tmpBoxShape.SerializeSelf();
			return QList<float>(serializedTmpBox.begin(), serializedTmpBox.end());
		}
		break;
	case SHAPE_Polygon:
	case SHAPE_LineChain: {
		std::vector<glm::vec2> vertexList;
		for(const GrabPointModel &grabPt : m_GrabPointList)
		{
			QPointF ptVertex = grabPt.GetPosition();
			vertexList.push_back(glm::vec2(static_cast<float>(ptVertex.x()), static_cast<float>(ptVertex.y())));
		}
		b2AABB boundingBox;
		HyMath::ComputeAABB(boundingBox, vertexList.data(), static_cast<int>(vertexList.size()), 5.0f);

		glm::vec2 ptCenter = glm::vec2(0.5f * (boundingBox.lowerBound.x + boundingBox.upperBound.x), 0.5f * (boundingBox.lowerBound.y + boundingBox.upperBound.y));
		glm::vec2 vHalfExtents = glm::vec2(0.5f * (boundingBox.upperBound.x - boundingBox.lowerBound.x), 0.5f * (boundingBox.upperBound.y - boundingBox.lowerBound.y));
		tmpBoxShape.SetAsBox(HyRect(vHalfExtents.x, vHalfExtents.y, ptCenter, 0.0f));
		std::vector<float> serializedTmpBox = tmpBoxShape.SerializeSelf();
		return QList<float>(serializedTmpBox.begin(), serializedTmpBox.end()); }

	default:
		HyGuiLog("Polygon2dModel::SetType - Unhandled conversion to Box", LOGTYPE_Error);
		break;
	}
	
	return QList<float>();
}

QList<float> Polygon2dModel::ConvertedCircleData() const
{
	QList<float> convertedDataList;
	switch(m_eType)
	{
	case SHAPE_None:
		break;
	case SHAPE_Box:
		if(m_FixtureList[0]->IsValid())
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
			convertedDataList.append(ptCenter.x);
			convertedDataList.append(ptCenter.y);
			convertedDataList.append(fMaxDist);
		}
		break;
	case SHAPE_Circle:
		return GetData();
	case SHAPE_LineSegment:
		if(m_FixtureList[0]->IsValid())
		{
			b2Segment b2LineSeg = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsSegment();
			glm::vec2 pt1(b2LineSeg.point1.x, b2LineSeg.point1.y);
			glm::vec2 pt2(b2LineSeg.point2.x, b2LineSeg.point2.y);
			glm::vec2 ptCenter = (pt1 + pt2) * 0.5f;
			float fRadius = glm::distance(pt1, pt2) * 0.5f;
			convertedDataList.append(ptCenter.x);
			convertedDataList.append(ptCenter.y);
			convertedDataList.append(fRadius);
		}
		break;
	case SHAPE_Capsule:
		if(m_FixtureList[0]->IsValid())
		{
			b2Capsule b2Cap = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsCapsule();
			glm::vec2 pt1(b2Cap.center1.x, b2Cap.center1.y);
			glm::vec2 pt2(b2Cap.center2.x, b2Cap.center2.y);
			glm::vec2 ptCenter = (pt1 + pt2) * 0.5f;
			float fRadius = glm::distance(pt1, pt2) * 0.5f + b2Cap.radius;
			convertedDataList.append(ptCenter.x);
			convertedDataList.append(ptCenter.y);
			convertedDataList.append(fRadius);
		}
		break;

	case SHAPE_Polygon:
	case SHAPE_LineChain: {
		std::vector<glm::vec2> vertexList;
		for(const GrabPointModel &grabPt : m_GrabPointList)
		{
			QPointF ptVertex = grabPt.GetPosition();
			vertexList.push_back(glm::vec2(static_cast<float>(ptVertex.x()), static_cast<float>(ptVertex.y())));
		}
		if(vertexList.size() > 1)
		{
			b2AABB boundingBox;
			HyMath::ComputeAABB(boundingBox, vertexList.data(), static_cast<int>(vertexList.size()), 5.0f);
			glm::vec2 ptCenter = glm::vec2(0.5f * (boundingBox.lowerBound.x + boundingBox.upperBound.x), 0.5f * (boundingBox.lowerBound.y + boundingBox.upperBound.y));
			glm::vec2 vHalfExtents = glm::vec2(0.5f * (boundingBox.upperBound.x - boundingBox.lowerBound.x), 0.5f * (boundingBox.upperBound.y - boundingBox.lowerBound.y));
			float fRadius = HyMath::Max(vHalfExtents.x, vHalfExtents.y);
			convertedDataList.append(ptCenter.x);
			convertedDataList.append(ptCenter.y);
			convertedDataList.append(fRadius);
		}
		break; }
	
	default:
		HyGuiLog("Polygon2dModel::SetType - Unhandled conversion to Circle", LOGTYPE_Error);
		break;
	}
	
	return convertedDataList;
}

QList<float> Polygon2dModel::ConvertedLineSegmentData() const
{
	QList<float> convertedDataList;
	switch(m_eType)
	{
	case SHAPE_None:
		break;
	case SHAPE_Box:
		if(m_FixtureList[0]->IsValid())
		{
			b2Polygon b2Poly = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsPolygon();
			convertedDataList.append(b2Poly.vertices[0].x);
			convertedDataList.append(b2Poly.vertices[0].y);
			convertedDataList.append(b2Poly.vertices[2].x);
			convertedDataList.append(b2Poly.vertices[2].y);
		}
		break;
	case SHAPE_Circle:
		if(m_FixtureList[0]->IsValid())
		{
			b2Circle b2Circ = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsCircle();
			glm::vec2 ptCenter(b2Circ.center.x, b2Circ.center.y);
			float fRadius = b2Circ.radius;
			convertedDataList.append(ptCenter.x - fRadius);
			convertedDataList.append(ptCenter.y);
			convertedDataList.append(ptCenter.x + fRadius);
			convertedDataList.append(ptCenter.y);
		}
		break;
	case SHAPE_LineSegment:
		return GetData();
	case SHAPE_Capsule:
		if(m_FixtureList[0]->IsValid())
		{
			b2Capsule b2Cap = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsCapsule();
			glm::vec2 pt1(b2Cap.center1.x, b2Cap.center1.y);
			glm::vec2 pt2(b2Cap.center2.x, b2Cap.center2.y);
			convertedDataList.append(pt1.x);
			convertedDataList.append(pt1.y);
			convertedDataList.append(pt2.x);
			convertedDataList.append(pt2.y);
		}
		break;
	case SHAPE_Polygon:
	case SHAPE_LineChain: {
		std::vector<glm::vec2> vertexList;
		for(const GrabPointModel &grabPt : m_GrabPointList)
		{
			QPointF ptVertex = grabPt.GetPosition();
			vertexList.push_back(glm::vec2(static_cast<float>(ptVertex.x()), static_cast<float>(ptVertex.y())));
		}
		if(vertexList.size() > 1)
		{
			b2AABB boundingBox;
			HyMath::ComputeAABB(boundingBox, vertexList.data(), static_cast<int>(vertexList.size()), 5.0f);
			convertedDataList.append(boundingBox.lowerBound.x);
			convertedDataList.append(boundingBox.lowerBound.y);
			convertedDataList.append(boundingBox.upperBound.x);
			convertedDataList.append(boundingBox.upperBound.y);
		}
		break; }

	default:
		HyGuiLog("Polygon2dModel::SetType - Unhandled conversion to LineSegment", LOGTYPE_Error);
		break;
	}
	
	return convertedDataList;
}

QList<float> Polygon2dModel::ConvertedCapsuleData() const
{
	QList<float> convertedDataList;
	switch(m_eType)
	{
	case SHAPE_None:
		break;
	case SHAPE_Box:
		if(m_FixtureList[0]->IsValid())
		{
			b2Polygon b2Poly = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsPolygon();
			glm::vec2 pt1(b2Poly.vertices[1].x, b2Poly.vertices[1].y);
			glm::vec2 pt2(b2Poly.vertices[3].x, b2Poly.vertices[3].y);
			float fRadius = glm::distance(pt1, pt2) * 0.5f;
			convertedDataList.append(pt1.x);
			convertedDataList.append(pt1.y);
			convertedDataList.append(pt2.x);
			convertedDataList.append(pt2.y);
			convertedDataList.append(fRadius);
		}
		break;
	case SHAPE_Circle:
		if(m_FixtureList[0]->IsValid())
		{
			b2Circle b2Circ = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsCircle();
			glm::vec2 ptCenter(b2Circ.center.x, b2Circ.center.y);
			float fRadius = b2Circ.radius;
			glm::vec2 pt1 = ptCenter + glm::vec2(0.0f, -fRadius);
			glm::vec2 pt2 = ptCenter + glm::vec2(0.0f, fRadius);
			convertedDataList.append(pt1.x);
			convertedDataList.append(pt1.y);
			convertedDataList.append(pt2.x);
			convertedDataList.append(pt2.y);
			convertedDataList.append(fRadius);
		}
		break;
	case SHAPE_Capsule:
		return GetData();
	case SHAPE_Polygon:
	case SHAPE_LineSegment: {
		std::vector<glm::vec2> vertexList;
		for(const GrabPointModel &grabPt : m_GrabPointList)
		{
			QPointF ptVertex = grabPt.GetPosition();
			vertexList.push_back(glm::vec2(static_cast<float>(ptVertex.x()), static_cast<float>(ptVertex.y())));
		}
		if(vertexList.size() > 1)
		{
			b2AABB boundingBox;
			HyMath::ComputeAABB(boundingBox, vertexList.data(), static_cast<int>(vertexList.size()), 5.0f);
			convertedDataList.append(boundingBox.lowerBound.x);
			convertedDataList.append(boundingBox.lowerBound.y);
			convertedDataList.append(boundingBox.upperBound.x);
			convertedDataList.append(boundingBox.upperBound.y);
			float fRadius = HyMath::Min(0.5f * (boundingBox.upperBound.x - boundingBox.lowerBound.x), 0.5f * (boundingBox.upperBound.y - boundingBox.lowerBound.y));
			convertedDataList.append(fRadius);
		}
		break; }
	default:
		HyGuiLog("Polygon2dModel::SetType - Unhandled conversion to Capsule", LOGTYPE_Error);
		break;
	}

	return convertedDataList;
}

QList<float> Polygon2dModel::ConvertedPolygonOrLineChainData() const
{
	QList<float> convertedDataList;
	switch(m_eType)
	{
	case SHAPE_None:
		break;
	case SHAPE_Box:
		if(m_FixtureList[0]->IsValid())
		{
			b2Polygon b2Poly = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsPolygon();
			for(int i = 0; i < b2Poly.count; ++i)
			{
				convertedDataList.append(b2Poly.vertices[i].x);
				convertedDataList.append(b2Poly.vertices[i].y);
			}
			convertedDataList.append(1.0f); // Closed loop
		}
		break;
	case SHAPE_Circle:
		if(m_FixtureList[0]->IsValid())
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
				convertedDataList.append(ptVertList[i].x);
				convertedDataList.append(ptVertList[i].y);
			}
			convertedDataList.append(1.0f); // Closed loop
		}
		break;
	case SHAPE_LineSegment:
		if(m_FixtureList[0]->IsValid())
		{
			b2Segment b2LineSeg = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsSegment();
			convertedDataList.append(b2LineSeg.point1.x);
			convertedDataList.append(b2LineSeg.point1.y);
			convertedDataList.append(b2LineSeg.point2.x);
			convertedDataList.append(b2LineSeg.point2.y);
			convertedDataList.append(0.0f); // Open loop
		}
		break;
	case SHAPE_Polygon:
	case SHAPE_LineChain:
		convertedDataList = GetData();
		break;
	case SHAPE_Capsule:
		if(m_FixtureList[0]->IsValid())
		{
			b2Capsule b2Cap = static_cast<HyShape2d *>(m_FixtureList[0])->GetAsCapsule();
			glm::vec2 pt1(b2Cap.center1.x, b2Cap.center1.y);
			glm::vec2 pt2(b2Cap.center2.x, b2Cap.center2.y);
			float fRadius = b2Cap.radius;
			glm::vec2 vDir = glm::normalize(pt2 - pt1);
			glm::vec2 vPerp(-vDir.y, vDir.x);
			// Approximate capsule with 8 vertices
			convertedDataList.append(pt1.x + vPerp.x * fRadius);
			convertedDataList.append(pt1.y + vPerp.y * fRadius);
			convertedDataList.append(pt1.x - vPerp.x * fRadius);
			convertedDataList.append(pt1.y - vPerp.y * fRadius);
			convertedDataList.append(pt2.x - vPerp.x * fRadius);
			convertedDataList.append(pt2.y - vPerp.y * fRadius);
			convertedDataList.append(pt2.x + vPerp.x * fRadius);
			convertedDataList.append(pt2.y + vPerp.y * fRadius);
			convertedDataList.append(1.0f); // Closed loop
		}
		break;
	default:
		HyGuiLog("Polygon2dModel::SetType - Unhandled conversion to Polygon/LineChain", LOGTYPE_Error);
		break;
	}
	
	return convertedDataList;
}
