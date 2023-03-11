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
#include "IDraw.h"
#include "EntityModel.h"
#include "EntityDrawItem.h"

class EntityDraw : public IDraw
{
	QList<EntityDrawItem *>					m_ItemList;
	QList<EntityDrawItem *>					m_StaleItemList;
	QList<EntityDrawItem *>					m_SelectedItemList;

	MarqueeBox								m_Marquee;

	// Multi transform used when 'm_SelectedItemList' contains more than 1 item. It wraps around all selected items.
	TransformCtrl							m_MultiTransform;
	float									m_fMultiTransformStartRot;

	// Mouse hovering
	EntityDrawItem *						m_pCurHoverItem;
	GrabPoint								m_eCurHoverGrabPoint;

	// Mouse press/dragging
	HyTimer									m_PressTimer;
	enum DragState
	{
		DRAGSTATE_None = 0,
		DRAGSTATE_Marquee,					// When clicking outside any items' bounds
		DRAGSTATE_Pending,					// Using 'm_PressTimer' (or mouse movement) to determine if selection or drag will occur
		DRAGSTATE_Transforming				// Dragging confirmed, and transforming is occurring
	};
	DragState								m_eDragState;
	glm::vec2								m_ptDragStart;
	glm::vec2								m_ptDragCenter;
	glm::vec2								m_vDragStartSize;


	//EditorShape								m_eCurDrawShape;
	//HyPrimitive2d							m_DrawShape;

	// Used to help transform (translate, rotate, scale) selected items
	HyEntity2d								m_ActiveTransform;
	QList<glm::mat4>						m_PrevTransformList;

public:
	EntityDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef);
	virtual ~EntityDraw();

	virtual void OnKeyPressEvent(QKeyEvent *pEvent) override;
	virtual void OnKeyReleaseEvent(QKeyEvent *pEvent) override;

	virtual void OnMouseWheelEvent(QWheelEvent *pEvent) override;

	virtual void OnMouseMoveEvent(QMouseEvent *pEvent) override;
	virtual void OnMousePressEvent(QMouseEvent *pEvent) override;
	virtual void OnMouseReleaseEvent(QMouseEvent *pEvent) override;

	void OnSelectionChange(QList<EntityTreeItemData *> selectedItemDataList, QList<EntityTreeItemData *> deselectedItemDataList);
	void RequestSelection(QList<EntityDrawItem *> selectionList);

	void RefreshTransforms();

	//void SetDrawShape(EditorShape eShape, bool bAsPrimitive);
	//void UpdateDrawShape(bool bCtrlModifer);
	//void ClearDrawShape();

protected:
	virtual void OnApplyJsonMeta(QJsonObject &itemMetaObj) override;
	virtual void OnShow() override;
	virtual void OnHide() override;
	virtual void OnResizeRenderer() override;
	virtual void OnZoom(HyZoomLevel eZoomLevel) override;

	EntityDrawItem *FindStaleChild(HyGuiItemType eType, QUuid uuid);
	Qt::CursorShape GetGrabPointCursorShape(GrabPoint eGrabPoint, float fRotation) const;

	void DoMouseMove_Select();
	void DoMousePress_Select(bool bCtrlMod, bool bShiftMod);
	void DoMouseRelease_Select(bool bCtrlMod, bool bShiftMod);

	void BeginTransform();

	void DoMouseMove_Transform(bool bCtrlMod, bool bShiftMod);
	void DoMouseRelease_Transform();

	void DoMouseMove_NewShape();
	void DoMousePress_NewShape();
	void DoMouseRelease_NewShape();

	void DoMouseMove_EditShape();
	void DoMousePress_EditShape();
	void DoMouseRelease_EditShape();

	static void OnMousePressTimer(void *pData);
};

#endif // ENTITYDRAW_H
