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

	bool									m_bIsShapeAddPrimitive; // True when primitive, false when bounding volume shape
	EntityDrawItem *						m_pCurVertexEditItem;

public:
	EntityDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef);
	virtual ~EntityDraw();

	virtual void OnUndoStackIndexChanged(int iIndex) override;

	virtual void OnKeyPressEvent(QKeyEvent *pEvent) override;
	virtual void OnKeyReleaseEvent(QKeyEvent *pEvent) override;

	virtual void OnMouseMoveEvent(QMouseEvent *pEvent) override;
	virtual void OnMousePressEvent(QMouseEvent *pEvent) override;
	virtual void OnMouseReleaseEvent(QMouseEvent *pEvent) override;

	bool IsActionSemIdle() const;
	bool IsActionSemTransforming() const;

	EditorShape GetShapeAddType() const;
	bool SetAsShapeAdd(EditorShape eShape, bool bAsPrimitive);
	void SetAsShapeEditMode();
	void ActivateVemOnNextJsonMeta();

	void RequestClearShapeEdit();
	void ClearShapeEdit();

	void SetExtrapolatedProperties();

protected:
	virtual void OnApplyJsonMeta(QJsonObject &itemMetaObj) override;
	virtual void OnResizeRenderer() override;
	virtual void OnRequestSelection(QList<IDrawExItem *> selectionList) override;
	virtual void OnPerformTransform() override;
};

#endif // ENTITYDRAW_H
