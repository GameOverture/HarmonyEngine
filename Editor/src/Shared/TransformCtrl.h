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

class ShapeCtrl : public HyEntity2d
{
	EditorShape					m_eShape;

	HyPrimitive2d				m_BoundingVolume;	// Uses world/camera coordinates
	HyPrimitive2d				m_Outline;			// Uses window coordinates (unaffected by zoom)

	QList<float>				m_DeserializedFloatList;
	QList<GrabPoint *>			m_GrabPointList;

public:
	ShapeCtrl(HyEntity2d *pParent);
	virtual ~ShapeCtrl();

	EditorShape GetShapeType() const;
	void SetShapeType(EditorShape eShape);

	HyPrimitive2d &GetPrimitive(bool bWorldSpace);

	void SetAsDrag(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos, HyCamera2d *pCamera);

	QString Serialize();
	void Deserialize(QString sData, HyCamera2d *pCamera);

	void RefreshTransform(HyCamera2d *pCamera);

protected:
	void ConvertTo(EditorShape eShape);
};

#endif // TRANSFORMCTRL_H
