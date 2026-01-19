/**************************************************************************
 *	GfxTransformCtrl.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "GfxTransformCtrl.h"
#include "IDrawExItem.h"
#include "GfxGrabPointModel.h"
#include "GfxGrabPointView.h"

GfxTransformCtrl::GfxTransformCtrl(HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_bIsShown(false),
	m_bShowGrabPoints(false),
	m_fCachedRotation(0.0f)
{
	m_BoundingVolume.SetTint(HyGlobal::GetEditorColor(EDITORCOLOR_TransformBoundingVolume));
	m_BoundingVolume.SetWireframe(true);
	ChildAppend(m_BoundingVolume);

	m_ExtrudeSegment.SetTint(HyGlobal::GetEditorColor(EDITORCOLOR_TransformBoundingVolume));
	m_ExtrudeSegment.SetWireframe(true);
	ChildAppend(m_ExtrudeSegment);

	for(uint i = 0; i < GRAB_Rotate; ++i)
	{
		m_GrabPointModels[i] = new GfxGrabPointModel(GRABPOINT_Transform);
		m_GrabPointViews[i] = new GfxGrabPointView(this);
	}
	m_GrabPointModels[GRAB_Rotate] = new GfxGrabPointModel(GRABPOINT_TransformRotate);
	m_GrabPointViews[GRAB_Rotate] = new GfxGrabPointView(this);

	UseWindowCoordinates(0);
	SetDisplayOrder(DISPLAYORDER_TransformCtrl);

	Hide();
}

/*virtual*/ GfxTransformCtrl::~GfxTransformCtrl()
{
	for(uint i = 0; i < NUM_GRABPOINTS; ++i)
	{
		delete m_GrabPointViews[i];
		delete m_GrabPointModels[i];
	}
}

bool GfxTransformCtrl::IsValid() const
{
	return m_BoundingVolume.GetShapeType() != HYFIXTURE_Nothing;
}

void GfxTransformCtrl::WrapTo(const HyShape2d &boundingShape, glm::mat4 mtxShapeTransform, HyCamera2d *pCamera)
{
	if(boundingShape.IsValid() == false)
		return;

	glm::vec2 ptGrabPos[NUM_GRABPOINTS];

	b2AABB aabb;
	boundingShape.ComputeAABB(aabb, glm::identity<glm::mat4>());
	HySetVec(ptGrabPos[GRAB_BotLeft], aabb.lowerBound.x, aabb.lowerBound.y);
	HySetVec(ptGrabPos[GRAB_BotRight], aabb.upperBound.x, aabb.lowerBound.y);
	HySetVec(ptGrabPos[GRAB_TopRight], aabb.upperBound.x, aabb.upperBound.y);
	HySetVec(ptGrabPos[GRAB_TopLeft], aabb.lowerBound.x, aabb.upperBound.y);
	ptGrabPos[GRAB_BotMid] = ptGrabPos[GRAB_BotLeft] + ((ptGrabPos[GRAB_BotRight] - ptGrabPos[GRAB_BotLeft]) * 0.5f);
	ptGrabPos[GRAB_MidRight] = ptGrabPos[GRAB_BotRight] + ((ptGrabPos[GRAB_TopRight] - ptGrabPos[GRAB_BotRight]) * 0.5f);
	ptGrabPos[GRAB_TopMid] = ptGrabPos[GRAB_TopLeft] + ((ptGrabPos[GRAB_TopRight] - ptGrabPos[GRAB_TopLeft]) * 0.5f);
	ptGrabPos[GRAB_MidLeft] = ptGrabPos[GRAB_BotLeft] + ((ptGrabPos[GRAB_TopLeft] - ptGrabPos[GRAB_BotLeft]) * 0.5f);
	
	// Create the top rotate grab anchor - Along the top edge, find the midpoint and extrude a line segment out perpendicularly
	glm::vec2 vTopEdge = ptGrabPos[GRAB_TopRight] - ptGrabPos[GRAB_TopLeft];
	glm::vec2 vExtrudeDir = glm::normalize(HyMath::PerpendicularCounterClockwise(vTopEdge));
	glm::vec2 ptExtrudeStart = ptGrabPos[GRAB_TopLeft] + (vTopEdge * 0.5f);
	ptGrabPos[GRAB_Rotate] = ptExtrudeStart + (vExtrudeDir * 50.0f); // 50px line segment length

	for(int i = 0; i < NUM_GRABPOINTS; ++i)
	{
		glm::vec4 ptTransformPos(ptGrabPos[i].x, ptGrabPos[i].y, 0.0f, 1.0f);
		ptTransformPos = mtxShapeTransform * ptTransformPos;
		HySetVec(ptGrabPos[i], ptTransformPos.x, ptTransformPos.y);
		
		m_GrabPointModels[i]->Setup(ptGrabPos[i]);
		m_GrabPointViews[i]->Sync(m_GrabPointModels[i]);
	}

	for(int i = 0; i < 4; ++i)
		pCamera->ProjectToCamera(ptGrabPos[i], ptGrabPos[i]);
	m_BoundingVolume.SetAsPolygon(ptGrabPos, 4);

	glm::vec2 ptRotatePos = m_GrabPointModels[GRAB_Rotate]->GetPos();

	glm::vec4 ptTransformPos(ptExtrudeStart.x, ptExtrudeStart.y, 0.0f, 1.0f);
	ptTransformPos = mtxShapeTransform * ptTransformPos;
	HySetVec(ptExtrudeStart, ptTransformPos.x, ptTransformPos.y);
	pCamera->ProjectToCamera(ptExtrudeStart, ptExtrudeStart);
	m_ExtrudeSegment.SetAsLineSegment(ptExtrudeStart, ptRotatePos);

	m_fCachedRotation = HyMath::AngleFromVector(ptRotatePos - ptExtrudeStart) - 90.0f;
}

void GfxTransformCtrl::WrapTo(QList<IDrawExItem *> itemDrawList, HyCamera2d *pCamera)
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
	for(IDrawExItem *pDrawItem : itemDrawList)
	{
		HyShape2d *pItemShape = new HyShape2d();
		glm::mat4 mtxItemTransform;
		pDrawItem->ExtractTransform(*pItemShape, mtxItemTransform);
		if(pItemShape->IsValid() == false)
		{
			delete pItemShape;
			continue;
		}

		b2AABB itemAabb;
		pItemShape->ComputeAABB(itemAabb, mtxItemTransform);

		if(b2IsValidAABB(combinedAabb) == false)
			combinedAabb = itemAabb;
		else
			combinedAabb = b2AABB_Union(combinedAabb, itemAabb);
	}
	HySetVec(ptRotPivot, b2AABB_Center(combinedAabb).x, b2AABB_Center(combinedAabb).y, 0.0f);

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
	boundingShape.SetAsBox(HyRect(b2AABB_Extents(combinedAabb).x, b2AABB_Extents(combinedAabb).y, ptRotPivot, 0.0f));

	//glm::mat4 mtxRotateCompensate(1.0f);
	//mtxRotateCompensate = glm::translate(mtxRotateCompensate, ptRotPivot);
	//mtxRotateCompensate = glm::rotate(mtxRotateCompensate, glm::radians(m_fCachedRotation), glm::vec3(0, 0, 1));
	//mtxRotateCompensate = glm::translate(mtxRotateCompensate, ptRotPivot * -1.0f);
	//
	//boundingShape.TransformSelf(mtxRotateCompensate);
	WrapTo(boundingShape, glm::mat4(1.0f), pCamera);
}

bool GfxTransformCtrl::IsShown() const
{
	return m_bIsShown;
}

void GfxTransformCtrl::Show(bool bShowGrabPoints)
{
	m_bIsShown = true;
	m_bShowGrabPoints = bShowGrabPoints;

	for(int32 i = 0; i < NUM_GRABPOINTS; ++i)
		m_GrabPointViews[i]->SetVisible(m_bShowGrabPoints);
	m_ExtrudeSegment.SetVisible(m_bShowGrabPoints);

	m_BoundingVolume.SetVisible(true);
}

void GfxTransformCtrl::Hide()
{
	m_bIsShown = false;

	for(int32 i = 0; i < NUM_GRABPOINTS; ++i)
		m_GrabPointViews[i]->SetVisible(false);

	m_BoundingVolume.SetVisible(false);
	m_ExtrudeSegment.SetVisible(false);
}

void GfxTransformCtrl::GetCentroid(glm::vec2 &ptCenterOut) const
{
	if(IsValid())
		m_BoundingVolume.GetCentroid(ptCenterOut);
	else
		HyGuiLog("GfxTransformCtrl::GetCentroid() called on invalid bounding volume", LOGTYPE_Error);
}

glm::vec2 GfxTransformCtrl::GetGrabPointWorldPos(GrabPointType eGrabPoint, HyCamera2d *pCamera) const
{
	HyAssert(eGrabPoint > GRAB_None && eGrabPoint < NUM_GRABPOINTS, "GfxTransformCtrl::GetGrabPointPos invalid grab enum");
	return m_GrabPointModels[eGrabPoint]->GetPos();
}

float GfxTransformCtrl::GetCachedRotation() const
{
	return m_fCachedRotation;
}

bool GfxTransformCtrl::IsMouseOverBoundingVolume()
{
	glm::vec2 ptWorldMousePos;
	if(m_bIsShown == false || HyEngine::Input().GetWorldMousePos(ptWorldMousePos) == false)
		return false;

	ptWorldMousePos = HyEngine::Input().GetMousePos();

	HyShape2d tmpShape;
	m_BoundingVolume.CalcLocalBoundingShape(tmpShape);
	
	return tmpShape.TestPoint(ptWorldMousePos, m_BoundingVolume.GetSceneTransform(0.0f));
}

GfxTransformCtrl::GrabPointType GfxTransformCtrl::IsMouseOverGrabPoint() const
{
	glm::vec2 ptWorldMousePos;
	if(m_bIsShown == false || m_bShowGrabPoints == false || HyEngine::Input().GetWorldMousePos(ptWorldMousePos) == false)
		return GRAB_None;

	ptWorldMousePos = HyEngine::Input().GetMousePos();

	for(int32 i = 0; i < NUM_GRABPOINTS; ++i)
	{
		if(m_GrabPointModels[i]->TestPoint(ptWorldMousePos))
			return static_cast<GrabPointType>(i);
	}

	return GRAB_None;
}

bool GfxTransformCtrl::IsContained(const b2AABB &aabb, HyCamera2d *pCamera) const
{
	for(uint i = GRAB_BotLeft; i < 4; ++i)
	{
		glm::vec2 ptVertex = m_GrabPointModels[i]->GetPos();
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
