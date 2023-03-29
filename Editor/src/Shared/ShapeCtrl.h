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

class ShapeCtrl
{
public:
	enum VertexEditType
	{
		EDIT_Add = 0,
		EDIT_Remove,
		EDIT_Translate
	};

protected:
	EditorShape					m_eShape;

	HyPrimitive2d				m_BoundingVolume;	// Uses world/camera coordinates
	HyPrimitive2d				m_Outline;			// Uses window coordinates (unaffected by zoom)

	QList<float>				m_DeserializedFloatList;
	QList<GrabPoint *>			m_VertexGrabPointList;

public:
	ShapeCtrl();
	ShapeCtrl(const ShapeCtrl &copyRef);
	virtual ~ShapeCtrl();

	void Setup(EditorShape eShape, HyColor color, float fBvAlpha, float fOutlineAlpha); // Alphas of 0.0f just get their SetVisible(false)

	EditorShape GetShapeType() const;
	HyPrimitive2d &GetPrimitive();

	void SetAsDrag(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos, HyCamera2d *pCamera);

	QString Serialize();
	void Deserialize(QString sData, HyCamera2d *pCamera);

	void TransformSelf(glm::mat4 mtxTransform, HyCamera2d *pCamera);

	void RefreshOutline(HyCamera2d *pCamera);

	void EnableVertexEditMode();
	void ClearVertexEditMode();

protected:
	void ConvertTo(EditorShape eShape);
};

#endif // SHAPECTRL_H
