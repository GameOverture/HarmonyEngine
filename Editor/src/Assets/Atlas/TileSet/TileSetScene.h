/**************************************************************************
 *	TileSetScene.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef TILESETSCENE_H
#define TILESETSCENE_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include "AuxTileSet.h"

class AtlasTileSet;
class TileSetGfxItem;
class TileData;

class TileSetGfxItemGroup : public QGraphicsItemGroup {
public:
	TileSetGfxItemGroup(QGraphicsItem *parent = nullptr) :
		QGraphicsItemGroup(parent)
	{
		setHandlesChildEvents(false);
		setAcceptedMouseButtons(Qt::AllButtons);
	}
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *pEvent) override { pEvent->ignore(); } // Pass the event to child items
};

class TileSetScene : public QGraphicsScene
{
	Q_OBJECT

	AtlasTileSet *													m_pTileSet;

	QMap<TileData*, TileSetGfxItem*>								m_SetupTileMap;			// Keys are pointing to Actual concrete tiles from AtlasTileSet::m_TileDataMap
	QGraphicsRectItem												m_SetupBorderRect;		// A dash-line box that encompasses the working-portion of the 'setup' tiles
	TileSetGfxItemGroup*											m_pModeSetupGroup;

	QPointF															m_vArrangingStartMousePos;

	QMap<QPoint, TileSetGfxItem*>									m_ImportTileMap;		// Pending import tiles
	QGraphicsRectItem												m_ImportBorderRect;		// A dash-line box that encompasses the working-portion of the 'import' tiles
	TileSetGfxItemGroup*											m_pModeImportGroup;
	QSize															m_vImportRegionSize;

	Qt::Edge														m_eImportAppendEdge;

public:
	TileSetScene();
	virtual ~TileSetScene();

	void Initialize(AtlasTileSet *pTileSet);

	void OnTileSetPageChange(TileSetPage ePage);
	
	QPointF GetFocusPt(TileSetPage ePage) const;

	int GetNumImportPixmaps() const;
	QSize GetImportRegionSize() const;
	QMap<QPoint, QPixmap> AssembleImportMap();
	void SetImportAppendEdge(Qt::Edge eEdge);

	int GetNumSetupSelected() const;
	QMap<TileData *, TileSetGfxItem *> GetSelectedSetupTiles() const;

	void OnMarqueeRelease(TileSetPage ePage, Qt::MouseButton eMouseBtn, bool bShiftHeld, QPointF ptStartDrag, QPointF ptEndDrag);
	void ClearSetupSelection();

	TileSetGfxItem* GetSetupTileAt(QPointF ptScenePos) const;

	void AddTile(bool bImportTile, TileData* pTileData, const QPolygonF& outlinePolygon, QPoint ptGridPos, QPixmap pixmap, bool bDefaultSelected);
	
	// Sync m_pTileSet's 'AtlasTileSet::m_TileDataList' with m_SetupTileMap
	// Sets graphics items to match the data of m_pTileSet and current import tiles
	// Also repositions tiles during sorting operations
	void RefreshTiles(QPointF vDragDelta = QPointF());

	void ClearImportTiles();
	void ClearSetupTiles();

	void OnArrangingTilesMousePress(QPointF ptMouseScenePos);
	void OnArrangingTilesMouseMove(QPointF ptMouseScenePos);
	void OnArrangingTilesMouseRelease(AuxTileSet &auxTileSetRef, QPointF ptMouseScenePos);

private:
	// Used during a drag operation, displace unselected tiles by the given grid delta
	// and assign their temp 'TileSetGfxItem::m_ptDraggingGridPos'
	void DisplaceTiles(QPoint vGridDelta);
};

#endif // TILESETSCENE_H
