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

	QPen								m_SelectedPen;
	QPen								m_SelectedHoverPen;
	QPen								m_UnselectedPen;
	QPen								m_UnselectedHoverPen;
	QPen								m_ShapePen;

	QGraphicsRectItem *					m_pRectItem;
	QGraphicsPixmapItem *				m_pPixmapItem;
	QGraphicsPolygonItem *				m_pShapeItem;		// This is the shape outline of the tile as it sits in a grid, especially helpful for isometric and hexagon

public:
	TileSetGfxItem(const QPixmap& pixmapRef, const QPolygonF& outlinePolygon);
	~TileSetGfxItem();


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

	//	}
	//};
	//SelectionRectItem *		m_pGfxRectItem;

	bool IsSelected() const;
	void SetSelected(bool bSelected);

	QPointF GetPos() const;
	void SetPos(QPointF ptNewPos, QSize regionSize, QPointF vOffset, const QPolygonF& outlinePolygon);

	QPixmap GetPixmap() const;

	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget) override;
	//virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
	//virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
	//virtual QVariant itemChange(GraphicsItemChange eChange, const QVariant &value) override;
	//virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	//virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	//virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
	//virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
};

#endif // TILEGFXITEM_H
