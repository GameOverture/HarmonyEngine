/**************************************************************************
*	HarmonyRulerGfxView.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2024 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HARMONYRULERGFXVIEW_H
#define HARMONYRULERGFXVIEW_H

#include <QGraphicsView>

class HarmonyRulerGfxView : public QGraphicsView
{
	HyOrientation		m_eOrientation;

public:
	HarmonyRulerGfxView(QWidget *pParent = nullptr);
	virtual ~HarmonyRulerGfxView();

	void Init(HyOrientation eOrientation);

protected:
	virtual void drawBackground(QPainter *painter, const QRectF &rect) override;
	virtual void drawForeground(QPainter *pPainter, const QRectF &rect) override;

	virtual bool event(QEvent *pEvent) override;

	virtual void mouseMoveEvent(QMouseEvent *pEvent) override;
	virtual void mousePressEvent(QMouseEvent *pEvent) override;
	virtual void mouseReleaseEvent(QMouseEvent *pEvent) override;
};

#endif // HARMONYRULERGFXVIEW_H
