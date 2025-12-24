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
class TileData;

class TileSetGfxItem : public QGraphicsItem
{
	bool								m_bSelected;
	bool								m_bDragged;

	TileSetShape						m_eTileSetShape;
	AutoTileType						m_eAutoTileType;

	QPen								m_SelectedPen;
	QPen								m_UnselectedPen;
	QPen								m_DraggedPen;
	QPen								m_ShapePen;

	QGraphicsRectItem *					m_pRectItem;
	QGraphicsPixmapItem *				m_pPixmapItem;
	QGraphicsPolygonItem *				m_pShapeItem;				// This is the shape outline of the tile as it sits in a grid, especially helpful for isometric and hexagon, or when the tile size is not the full texture region size

	QPointF								m_ptDraggingInitialPos;		// The scene positions of this tile when starting a arranging operation
	QPoint								m_ptDraggingGridPos;		// A temp variable used during arranging of tiles in the scene

	QGraphicsRectItem *					m_pAnimationRectItem;		// Optional rectangle drawn around the tile when it is part of an animation

	QGraphicsPolygonItem *				m_pTerrainParts[NUM_AUTOTILEPARTS];

public:
	TileSetGfxItem(const QPixmap& pixmapRef, const QPolygonF& outlinePolygon);
	virtual ~TileSetGfxItem();

	void Refresh(QSize regionSize, AtlasTileSet *pTileSet, TileSetPage ePage, TileData *pTileData);

	bool IsSelected() const;
	void SetSelected(bool bSelected);
	
	void SetAsDragged(bool bDragged);
	QPointF GetDraggingInitialPos() const;
	void SetDraggingInitialPos(QPointF ptInitialPos);
	QPoint GetDraggingGridPos() const;
	void SetDraggingGridPos(QPoint ptGridPos);

	QPixmap GetPixmap() const;

	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget) override;

	void SetAnimation(bool bShow, HyColor color);

	void AllocateAutoTileParts(AtlasTileSet *pTileSet, AutoTileType eAutoTileType, TileSetShape eTileSetShape);
	QGraphicsPolygonItem *GetAutoTilePartAt(QPointF ptLocalPos, TileSetAutoTilePart &ePartOut);

private:
	QPolygonF AssembleAutoTilePolygon(AutoTileType eAutoTileType, TileSetShape eTileSetShape, TileSetAutoTilePart ePart);
};

#endif // TILEGFXITEM_H
