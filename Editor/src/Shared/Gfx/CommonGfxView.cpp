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
	m_uiPanFlags(0),
	m_bMiddleMousePanning(false)
{
	setAlignment(Qt::AlignLeft | Qt::AlignTop);

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	
	connect(&m_PanTimer, SIGNAL(timeout()), this, SLOT(OnPanTimer()));
}

/*virtual*/ CommonGfxView::~CommonGfxView()
{
}

QString CommonGfxView::GetStatusLabel() const
{
	return m_sStatusLabel;
}

void CommonGfxView::SetStatusLabel(const QString &sStatusLabel)
{
	if(m_sStatusLabel != sStatusLabel)
	{
		m_sStatusLabel = sStatusLabel;
		update();
	}
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

/*virtual*/ void CommonGfxView::paintEvent(QPaintEvent *pEvent) /*override*/
{
	QGraphicsView::paintEvent(pEvent); // Paint scene normally

	if(m_sStatusLabel.isEmpty())
		return;

	// Now draw the overlay text
	const int iMARGIN = 5;

	QPainter painter(viewport());
	painter.setRenderHint(QPainter::TextAntialiasing);

	QFont font = painter.font();
	font.setPointSize(10);
	painter.setFont(font);

	// Draw semi-transparent black background
	QRectF bgRect = painter.boundingRect(QRectF(), Qt::AlignLeft | Qt::AlignTop, m_sStatusLabel).marginsAdded(QMarginsF(iMARGIN, iMARGIN, iMARGIN, iMARGIN));
	bgRect.moveTo(0, 0);

	painter.setBrush(QColor(0, 0, 0, 120));
	painter.setPen(Qt::NoPen);
	painter.drawRect(bgRect);
	// Draw label on the rect
	painter.setPen(Qt::white);
	painter.setBrush(Qt::NoBrush);
	bgRect = bgRect.marginsRemoved(QMarginsF(iMARGIN, iMARGIN, iMARGIN, iMARGIN));
	painter.drawText(bgRect, Qt::AlignLeft | Qt::AlignTop, m_sStatusLabel);
}

/*virtual*/ void CommonGfxView::keyPressEvent(QKeyEvent *pEvent) /*override*/
{
	if(m_bMiddleMousePanning)
		return;

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
	if(m_bMiddleMousePanning && m_MouseScenePos.isNull() == false)
	{
		QPointF vMovement = mapToScene(pEvent->pos()) - m_MouseScenePos.toPoint();
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() + (vMovement.x() * -1.0f));
		verticalScrollBar()->setValue(verticalScrollBar()->value() + (vMovement.y() * -1.0f));
	}

	m_MouseScenePos = mapToScene(pEvent->pos());
	QGraphicsView::mouseMoveEvent(pEvent);
}

/*virtual*/ void CommonGfxView::mousePressEvent(QMouseEvent *pEvent) /*override*/
{
	// If middle mouse button is pressed, start panning
	if(pEvent->button() == Qt::MiddleButton)
	{
		m_bMiddleMousePanning = true;
		m_PanTimer.stop();
		m_uiPanFlags = 0;
	}

	// TODO: Swap control and shift modifiers when QGraphicsView takes the wheel
	if(m_bMiddleMousePanning == false)
		QGraphicsView::mousePressEvent(pEvent);
}

/*virtual*/ void CommonGfxView::mouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
	m_bMiddleMousePanning = false;
	QGraphicsView::mouseReleaseEvent(pEvent);
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

	m_PanLocomotion.UpdateSimple();

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
