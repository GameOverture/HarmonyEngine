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
	QList<EntityDrawItem *>					m_SelectedItemList;

	ShapeCtrl								m_DragShape;		// Used for marquee selects and initially adding new shapes

	// Multi transform used when 'm_SelectedItemList' contains more than 1 item. It wraps around all selected items.
	TransformCtrl							m_MultiTransform;
	float									m_fMultiTransformStartRot;

	// Mouse hovering
	EntityDrawItem *						m_pCurHoverItem;
	TransformCtrl::GrabPointType			m_eCurHoverGrabPoint;

	// Mouse press/dragging
	HyTimer									m_PressTimer;
	bool									m_bSelectionHandled;
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

	enum ShapeEditState
	{
		SHAPESTATE_None = 0,
		SHAPESTATE_DragAddPrimitive,		// Uses 'm_DragShape' when initially placing a new primitive
		SHAPESTATE_DragAddShape,			// Uses 'm_DragShape' when initially placing a new shape
		SHAPESTATE_VertexEditMode			// When editing polygons, line chains, and line loops
	};
	ShapeEditState							m_eShapeEditState;
	EntityDrawItem *						m_pCurVertexEditItem;
	ShapeCtrl::VemAction					m_eCurVemAction;

	// Used to help transform (translate, rotate, scale) selected items
	HyEntity2d								m_ActiveTransform;
	QList<glm::mat4>						m_PrevTransformList;

	bool									m_bActivateVemOnNextJsonMeta;

public:
	EntityDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef);
	virtual ~EntityDraw();

	virtual void OnUndoStackIndexChanged(int iIndex) override;

	virtual void OnKeyPressEvent(QKeyEvent *pEvent) override;
	virtual void OnKeyReleaseEvent(QKeyEvent *pEvent) override;

	virtual void OnMouseWheelEvent(QWheelEvent *pEvent) override;

	virtual void OnMouseMoveEvent(QMouseEvent *pEvent) override;
	virtual void OnMousePressEvent(QMouseEvent *pEvent) override;
	virtual void OnMouseReleaseEvent(QMouseEvent *pEvent) override;

	void RequestSelection(QList<EntityDrawItem *> selectionList);

	void RefreshTransforms();

	void SetShapeEditDrag(EditorShape eShape, bool bAsPrimitive);
	void ActivateVemOnNextJsonMeta();
	void SetShapeEditVertex();

	void RequestClearShapeEdit();
	void ClearShapeEdit();

protected:
	virtual void OnApplyJsonMeta(QJsonObject &itemMetaObj) override;
	virtual void OnShow() override;
	virtual void OnHide() override;
	virtual void OnResizeRenderer() override;
	virtual void OnZoom(HyZoomLevel eZoomLevel) override;

	Qt::CursorShape GetGrabPointCursorShape(TransformCtrl::GrabPointType eGrabPoint, float fRotation) const;

	void DoMouseMove(bool bCtrlMod, bool bShiftMod);

	void DoMouseMove_Select(bool bCtrlMod, bool bShiftMod);
	void DoMousePress_Select(bool bCtrlMod, bool bShiftMod);
	void DoMouseRelease_Select(bool bCtrlMod, bool bShiftMod);

	void BeginTransform();

	void DoMouseMove_Transform(bool bCtrlMod, bool bShiftMod);
	void DoMouseRelease_Transform();

	void DoMouseMove_ShapeEdit(bool bCtrlMod, bool bShiftMod);
	void DoMousePress_ShapeEdit(bool bCtrlMod, bool bShiftMod);
	void DoMouseRelease_ShapeEdit(bool bCtrlMod, bool bShiftMod);

	static void OnMousePressTimer(void *pData);
};

#endif // ENTITYDRAW_H
