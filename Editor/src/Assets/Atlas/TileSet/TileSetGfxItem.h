/**************************************************************************
 *	TileGfxItem.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef TILEGFXITEM_H
#define TILEGFXITEM_H

#include "Global.h"
#include <QGraphicsScene>
#include <QGraphicsRectItem>

class AtlasTileSet;

class TileSetGfxItem : public QGraphicsItem
{
	bool								m_bSelected;
	QGraphicsRectItem *					m_pRectItem;
	QGraphicsPixmapItem *				m_pPixmapItem;
	QGraphicsPolygonItem *				m_pShapeItem;		// This is the shape outline of the tile as it sits in a grid, especially helpful for isometric and hexagon

public:
	TileSetGfxItem(QPointF ptCurPos, QSize vSize, const QPixmap& pixmapRef, const QPolygonF& outlinePolygon);
	~TileSetGfxItem();

	//void SetSelected(bool bSelected)
	//{
	//	m_bSelected = bSelected;

	//	HyColor selectedColor = m_bSelected ? HyColor::Orange : HyColor::Black;
	//	m_pRectItem->setPen(QPen(QBrush(HyGlobal::ConvertHyColor(selectedColor)), 1.0f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
	//	m_pShapeItem->setVisible(m_bSelected);
	//}

	//class SelectionRectItem : public QGraphicsRectItem {
	//public:
	//	SelectionRectItem(const int iPADDING, TileGfxItem *pParent = nullptr) :
	//		QGraphicsRectItem(pParent)
	//	{
	//		QSize vPixmapSize = pParent->pixmap().size();
	//		setRect(-iPADDING, -iPADDING, vPixmapSize.width() + (iPADDING * 2), vPixmapSize.height() + (iPADDING * 2));
	//		setPen(QPen(HyGlobal::ConvertHyColor(HyColor::Orange), 1.0f, Qt::PenStyle::SolidLine));
	//		setBrush(Qt::NoBrush);

	//		//setFlags(QGraphicsItem::ItemIsSelectable |
	//		//	QGraphicsItem::ItemIsMovable |
	//		//	QGraphicsItem::ItemSendsGeometryChanges);
	//		setVisible(false);
	//		setAcceptHoverEvents(true);
	//	}

	//protected:
	//	void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override {
	//		setPen(QPen(HyGlobal::ConvertHyColor(HyColor::White), 1.0f, Qt::PenStyle::SolidLine));
	//		QGraphicsRectItem::hoverEnterEvent(event);
	//	}

	//	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override {
	//		setPen(QPen(HyGlobal::ConvertHyColor(HyColor::Orange), 1.0f, Qt::PenStyle::SolidLine));
	//		QGraphicsRectItem::hoverLeaveEvent(event);
	//	}
	//};
	//SelectionRectItem *		m_pGfxRectItem;

	bool IsSelected() const;
	void SetSelected(bool bSelected);

	QPointF GetPos() const;
	void SetPos(QPointF ptNewPos, QSize regionSize, QPointF vOffset, const QPolygonF& outlinePolygon)
	{
		m_pShapeItem->setPos(ptNewPos);

		m_pRectItem->setRect(0.0f, 0.0f, regionSize.width(), regionSize.height());
		m_pRectItem->setPos(ptNewPos);

		m_pPixmapItem->setPos(ptNewPos);

		m_pShapeItem->setPolygon(outlinePolygon);
		m_pShapeItem->setPos(ptNewPos.x() + (regionSize.width() * 0.5f) + vOffset.x(), ptNewPos.y() + (regionSize.height() * 0.5f) + vOffset.y());
	}

	const QPixmap& GetPixmap() const;

	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
	//virtual QVariant itemChange(GraphicsItemChange eChange, const QVariant &value) override;
	//virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	//virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	//virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
	//virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

	
};

#endif // TILEGFXITEM_H
