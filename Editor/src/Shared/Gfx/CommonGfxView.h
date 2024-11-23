/**************************************************************************
*	CommonGfxView.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2024 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef COMMONGFXVIEW_H
#define COMMONGFXVIEW_H

#include <QGraphicsView>
#include <QTimer>

class AuxDopeSheet;
class EntityStateData;
class EntityTreeItemData;
class EntityDopeSheetScene;
class GraphicsTweenKnobItem;

class CommonGfxView : public QGraphicsView
{
	Q_OBJECT

protected:
	QPointF						m_MouseScenePos;

	QTimer						m_PanTimer;
	uint32						m_uiPanFlags;
	HyLocomotion2d				m_PanLocomotion;

public:
	CommonGfxView(QWidget *pParent = nullptr);
	virtual ~CommonGfxView();

	float GetZoom() const;

protected:
	virtual bool event(QEvent *pEvent) override;

	virtual void keyPressEvent(QKeyEvent *pEvent) override;
	virtual void keyReleaseEvent(QKeyEvent *pEvent) override;

	virtual void mouseMoveEvent(QMouseEvent *pEvent) override;
	virtual void wheelEvent(QWheelEvent *pEvent) override;

private Q_SLOTS:
	void OnPanTimer();
};

#endif // COMMONGFXVIEW_H
