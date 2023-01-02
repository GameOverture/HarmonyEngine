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

		NUM_GRABPOINTS
	};

protected:
	glm::vec2					m_ptGrabPos[NUM_GRABPOINTS];

	HyPrimitive2d				m_BoundingVolume;
	HyPrimitive2d				m_GrabOutline[NUM_GRABPOINTS];
	HyPrimitive2d				m_GrabFill[NUM_GRABPOINTS];


public:
	TransformCtrl(HyEntity2d *pParent);
	virtual ~TransformCtrl();

	void Resize(HyGuiItemType eGuiType, IHyLoadable2d *pNode, HyCamera2d *pCamera);
};

#endif // TRANSFORMCTRL_H
