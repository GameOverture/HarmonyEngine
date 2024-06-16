/**************************************************************************
*	IDrawExItem.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2024 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IDRAWEXITEM_H
#define IDRAWEXITEM_H

#include "Global.h"
#include "TransformCtrl.h"
#include "ShapeCtrl.h"

class IDrawExItem
{
	TransformCtrl							m_Transform;
	ShapeCtrl								m_ShapeCtrl;

public:
	IDrawExItem(HyEntity2d *pParent);
	virtual ~IDrawExItem();

	virtual void InitHyNode() = 0;
	virtual IHyBody2d *GetHyNode() = 0;

	TransformCtrl &GetTransformCtrl();
	ShapeCtrl &GetShapeCtrl();

	ShapeCtrl &GetShapeCtrl();
	TransformCtrl &GetTransformCtrl();

	bool IsMouseInBounds();

	void RefreshTransform(HyCamera2d *pCamera);

	void ShowTransformCtrl(bool bShowGrabPoints);
	void HideTransformCtrl();
	void ExtractTransform(HyShape2d &boundingShapeOut, glm::mat4 &transformMtxOut);
};

#endif // IDRAWITEM_H
