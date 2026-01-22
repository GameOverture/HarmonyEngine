/**************************************************************************
 *	GfxTransformCtrl.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef GFXTRANSFORMCTRL_H
#define GFXTRANSFORMCTRL_H

#include "Global.h"

class IDrawExItem;
class GfxGrabPointModel;
class GfxGrabPointView;

class GfxTransformCtrl : public HyEntity2d
{
public:
	enum GrabPointType
	{
		GRAB_None = -1,

		// NOTE: Order matters - snap logic relies on this order
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
	HyPrimitive2d				m_BoundingVolume;
	HyPrimitive2d				m_ExtrudeSegment;
	GfxGrabPointModel *			m_GrabPointModels[NUM_GRABPOINTS];
	GfxGrabPointView *			m_GrabPointViews[NUM_GRABPOINTS];

	bool						m_bIsShown;
	bool						m_bShowGrabPoints;
	float						m_fCachedRotation;

public:
	GfxTransformCtrl(HyEntity2d *pParent);
	virtual ~GfxTransformCtrl();

	bool IsValid() const;

	void WrapTo(const HyShape2d &boundingShape, glm::mat4 mtxShapeTransform);
	void WrapTo(QList<IDrawExItem *> itemDrawLis);

	bool IsShown() const;
	void Show(bool bShowGrabPoints);
	void Hide();

	void GetCentroid(glm::vec2 &ptCenterOut) const;
	glm::vec2 GetGrabPointWorldPos(GrabPointType eGrabPoint) const;

	float GetCachedRotation() const;

	bool IsMouseOverBoundingVolume();
	GrabPointType IsMouseOverGrabPoint() const;

	bool IsContained(const b2AABB &aabb) const;
};

#endif // GFXTRANSFORMCTRL_H
