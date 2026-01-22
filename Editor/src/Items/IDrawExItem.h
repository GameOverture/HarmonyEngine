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
#include "GfxTransformCtrl.h"

class IDrawExItem
{
protected:
	GfxTransformCtrl						m_Transform;

public:
	IDrawExItem(HyEntity2d *pParent);
	virtual ~IDrawExItem();

	virtual IHyBody2d *GetHyNode() = 0;
	virtual bool IsSelectable() const { return true; }
	virtual bool IsSelected() = 0;

	GfxTransformCtrl &GetTransformCtrl();

	bool IsMouseInBounds();

	virtual void RefreshTransform();

	void ShowTransformCtrl(bool bShowGrabPoints);
	void HideTransformCtrl();
	void ExtractTransform(HyShape2d &boundingShapeOut, glm::mat4 &transformMtxOut);
};

#endif // IDRAWITEM_H
