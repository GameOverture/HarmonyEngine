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

	void RefreshJson(QJsonObject descObj, QJsonObject propObj, HyCamera2d *pCamera);
	void RefreshTransform(HyCamera2d *pCamera);

	void ExtractTransform(HyShape2d &boundingShapeOut, glm::mat4 &transformMtxOut);

	void ShowTransformCtrl(bool bShowGrabPoints);
	void HideTransformCtrl();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SubEntity : public HyEntity2d
{
	QList<IHyLoadable2d *>					m_ChildPtrList;

public:
	SubEntity(HyEntity2d *pParent);
	virtual ~SubEntity();

	void Assemble(Project &projectRef, QJsonArray descListArray, QJsonArray propListArray);
};

#endif // ENTITYDRAWITEM_H
