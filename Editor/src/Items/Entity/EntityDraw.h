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

#include "Global.h"
#include "IDrawEx.h"
#include "EntityModel.h"
#include "EntityDrawItem.h"

enum EditModeState
{
	EDITMODE_Off = 0,
	EDITMODE_Idle,					// Mouse cursor determined by draw models
	EDITMODE_MouseDownOutside,		// Click started outside of the edit item's bounds
	EDITMODE_MouseDragMarquee,		// Click-dragging a marquee select box
	EDITMODE_MouseDownTransform,	// Click started on item to be manipulated
	EDITMODE_MouseDragTransform,	// Transforming (translating, rotating, scaling) the edit item
};

class EntityDraw : public IDrawEx
{
	HyEntity2d 								m_RootEntity;

	bool									m_bPlayingPreview;

	EditModeState							m_eEditModeState;
	HyPrimitive2d							m_EditModeWindowOutline;

public:
	EntityDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef);
	virtual ~EntityDraw();

	virtual void OnUndoStackIndexChanged(int iIndex) override;

	virtual void OnKeyPressEvent(QKeyEvent *pEvent) override;
	virtual void OnKeyReleaseEvent(QKeyEvent *pEvent) override;

	virtual void OnMouseMoveEvent(QMouseEvent *pEvent) override;
	virtual void OnMousePressEvent(QMouseEvent *pEvent) override;
	virtual void OnMouseReleaseEvent(QMouseEvent *pEvent) override;

	bool SetEditMode(bool bEnable);
	EntityDrawItem *GetCurEditItem() const;

	//void RequestClearShapeEdit();

	void SetExtrapolatedProperties();

protected:
	virtual void OnApplyJsonMeta(QJsonObject &itemMetaObj) override;
	virtual void OnResizeRenderer() override;
	virtual void OnRequestSelection(QList<IDrawExItem *> selectionList) override;
	virtual void OnPerformTransform() override;
};

#endif // ENTITYDRAW_H
