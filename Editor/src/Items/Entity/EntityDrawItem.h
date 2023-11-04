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
class EntityDopeSheetScene;

// NOTE: this class does not keep its state when removed, it is deleted (should not be passed to UndoCmd's)
class EntityDrawItem
{
	EntityTreeItemData *					m_pEntityTreeItemData;

	IHyLoadable2d *							m_pChild;

	TransformCtrl							m_Transform;
	ShapeCtrl								m_ShapeCtrl;

	struct TweenInfo
	{
		int m_iStartFrame;
		QVariant m_Start;
		QVariant m_Destination;
		float m_fDuration;
		TweenType m_eTweenType;

		TweenInfo() {
			Clear();
		}

		void Clear()
		{
			m_iStartFrame = -1;
			m_Start.clear();
			m_Destination.clear();
			m_fDuration = 0.0f;
			m_eTweenType = TWEEN_Unknown;
		}
	};

public:
	EntityDrawItem(Project &projectRef, EntityTreeItemData *pModelItemData, HyEntity2d *pParent);
	virtual ~EntityDrawItem();

	EntityTreeItemData *GetEntityTreeItemData() const;

	IHyLoadable2d *GetHyNode();
	void SetHyNode(const EntityDopeSheetScene &entityDopeSheetSceneRef, HyCamera2d *pCamera);

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

	void RefreshProperties(const QList<QJsonObject> &propsObjList, float fElapsedTime);
};
#endif // ENTITYDRAWITEM_H
