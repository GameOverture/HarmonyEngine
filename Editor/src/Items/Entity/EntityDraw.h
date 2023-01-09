/**************************************************************************
*	EntityDraw.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ENTITYDRAW_H
#define ENTITYDRAW_H

#include "IDraw.h"
#include "EntityModel.h"
#include "EntityItemDraw.h"

class EntityDraw : public IDraw
{
	QList<EntityItemDraw *>					m_ItemList;
	TransformCtrl							m_Transform;

public:
	EntityDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef);
	virtual ~EntityDraw();

	virtual void OnKeyPressEvent(QKeyEvent *pEvent) override;
	virtual void OnKeyReleaseEvent(QKeyEvent *pEvent) override;
	virtual void OnMousePressEvent(QMouseEvent *pEvent) override;
	virtual void OnMouseReleaseEvent(QMouseEvent *pEvent) override;
	virtual void OnMouseWheelEvent(QWheelEvent *pEvent) override;
	virtual void OnMouseMoveEvent(QMouseEvent *pEvent) override;

	void RefreshSelectedItems();

protected:
	virtual void OnApplyJsonMeta(QJsonObject &itemMetaObj) override;
	virtual void OnShow() override;
	virtual void OnHide() override;
	virtual void OnResizeRenderer() override;
	virtual void OnZoom(HyZoomLevel eZoomLevel) override;

	void SetEverythingStale();
	EntityItemDraw *FindStaleChild(HyGuiItemType eType, QUuid uuid);
	void DeleteStaleChildren();

	void RefreshTransforms();

	QList<EntityItemDraw *> GetSelectedItems() const;
};

#endif // ENTITYDRAW_H
