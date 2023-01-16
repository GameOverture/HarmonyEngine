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

TransformCtrl::TransformCtrl() :
	HyEntity2d(nullptr),
	m_bUseExtrudeSegment(false)
{
	m_BoundingVolume.SetTint(HyColor::Blue.Lighten());
	m_BoundingVolume.SetWireframe(true);
	ChildAppend(m_BoundingVolume);

	m_ExtrudeSegment.SetTint(HyColor::Blue.Lighten());
	m_ExtrudeSegment.SetWireframe(true);
	ChildAppend(m_ExtrudeSegment);

	for(uint i = 0; i < NUM_GRABPOINTS; ++i)
	{
		m_GrabOutline[i].SetTint(HyColor::White);
		ChildAppend(m_GrabOutline[i]);

		m_GrabFill[i].SetTint(HyColor::Blue.Lighten());
		ChildAppend(m_GrabFill[i]);
	}
	m_GrabOutline[GRAB_Rotate].SetTint(HyColor::Blue.Lighten());
	m_GrabFill[GRAB_Rotate].SetTint(HyColor::White);

	UseWindowCoordinates(0);
	SetDisplayOrder(DISPLAYORDER_TransformCtrl);
}

/*virtual*/ TransformCtrl::~TransformCtrl()
{
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

		const float fRADIUS = 5.0f;
		for(int i = 0; i < NUM_GRABPOINTS; ++i)
		{
			glm::vec4 ptTransformPos(m_ptGrabPos[i].x, m_ptGrabPos[i].y, 0.0f, 1.0f);
			ptTransformPos = mtxShapeTransform * ptTransformPos;

			HySetVec(m_ptGrabPos[i], ptTransformPos.x, ptTransformPos.y);
			pCamera->ProjectToCamera(m_ptGrabPos[i], m_ptGrabPos[i]);

			m_GrabOutline[i].SetAsCircle(m_ptGrabPos[i], fRADIUS);
			m_GrabFill[i].SetAsCircle(m_ptGrabPos[i], fRADIUS - 1.0f);
		}

		m_BoundingVolume.SetAsPolygon(m_ptGrabPos, 4);

		glm::vec4 ptTransformPos(ptExtrudeStart.x, ptExtrudeStart.y, 0.0f, 1.0f);
		ptTransformPos = mtxShapeTransform * ptTransformPos;
		HySetVec(ptExtrudeStart, ptTransformPos.x, ptTransformPos.y);
		pCamera->ProjectToCamera(ptExtrudeStart, ptExtrudeStart);
		m_ExtrudeSegment.SetAsLineSegment(ptExtrudeStart, m_ptGrabPos[GRAB_Rotate]);
		m_bUseExtrudeSegment = true;
	}
}

void TransformCtrl::WrapTo(QList<EntityDrawItem *> itemDrawList, HyCamera2d *pCamera)
{
	if(itemDrawList.size() == 1)
	{
		HyShape2d boundingShape;
		glm::mat4 mtxShapeTransform;
		itemDrawList[0]->ExtractTransform(boundingShape, mtxShapeTransform);

		WrapTo(boundingShape, mtxShapeTransform, pCamera);
		return;
	}

	bool bCommonRotation = true;
	float fRotation = -1.0f; // Not yet set (valid values are 0.0-360.0)
	QList<b2Shape *> transformedShapeList;
	for(EntityDrawItem *pDrawItem : itemDrawList)
	{
		HyShape2d *pLocalShape = new HyShape2d();
		glm::mat4 mtxShapeTransform;
		pDrawItem->ExtractTransform(*pLocalShape, mtxShapeTransform);
		if(pLocalShape->IsValidShape() == false)
		{
			delete pLocalShape;
			continue;
		}
		b2Shape *pSceneShape = pLocalShape->CloneTransform(mtxShapeTransform);
		delete pLocalShape;
		transformedShapeList.push_back(pSceneShape);

		if(bCommonRotation && pDrawItem->GetGuiType() != ITEM_Shape)
		{
			if(fRotation < 0.0f)
				fRotation = pDrawItem->GetNodeChild()->rot.Get();

			if(fRotation >= 0.0f && fRotation != pDrawItem->GetNodeChild()->rot.Get())
				bCommonRotation = false;
		}
	}

	if(bCommonRotation)
		fRotation = glm::radians(-fRotation);
	else
		fRotation = 0.0f;

	b2AABB combinedAabb;
	HyMath::InvalidateAABB(combinedAabb);
	for(b2Shape *pTransformedShape : transformedShapeList)
	{
		b2AABB shapeAabb;
		pTransformedShape->ComputeAABB(&shapeAabb, b2Transform(b2Vec2(0.0f, 0.0f), b2Rot(fRotation)), 0);
		
		if(combinedAabb.IsValid() == false)
			combinedAabb = shapeAabb;
		else
			combinedAabb.Combine(shapeAabb);

		delete pTransformedShape;
	}
	transformedShapeList.clear();

	HyShape2d combinedShape;
	std::vector<glm::vec2> ptExtents;
	ptExtents.push_back(glm::vec2(combinedAabb.lowerBound.x, combinedAabb.lowerBound.y));
	ptExtents.push_back(glm::vec2(combinedAabb.upperBound.x, combinedAabb.lowerBound.y));
	ptExtents.push_back(glm::vec2(combinedAabb.upperBound.x, combinedAabb.upperBound.y));
	ptExtents.push_back(glm::vec2(combinedAabb.lowerBound.x, combinedAabb.upperBound.y));

	combinedShape.SetAsPolygon(ptExtents);

	WrapTo(combinedShape, glm::mat4(1.0f), pCamera);
}

void TransformCtrl::Show(bool bShowGrabPoints)
{
	for(int32 i = 0; i < NUM_GRABPOINTS; ++i)
	{
		m_GrabOutline[i].SetVisible(bShowGrabPoints);
		m_GrabFill[i].SetVisible(bShowGrabPoints);
	}

	m_BoundingVolume.SetVisible(true);
	m_ExtrudeSegment.SetVisible(m_bUseExtrudeSegment);
}

void TransformCtrl::Hide()
{
	for(int32 i = 0; i < NUM_GRABPOINTS; ++i)
	{
		m_GrabOutline[i].SetVisible(false);
		m_GrabFill[i].SetVisible(false);
	}

	m_BoundingVolume.SetVisible(false);
	m_ExtrudeSegment.SetVisible(false);
}
