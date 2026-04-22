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
	IGfxEditModel(EDITMODETYPE_Shape, color)
{
}

/*virtual*/ GfxShapeModel::~GfxShapeModel()
{
	ClearFixtures();
}

EditorShape GfxShapeModel::GetShapeType() const
{
	return m_eShapeType;
}

void GfxShapeModel::SetShapeType(EditorShape eNewShape, QList<float> floatList)
{
	if(floatList.empty())
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
	m_FixtureList.push_back(new HyShape2d());

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



HyShape2d *GfxShapeModel::GetShape(int iIndex)
{
	if(iIndex < 0 || iIndex >= m_FixtureList.size())
		return nullptr;
	return static_cast<HyShape2d *>(m_FixtureList[iIndex]);
}

const HyShape2d *GfxShapeModel::GetShape(int iIndex) const
{
	if(iIndex < 0 || iIndex >= m_FixtureList.size())
		return nullptr;
	return static_cast<const HyShape2d *>(m_FixtureList[iIndex]);
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

///*virtual*/ QJsonObject GfxShapeModel::GetActionSerialized() const /*override*/
//{
//	switch(m_eCurAction)
//	{
//	case EDITMODEACTION_Creation:
//		return Serialize();
//
//	case EDITMODEACTION_Inside:
//		if(IsAllGrabPointsSelected() == false)
//			HyGuiLog("GfxShapeModel::GetActionSerialized - EDITMODEACTION_Inside with not all grab points selected", LOGTYPE_Error);
//		[[fallthrough]];
//	case EDITMODEACTION_HoverCenter:
//		if(m_eShapeType != SHAPE_Polygon)
//		{
//			if(m_FixtureList.size() != 1)
//				HyGuiLog("GfxShapeModel::GetActionSerialized - Expected exactly one shape fixture for non-polygon shape type", LOGTYPE_Error);
//			
//			// Translate entire shape by the drag delta.
//			HyShape2d tmpShape = *GetShape(0);
//			tmpShape.TransformSelf(glm::translate(glm::mat4(1.0f), glm::vec3(m_vDragDelta, 0.0f)));
//
//			std::vector<float> serializedData = tmpShape.SerializeSelf();
//			QJsonObject serializedObj;
//			serializedObj.insert("type", HyGlobal::ShapeName(m_eShapeType));
//			QJsonArray dataArray;
//			for(float f : serializedData)
//				dataArray.push_back(f);
//			serializedObj.insert("data", dataArray);
//			serializedObj.insert("outline", m_fOutline);
//			return serializedObj;
//		}
//		else // SHAPE_Polygon
//		{
//			QList<float> returnList;
//			for(const GfxGrabPointModel &grabPt : m_GrabPointList)
//			{
//				glm::vec2 ptVertex = grabPt.GetPos();
//				returnList.push_back(static_cast<float>(ptVertex.x));
//				returnList.push_back(static_cast<float>(ptVertex.y));
//			}
//
//			// Translate entire shape by the drag delta.
//			for(int i = 0; i < returnList.size(); i += 2)
//			{
//				returnList[i] += m_vDragDelta.x;
//				returnList[i + 1] += m_vDragDelta.y;
//			}
//
//			returnList.push_back(m_bLoopClosed ? 1.0f : 0.0f); // Final float indicates whether loop is closed
//
//			QJsonObject serializedObj;
//			serializedObj.insert("type", HyGlobal::ShapeName(m_eShapeType));
//			QJsonArray dataArray;
//			for(float f : returnList)
//				dataArray.push_back(f);
//			serializedObj.insert("data", dataArray);
//			serializedObj.insert("outline", m_fOutline);
//
//			return serializedObj;
//		}
//		break;
//
//	case EDITMODEACTION_AppendVertex:
//	case EDITMODEACTION_InsertVertex:
//		// Guaranteed to be SHAPE_Polygon, translate all (selected) vertices by the drag delta.
//		// AppendVertex/InsertVertex both work with EDITMODEACTION_HoverGrabPoint's SHAPE_Polygon logic, so they can share the same serialization format.
//		[[fallthrough]];
//	case EDITMODEACTION_HoverGrabPoint:
//		switch(m_eShapeType)
//		{
//		case SHAPE_Box:
//
//			break;
//		case SHAPE_Circle:
//			break;
//		case SHAPE_LineSegment:
//			break;
//		case SHAPE_Polygon:
//			break;
//		case SHAPE_Capsule:
//			break;
//
//		default:
//			HyGuiLog("GfxShapeModel::GetActionSerialized - Unknown shape type encountered", LOGTYPE_Error);
//			break;
//		}
//		break;
//
//	default:
//		HyGuiLog("GfxShapeModel::GetActionSerialized - Invalid m_eCurTransform", LOGTYPE_Error);
//		break;
//	}
//
//	return QJsonObject();
//}

/*virtual*/ QString GfxShapeModel::DoDeserialize(const QJsonObject &serializedObj) /*override*/
{
	
}

/*virtual*/ EditModeAction GfxShapeModel::DoMouseMoveIdle() /*override*/
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

	if(m_eShapeType == SHAPE_Polygon)
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

	for(IHyFixture2d *pFixture : m_FixtureList)
	{
		glm::vec2 ptWorldMousePos;
		if(HyEngine::Input().GetWorldMousePos(ptWorldMousePos) == false)
			HyGuiLog("GfxShapeModel::DoMouseMoveIdle - Failed to get world mouse position for hit testing", LOGTYPE_Error);

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
	case SHAPE_Box: {
		GetShape(0)->SetAsBox(HyRect((ptUpperBound.x - ptLowerBound.x) * 0.5f, (ptUpperBound.y - ptLowerBound.y) * 0.5f, ptCenter, 0.0f));
		m_GrabPointList.clear();
		std::vector<glm::vec2> lineGrabPoints = GetShape(0)->CalcGrabPoints();
		for(glm::vec2 &ptGrab : lineGrabPoints)
			m_GrabPointList.append(GfxGrabPointModel(GRABPOINT_Endpoint, ptGrab));
		break; }

	case SHAPE_Circle: {
		GetShape(0)->SetAsCircle(ptCenter, glm::distance(ptCenter, ptUpperBound));
		m_GrabPointList.clear();
		std::vector<glm::vec2> lineGrabPoints = GetShape(0)->CalcGrabPoints();
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
		GetShape(0)->SetAsCapsule(ptStartPos, ptDragPos, fRadius);
		m_GrabPointList.clear();
		std::vector<glm::vec2> lineGrabPoints = GetShape(0)->CalcGrabPoints();
		for(glm::vec2 &ptGrab : lineGrabPoints)
			m_GrabPointList.append(GfxGrabPointModel(GRABPOINT_Endpoint, ptGrab));
		break; }

	case SHAPE_LineSegment: {
		GetShape(0)->SetAsLineSegment(ptStartPos, ptDragPos);
		m_GrabPointList.clear();
		std::vector<glm::vec2> lineGrabPoints = GetShape(0)->CalcGrabPoints();
		for(glm::vec2 &ptGrab : lineGrabPoints)
			m_GrabPointList.append(GfxGrabPointModel(GRABPOINT_Endpoint, ptGrab));
		break; }

	case SHAPE_Polygon:
		m_GrabPointList.clear();
		m_GrabPointList.append(GfxGrabPointModel(GRABPOINT_Endpoint, ptDragPos));
		m_GrabPointList.append(GfxGrabPointModel(GRABPOINT_Endpoint, ptDragPos));
		
		m_GrabPointList[0].SetSelected(false);
		m_GrabPointList[1].Set(GRABPOINT_EndpointSelected, ptDragPos);
		m_sMalformedReason = "Incomplete shape";
		break;

	default:
		HyGuiLog("GfxShapeModel::DoTransformCreation - Initial transform called with unsupported shape type: " % QString::number(m_eShapeType), LOGTYPE_Error);
		break;
	}
}

bool GfxShapeModel::CheckIfAddVertexOnEdge()
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

		// TODO: Test whether to convert all this to camera space
		glm::vec2 pt1 = m_GrabPointList[i].GetPos();
		glm::vec2 pt2 = m_GrabPointList[(i + 1) % m_GrabPointList.size()].GetPos();

		glm::vec2 ptWorldMousePos;
		if(HyEngine::Input().GetWorldMousePos(ptWorldMousePos) == false)
		{
			HyGuiLog("GfxShapeModel::CheckIfAddVertexOnEdge - Failed to get world mouse position for hit testing", LOGTYPE_Error);
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








QList<float> GfxShapeModel::ConvertedBoxData() const
{
	HyShape2d tmpBoxShape;
	switch(m_eShapeType)
	{
	case SHAPE_None:
		break;
	case SHAPE_Box:
		return SerializeData();
	case SHAPE_Circle:
		if(GetShape(0)->IsValid())
		{
			b2Circle b2Circ = GetShape(0)->GetAsCircle();
			glm::vec2 ptCenter(b2Circ.center.x, b2Circ.center.y);
			float fRadius = b2Circ.radius;
			tmpBoxShape.SetAsBox(HyRect(fRadius, fRadius, ptCenter, 0.0f));
			std::vector<float> serializedTmpBox = tmpBoxShape.SerializeSelf();
			return QList<float>(serializedTmpBox.begin(), serializedTmpBox.end());
		}
		break;
	case SHAPE_LineSegment:
		if(GetShape(0)->IsValid())
		{
			b2Segment b2LineSeg = GetShape(0)->GetAsSegment();
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
		if(GetShape(0)->IsValid())
		{
			b2Capsule b2Cap = GetShape(0)->GetAsCapsule();
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
		if(GetShape(0)->IsValid())
		{
			b2Polygon b2Poly = GetShape(0)->GetAsPolygon();
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
		return SerializeData();
	case SHAPE_LineSegment:
		if(GetShape(0)->IsValid())
		{
			b2Segment b2LineSeg = GetShape(0)->GetAsSegment();
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
		if(GetShape(0)->IsValid())
		{
			b2Capsule b2Cap = GetShape(0)->GetAsCapsule();
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
		if(GetShape(0)->IsValid())
		{
			b2Polygon b2Poly = GetShape(0)->GetAsPolygon();
			convertedDataList.append(b2Poly.vertices[0].x);
			convertedDataList.append(b2Poly.vertices[0].y);
			convertedDataList.append(b2Poly.vertices[2].x);
			convertedDataList.append(b2Poly.vertices[2].y);
		}
		break;
	case SHAPE_Circle:
		if(GetShape(0)->IsValid())
		{
			b2Circle b2Circ = GetShape(0)->GetAsCircle();
			glm::vec2 ptCenter(b2Circ.center.x, b2Circ.center.y);
			float fRadius = b2Circ.radius;
			convertedDataList.append(ptCenter.x - fRadius);
			convertedDataList.append(ptCenter.y);
			convertedDataList.append(ptCenter.x + fRadius);
			convertedDataList.append(ptCenter.y);
		}
		break;
	case SHAPE_LineSegment:
		return SerializeData();
	case SHAPE_Capsule:
		if(GetShape(0)->IsValid())
		{
			b2Capsule b2Cap = GetShape(0)->GetAsCapsule();
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
		if(GetShape(0)->IsValid())
		{
			b2Polygon b2Poly = GetShape(0)->GetAsPolygon();
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
		if(GetShape(0)->IsValid())
		{
			b2Circle b2Circ = GetShape(0)->GetAsCircle();
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
		return SerializeData();
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
		if(GetShape(0)->IsValid())
		{
			b2Polygon b2Poly = GetShape(0)->GetAsPolygon();
			for(int i = 0; i < b2Poly.count; ++i)
			{
				convertedDataList.append(b2Poly.vertices[i].x);
				convertedDataList.append(b2Poly.vertices[i].y);
			}
			convertedDataList.append(1.0f); // Closed loop
		}
		break;
	case SHAPE_Circle:
		if(GetShape(0)->IsValid())
		{
			b2Circle b2Circ = GetShape(0)->GetAsCircle();
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
		if(GetShape(0)->IsValid())
		{
			b2Segment b2LineSeg = GetShape(0)->GetAsSegment();
			convertedDataList.append(b2LineSeg.point1.x);
			convertedDataList.append(b2LineSeg.point1.y);
			convertedDataList.append(b2LineSeg.point2.x);
			convertedDataList.append(b2LineSeg.point2.y);
			convertedDataList.append(0.0f); // Open loop
		}
		break;
	case SHAPE_Polygon:
		convertedDataList = SerializeData();
		break;
	case SHAPE_Capsule:
		if(GetShape(0)->IsValid())
		{
			b2Capsule b2Cap = GetShape(0)->GetAsCapsule();
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


