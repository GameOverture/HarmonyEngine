/**************************************************************************
*	EntityDrawItem.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ENTITYDRAWITEM_H
#define ENTITYDRAWITEM_H

#include "IDraw.h"

class EntityDrawItem
{
	HyGuiItemType							m_eGuiType;
	QUuid									m_Uuid;
	QUuid									m_ItemUuid;

	IHyLoadable2d *							m_pChild;
	HyShape2d *								m_pShape;

	TransformCtrl							m_Transform;

	bool									m_bStale;

public:
	EntityDrawItem(HyGuiItemType eGuiType, QUuid uuid, QUuid itemUuid, HyEntity2d *pParent);
	virtual ~EntityDrawItem();

	HyGuiItemType GetGuiType() const;
	const QUuid &GetThisUuid() const;
	const QUuid &GetItemUuid() const;

	IHyLoadable2d *GetNodeChild() const;
	HyShape2d *GetShape() const;
	TransformCtrl &GetTransformCtrl();

	bool IsStale() const;
	void SetStale();

	bool IsMouseInBounds() const;

	void RefreshJson(HyCamera2d *pCamera, QJsonObject childObj); // Clears stale flag
	void RefreshTransform(HyCamera2d *pCamera);
	void RefreshOverrideData();

	void ExtractTransform(HyShape2d &boundingShapeOut, glm::mat4 &transformMtxOut) const;

	void ShowTransformCtrl(bool bShowGrabPoints);
	void HideTransformCtrl();
};

#endif // ENTITYDRAWITEM_H
