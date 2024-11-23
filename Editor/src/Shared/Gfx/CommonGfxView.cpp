/**************************************************************************
*	CommonGfxView.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2024 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "CommonGfxView.h"

#include <QPainter>
#include <QScrollBar>
#include <QGraphicsItem>
#include <QMouseEvent>

CommonGfxView::CommonGfxView(QWidget *pParent /*= nullptr*/) :
	QGraphicsView(pParent),
	m_PanTimer(this),
	m_uiPanFlags(0)
{
	setAlignment(Qt::AlignLeft | Qt::AlignTop);

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	
	connect(&m_PanTimer, SIGNAL(timeout()), this, SLOT(OnPanTimer()));
}

/*virtual*/ CommonGfxView::~CommonGfxView()
{
}

float CommonGfxView::GetZoom() const
{
	return transform().m11();
}

/*virtual*/ bool CommonGfxView::event(QEvent *pEvent) /*override*/
{
	if(pEvent->type() == QEvent::HoverEnter || pEvent->type() == QEvent::HoverLeave)
	{
		m_MouseScenePos.setX(0.0f);
		m_MouseScenePos.setY(0.0f);
		update();

		if(pEvent->type() == QEvent::HoverEnter)
			setFocus();
		else if(pEvent->type() == QEvent::HoverLeave)
		{
			m_uiPanFlags = 0;
			clearFocus();
		}
	}

	return QGraphicsView::event(pEvent);
}

/*virtual*/ void CommonGfxView::keyPressEvent(QKeyEvent *pEvent) /*override*/
{
	if(pEvent->key() == Qt::Key_A)
	{
		m_uiPanFlags |= PAN_LEFT;
		if(m_PanTimer.isActive() == false)
			m_PanTimer.start(16);
	}
	else if(pEvent->key() == Qt::Key_D)
	{
		m_uiPanFlags |= PAN_RIGHT;
		if(m_PanTimer.isActive() == false)
			m_PanTimer.start(16);
	}
	else if(pEvent->key() == Qt::Key_W)
	{
		m_uiPanFlags |= PAN_UP;
		if(m_PanTimer.isActive() == false)
			m_PanTimer.start(16);
	}
	else if(pEvent->key() == Qt::Key_S)
	{
		m_uiPanFlags |= PAN_DOWN;
		if(m_PanTimer.isActive() == false)
			m_PanTimer.start(16);
	}
}

/*virtual*/ void CommonGfxView::keyReleaseEvent(QKeyEvent *pEvent) /*override*/
{
	if(pEvent->key() == Qt::Key_A)
		m_uiPanFlags &= ~PAN_LEFT;
	else if(pEvent->key() == Qt::Key_D)
		m_uiPanFlags &= ~PAN_RIGHT;
	else if(pEvent->key() == Qt::Key_W)
		m_uiPanFlags &= ~PAN_UP;
	else if(pEvent->key() == Qt::Key_S)
		m_uiPanFlags &= ~PAN_DOWN;
}

/*virtual*/ void CommonGfxView::mouseMoveEvent(QMouseEvent *pEvent) /*override*/
{
	m_MouseScenePos = mapToScene(pEvent->pos());
	QGraphicsView::mouseMoveEvent(pEvent);
}

/*virtual*/ void CommonGfxView::wheelEvent(QWheelEvent *pEvent) /*override*/
{
	if(pEvent->angleDelta().y() > 0)
		scale(1.1, 1.1);
	else
		scale(0.9, 0.9);
	pEvent->accept();
}

void CommonGfxView::OnPanTimer()
{
	if(m_uiPanFlags & PAN_LEFT)
		m_PanLocomotion.GoLeft();
	if(m_uiPanFlags & PAN_RIGHT)
		m_PanLocomotion.GoRight();
	if(m_uiPanFlags & PAN_UP)
		m_PanLocomotion.GoUp();
	if(m_uiPanFlags & PAN_DOWN)
		m_PanLocomotion.GoDown();

	m_PanLocomotion.Update();

	if(m_PanLocomotion.IsMoving())
	{
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() + m_PanLocomotion.GetVelocity().x);
		verticalScrollBar()->setValue(verticalScrollBar()->value() + (m_PanLocomotion.GetVelocity().y * -1.0f));

		if(horizontalScrollBar()->value() == horizontalScrollBar()->minimum() || horizontalScrollBar()->value() == horizontalScrollBar()->maximum())
			m_PanLocomotion.StopX();
		if(verticalScrollBar()->value() == verticalScrollBar()->minimum() || verticalScrollBar()->value() == verticalScrollBar()->maximum())
			m_PanLocomotion.StopY();
	}
	else
		m_PanTimer.stop();
}
