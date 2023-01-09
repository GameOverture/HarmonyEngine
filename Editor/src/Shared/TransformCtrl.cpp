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
#include "EntityItemDraw.h"

TransformCtrl::TransformCtrl() :
	HyEntity2d(nullptr)
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
}

/*virtual*/ TransformCtrl::~TransformCtrl()
{
}

void TransformCtrl::WrapTo(EntityItemDraw *pDrawItem, HyCamera2d *pCamera)
{
	glm::mat4 transformMtx;
	HyShape2d boundingShape;
	ExtractTransform(pDrawItem, transformMtx, boundingShape);

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
			ptTransformPos = transformMtx * ptTransformPos;

			HySetVec(m_ptGrabPos[i], ptTransformPos.x, ptTransformPos.y);
			pCamera->ProjectToCamera(m_ptGrabPos[i], m_ptGrabPos[i]);

			m_GrabOutline[i].SetAsCircle(m_ptGrabPos[i], fRADIUS);
			m_GrabFill[i].SetAsCircle(m_ptGrabPos[i], fRADIUS - 1.0f);
		}

		m_BoundingVolume.SetAsPolygon(m_ptGrabPos, 4);

		m_ExtrudeSegment.SetAsLineSegment(ptExtrudeStart, m_ptGrabPos[GRAB_Rotate]);
	}
}

void TransformCtrl::WrapTo(QList<EntityItemDraw *> itemDrawList, HyCamera2d *pCamera)
{
	if(itemDrawList.size() == 1)
	{
		WrapTo(itemDrawList[0], pCamera);
		return;
	}

	bool bCommonRotation = true;
	float fRotation = 0.0f;
	for(EntityItemDraw *pDrawItem : itemDrawList)
	{
		//if(pDrawItem->GetGuiType() != ITEM_Shape)
			//pDrawItem

		glm::mat4 transformMtx;
		HyShape2d boundingShape;

		ExtractTransform(pDrawItem, transformMtx, boundingShape);
		
		if(boundingShape.IsValidShape() == false)
			continue;

		b2Shape *pB2Shape = boundingShape.CloneTransform(transformMtx);

		delete pB2Shape;
	}
}

void TransformCtrl::ExtractTransform(EntityItemDraw *pDrawItem, glm::mat4 &transformMtxOut, HyShape2d &boundingShapeOut)
{
	transformMtxOut = glm::identity<glm::mat4>();
	switch(pDrawItem->GetGuiType())
	{
	case ITEM_Shape:
		boundingShapeOut = *pDrawItem->GetShape();
		break;

	case ITEM_AtlasImage:
	case ITEM_Primitive:
	case ITEM_Text:
	case ITEM_Spine:
	case ITEM_Sprite: {
		IHyDrawable2d *pDrawable = static_cast<IHyDrawable2d *>(pDrawItem->GetNodeChild());
		pDrawable->CalcLocalBoundingShape(boundingShapeOut);
		transformMtxOut = pDrawItem->GetNodeChild()->GetSceneTransform(0.0f);
		break; }

	case ITEM_Audio:
	case ITEM_Entity:
	default:
		HyLogError("TransformCtrl::Resize - unhandled child node type");
		break;
	}
}
