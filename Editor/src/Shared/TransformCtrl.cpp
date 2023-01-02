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

TransformCtrl::TransformCtrl(HyEntity2d *pParent) :
	HyEntity2d(pParent)
{
	m_BoundingVolume.SetTint(HyColor::Blue.Lighten());
	m_BoundingVolume.SetWireframe(true);
	ChildAppend(m_BoundingVolume);

	for(uint i = 0; i < NUM_GRABPOINTS; ++i)
	{
		m_GrabOutline[i].SetTint(HyColor::White);
		ChildAppend(m_GrabOutline[i]);

		m_GrabFill[i].SetTint(HyColor::Blue.Lighten());
		ChildAppend(m_GrabFill[i]);
	}

	UseWindowCoordinates(0);
}

/*virtual*/ TransformCtrl::~TransformCtrl()
{
}

void TransformCtrl::Resize(HyGuiItemType eGuiType, IHyLoadable2d *pNode, HyCamera2d *pCamera)
{
	HyShape2d boundingShape;

	switch(eGuiType)
	{
	case ITEM_Primitive:
	case ITEM_Text:
	case ITEM_Spine:
	case ITEM_Sprite: {
		IHyDrawable2d *pDrawable = static_cast<IHyDrawable2d *>(pNode);
		pDrawable->CalcLocalBoundingShape(boundingShape);
		break; }

	case ITEM_Audio:
	case ITEM_AtlasImage:
	case ITEM_Entity:
	default:
		HyLogError("TransformCtrl::Resize - unhandled child node type");
		break;
	}
	if(boundingShape.IsValidShape() == false)
		return;

	if(boundingShape.GetType() == HYSHAPE_Polygon)
	{
		const b2PolygonShape *pPolyShape = static_cast<const b2PolygonShape *>(boundingShape.GetB2Shape());
		if(pPolyShape->m_count != 4)
		{
			HyLogError("TransformCtrl::Resize - unhandled child node type");
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

		const glm::mat4 &sceneMtx = pNode->GetSceneTransform(0.0f);

		const float fRADIUS = 5.0f;
		for(int i = 0; i < NUM_GRABPOINTS; ++i)
		{
			glm::vec4 ptTransformPos(m_ptGrabPos[i].x, m_ptGrabPos[i].y, 0.0f, 0.0f);
			ptTransformPos = sceneMtx * ptTransformPos;
			HySetVec(m_ptGrabPos[i], ptTransformPos.x, ptTransformPos.y);
			pCamera->ProjectToCamera(m_ptGrabPos[i], m_ptGrabPos[i]);

			m_GrabOutline[i].SetAsCircle(m_ptGrabPos[i], fRADIUS);
			m_GrabFill[i].SetAsCircle(m_ptGrabPos[i], fRADIUS - 1.0f);
		}

		m_BoundingVolume.SetAsPolygon(m_ptGrabPos, 4);
	}
}
