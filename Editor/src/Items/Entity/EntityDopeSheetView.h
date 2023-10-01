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

class EntityStateData;
class EntityDopeSheetScene;

class EntityDopeSheetView : public QGraphicsView
{
	EntityStateData *			m_pStateData;

	bool						m_bTimeLineMouseDown;

public:
	EntityDopeSheetView(QWidget *pParent = nullptr);
	virtual ~EntityDopeSheetView();

	EntityDopeSheetScene *GetScene() const;
	void SetScene(EntityStateData *pStateData);

protected:
	virtual void drawBackground(QPainter *painter, const QRectF &rect) override;
	virtual void drawForeground(QPainter *pPainter, const QRectF &rect) override;

	virtual void mouseMoveEvent(QMouseEvent *pEvent) override;
	virtual void mousePressEvent(QMouseEvent *pEvent) override;
	virtual void mouseReleaseEvent(QMouseEvent *pEvent) override;

private:
	void DrawShadowText(QPainter *pPainter, QRectF textRect, const QString &sText, HyColor color = HyColor::WidgetFrame, HyColor shadowColor = HyColor::Black);
	void DrawCurrentFrameIndicator(QPainter *pPainter, qreal fPosX, qreal fPosY, HyColor color);

	void OnMousePressTimeline(QPoint ptScreenPos);
};

#endif // ENTITYDOPESHEETVIEW_H
