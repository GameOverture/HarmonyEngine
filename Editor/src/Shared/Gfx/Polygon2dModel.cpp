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
	m_eTransformType(TRANSFORM_None)
{
	SetData(color, eShape, floatList);
}

/*virtual*/ Polygon2dModel::~Polygon2dModel()
{
	ClearFixtures();
}

HyColor Polygon2dModel::GetColor() const
{
	return m_Color;
}

EditorShape Polygon2dModel::GetType() const
{
	return m_eType;
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
	return returnList;
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

bool Polygon2dModel::IsValidShape() const
{
	if(m_eType == SHAPE_LineChain)
	{
		HyChain2d *pChain2d = static_cast<HyChain2d *>(m_FixtureList[0]);
		if(pChain2d->GetData().iCount < 4)
			return false;

		return true;
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

void Polygon2dModel::SetColor(HyColor color)
{
	m_Color = color;

	for(IPolygon2dView *pView : m_ViewList)
		pView->RefreshColor();
}

void Polygon2dModel::SetData(HyColor color, EditorShape eShape, const QList<float> &floatList)
{
	m_Color = color;

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

	// Assemble `m_GrabPointList`
	std::vector<glm::vec2> grabPointList;
	if(m_eType != SHAPE_Polygon)
		grabPointList = m_FixtureList[0]->DeserializeSelf(HyGlobal::ConvertShapeToFixtureType(m_eType), std::vector<float>(floatList.begin(), floatList.end()));
	else
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
	
	if(m_eType == SHAPE_Polygon) // Assemble `m_FixtureList` with valid sub-polygons (convex and <= 8 vertices)
	{
		std::vector<glm::vec2> ccwOrderedVertexList;
		ccwOrderedVertexList.reserve(m_GrabPointList.size());
		for(const GrabPointModel &grabPt : m_GrabPointList)
			ccwOrderedVertexList.emplace_back(static_cast<float>(grabPt.GetPosition().x()), static_cast<float>(grabPt.GetPosition().y()));

		// Validate simple polygon (no self-intersections)
		m_bSelfIntersecting = false;
		HySetVec(m_ptSelfIntersection, 0.0f, 0.0f);
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
				m_bSelfIntersecting = HyMath::TestSegmentsOverlap(pt1, pt2, pt3, pt4, m_ptSelfIntersection);
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

	for(IPolygon2dView *pView : m_ViewList)
		pView->RefreshView();
}

void Polygon2dModel::TransformSelf(glm::mat4 mtxTransform)
{
	for(IHyFixture2d *pFixture : m_FixtureList)
		pFixture->TransformSelf(mtxTransform);
}

ShapeMouseMoveResult Polygon2dModel::OnMouseMoveEvent(QPointF ptWorldMousePos)
{
	m_iGrabPointHoverIndex = m_iInsertVertexIndex = -1;

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

	if(IsValidShape() == false || m_eType == SHAPE_LineChain)
	{
		if(CheckIfAddVertexOnEdge(ptWorldMousePos))
			return SHAPEMOUSEMOVE_AddVertex;

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
				HySetVec(m_ptInsertVertexPos, ptWorldMousePos.x(), ptWorldMousePos.y());
				return SHAPEMOUSEMOVE_Crosshair;
			}
			else if(m_GrabPointList.back().IsSelected())
			{
				m_iInsertVertexIndex = m_GrabPointList.size();
				HySetVec(m_ptInsertVertexPos, ptWorldMousePos.x(), ptWorldMousePos.y());
				return SHAPEMOUSEMOVE_Crosshair;
			}
		}

		return SHAPEMOUSEMOVE_Outside;
	}

	if(m_eType == SHAPE_Polygon)
	{
		if(CheckIfAddVertexOnEdge(ptWorldMousePos))
			return SHAPEMOUSEMOVE_AddVertex;

		if(m_GrabPointList.size() < 3 || m_bLoopClosed == false)
		{
			m_iInsertVertexIndex = m_GrabPointList.size();
			HySetVec(m_ptInsertVertexPos, ptWorldMousePos.x(), ptWorldMousePos.y());
			return SHAPEMOUSEMOVE_Crosshair;
		}
	}

	for(IHyFixture2d *pFixture : m_FixtureList)
	{
		if(pFixture->TestPoint(glm::vec2(static_cast<float>(ptWorldMousePos.x()), static_cast<float>(ptWorldMousePos.y())), glm::identity<glm::mat4>()))
			return SHAPEMOUSEMOVE_Inside;
	}

	return SHAPEMOUSEMOVE_Outside;
}

bool Polygon2dModel::OnMousePressEvent(bool bShiftHeld, Qt::MouseButtons uiButtonFlags, QPointF ptWorldMousePos)
{
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
		m_eTransformType = TRANSFORM_TranslateAllVerts;
		return true;

	case SHAPEMOUSEMOVE_Crosshair:
	case SHAPEMOUSEMOVE_AddVertex:
		if(m_iInsertVertexIndex == -1)
		{
			HyGuiLog("Polygon2dModel::OnMousePressEvent - Insert vertex index was -1 on AddVertex/Crosshair", LOGTYPE_Error);
			return false;
		}
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
				m_eTransformType = TRANSFORM_TranslateSelectedVerts;
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
				m_eTransformType = TRANSFORM_TranslateSelectedVerts;
				return true;
			}
			else if(uiButtonFlags & Qt::RightButton)
			{
				m_GrabPointList[m_iGrabPointHoverIndex].SetSelected(false);
				return false;
			}
		}
		return false;

	default:
		HyGuiLog("Polygon2dModel::OnMousePressEvent - Unknown shape mouse move result encountered", LOGTYPE_Error);
		break;
	}

	return false;
}

void Polygon2dModel::OnMouseMarqueeReleased(Qt::MouseButtons uiButtonFlags, QPointF ptBotLeft, QPointF ptTopRight)
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
}

void Polygon2dModel::OnMouseTransformDrag(QPointF ptWorldMousePos)
{
}

QString Polygon2dModel::OnMouseTransformReleased(QPointF ptWorldMousePos)
{
	return QString();
}

bool Polygon2dModel::CheckIfAddVertexOnEdge(QPointF ptWorldMousePos)
{
	HyShape2d tmpEdgeShape;
	for(int i = 0; i < m_GrabPointList.size(); ++i)
	{
		glm::vec2 pt1(m_GrabPointList[i].GetPosition().x(), m_GrabPointList[i].GetPosition().y());
		glm::vec2 pt2(m_GrabPointList[(i + 1) % m_GrabPointList.size()].GetPosition().x(), m_GrabPointList[(i + 1) % m_GrabPointList.size()].GetPosition().y());
		tmpEdgeShape.SetAsLineSegment(pt1, pt2);
		glm::vec2 ptTest(static_cast<float>(ptWorldMousePos.x()), static_cast<float>(ptWorldMousePos.y()));
		if(tmpEdgeShape.TestPoint(ptTest, glm::identity<glm::mat4>()));
		{
			m_iInsertVertexIndex = i + 1;
			m_ptInsertVertexPos = HyMath::ClosestPointOnSegment(pt1, pt2, ptTest);
			return true;
		}
	}

	return false;
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
