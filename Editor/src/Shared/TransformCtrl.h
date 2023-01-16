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

class EntityDrawItem;

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

	bool						m_bUseExtrudeSegment;

public:
	TransformCtrl();
	virtual ~TransformCtrl();

	void WrapTo(HyShape2d boundingShape, glm::mat4 mtxShapeTransform, HyCamera2d *pCamera);
	void WrapTo(QList<EntityDrawItem *> itemDrawList, HyCamera2d *pCamera);

	void Show(bool bShowGrabPoints);
	void Hide();
};

#endif // TRANSFORMCTRL_H
