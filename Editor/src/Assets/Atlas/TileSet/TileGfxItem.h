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

#include <QGraphicsScene>
#include <QGraphicsRectItem>

class AtlasTileSet;

class TileGfxItem : public QGraphicsPixmapItem
{
	class SelectionRectItem : public QGraphicsRectItem {
	public:
		SelectionRectItem(const int iPADDING, TileGfxItem *pParent = nullptr) :
			QGraphicsRectItem(pParent)
		{
			QSize vPixmapSize = pParent->pixmap().size();
			setRect(-iPADDING, -iPADDING, vPixmapSize.width() + (iPADDING * 2), vPixmapSize.height() + (iPADDING * 2));
			setPen(QPen(Qt::DashLine));
			setBrush(Qt::NoBrush);
			//setFlags(QGraphicsItem::ItemIsSelectable |
			//	QGraphicsItem::ItemIsMovable |
			//	QGraphicsItem::ItemSendsGeometryChanges);
			setVisible(false);
			setAcceptHoverEvents(true);
		}

	protected:
		void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override {
			setPen(QPen(Qt::black));
			setVisible(true);
			QGraphicsRectItem::hoverEnterEvent(event);
		}

		void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override {
			setVisible(false);
			QGraphicsRectItem::hoverLeaveEvent(event);
		}
	};
	SelectionRectItem *		m_pGfxRectItem;

public:
	TileGfxItem(const QPixmap &pixmap, QGraphicsItem *pParent = nullptr);

	virtual QRectF boundingRect() const override;
	virtual QVariant itemChange(GraphicsItemChange eChange, const QVariant &value) override;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
};

#endif // TILEGFXITEM_H
