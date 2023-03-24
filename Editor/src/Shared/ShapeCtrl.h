/**************************************************************************
 *	ShapeCtrl.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef SHAPECTRL_H
#define SHAPECTRL_H

#include "Global.h"
#include "TransformCtrl.h"

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

	void SetTint(HyColor color);

	HyPrimitive2d &GetPrimitive(bool bWorldSpace);

	void SetAsDrag(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos, HyCamera2d *pCamera);

	QString Serialize();
	void Deserialize(QString sData, HyCamera2d *pCamera);

	void RefreshTransform(HyCamera2d *pCamera);

protected:
	void ConvertTo(EditorShape eShape);
};

#endif // SHAPECTRL_H
