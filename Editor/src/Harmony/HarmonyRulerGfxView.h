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

#define RULER_WIDTH				20.0f
#define RULER_TEXT_HEIGHT		8.0f
#define RULER_MAIN_NOTCH		8.0f
#define RULER_SUB_NOTCH			5.0f

class HarmonyRulerGfxView : public QGraphicsView
{
	HyOrientation		m_eOrientation;

	int					m_iDrawStart;
	int					m_iDrawEnd;
	int					m_iDrawWidth;

	int					m_iWorldStart;
	int					m_iWorldEnd;
	int					m_iWorldWidth;

	bool				m_bShowMouse;

public:
	HarmonyRulerGfxView(QWidget *pParent = nullptr);
	virtual ~HarmonyRulerGfxView();

	void Init(HyOrientation eOrientation, HyColor bgColor);

	bool IsShowMouse() const;
	void ShowMouse(bool bShowMouse);

protected:
	virtual void drawForeground(QPainter *pPainter, const QRectF &rect) override;

	virtual void mouseMoveEvent(QMouseEvent *pEvent) override;
	virtual void mousePressEvent(QMouseEvent *pEvent) override;
	virtual void mouseReleaseEvent(QMouseEvent *pEvent) override;

private:
	float ConvertWorldToDraw(float fWorldPos);
	float ConvertDrawToWorld(float fDrawPos);
};

#endif // HARMONYRULERGFXVIEW_H
