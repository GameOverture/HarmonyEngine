/**************************************************************************
*	EntityDopeSheetView.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ENTITYDOPESHEETVIEW_H
#define ENTITYDOPESHEETVIEW_H

#include "CommonGfxView.h"

#include <QGraphicsView>
#include <QTimer>

class AuxDopeSheet;
class EntityStateData;
class EntityTreeItemData;
class EntityDopeSheetScene;
class GraphicsTweenKnobItem;

class EntityDopeSheetView : public CommonGfxView
{
	Q_OBJECT

	AuxDopeSheet *				m_pAuxDopeSheet;
	EntityStateData *			m_pStateData;

	EntityTreeItemData *		m_pMouseHoverItem;
	bool						m_bHoveringExpandArrow;

	bool						m_bTimeLineMouseDown;
	bool						m_bLeftSideDirty;

	enum DragState
	{
		DRAGSTATE_None,
		DRAGSTATE_InitialPress,
		DRAGSTATE_Dragging,
	};
	DragState					m_eDragState;
	QPoint						m_ptDragStart; // In View Coordinates
	int							m_iDragFrame;
	GraphicsTweenKnobItem *		m_pGfxDragTweenKnobItem; // If dragging a tween duration knob

	EntityTreeItemData *		m_pContextClickItem;

public:
	EntityDopeSheetView(QWidget *pParent = nullptr);
	virtual ~EntityDopeSheetView();

	EntityDopeSheetScene *GetScene() const;
	void SetScene(AuxDopeSheet *pAuxDopeSheet, EntityStateData *pStateData);

	EntityTreeItemData *GetContextClickItem();

	void EnsureSelectedFrameVisible();

protected:
	virtual void contextMenuEvent(QContextMenuEvent *pEvent) override;

	virtual void drawBackground(QPainter *pPainter, const QRectF &rect) override;
	virtual void drawForeground(QPainter *pPainter, const QRectF &rect) override; // TODO: drawForeground() is used incorrectly - implement overlays for QGraphicsView by reimplementing paintEvent(), calling the base implementation and simply rendering on top of the viewport contents with QPainter

	virtual void mouseMoveEvent(QMouseEvent *pEvent) override;
	virtual void mousePressEvent(QMouseEvent *pEvent) override;
	virtual void mouseDoubleClickEvent(QMouseEvent *pEvent) override;
	virtual void mouseReleaseEvent(QMouseEvent *pEvent) override;


	// TODO: Remove once fixed
	virtual void wheelEvent(QWheelEvent *pEvent) override { QGraphicsView::wheelEvent(pEvent); };

private:
	void DrawShadowText(QPainter *pPainter, QRectF textRect, const QString &sText, HyColor textColor);
	void DrawCurrentFrameIndicator(QPainter *pPainter, qreal fPosX, qreal fPosY);

	QList<EntityTreeItemData *> GetItems() const;

	int GetNearestFrame(qreal fScenePosX) const;
};

#endif // ENTITYDOPESHEETVIEW_H
