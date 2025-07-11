/**************************************************************************
 *	TileGfxItem.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "TileGfxItem.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

const int iTILE_PADDING = 0;

TileGfxItem::TileGfxItem(const QPixmap &pixmap, QGraphicsItem *pParent /*= nullptr*/) :
	QGraphicsPixmapItem(pixmap, pParent),
	m_pGfxRectItem(nullptr)
{
	// Have this item pass through mouse events to the child m_pGfxRectItem
	setAcceptHoverEvents(false);
	setAcceptedMouseButtons(Qt::NoButton);

	m_pGfxRectItem = new SelectionRectItem(iTILE_PADDING, this);

	m_pGfxRectItem->setPen(QPen(Qt::DashLine));
	m_pGfxRectItem->setBrush(Qt::NoBrush);

	//m_pGfxRectItem->setZValue(0.1f);
	//m_pGfxRectItem->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);
	m_pGfxRectItem->setAcceptHoverEvents(true);
}

/*virtual*/ QRectF TileGfxItem::boundingRect() const /*override*/
{
	return QGraphicsPixmapItem::boundingRect().adjusted(-iTILE_PADDING, -iTILE_PADDING, iTILE_PADDING, iTILE_PADDING); // Adjust for selection border
}

/*virtual*/ QVariant TileGfxItem::itemChange(GraphicsItemChange eChange, const QVariant &value) /*override*/
{
	if(eChange == QGraphicsItem::ItemSelectedHasChanged)
	{
		if(isSelected())
			setZValue(1.0); // Bring selected item to the front
		else
			setZValue(0.0); // Reset z-value when not selected
	}

	return QGraphicsPixmapItem::itemChange(eChange, value);
}

/*virtual*/ void TileGfxItem::paint(QPainter *pPainter, const QStyleOptionGraphicsItem *option, QWidget *widget) /*override*/
{
	QGraphicsPixmapItem::paint(pPainter, option, widget);
	if(isSelected())
	{
		pPainter->setPen(Qt::DashLine);
		pPainter->drawRect(boundingRect());
	}
}

/*virtual*/ void TileGfxItem::mousePressEvent(QGraphicsSceneMouseEvent *pEvent) /*override*/
{
	if(pEvent->button() == Qt::LeftButton)
	{
		setSelected(!isSelected()); // Toggle selection on left click
		pEvent->accept();
	}
	else
	{
		QGraphicsPixmapItem::mousePressEvent(pEvent); // Pass other events to base class
	}
}

/*virtual*/ void TileGfxItem::mouseMoveEvent(QGraphicsSceneMouseEvent *pEvent) /*override*/
{
	if(isSelected() && pEvent->buttons() & Qt::LeftButton)
	{
		setPos(mapToScene(pEvent->pos() - pEvent->buttonDownPos(Qt::LeftButton)));
		pEvent->accept();
	}
	else
	{
		QGraphicsPixmapItem::mouseMoveEvent(pEvent); // Pass other events to base class
	}
}

/*virtual*/ void TileGfxItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *pEvent) /*override*/
{
	if(pEvent->button() == Qt::LeftButton)
	{
		pEvent->accept();
	}
	else
	{
		QGraphicsPixmapItem::mouseReleaseEvent(pEvent); // Pass other events to base class
	}
}

/*virtual*/ void TileGfxItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *pEvent) /*override*/
{
	QGraphicsPixmapItem::contextMenuEvent(pEvent); // Pass context menu events to base class
}
