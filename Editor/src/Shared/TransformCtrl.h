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

class GrabPoint : public HyEntity2d
{
	HyPrimitive2d				m_GrabOutline;
	HyPrimitive2d				m_GrabFill;

public:
	GrabPoint(HyColor outlineColor, HyColor fillColor, HyEntity2d *pParent);
	virtual ~GrabPoint();

	void GetLocalBoundingShape(HyShape2d &shapeRefOut);
};

class TransformCtrl : public HyEntity2d
{
public:
	enum GrabPointType
	{
		GRAB_None = -1,

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
	GrabPoint *					m_GrabPoints[NUM_GRABPOINTS];

	bool						m_bIsShown;
	bool						m_bShowGrabPoints;
	float						m_fCachedRotation;

public:
	TransformCtrl(HyEntity2d *pParent);
	virtual ~TransformCtrl();

	void WrapTo(HyShape2d boundingShape, glm::mat4 mtxShapeTransform, HyCamera2d *pCamera);
	void WrapTo(QList<EntityDrawItem *> itemDrawList, HyCamera2d *pCamera);

	bool IsShown() const;
	void Show(bool bShowGrabPoints);
	void Hide();

	void GetCentroid(glm::vec2 &ptCenterOut) const;
	glm::vec2 GetGrabPointWorldPos(GrabPointType eGrabPoint, HyCamera2d *pCamera) const;

	float GetCachedRotation() const;

	bool IsMouseOverBoundingVolume();
	GrabPointType IsMouseOverGrabPoint();

	bool IsContained(const b2AABB &aabb, HyCamera2d *pCamera) const;
};

class MarqueeBox : public HyEntity2d
{
	HyPrimitive2d				m_BoundingVolume;
	HyPrimitive2d				m_Outline;

	glm::vec2					m_ptStartPos;

public:
	MarqueeBox(HyEntity2d *pParent);
	virtual ~MarqueeBox();

	b2AABB GetSelectionBox();

	void SetStartPt(glm::vec2 ptStartPos);
	void SetDragPt(glm::vec2 ptDragPos, HyCamera2d *pCamera);
	void Clear();
};

// Essentially a wrapper around 'm_PrimShape' that will allow the user to manipulate what the HyPrimitive is in the editor
class ShapeCtrl
{
	EditorShape					m_eDrawShape;
	HyPrimitive2d				m_PrimShape;

	QList<GrabPoint *>			m_GrabPointList;

public:
	ShapeCtrl(HyEntity2d *pParent);
	virtual ~ShapeCtrl();

	void GetShape(HyShape2d &shapeRefOut);
};

#endif // TRANSFORMCTRL_H
