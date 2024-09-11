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
protected:
	TransformCtrl							m_Transform;

public:
	IDrawExItem(HyEntity2d *pParent);
	virtual ~IDrawExItem();

	virtual IHyBody2d *GetHyNode() = 0;
	virtual bool IsSelected() = 0;

	TransformCtrl &GetTransformCtrl();

	bool IsMouseInBounds();

	void RefreshTransform(HyCamera2d *pCamera);

	void ShowTransformCtrl(bool bShowGrabPoints);
	void HideTransformCtrl();
	void ExtractTransform(HyShape2d &boundingShapeOut, glm::mat4 &transformMtxOut);
};

#endif // IDRAWITEM_H
