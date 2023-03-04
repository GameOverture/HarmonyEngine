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
#include "EntityDrawItem.h"

enum EditorShape
{
	SHAPE_None = -1,
	SHAPE_Box = 0,
	SHAPE_Circle,
	SHAPE_Polygon,
	SHAPE_Segment,
	SHAPE_LineChain,
	SHAPE_LineLoop
};

class EntityDraw : public IDraw
{
	QList<EntityDrawItem *>					m_ItemList;

	QList<EntityDrawItem *>					m_SelectedItemList;
	TransformCtrl							m_MultiTransform;
	float									m_fMultiTransformStartRot;

	bool									m_bCurHoverMultiTransform;
	EntityDrawItem *						m_pCurHoverItem;
	GrabPoint								m_eCurHoverGrabPoint;

	enum DragState
	{
		DRAGSTATE_None = 0,
		DRAGSTATE_Marquee,
		DRAGSTATE_Starting,
		DRAGSTATE_Transforming,
		DRAGSTATE_DrawingShape,
	};
	DragState								m_eDragState;
	glm::vec2								m_ptDragStart;
	glm::vec2								m_ptDragCenter;
	glm::vec2								m_vDragStartSize;
	glm::vec2								m_ptDragAnchorPoint;

	EditorShape								m_eCurDrawShape;
	HyPrimitive2d							m_DrawShape;

	HyEntity2d								m_ActiveTransform;
	QList<glm::mat4>						m_PrevTransformList;

public:
	EntityDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef);
	virtual ~EntityDraw();

	virtual void OnKeyPressEvent(QKeyEvent *pEvent) override;
	virtual void OnKeyReleaseEvent(QKeyEvent *pEvent) override;
	virtual void OnMousePressEvent(QMouseEvent *pEvent) override;
	virtual void OnMouseReleaseEvent(QMouseEvent *pEvent) override;
	virtual void OnMouseWheelEvent(QWheelEvent *pEvent) override;
	virtual void OnMouseMoveEvent(QMouseEvent *pEvent) override;

	void OnSelectionChange(QList<EntityTreeItemData *> selectedItemDataList, QList<EntityTreeItemData *> deselectedItemDataList);
	void RequestSelection(QList<EntityDrawItem *> selectionList);

	void RefreshTransforms();

	void SetDrawShape(EditorShape eShape, bool bAsPrimitive);
	void UpdateDrawShape(bool bCtrlModifer);
	void ClearDrawShape();

protected:
	virtual void OnApplyJsonMeta(QJsonObject &itemMetaObj) override;
	virtual void OnShow() override;
	virtual void OnHide() override;
	virtual void OnResizeRenderer() override;
	virtual void OnZoom(HyZoomLevel eZoomLevel) override;

	void SetEverythingStale();
	EntityDrawItem *FindStaleChild(HyGuiItemType eType, QUuid uuid);
	void DeleteStaleChildren();

	Qt::CursorShape GetGrabPointCursorShape(GrabPoint eGrabPoint, float fRotation) const;
};

#endif // ENTITYDRAW_H
