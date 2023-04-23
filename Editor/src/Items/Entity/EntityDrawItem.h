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

// NOTE: this class does not keep its state when removed, it is deleted (should not be passed to UndoCmd's)
class EntityDrawItem
{
	ItemType								m_eGuiType;
	QUuid									m_Uuid;
	QUuid									m_ProjItemUuid;

	IHyLoadable2d *							m_pChild;

	TransformCtrl							m_Transform;
	ShapeCtrl								m_ShapeCtrl;

public:
	EntityDrawItem(Project &projectRef, ItemType eGuiType, QUuid uuid, QUuid itemUuid, HyEntity2d *pParent);
	virtual ~EntityDrawItem();

	ItemType GetGuiType() const;
	const QUuid &GetThisUuid() const;
	const QUuid &GetProjItemUuid() const;

	IHyLoadable2d *GetHyNode();
	ShapeCtrl &GetShapeCtrl();
	TransformCtrl &GetTransformCtrl();

	bool IsMouseInBounds();

	void RefreshJson(QJsonObject descObj, QJsonObject propObj, HyCamera2d *pCamera);
	void RefreshTransform(HyCamera2d *pCamera);
	void RefreshOverrideData(Project &projectRef);

	void ExtractTransform(HyShape2d &boundingShapeOut, glm::mat4 &transformMtxOut);

	void ShowTransformCtrl(bool bShowGrabPoints);
	void HideTransformCtrl();

protected:
	void SubEntityRefreshOverrideData(QJsonObject metaObj);
};

#endif // ENTITYDRAWITEM_H
