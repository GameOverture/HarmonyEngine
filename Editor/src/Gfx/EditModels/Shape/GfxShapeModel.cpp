/**************************************************************************
*	GfxShapeModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2025 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "GfxShapeModel.h"
#include "IGfxEditView.h"

GfxShapeModel::GfxShapeModel(HyColor color) :
	IGfxEditModel(EDITMODETYPE_Shape, color),
	m_eShapeType(SHAPE_None),
	m_bSelfIntersecting(false),
	m_ptSelfIntersection(0.0f, 0.0f),
	m_bReverseWindingOrder(false),
	m_bLoopClosed(false)
{
}

/*virtual*/ GfxShapeModel::~GfxShapeModel()
{
	ClearFixtures();
}

/*virtual*/ bool GfxShapeModel::IsValidModel() const /*override*/
{
	switch(m_eShapeType)
	{
	case SHAPE_None:
		return true;

	case SHAPE_Polygon:
		if(m_bSelfIntersecting || m_GrabPointList.size() < 3)
			return false;
		for(HyShape2d *pFixture : m_ShapeList)
		{
			if(pFixture->IsValid() == false)
				return false;
		}
		return true;

	case SHAPE_Box:
	case SHAPE_Circle:
	case SHAPE_LineSegment:
	case SHAPE_Capsule:
		return m_ShapeList[0]->IsValid();

	default:
		HyGuiLog("GfxShapeModel::IsValidShape: Unknown shape type encountered", LOGTYPE_Error);
		break;
	}

	return false;
}

EditorShape GfxShapeModel::GetShapeType() const
{
	return m_eShapeType;
}

void GfxShapeModel::SetShapeType(EditorShape eNewShape, QList<float> floatList)
{
	if(m_eShapeType == eNewShape)
		return;

	if(floatList.empty() == false)
	{
		// Convert existing data to new shape type
		switch(eNewShape)
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
	}

	m_eShapeType = eNewShape;

	ClearFixtures();
	m_ShapeList.push_back(new HyShape2d());

	Deserialize(floatList);
}

/*virtual*/ QList<float> GfxShapeModel::Serialize() const /*override*/
{
	if(m_ShapeList.empty())
		return QList<float>();
	if(m_eShapeType != SHAPE_Polygon)
	{
		std::vector<float> serializedData = m_ShapeList[0]->SerializeSelf();
		QList<float> returnList(serializedData.begin(), serializedData.end());
		return returnList;
	}

	// SHAPE_Polygon
	QList<float> returnList;
	for(const GfxGrabPointModel &grabPt : m_GrabPointList)
	{
		glm::vec2 ptVertex = grabPt.GetPos();
		returnList.push_back(static_cast<float>(ptVertex.x));
		returnList.push_back(static_cast<float>(ptVertex.y));
	}
	returnList.push_back(m_bLoopClosed ? 1.0f : 0.0f); // Final float indicates whether loop is closed
	return returnList;
}

void GfxShapeModel::TransformData(glm::mat4 mtxTransform)
{
	for(HyShape2d *pFixture : m_ShapeList)
		pFixture->TransformSelf(mtxTransform);

	SyncViews(EDITMODE_Idle, EDITMODEACTION_None);
}

int GfxShapeModel::GetNumShapeFixtures() const
{
	return m_ShapeList.size();
}

HyShape2d *GfxShapeModel::GetShapeFixture(int iIndex) const
{
	if(iIndex < 0 || iIndex >= m_ShapeList.size())
		return nullptr;
	return m_ShapeList[iIndex];
}

bool GfxShapeModel::IsLoopClosed() const
{
	return m_bLoopClosed;
}

/*virtual*/ QString GfxShapeModel::GetActionText(QString sNodeCodeName) const /*override*/
{
	QString sUndoText;
	switch(m_eCurAction)
	{
	case EDITMODEACTION_None:
	case EDITMODEACTION_Outside:
		break;
	case EDITMODEACTION_Creation:
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
		if(m_eShapeType == SHAPE_Polygon || m_eShapeType == SHAPE_LineSegment)
			sUndoText = "Translate vert(s) on " % sNodeCodeName;
		else if(m_eShapeType == SHAPE_Circle)
			sUndoText = "Adjust circle radius on " % sNodeCodeName;
		else if(m_eShapeType == SHAPE_Box)
			sUndoText = "Adjust box size on " % sNodeCodeName;
		else if(m_eShapeType == SHAPE_Capsule)
			sUndoText = "Adjust capsule size on " % sNodeCodeName;
		else
			HyGuiLog("GfxShapeModel::MouseTransformReleased - Invalid shape type for EDITMODEACTION_HoverGrabPoint", LOGTYPE_Error);
		break;

	default:
		HyGuiLog("GfxShapeModel::MouseTransformReleased - Invalid m_eCurTransform", LOGTYPE_Error);
		break;
	}

	return sUndoText;
}

/*virtual*/ QList<float> GfxShapeModel::GetActionSerialized() const /*override*/
{
	switch(m_eCurAction)
	{
	case EDITMODEACTION_Creation:
		return Serialize();

	case EDITMODEACTION_Inside:
		if(IsAllGrabPointsSelected() == false)
			HyGuiLog("GfxShapeModel::GetActionSerialized - EDITMODEACTION_Inside with not all grab points selected", LOGTYPE_Error);
		[[fallthrough]];
	case EDITMODEACTION_HoverCenter:
		if(m_eShapeType != SHAPE_Polygon)
		{
			if(m_ShapeList.size() != 1)
				HyGuiLog("GfxShapeModel::GetActionSerialized - Expected exactly one shape fixture for non-polygon shape type", LOGTYPE_Error);
			
			// Translate entire shape by the drag delta.
			HyShape2d tmpShape = *m_ShapeList[0];
			tmpShape.TransformSelf(glm::translate(glm::mat4(1.0f), glm::vec3(m_vDragDelta, 0.0f)));

			std::vector<float> serializedData = tmpShape.SerializeSelf();
			return QList<float>(serializedData.begin(), serializedData.end());
		}
		else // SHAPE_Polygon
		{
			QList<float> returnList;
			for(const GfxGrabPointModel &grabPt : m_GrabPointList)
			{
				glm::vec2 ptVertex = grabPt.GetPos();
				returnList.push_back(static_cast<float>(ptVertex.x));
				returnList.push_back(static_cast<float>(ptVertex.y));
			}

			// Translate entire shape by the drag delta.
			for(int i = 0; i < returnList.size(); i += 2)
			{
				returnList[i] += m_vDragDelta.x;
				returnList[i + 1] += m_vDragDelta.y;
			}

			returnList.push_back(m_bLoopClosed ? 1.0f : 0.0f); // Final float indicates whether loop is closed
			return returnList;
		}
		break;

	case EDITMODEACTION_AppendVertex:
	case EDITMODEACTION_InsertVertex:
		// Guaranteed to be SHAPE_Polygon, translate all (selected) vertices by the drag delta.
		// AppendVertex/InsertVertex both work with EDITMODEACTION_HoverGrabPoint's SHAPE_Polygon logic, so they can share the same serialization format.
		[[fallthrough]];
	case EDITMODEACTION_HoverGrabPoint:
		switch(m_eShapeType)
		{
		case SHAPE_Box:

			break;
		case SHAPE_Circle:
			break;
		case SHAPE_LineSegment:
			break;
		case SHAPE_Polygon:
			break;
		case SHAPE_Capsule:
			break;

		default:
			HyGuiLog("GfxShapeModel::GetActionSerialized - Unknown shape type encountered", LOGTYPE_Error);
			break;
		}
		break;

	default:
		HyGuiLog("GfxShapeModel::GetActionSerialized - Invalid m_eCurTransform", LOGTYPE_Error);
		break;
	}

	return QList<float>();
}

/*virtual*/ QString GfxShapeModel::DoDeserialize(const QList<float> &floatList) /*override*/
{
	if(m_ShapeList.empty())
		m_ShapeList.push_back(new HyShape2d());

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Assemble `m_GrabPointList` and `m_GrabPointCenter`
	std::vector<glm::vec2> grabPointList;
	if(floatList.empty() == false)
	{
		glm::vec2 ptCentroid;
		if(m_eShapeType != SHAPE_Polygon)
		{
			grabPointList = m_ShapeList[0]->DeserializeSelf(HyGlobal::ConvertShapeToFixtureType(m_eShapeType), std::vector<float>(floatList.begin(), floatList.end()));
			m_ShapeList[0]->GetCentroid(ptCentroid);
		}
		else // SHAPE_Polygon deserialization
		{	
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
		}
		m_GrabPointCenter.Setup(ptCentroid);
	}

	// grabPointList is allowed to be empty, otherwise it will have proper data for the shape type
	if(grabPointList.empty())
		m_GrabPointList.clear();
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
				m_GrabPointList[i].Setup(GRABPOINT_ShapeCtrlAll, grabPointList[i]);
			break;

		case SHAPE_Circle:
			if(grabPointList.size() != 4)
				return "Invalid circle shape data";

			m_GrabPointList.resize(4);
			m_GrabPointList[0].Setup(GRABPOINT_ShapeCtrlVert, grabPointList[0]); // Top
			m_GrabPointList[1].Setup(GRABPOINT_ShapeCtrlHorz, grabPointList[1]); // Right
			m_GrabPointList[2].Setup(GRABPOINT_ShapeCtrlVert, grabPointList[2]); // Bottom
			m_GrabPointList[3].Setup(GRABPOINT_ShapeCtrlHorz, grabPointList[3]); // Left
			break;

		case SHAPE_LineSegment:
			if(grabPointList.size() != 2)
				return "Invalid line segment shape data";

			m_GrabPointList.resize(2);
			m_GrabPointList[0].Setup(GRABPOINT_ShapeCtrlAll, grabPointList[0]);
			m_GrabPointList[1].Setup(GRABPOINT_ShapeCtrlAll, grabPointList[1]);
			break;

		case SHAPE_Polygon:
			// Preserve existing grab points where possible (keeps selection)
			for(int i = 0; i < grabPointList.size(); ++i)
			{
				if(static_cast<int>(m_GrabPointList.size()) - 1 < i)
					m_GrabPointList.push_back(GfxGrabPointModel(GRABPOINT_Vertex, grabPointList[i]));
				else
					m_GrabPointList[i].Setup(m_GrabPointList[i].IsSelected() ? GRABPOINT_VertexSelected : GRABPOINT_Vertex, grabPointList[i]);
			}
			if(static_cast<int>(m_GrabPointList.size()) > static_cast<int>(grabPointList.size())) // Truncate to new size
				m_GrabPointList.resize(grabPointList.size());

			if(m_bLoopClosed == false && m_GrabPointList.size() > 1)
			{
				m_GrabPointList.front().Setup(m_GrabPointList.front().IsSelected() ? GRABPOINT_EndpointSelected : GRABPOINT_Endpoint);
				m_GrabPointList.back().Setup(m_GrabPointList.back().IsSelected() ? GRABPOINT_EndpointSelected : GRABPOINT_Endpoint);
			}
			break;

		case SHAPE_Capsule:
			if(grabPointList.size() != 6)
				return "Invalid capsule shape data";

			m_GrabPointList.resize(6);
			m_GrabPointList[0].Setup(GRABPOINT_ShapeCtrlVert, grabPointList[0]); // Pt1
			m_GrabPointList[1].Setup(GRABPOINT_ShapeCtrlVert, grabPointList[1]); // Pt2
			m_GrabPointList[2].Setup(GRABPOINT_ShapeCtrlHorz, grabPointList[2]); // Center1 Right
			m_GrabPointList[3].Setup(GRABPOINT_ShapeCtrlHorz, grabPointList[3]); // Center1 Left
			m_GrabPointList[4].Setup(GRABPOINT_ShapeCtrlHorz, grabPointList[4]); // Center2 Right
			m_GrabPointList[5].Setup(GRABPOINT_ShapeCtrlHorz, grabPointList[5]); // Center2 Left
			break;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	if(m_GrabPointList.empty())
	{
		for(HyShape2d *pFixture : m_ShapeList)
			pFixture->SetAsNothing();

		if(m_eShapeType != SHAPE_None)
			return "No shape data provided";
	}
	else if(m_eShapeType == SHAPE_Polygon) // Assemble `m_FixtureList` with valid sub-polygons (convex and <= 8 vertices)
	{
		std::vector<glm::vec2> ccwOrderedVertexList;
		ccwOrderedVertexList.reserve(m_GrabPointList.size());
		for(const GfxGrabPointModel &grabPt : m_GrabPointList)
			ccwOrderedVertexList.emplace_back(grabPt.GetPos());

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
					return "Polygon has self-intersecting edges";
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

	return QString();
}

/*virtual*/ EditModeAction GfxShapeModel::DoMouseMoveIdle(glm::vec2 ptWorldMousePos) /*override*/
{
	m_iGrabPointIndex = -1;
	m_ptGrabPointPos = glm::vec2(0.0f, 0.0f);

	for(int i = 0; i < m_GrabPointList.size(); ++i)
	{
		if(m_GrabPointList[i].TestPoint(ptWorldMousePos))
		{
			m_iGrabPointIndex = i;
			return EDITMODEACTION_HoverGrabPoint;
		}
	}
	if(m_GrabPointCenter.TestPoint(ptWorldMousePos))
		return EDITMODEACTION_HoverCenter;

	if(m_eShapeType == SHAPE_Polygon)
	{
		if(m_GrabPointList.empty())
			return EDITMODEACTION_Creation;

		if(CheckIfAddVertexOnEdge(ptWorldMousePos))
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
					m_ptGrabPointPos = ptWorldMousePos;
					return EDITMODEACTION_AppendVertex;
				}
				else if(m_GrabPointList.back().IsSelected())
				{
					m_iGrabPointIndex = m_GrabPointList.size();
					m_ptGrabPointPos = ptWorldMousePos;
					return EDITMODEACTION_AppendVertex;
				}
			}
		}
	}
	else if(IsValidModel() == false) // Any shape besides SHAPE_Polygon
		return EDITMODEACTION_Creation;

	for(HyShape2d *pFixture : m_ShapeList)
	{
		if(pFixture->TestPoint(ptWorldMousePos, glm::identity<glm::mat4>()))
			return EDITMODEACTION_Inside;
	}
	
	return EDITMODEACTION_Outside;
}

void GfxShapeModel::DoTransformCreation(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos)
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

	switch(m_eShapeType)
	{
	case SHAPE_Box:
		m_ShapeList[0]->SetAsBox(HyRect((ptUpperBound.x - ptLowerBound.x) * 0.5f, (ptUpperBound.y - ptLowerBound.y) * 0.5f, ptCenter, 0.0f));
		break;
	case SHAPE_Circle:
		m_ShapeList[0]->SetAsCircle(ptCenter, glm::distance(ptCenter, ptUpperBound));
		break;
	case SHAPE_Capsule: {
		glm::vec2 pt1, pt2;
		pt1.x = ptCenter.x;
		pt1.y = ptLowerBound.y;
		pt2.x = ptCenter.x;
		pt2.y = ptUpperBound.y;
		float fRadius = 0.5f * glm::distance(pt1, pt2);
		m_ShapeList[0]->SetAsCapsule(ptStartPos, ptDragPos, fRadius);
		break; }
	case SHAPE_LineSegment:
		m_ShapeList[0]->SetAsLineSegment(ptStartPos, ptDragPos);
		break;
	case SHAPE_Polygon:
		if(m_GrabPointList.size() == 0)
			m_GrabPointList.append(GfxGrabPointModel(GRABPOINT_Endpoint, ptDragPos));
		if(m_GrabPointList.size() == 1)
			m_GrabPointList.append(GfxGrabPointModel(GRABPOINT_Endpoint, ptDragPos));
		if(m_GrabPointList.size() != 2)
			HyGuiLog("GfxShapeModel::DoTransformCreation - Polygon or LineChain initial dragging with != 2 verts", LOGTYPE_Error);

		m_GrabPointList[0].SetSelected(false);
		m_GrabPointList[1].Setup(GRABPOINT_EndpointSelected, ptDragPos);
		break;

	default:
		HyGuiLog("GfxShapeModel::DoTransformCreation - Initial transform called with unsupported shape type: " % QString::number(m_eShapeType), LOGTYPE_Error);
		break;
	}
}

bool GfxShapeModel::CheckIfAddVertexOnEdge(glm::vec2 ptWorldMousePos)
{
	if(m_eShapeType != SHAPE_Polygon)
	{
		HyGuiLog("GfxShapeModel::CheckIfAddVertexOnEdge invoked with shape that isn't a linechain or polygon", LOGTYPE_Error);
		return false;
	}
	if(m_GrabPointList.size() < 2)
		return false;

	HyShape2d tmpEdgeShape;
	for(int i = 0; i < m_GrabPointList.size(); ++i)
	{
		if(i == (m_GrabPointList.size() - 1) && m_bLoopClosed == false)
			break;

		glm::vec2 pt1 = m_GrabPointList[i].GetPos();
		glm::vec2 pt2 = m_GrabPointList[(i + 1) % m_GrabPointList.size()].GetPos();
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

bool GfxShapeModel::IsShareEdge(const std::vector<glm::vec2> &a, const std::vector<glm::vec2> &b, int &a0, int &a1, int &b0, int &b1)
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

std::vector<glm::vec2> GfxShapeModel::MergePolygons(const std::vector<glm::vec2> &ptA, const std::vector<glm::vec2> &ptB, int a0, int a1, int b0, int b1)
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

std::vector<std::vector<glm::vec2>> GfxShapeModel::MergeTriangles(const std::vector<HyTriangle2d> &triangleList)
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

void GfxShapeModel::ClearFixtures()
{
	for(HyShape2d *pFixture : m_ShapeList)
		delete pFixture;
	m_ShapeList.clear();
}

void GfxShapeModel::AssemblePolygonFixtures(std::vector<std::vector<glm::vec2>> subPolygonList)
{
	ClearFixtures();
	for(std::vector<glm::vec2> &subPoly : subPolygonList)
	{
		HyShape2d *pShape2d = new HyShape2d();
		pShape2d->SetAsPolygon(subPoly);
		m_ShapeList.push_back(pShape2d);
	}
}

QList<float> GfxShapeModel::ConvertedBoxData() const
{
	HyShape2d tmpBoxShape;
	switch(m_eShapeType)
	{
	case SHAPE_None:
		break;
	case SHAPE_Box:
		return Serialize();
	case SHAPE_Circle:
		if(m_ShapeList[0]->IsValid())
		{
			b2Circle b2Circ = m_ShapeList[0]->GetAsCircle();
			glm::vec2 ptCenter(b2Circ.center.x, b2Circ.center.y);
			float fRadius = b2Circ.radius;
			tmpBoxShape.SetAsBox(HyRect(fRadius, fRadius, ptCenter, 0.0f));
			std::vector<float> serializedTmpBox = tmpBoxShape.SerializeSelf();
			return QList<float>(serializedTmpBox.begin(), serializedTmpBox.end());
		}
		break;
	case SHAPE_LineSegment:
		if(m_ShapeList[0]->IsValid())
		{
			b2Segment b2LineSeg = m_ShapeList[0]->GetAsSegment();
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
		if(m_ShapeList[0]->IsValid())
		{
			b2Capsule b2Cap = m_ShapeList[0]->GetAsCapsule();
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
	case SHAPE_Polygon: {
		std::vector<glm::vec2> vertexList;
		for(const GfxGrabPointModel &grabPt : m_GrabPointList)
			vertexList.push_back(grabPt.GetPos());
		
		b2AABB boundingBox;
		HyMath::ComputeAABB(boundingBox, vertexList.data(), static_cast<int>(vertexList.size()), 5.0f);

		glm::vec2 ptCenter = glm::vec2(0.5f * (boundingBox.lowerBound.x + boundingBox.upperBound.x), 0.5f * (boundingBox.lowerBound.y + boundingBox.upperBound.y));
		glm::vec2 vHalfExtents = glm::vec2(0.5f * (boundingBox.upperBound.x - boundingBox.lowerBound.x), 0.5f * (boundingBox.upperBound.y - boundingBox.lowerBound.y));
		tmpBoxShape.SetAsBox(HyRect(vHalfExtents.x, vHalfExtents.y, ptCenter, 0.0f));
		std::vector<float> serializedTmpBox = tmpBoxShape.SerializeSelf();
		return QList<float>(serializedTmpBox.begin(), serializedTmpBox.end()); }

	default:
		HyGuiLog("GfxShapeModel::SetType - Unhandled conversion to Box", LOGTYPE_Error);
		break;
	}
	
	return QList<float>();
}

QList<float> GfxShapeModel::ConvertedCircleData() const
{
	QList<float> convertedDataList;
	switch(m_eShapeType)
	{
	case SHAPE_None:
		break;
	case SHAPE_Box:
		if(m_ShapeList[0]->IsValid())
		{
			b2Polygon b2Poly = m_ShapeList[0]->GetAsPolygon();
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
		return Serialize();
	case SHAPE_LineSegment:
		if(m_ShapeList[0]->IsValid())
		{
			b2Segment b2LineSeg = m_ShapeList[0]->GetAsSegment();
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
		if(m_ShapeList[0]->IsValid())
		{
			b2Capsule b2Cap = m_ShapeList[0]->GetAsCapsule();
			glm::vec2 pt1(b2Cap.center1.x, b2Cap.center1.y);
			glm::vec2 pt2(b2Cap.center2.x, b2Cap.center2.y);
			glm::vec2 ptCenter = (pt1 + pt2) * 0.5f;
			float fRadius = glm::distance(pt1, pt2) * 0.5f + b2Cap.radius;
			convertedDataList.append(ptCenter.x);
			convertedDataList.append(ptCenter.y);
			convertedDataList.append(fRadius);
		}
		break;

	case SHAPE_Polygon: {
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
			convertedDataList.append(ptCenter.x);
			convertedDataList.append(ptCenter.y);
			convertedDataList.append(fRadius);
		}
		break; }
	
	default:
		HyGuiLog("GfxShapeModel::SetType - Unhandled conversion to Circle", LOGTYPE_Error);
		break;
	}
	
	return convertedDataList;
}

QList<float> GfxShapeModel::ConvertedLineSegmentData() const
{
	QList<float> convertedDataList;
	switch(m_eShapeType)
	{
	case SHAPE_None:
		break;
	case SHAPE_Box:
		if(m_ShapeList[0]->IsValid())
		{
			b2Polygon b2Poly = m_ShapeList[0]->GetAsPolygon();
			convertedDataList.append(b2Poly.vertices[0].x);
			convertedDataList.append(b2Poly.vertices[0].y);
			convertedDataList.append(b2Poly.vertices[2].x);
			convertedDataList.append(b2Poly.vertices[2].y);
		}
		break;
	case SHAPE_Circle:
		if(m_ShapeList[0]->IsValid())
		{
			b2Circle b2Circ = m_ShapeList[0]->GetAsCircle();
			glm::vec2 ptCenter(b2Circ.center.x, b2Circ.center.y);
			float fRadius = b2Circ.radius;
			convertedDataList.append(ptCenter.x - fRadius);
			convertedDataList.append(ptCenter.y);
			convertedDataList.append(ptCenter.x + fRadius);
			convertedDataList.append(ptCenter.y);
		}
		break;
	case SHAPE_LineSegment:
		return Serialize();
	case SHAPE_Capsule:
		if(m_ShapeList[0]->IsValid())
		{
			b2Capsule b2Cap = m_ShapeList[0]->GetAsCapsule();
			glm::vec2 pt1(b2Cap.center1.x, b2Cap.center1.y);
			glm::vec2 pt2(b2Cap.center2.x, b2Cap.center2.y);
			convertedDataList.append(pt1.x);
			convertedDataList.append(pt1.y);
			convertedDataList.append(pt2.x);
			convertedDataList.append(pt2.y);
		}
		break;
	case SHAPE_Polygon: {
		std::vector<glm::vec2> vertexList;
		for(const GfxGrabPointModel &grabPt : m_GrabPointList)
			vertexList.push_back(grabPt.GetPos());
		
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
		HyGuiLog("GfxShapeModel::SetType - Unhandled conversion to LineSegment", LOGTYPE_Error);
		break;
	}
	
	return convertedDataList;
}

QList<float> GfxShapeModel::ConvertedCapsuleData() const
{
	QList<float> convertedDataList;
	switch(m_eShapeType)
	{
	case SHAPE_None:
		break;
	case SHAPE_Box:
		if(m_ShapeList[0]->IsValid())
		{
			b2Polygon b2Poly = m_ShapeList[0]->GetAsPolygon();
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
		if(m_ShapeList[0]->IsValid())
		{
			b2Circle b2Circ = m_ShapeList[0]->GetAsCircle();
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
		return Serialize();
	case SHAPE_Polygon:
	case SHAPE_LineSegment: {
		std::vector<glm::vec2> vertexList;
		for(const GfxGrabPointModel &grabPt : m_GrabPointList)
			vertexList.push_back(grabPt.GetPos());
		
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
		HyGuiLog("GfxShapeModel::SetType - Unhandled conversion to Capsule", LOGTYPE_Error);
		break;
	}

	return convertedDataList;
}

QList<float> GfxShapeModel::ConvertedPolygonOrLineChainData() const
{
	QList<float> convertedDataList;
	switch(m_eShapeType)
	{
	case SHAPE_None:
		break;
	case SHAPE_Box:
		if(m_ShapeList[0]->IsValid())
		{
			b2Polygon b2Poly = m_ShapeList[0]->GetAsPolygon();
			for(int i = 0; i < b2Poly.count; ++i)
			{
				convertedDataList.append(b2Poly.vertices[i].x);
				convertedDataList.append(b2Poly.vertices[i].y);
			}
			convertedDataList.append(1.0f); // Closed loop
		}
		break;
	case SHAPE_Circle:
		if(m_ShapeList[0]->IsValid())
		{
			b2Circle b2Circ = m_ShapeList[0]->GetAsCircle();
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
		if(m_ShapeList[0]->IsValid())
		{
			b2Segment b2LineSeg = m_ShapeList[0]->GetAsSegment();
			convertedDataList.append(b2LineSeg.point1.x);
			convertedDataList.append(b2LineSeg.point1.y);
			convertedDataList.append(b2LineSeg.point2.x);
			convertedDataList.append(b2LineSeg.point2.y);
			convertedDataList.append(0.0f); // Open loop
		}
		break;
	case SHAPE_Polygon:
		convertedDataList = Serialize();
		break;
	case SHAPE_Capsule:
		if(m_ShapeList[0]->IsValid())
		{
			b2Capsule b2Cap = m_ShapeList[0]->GetAsCapsule();
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
		HyGuiLog("GfxShapeModel::SetType - Unhandled conversion to Polygon/LineChain", LOGTYPE_Error);
		break;
	}
	
	return convertedDataList;
}
