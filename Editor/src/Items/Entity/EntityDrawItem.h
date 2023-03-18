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

	TransformCtrl							m_Transform;
	ShapeCtrl								m_ShapeCtrl;

public:
	EntityDrawItem(HyGuiItemType eGuiType, QUuid uuid, QUuid itemUuid, HyEntity2d *pParent);
	virtual ~EntityDrawItem();

	HyGuiItemType GetGuiType() const;
	const QUuid &GetThisUuid() const;
	const QUuid &GetItemUuid() const;

	IHyLoadable2d *GetAsChild();
	ShapeCtrl &GetShapeCtrl();
	TransformCtrl &GetTransformCtrl();

	bool IsMouseInBounds();

	void RefreshJson(QJsonObject childObj);
	void RefreshTransform(HyCamera2d *pCamera);
	void RefreshOverrideData();

	void ExtractTransform(HyShape2d &boundingShapeOut, glm::mat4 &transformMtxOut);

	void ShowTransformCtrl(bool bShowGrabPoints);
	void HideTransformCtrl();
};

#endif // ENTITYDRAWITEM_H
