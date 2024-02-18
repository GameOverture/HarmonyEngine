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

#include <QGraphicsView>

class AuxDopeSheet;
class EntityStateData;
class EntityTreeItemData;
class EntityDopeSheetScene;
class GraphicsTweenKnobItem;

class EntityDopeSheetView : public QGraphicsView
{
	AuxDopeSheet *				m_pAuxDopeSheet;
	EntityStateData *			m_pStateData;

	QPointF						m_MouseScenePos;
	EntityTreeItemData *		m_pMouseHoverItem;

	bool						m_bTimeLineMouseDown;
	bool						m_bLeftSideDirty;

	float						m_fZoom;

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

	float GetZoom() const;

protected:
	virtual void contextMenuEvent(QContextMenuEvent *pEvent) override;

	virtual void drawBackground(QPainter *painter, const QRectF &rect) override;
	virtual void drawForeground(QPainter *pPainter, const QRectF &rect) override;

	virtual bool event(QEvent *pEvent) override;

	virtual void mouseMoveEvent(QMouseEvent *pEvent) override;
	virtual void mousePressEvent(QMouseEvent *pEvent) override;
	virtual void mouseReleaseEvent(QMouseEvent *pEvent) override;

protected Q_SLOTS:
	void OnCopy();
	void OnPaste();

private:
	void DrawShadowText(QPainter *pPainter, QRectF textRect, const QString &sText, HyColor color = HyColor::WidgetFrame, HyColor shadowColor = HyColor::Black);
	void DrawCurrentFrameIndicator(QPainter *pPainter, qreal fPosX, qreal fPosY, HyColor color);

	QList<EntityTreeItemData *> GetItems() const;

	int GetNearestFrame(qreal fScenePosX) const;

private Q_SLOTS:
	void OnSelectAllItemKeyFrames();
	void OnDeselectItemKeyFrames();
	void OnDeleteKeyFrames();
};

#endif // ENTITYDOPESHEETVIEW_H
