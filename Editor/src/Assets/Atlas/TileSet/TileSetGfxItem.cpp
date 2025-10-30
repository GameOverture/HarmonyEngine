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
#include "TileSetGfxItem.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

const int iTILE_PADDING = 0;

TileSetGfxItem::TileSetGfxItem(QPointF ptCurPos, QSize vSize, const QPixmap& pixmapRef, const QPolygonF& outlinePolygon) :
	QGraphicsItem(nullptr),
	m_bSelected(true),
	m_pRectItem(nullptr),
	m_pPixmapItem(nullptr),
	m_pShapeItem(nullptr)
{
	m_pRectItem = new QGraphicsRectItem(0.0f, 0.0f, vSize.width() + 1, vSize.height() + 1);
	m_pRectItem->setPos(ptCurPos.x(), ptCurPos.y());
	m_pRectItem->setPen(QPen(QBrush(HyGlobal::ConvertHyColor(HyColor::Green)), 1.0f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
	m_pRectItem->setParentItem(this);

	m_pPixmapItem = new QGraphicsPixmapItem(pixmapRef);
	m_pPixmapItem->setParentItem(this);

	m_pShapeItem = new QGraphicsPolygonItem(outlinePolygon);
	m_pShapeItem->setPen(QPen(QBrush(HyGlobal::ConvertHyColor(HyColor::Orange)), 1.0f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
	m_pShapeItem->setParentItem(this);

	//// Have this item pass through mouse events to the child m_pGfxRectItem
	//setAcceptHoverEvents(false);
	//setAcceptedMouseButtons(Qt::NoButton);
}

TileSetGfxItem::~TileSetGfxItem()
{
	delete m_pRectItem;
	delete m_pPixmapItem;
	delete m_pShapeItem;
}

bool TileSetGfxItem::IsSelected() const
{
	return m_bSelected;
}

void TileSetGfxItem::SetSelected(bool bSelected)
{
	m_bSelected = bSelected;
}

QPointF TileSetGfxItem::GetPos() const
{
	return m_pRectItem->pos();
}

const QPixmap& TileSetGfxItem::GetPixmap() const
{
	return m_pPixmapItem->pixmap();
}

/*virtual*/ QRectF TileSetGfxItem::boundingRect() const /*override*/
{
	return m_pRectItem->boundingRect().adjusted(-iTILE_PADDING, -iTILE_PADDING, iTILE_PADDING, iTILE_PADDING); // Adjust for selection border
}

/*virtual*/ void TileSetGfxItem::paint(QPainter *pPainter, const QStyleOptionGraphicsItem *option, QWidget *widget) /*override*/
{
	//QGraphicsPixmapItem::paint(pPainter, option, widget);
	//if(IsSelected())
	//{
	//	pPainter->setPen(Qt::DashLine);
	//	pPainter->drawRect(boundingRect());
	//}
}

///*virtual*/ QVariant TileGfxItem::itemChange(GraphicsItemChange eChange, const QVariant &value) /*override*/
//{
//	if(eChange == QGraphicsItem::ItemSelectedHasChanged)
//	{
//		if(IsSelected())
//			setZValue(1.0); // Bring selected item to the front
//		else
//			setZValue(0.0); // Reset z-value when not selected
//	}
//
//	return QGraphicsPixmapItem::itemChange(eChange, value);
//}

///*virtual*/ void TileGfxItem::mousePressEvent(QGraphicsSceneMouseEvent *pEvent) /*override*/
//{
//	if(pEvent->button() == Qt::LeftButton)
//	{
//		setSelected(!IsSelected()); // Toggle selection on left click
//		pEvent->accept();
//	}
//	else
//	{
//		QGraphicsPixmapItem::mousePressEvent(pEvent); // Pass other events to base class
//	}
//}
//
///*virtual*/ void TileGfxItem::mouseMoveEvent(QGraphicsSceneMouseEvent *pEvent) /*override*/
//{
//	if(IsSelected() && pEvent->buttons() & Qt::LeftButton)
//	{
//		setPos(mapToScene(pEvent->pos() - pEvent->buttonDownPos(Qt::LeftButton)));
//		pEvent->accept();
//	}
//	else
//	{
//		QGraphicsPixmapItem::mouseMoveEvent(pEvent); // Pass other events to base class
//	}
//}
//
///*virtual*/ void TileGfxItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *pEvent) /*override*/
//{
//	if(pEvent->button() == Qt::LeftButton)
//	{
//		pEvent->accept();
//	}
//	else
//	{
//		QGraphicsPixmapItem::mouseReleaseEvent(pEvent); // Pass other events to base class
//	}
//}
//
///*virtual*/ void TileGfxItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *pEvent) /*override*/
//{
//	QGraphicsPixmapItem::contextMenuEvent(pEvent); // Pass context menu events to base class
//}
