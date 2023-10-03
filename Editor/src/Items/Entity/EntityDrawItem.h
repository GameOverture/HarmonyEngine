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

class EntityTreeItemData;

// NOTE: this class does not keep its state when removed, it is deleted (should not be passed to UndoCmd's)
class EntityDrawItem
{
	EntityTreeItemData *					m_pEntityTreeItemData;

	IHyLoadable2d *							m_pChild;

	TransformCtrl							m_Transform;
	ShapeCtrl								m_ShapeCtrl;

public:
	EntityDrawItem(Project &projectRef, EntityTreeItemData *pModelItemData, HyEntity2d *pParent);
	virtual ~EntityDrawItem();

	EntityTreeItemData *GetEntityTreeItemData() const;

	IHyLoadable2d *GetHyNode();
	ShapeCtrl &GetShapeCtrl();
	TransformCtrl &GetTransformCtrl();

	bool IsMouseInBounds();

	void RefreshTransform(HyCamera2d *pCamera);

	void ExtractTransform(HyShape2d &boundingShapeOut, glm::mat4 &transformMtxOut);

	void ShowTransformCtrl(bool bShowGrabPoints);
	void HideTransformCtrl();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SubEntity : public HyEntity2d
{
	QList<QPair<IHyLoadable2d *, ItemType>>	m_ChildPtrList;

public:
	SubEntity(Project &projectRef, const QJsonArray &descArray, HyEntity2d *pParent);
	virtual ~SubEntity();

	void RefreshProperties(const QList<QJsonObject> &propsObjList);
};

void ApplyExtrapolatedProperties(IHyLoadable2d *pHyNode, ShapeCtrl *pShapeCtrl, ItemType eItemType, bool bIsSelected, QJsonObject propsObj, HyCamera2d *pCamera);

#endif // ENTITYDRAWITEM_H
