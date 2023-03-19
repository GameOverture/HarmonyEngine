/**************************************************************************
 *	TransformCtrl.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "TransformCtrl.h"
#include "EntityDrawItem.h"

GrabPoint::GrabPoint(HyColor outlineColor, HyColor fillColor, HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_GrabOutline(this),
	m_GrabFill(this)
{
	const float fRADIUS = 5.0f;
	m_GrabOutline.SetTint(outlineColor);
	m_GrabOutline.SetAsCircle(fRADIUS);

	m_GrabFill.SetTint(fillColor);
	m_GrabFill.SetAsCircle(fRADIUS - 1.0f);

	UseWindowCoordinates(0);
}

/*virtual*/ GrabPoint::~GrabPoint()
{
}

void GrabPoint::GetLocalBoundingShape(HyShape2d &shapeRefOut)
{
	m_GrabOutline.CalcLocalBoundingShape(shapeRefOut);
}

TransformCtrl::TransformCtrl(HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_bIsShown(false),
	m_bShowGrabPoints(false),
	m_fCachedRotation(0.0f)
{
	m_BoundingVolume.SetTint(HyColor::Blue.Lighten());
	m_BoundingVolume.SetWireframe(true);
	ChildAppend(m_BoundingVolume);

	m_ExtrudeSegment.SetTint(HyColor::Blue.Lighten());
	m_ExtrudeSegment.SetWireframe(true);
	ChildAppend(m_ExtrudeSegment);

	for(uint i = 0; i < GRAB_Rotate; ++i)
		m_GrabPoints[i] = new GrabPoint(HyColor::White, HyColor::Blue.Lighten(), this);
	m_GrabPoints[GRAB_Rotate] = new GrabPoint(HyColor::Blue.Lighten(), HyColor::White, this);

	UseWindowCoordinates(0);
	SetDisplayOrder(DISPLAYORDER_TransformCtrl);

	Hide();
}

/*virtual*/ TransformCtrl::~TransformCtrl()
{
	for(uint i = 0; i < NUM_GRABPOINTS; ++i)
		delete m_GrabPoints[i];
}

void TransformCtrl::WrapTo(HyShape2d boundingShape, glm::mat4 mtxShapeTransform, HyCamera2d *pCamera)
{
	if(boundingShape.IsValidShape() == false)
		return;

	if(boundingShape.GetType() == HYSHAPE_Polygon)
	{
		const b2PolygonShape *pPolyShape = static_cast<const b2PolygonShape *>(boundingShape.GetB2Shape());
		if(pPolyShape->m_count != 4)
		{
			HyLogError("TransformCtrl::WrapTo - HYSHAPE_Polygon shape did not have 4 vertices");
			return;
		}
		
		HySetVec(m_ptGrabPos[GRAB_BotLeft], pPolyShape->m_vertices[0].x, pPolyShape->m_vertices[0].y);
		HySetVec(m_ptGrabPos[GRAB_BotRight], pPolyShape->m_vertices[1].x, pPolyShape->m_vertices[1].y);
		HySetVec(m_ptGrabPos[GRAB_TopRight], pPolyShape->m_vertices[2].x, pPolyShape->m_vertices[2].y);
		HySetVec(m_ptGrabPos[GRAB_TopLeft], pPolyShape->m_vertices[3].x, pPolyShape->m_vertices[3].y);
		m_ptGrabPos[GRAB_BotMid] = m_ptGrabPos[GRAB_BotLeft] + ((m_ptGrabPos[GRAB_BotRight] - m_ptGrabPos[GRAB_BotLeft]) * 0.5f);
		m_ptGrabPos[GRAB_MidRight] = m_ptGrabPos[GRAB_BotRight] + ((m_ptGrabPos[GRAB_TopRight] - m_ptGrabPos[GRAB_BotRight]) * 0.5f);
		m_ptGrabPos[GRAB_TopMid] = m_ptGrabPos[GRAB_TopLeft] + ((m_ptGrabPos[GRAB_TopRight] - m_ptGrabPos[GRAB_TopLeft]) * 0.5f);
		m_ptGrabPos[GRAB_MidLeft] = m_ptGrabPos[GRAB_BotLeft] + ((m_ptGrabPos[GRAB_TopLeft] - m_ptGrabPos[GRAB_BotLeft]) * 0.5f);
		
		// Create the top rotate grab anchor - Along the top edge, find the midpoint and extrude a line segment out perpendicularly
		glm::vec2 vTopEdge = m_ptGrabPos[GRAB_TopRight] - m_ptGrabPos[GRAB_TopLeft];
		glm::vec2 vExtrudeDir = HyMath::PerpendicularCounterClockwise(vTopEdge);
		vExtrudeDir = glm::normalize(vExtrudeDir);
		vExtrudeDir *= 50.0f;  // 50px line segment length
		vTopEdge *= 0.5f;
		glm::vec2 ptExtrudeStart = m_ptGrabPos[GRAB_TopLeft] + vTopEdge;
		m_ptGrabPos[GRAB_Rotate] = ptExtrudeStart + vExtrudeDir;

		
		for(int i = 0; i < NUM_GRABPOINTS; ++i)
		{
			glm::vec4 ptTransformPos(m_ptGrabPos[i].x, m_ptGrabPos[i].y, 0.0f, 1.0f);
			ptTransformPos = mtxShapeTransform * ptTransformPos;

			HySetVec(m_ptGrabPos[i], ptTransformPos.x, ptTransformPos.y);
			pCamera->ProjectToCamera(m_ptGrabPos[i], m_ptGrabPos[i]);

			m_GrabPoints[i]->pos.Set(m_ptGrabPos[i]);
			//m_GrabOutline[i].SetAsCircle(m_ptGrabPos[i], fRADIUS);
			//m_GrabFill[i].SetAsCircle(m_ptGrabPos[i], fRADIUS - 1.0f);
		}

		m_BoundingVolume.SetAsPolygon(m_ptGrabPos, 4);

		glm::vec4 ptTransformPos(ptExtrudeStart.x, ptExtrudeStart.y, 0.0f, 1.0f);
		ptTransformPos = mtxShapeTransform * ptTransformPos;
		HySetVec(ptExtrudeStart, ptTransformPos.x, ptTransformPos.y);
		pCamera->ProjectToCamera(ptExtrudeStart, ptExtrudeStart);
		m_ExtrudeSegment.SetAsLineSegment(ptExtrudeStart, m_ptGrabPos[GRAB_Rotate]);

		m_fCachedRotation = HyMath::AngleFromVector(m_ptGrabPos[GRAB_Rotate] - ptExtrudeStart) - 90.0f;
	}
}

void TransformCtrl::WrapTo(QList<EntityDrawItem *> itemDrawList, HyCamera2d *pCamera)
{
	HyShape2d boundingShape;

	if(itemDrawList.size() == 1)
	{
		glm::mat4 mtxShapeTransform;

		itemDrawList[0]->ExtractTransform(boundingShape, mtxShapeTransform);
		WrapTo(boundingShape, mtxShapeTransform, pCamera);

		return;
	}

	//// Determine the rotation of this transform control. If not specified, determine if all draw items share a common rotation and use that
	//if(fRotation != 0.0f)
	//	m_fCachedRotation = fRotation;
	//else 
	//{
	//	// Find common rotation using an int (truncating float values) of degrees
	//	int32 iCommonRotation = 99999; // Not yet set (valid values are 0-360)
	//	for(EntityDrawItem *pDrawItem : itemDrawList)
	//	{
	//		if(pDrawItem->GetGuiType() != ITEM_Shape)
	//		{
	//			float fItemRotation = pDrawItem->GetNodeChild()->rot.Get();

	//			if(iCommonRotation == 99999)
	//				iCommonRotation = static_cast<int32>(fItemRotation);

	//			if(iCommonRotation != static_cast<int32>(fItemRotation))
	//			{
	//				iCommonRotation = 99999;
	//				break;
	//			}
	//		}
	//	}
	//	if(iCommonRotation == 99999)
	//		iCommonRotation = 0;

	//	m_fCachedRotation = iCommonRotation;
	//}

	// Determine the center point of all the items and store in 'ptRotPivot'
	glm::vec3 ptRotPivot;
	b2AABB combinedAabb;
	HyMath::InvalidateAABB(combinedAabb);
	for(EntityDrawItem *pDrawItem : itemDrawList)
	{
		HyShape2d *pItemShape = new HyShape2d();
		glm::mat4 mtxItemTransform;
		pDrawItem->ExtractTransform(*pItemShape, mtxItemTransform);
		if(pItemShape->IsValidShape() == false)
		{
			delete pItemShape;
			continue;
		}

		b2AABB itemAabb;
		pItemShape->ComputeAABB(itemAabb, mtxItemTransform);

		if(combinedAabb.IsValid() == false)
			combinedAabb = itemAabb;
		else
			combinedAabb.Combine(itemAabb);
	}
	HySetVec(ptRotPivot, combinedAabb.GetCenter().x, combinedAabb.GetCenter().y, 0.0f);

	//// Recalculate 'combinedAabb' but transform each item with a rotation matrix ('ptRotPivot' and by 'm_fCachedRotation' degrees)
	//HyMath::InvalidateAABB(combinedAabb);
	//for(EntityDrawItem *pDrawItem : itemDrawList)
	//{
	//	HyShape2d *pItemShape = new HyShape2d();
	//	glm::mat4 mtxItemTransform;
	//	pDrawItem->ExtractTransform(*pItemShape, mtxItemTransform);
	//	if(pItemShape->IsValidShape() == false)
	//	{
	//		delete pItemShape;
	//		continue;
	//	}

	//	// Rotation compensation
	//	//glm::mat4 mtxRotateCompensate(1.0f);
	//	//mtxRotateCompensate = glm::translate(mtxRotateCompensate, ptRotPivot);
	//	//mtxRotateCompensate = glm::rotate(mtxRotateCompensate, glm::radians(-m_fCachedRotation), glm::vec3(0, 0, 1));
	//	//mtxRotateCompensate = glm::translate(mtxRotateCompensate, ptRotPivot * -1.0f);
	//	//mtxItemTransform *= mtxRotateCompensate;

	//		
	//	//glm::vec3 ptItemPos = mtxItemTransform[3];
	//	//glm::vec3 vRotations = glm::eulerAngles(glm::quat_cast(mtxItemTransform));

	//	//glm::mat4 mtxOut(1.0f);
	//	//mtxOut = glm::translate(mtxOut, ptItemPos);

	//	//mtxOut = glm::translate(mtxOut, ptRotPivot);
	//	//mtxOut = glm::rotate(mtxOut, glm::radians(rot.Extrapolate(fExtrapolatePercent)), glm::vec3(0, 0, 1));
	//	//mtxOut = glm::translate(mtxOut, ptRotPivot * -1.0f);



	//	// Rotation compensation
	//	mtxItemTransform = glm::translate(mtxItemTransform, ptRotPivot * -1.0f);
	//	mtxItemTransform = glm::rotate(mtxItemTransform, glm::radians(-m_fCachedRotation), glm::vec3(0, 0, 1));
	//	mtxItemTransform = glm::translate(mtxItemTransform, ptRotPivot);
	//	//glm::mat4 mtxRotateCompensate(1.0f);
	//	//mtxRotateCompensate = glm::translate(mtxRotateCompensate, ptRotPivot * -1.0f);
	//	//mtxRotateCompensate = glm::rotate(mtxRotateCompensate, glm::radians(-m_fCachedRotation), glm::vec3(0, 0, 1));
	//	//mtxRotateCompensate = glm::translate(mtxRotateCompensate, ptRotPivot);
	//	//mtxItemTransform *= mtxRotateCompensate;

	//	b2AABB itemAabb;
	//	pItemShape->ComputeAABB(itemAabb, mtxItemTransform);

	//	if(combinedAabb.IsValid() == false)
	//		combinedAabb = itemAabb;
	//	else
	//		combinedAabb.Combine(itemAabb);
	//}

	// Using 'combinedAabb' create an OBB in 'boundingShape', then rotate it back by the previous rotation compensation
	// Cannot use SetAsPolygon() because Box2d wields the vertices in an odd order
	boundingShape.SetAsBox(combinedAabb.GetExtents().x, combinedAabb.GetExtents().y, ptRotPivot, 0.0f);

	//glm::mat4 mtxRotateCompensate(1.0f);
	//mtxRotateCompensate = glm::translate(mtxRotateCompensate, ptRotPivot);
	//mtxRotateCompensate = glm::rotate(mtxRotateCompensate, glm::radians(m_fCachedRotation), glm::vec3(0, 0, 1));
	//mtxRotateCompensate = glm::translate(mtxRotateCompensate, ptRotPivot * -1.0f);
	//
	//boundingShape.TransformSelf(mtxRotateCompensate);
	WrapTo(boundingShape, glm::mat4(1.0f), pCamera);
}

bool TransformCtrl::IsShown() const
{
	return m_bIsShown;
}

void TransformCtrl::Show(bool bShowGrabPoints)
{
	m_bIsShown = true;
	m_bShowGrabPoints = bShowGrabPoints;

	for(int32 i = 0; i < NUM_GRABPOINTS; ++i)
		m_GrabPoints[i]->SetVisible(m_bShowGrabPoints);
	m_ExtrudeSegment.SetVisible(m_bShowGrabPoints);

	m_BoundingVolume.SetVisible(true);
}

void TransformCtrl::Hide()
{
	m_bIsShown = false;

	for(int32 i = 0; i < NUM_GRABPOINTS; ++i)
		m_GrabPoints[i]->SetVisible(false);

	m_BoundingVolume.SetVisible(false);
	m_ExtrudeSegment.SetVisible(false);
}

void TransformCtrl::GetCentroid(glm::vec2 &ptCenterOut) const
{
	m_BoundingVolume.GetCentroid(ptCenterOut);
}

glm::vec2 TransformCtrl::GetGrabPointWorldPos(GrabPointType eGrabPoint, HyCamera2d *pCamera) const
{
	HyAssert(eGrabPoint > GRAB_None && eGrabPoint < NUM_GRABPOINTS, "TransformCtrl::GetGrabPointPos invalid grab enum");

	glm::vec2 ptWorldPos;
	pCamera->ProjectToWorld(m_ptGrabPos[eGrabPoint], ptWorldPos);
	
	return ptWorldPos;
}

float TransformCtrl::GetCachedRotation() const
{
	return m_fCachedRotation;
}

bool TransformCtrl::IsMouseOverBoundingVolume()
{
	glm::vec2 ptWorldMousePos;
	if(m_bIsShown == false || HyEngine::Input().GetWorldMousePos(ptWorldMousePos) == false)
		return false;

	ptWorldMousePos = HyEngine::Input().GetMousePos();

	HyShape2d tmpShape;
	m_BoundingVolume.CalcLocalBoundingShape(tmpShape);
	
	return tmpShape.TestPoint(m_BoundingVolume.GetSceneTransform(0.0f), ptWorldMousePos);
}

TransformCtrl::GrabPointType TransformCtrl::IsMouseOverGrabPoint()
{
	glm::vec2 ptWorldMousePos;
	if(m_bIsShown == false || m_bShowGrabPoints == false || HyEngine::Input().GetWorldMousePos(ptWorldMousePos) == false)
		return GRAB_None;

	ptWorldMousePos = HyEngine::Input().GetMousePos();

	for(int32 i = 0; i < NUM_GRABPOINTS; ++i)
	{
		HyShape2d tmpShape;
		m_GrabPoints[i]->GetLocalBoundingShape(tmpShape);
		if(tmpShape.TestPoint(m_GrabPoints[i]->GetSceneTransform(0.0f), ptWorldMousePos))
			return static_cast<GrabPointType>(i);
	}

	return GRAB_None;
}

bool TransformCtrl::IsContained(const b2AABB &aabb, HyCamera2d *pCamera) const
{
	for(uint i = GRAB_BotLeft; i < 4; ++i)
	{
		glm::vec2 ptVertex;
		pCamera->ProjectToWorld(m_ptGrabPos[i], ptVertex);

		if( ptVertex.x < aabb.lowerBound.x ||
			ptVertex.y < aabb.lowerBound.y ||
			ptVertex.x > aabb.upperBound.x ||
			ptVertex.y > aabb.upperBound.y)
		{
			return false;
		}
	}

	return true;
}

ShapeCtrl::ShapeCtrl(HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_eShape(SHAPE_None),
	m_BoundingVolume(this),
	m_Outline(this)
{
	m_BoundingVolume.alpha.Set(0.25f);
	m_BoundingVolume.SetTint(HyColor::Blue.Lighten());

	m_Outline.UseWindowCoordinates();
	m_Outline.SetTint(HyColor::Blue.Lighten());
	m_Outline.SetWireframe(true);

	SetVisible(false);
}

/*virtual*/ ShapeCtrl::~ShapeCtrl()
{
}

EditorShape ShapeCtrl::GetShapeType() const
{
	return m_eShape;
}

void ShapeCtrl::SetShapeType(EditorShape eShape)
{
	if(m_eShape != SHAPE_None)
		ConvertTo(eShape);

	m_eShape = eShape;
}

HyPrimitive2d &ShapeCtrl::GetPrimitive(bool bWorldSpace)
{
	return bWorldSpace ? m_BoundingVolume : m_Outline;
}

void ShapeCtrl::SetAsDrag(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos, HyCamera2d *pCamera)
{
	SetVisible(true);

	glm::vec2 ptLowerBound, ptUpperBound, ptCenter;
	if(bShiftMod)
	{
		ptCenter = ptStartPos;

		glm::vec2 vRadius = ptStartPos - ptDragPos;
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

	glm::vec2 ptWindowLowerBound, ptWindowUpperBound, ptWindowCenter;
	pCamera->ProjectToCamera(ptLowerBound, ptWindowLowerBound);
	pCamera->ProjectToCamera(ptUpperBound, ptWindowUpperBound);
	ptWindowCenter = ptWindowLowerBound + ((ptWindowUpperBound - ptWindowLowerBound) * 0.5f);

	switch(m_eShape)
	{
	case SHAPE_None:
		Clear();
		break;

	case SHAPE_Box:
		m_BoundingVolume.SetAsBox((ptUpperBound.x - ptLowerBound.x) * 0.5f, (ptUpperBound.y - ptLowerBound.y) * 0.5f, ptCenter, 0.0f);
		m_Outline.SetAsBox((ptWindowUpperBound.x - ptWindowLowerBound.x) * 0.5f, (ptWindowUpperBound.y - ptWindowLowerBound.y) * 0.5f, ptWindowCenter, 0.0f);
		break;

	case SHAPE_Circle:
		m_BoundingVolume.SetAsCircle(ptCenter, glm::distance(ptCenter, ptUpperBound));
		m_Outline.SetAsCircle(ptWindowCenter, glm::distance(ptWindowCenter, ptWindowUpperBound));
		break;

	case SHAPE_Polygon: {
		glm::vec2 ptVertList[6];
		float fRadius = glm::distance(ptCenter, ptUpperBound);
		ptVertList[0] = ptCenter + glm::vec2(fRadius, 0.0f);
		ptVertList[1] = ptCenter + glm::vec2(fRadius * 0.5f, fRadius * sqrt(3.0f) * 0.5f);
		ptVertList[2] = ptCenter + glm::vec2(-fRadius * 0.5f, fRadius * sqrt(3.0f) * 0.5f);
		ptVertList[3] = ptCenter + glm::vec2(-fRadius, 0.0f);
		ptVertList[4] = ptCenter + glm::vec2(-fRadius * 0.5f, -fRadius * sqrt(3.0f) * 0.5f);
		ptVertList[5] = ptCenter + glm::vec2(fRadius * 0.5f, -fRadius * sqrt(3.0f) * 0.5f);
		m_BoundingVolume.SetAsPolygon(ptVertList, 6);

		fRadius = glm::distance(ptWindowCenter, ptWindowUpperBound);
		ptVertList[0] = ptWindowCenter + glm::vec2(fRadius, 0.0f);
		ptVertList[1] = ptWindowCenter + glm::vec2(fRadius * 0.5f, fRadius * sqrt(3.0f) * 0.5f);
		ptVertList[2] = ptWindowCenter + glm::vec2(-fRadius * 0.5f, fRadius * sqrt(3.0f) * 0.5f);
		ptVertList[3] = ptWindowCenter + glm::vec2(-fRadius, 0.0f);
		ptVertList[4] = ptWindowCenter + glm::vec2(-fRadius * 0.5f, -fRadius * sqrt(3.0f) * 0.5f);
		ptVertList[5] = ptWindowCenter + glm::vec2(fRadius * 0.5f, -fRadius * sqrt(3.0f) * 0.5f);
		m_Outline.SetAsPolygon(ptVertList, 6);
		break; }

	case SHAPE_LineSegment:
		m_BoundingVolume.SetAsLineSegment(ptStartPos, ptDragPos);
		pCamera->ProjectToCamera(ptStartPos, ptStartPos);
		pCamera->ProjectToCamera(ptDragPos, ptDragPos);
		m_Outline.SetAsLineSegment(ptStartPos, ptDragPos);
		break;

	case SHAPE_LineChain: {
		std::vector<glm::vec2> vertList;
		vertList.push_back(ptStartPos);
		vertList.push_back(ptDragPos);
		m_BoundingVolume.SetAsLineChain(vertList);

		pCamera->ProjectToCamera(ptStartPos, ptStartPos);
		pCamera->ProjectToCamera(ptDragPos, ptDragPos);
		vertList.clear();
		vertList.push_back(ptStartPos);
		vertList.push_back(ptDragPos);
		m_Outline.SetAsLineChain(vertList);
		break; }

	case SHAPE_LineLoop: {
		std::vector<glm::vec2> vertList;
		vertList.push_back(ptStartPos);
		vertList.push_back(ptDragPos);
		m_BoundingVolume.SetAsLineLoop(vertList);

		pCamera->ProjectToCamera(ptStartPos, ptStartPos);
		pCamera->ProjectToCamera(ptDragPos, ptDragPos);
		vertList.clear();
		vertList.push_back(ptStartPos);
		vertList.push_back(ptDragPos);
		m_Outline.SetAsLineLoop(vertList);
		break; }

	default:
		HyGuiLog("ShapeCtrl::SetAsDrag - Unhandled shape type: " % QString::number(m_eShape), LOGTYPE_Error);
		break;
	}
}

void ShapeCtrl::Clear()
{
	SetVisible(false);

	m_BoundingVolume.SetAsNothing();
	m_Outline.SetAsNothing();
}

QString ShapeCtrl::Serialize()
{
	HyShape2d shape;
	m_BoundingVolume.CalcLocalBoundingShape(shape);

	QList<float> floatList;
	switch(m_eShape)
	{
	case SHAPE_None:
		break;

	case SHAPE_Polygon:
	case SHAPE_Box: {
		const b2PolygonShape *pPolyShape = static_cast<const b2PolygonShape *>(shape.GetB2Shape());
		for(int i = 0; i < pPolyShape->m_count; ++i)
		{
			floatList.push_back(pPolyShape->m_vertices[i].x);
			floatList.push_back(pPolyShape->m_vertices[i].y);
		}
		break; }

	case SHAPE_Circle: {
		const b2CircleShape *pCircleShape = static_cast<const b2CircleShape *>(shape.GetB2Shape());
		floatList.push_back(pCircleShape->m_p.x);
		floatList.push_back(pCircleShape->m_p.y);
		floatList.push_back(pCircleShape->m_radius);
		break; }

	case SHAPE_LineSegment: {
		const b2EdgeShape *pLineSegment = static_cast<const b2EdgeShape *>(shape.GetB2Shape());
		floatList.push_back(pLineSegment->m_vertex1.x);
		floatList.push_back(pLineSegment->m_vertex1.y);
		floatList.push_back(pLineSegment->m_vertex2.x);
		floatList.push_back(pLineSegment->m_vertex2.y);
		break; }

	case SHAPE_LineChain:
	case SHAPE_LineLoop: {
		const b2ChainShape *pChainShape = static_cast<const b2ChainShape *>(shape.GetB2Shape());
		for(int i = 0; i < pChainShape->m_count; ++i)
		{
			floatList.push_back(pChainShape->m_vertices[i].x);
			floatList.push_back(pChainShape->m_vertices[i].y);
		}
		break; }
	}

	QString sSerializedData;
	for(float f : floatList)
	{
		sSerializedData.append(QString::number(f));
		sSerializedData.append(',');
	}
	sSerializedData.chop(1);

	return sSerializedData;
}

void ShapeCtrl::Deserialize(QString sData, HyCamera2d *pCamera)
{
	QStringList sFloatList = sData.split(',', Qt::SkipEmptyParts);

	QList<float> floatList;
	for(QString sFloat : sFloatList)
	{
		bool bOk;
		float fValue = sFloat.toFloat(&bOk);
		if(bOk)
			floatList.push_back(fValue);
	}

	switch(m_eShape)
	{
	case SHAPE_None:
		break;

	case SHAPE_Polygon:
	case SHAPE_Box: {
		std::vector<glm::vec2> vertList;
		for(int i = 0; i < floatList.size(); i += 2)
			vertList.push_back(glm::vec2(floatList[i], floatList[i + 1]));

		m_BoundingVolume.SetAsPolygon(vertList);
		break; }

	case SHAPE_Circle: {
		glm::vec2 ptCenter(floatList[0], floatList[1]);
		float fRadius = floatList[2];
		m_BoundingVolume.SetAsCircle(ptCenter, fRadius);
		break; }

	case SHAPE_LineSegment: {
		glm::vec2 ptOne(floatList[0], floatList[1]);
		glm::vec2 ptTwo(floatList[2], floatList[3]);
		m_BoundingVolume.SetAsLineSegment(ptOne, ptTwo);
		break; }

	case SHAPE_LineChain:
	case SHAPE_LineLoop: {
		std::vector<glm::vec2> vertList;
		for(int i = 0; i < floatList.size(); i += 2)
			vertList.push_back(glm::vec2(floatList[i], floatList[i + 1]));

		m_BoundingVolume.SetAsLineChain(vertList);
		break; }
	}

	UpdateWindowOutline(pCamera);
}

void ShapeCtrl::UpdateWindowOutline(HyCamera2d * pCamera)
{
}

void ShapeCtrl::ConvertTo(EditorShape eShape)
{
	HyGuiLog("ShapeCtrl::ConvertTo - Not implemented", LOGTYPE_Error);
}
