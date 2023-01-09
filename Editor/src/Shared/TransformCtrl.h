/**************************************************************************
 *	TransformCtrl.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef TRANSFORMCTRL_H
#define TRANSFORMCTRL_H

#include "Global.h"

class EntityItemDraw;

class TransformCtrl : public HyEntity2d
{
	enum GrabPoints
	{
		GRAB_BotLeft = 0,
		GRAB_BotRight,
		GRAB_TopRight,
		GRAB_TopLeft,
		GRAB_BotMid,
		GRAB_MidRight,
		GRAB_TopMid,
		GRAB_MidLeft,

		GRAB_Rotate,

		NUM_GRABPOINTS
	};

protected:
	glm::vec2					m_ptGrabPos[NUM_GRABPOINTS];

	HyPrimitive2d				m_BoundingVolume;
	HyPrimitive2d				m_ExtrudeSegment;
	HyPrimitive2d				m_GrabOutline[NUM_GRABPOINTS];
	HyPrimitive2d				m_GrabFill[NUM_GRABPOINTS];

public:
	TransformCtrl();
	virtual ~TransformCtrl();

	void WrapTo(EntityItemDraw *pDrawItem, HyCamera2d *pCamera);
	void WrapTo(QList<EntityItemDraw *> itemDrawList, HyCamera2d *pCamera);

protected:
	void ExtractTransform(EntityItemDraw *pDrawItem, glm::mat4 &transformMtxOut, HyShape2d &boundingShapeOut);
};

#endif // TRANSFORMCTRL_H
