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

	b2AABB aabb;
	boundingShape.ComputeAABB(aabb, glm::identity<glm::mat4>());
	HySetVec(m_ptGrabPos[GRAB_BotLeft], aabb.lowerBound.x, aabb.lowerBound.y);
	HySetVec(m_ptGrabPos[GRAB_BotRight], aabb.upperBound.x, aabb.lowerBound.y);
	HySetVec(m_ptGrabPos[GRAB_TopRight], aabb.upperBound.x, aabb.upperBound.y);
	HySetVec(m_ptGrabPos[GRAB_TopLeft], aabb.lowerBound.x, aabb.upperBound.y);

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
