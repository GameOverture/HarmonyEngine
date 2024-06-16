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

class EntityDraw : public IDrawEx
{
	HyEntity2d 								m_RootEntity;

	bool									m_bActivateVemOnNextJsonMeta;
	bool									m_bPlayingPreview;

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

public:
	EntityDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef);
	virtual ~EntityDraw();

	virtual void OnUndoStackIndexChanged(int iIndex) override;

	virtual void OnKeyPressEvent(QKeyEvent *pEvent) override;
	virtual void OnKeyReleaseEvent(QKeyEvent *pEvent) override;

	virtual void OnMouseMoveEvent(QMouseEvent *pEvent) override;
	virtual void OnMousePressEvent(QMouseEvent *pEvent) override;
	virtual void OnMouseReleaseEvent(QMouseEvent *pEvent) override;

	void SetShapeEditDrag(EditorShape eShape, bool bAsPrimitive);
	void ActivateVemOnNextJsonMeta();
	void SetShapeEditVertex();

	void RequestClearShapeEdit();
	void ClearShapeEdit();

	void SetExtrapolatedProperties(bool bPreviewPlaying);

protected:
	virtual void OnApplyJsonMeta(QJsonObject &itemMetaObj) override;

	void RequestSelection(QList<EntityDrawItem *> selectionList);

	void DoMouseMove_ShapeEdit(bool bCtrlMod, bool bShiftMod);
	void DoMousePress_ShapeEdit(bool bCtrlMod, bool bShiftMod);
	void DoMouseRelease_ShapeEdit(bool bCtrlMod, bool bShiftMod);
};

#endif // ENTITYDRAW_H
