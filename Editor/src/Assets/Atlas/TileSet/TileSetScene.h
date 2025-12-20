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

class AtlasTileSet;
class AuxTileSet;
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

	QMap<TileData *, TileSetGfxItem *>								m_SetupTileMap;			// Keys are pointing to Actual concrete tiles from AtlasTileSet::m_TileDataMap
	QGraphicsRectItem												m_SetupBorderRect;		// A dash-line box that encompasses the working-portion of the 'setup' tiles
	TileSetGfxItemGroup *											m_pModeSetupGroup;

	QPointF															m_vArrangingStartMousePos;

	QMap<QPoint, TileSetGfxItem *>									m_ImportTileMap;		// Pending import tiles
	QGraphicsRectItem												m_ImportBorderRect;		// A dash-line box that encompasses the working-portion of the 'import' tiles
	TileSetGfxItemGroup *											m_pModeImportGroup;
	QSize															m_vImportRegionSize;
	Qt::Edge														m_eImportAppendEdge;

	QList<TileData *>												m_PaintStrokeAnimationList;

	QGraphicsPolygonItem *											m_pHoverAutoTilePartItem;
	QMap<TileData *, QBitArray>										m_PaintStrokeAutoTilePartMap;

public:
	TileSetScene();
	virtual ~TileSetScene();

	void Initialize(AtlasTileSet *pTileSet);

	QGraphicsRectItem &GetGfxBorderRect();
	QGraphicsRectItem &GetGfxImportBorderRect();

	void OnTileSetPageChange(TileSetPage ePage);

	int GetNumImportPixmaps() const;
	QSize GetImportRegionSize() const;
	QMap<QPoint, QPixmap> AssembleImportMap();
	void SetImportAppendEdge(Qt::Edge eEdge);

	TileSetGfxItem *GetGfxTile(TileData *pTileData) const;
	int GetNumSetupSelected() const;
	QMap<TileData *, TileSetGfxItem *> GetSelectedSetupTiles() const;

	TileData *IsPointInTile(QPointF ptScenePos) const;
	void OnMarqueeRelease(AuxTileSet &auxTileSetRef, Qt::MouseButton eMouseBtn, bool bShiftHeld, QPointF ptStartDrag, QPointF ptEndDrag);
	void ClearSetupSelection();

	void AddTile(bool bImportTile, TileData *pTileData, const QPolygonF &outlinePolygon, QPoint ptGridPos, QPixmap pixmap, bool bDefaultSelected);
	
	void RefreshImportTiles();

	// Sync m_pTileSet's 'AtlasTileSet::m_TileDataList' with m_SetupTileMap
	// Sets graphics items to match the data of m_pTileSet and current import tiles
	// Also repositions tiles during sorting operations
	void RefreshSetupTiles(TileSetPage ePage, QPointF vDragDelta = QPointF());


	void ClearImportTiles();
	void ClearSetupTiles();

	void OnArrangingTilesMousePress(QPointF ptMouseScenePos);
	void OnArrangingTilesMouseMove(QPointF ptMouseScenePos);
	void OnArrangingTilesMouseRelease(AuxTileSet &auxTileSetRef, QPointF ptMouseScenePos);

	void HoverAutoTilePartAt(QPointF ptScenePos);
	void OnTerrainSetApplied(TileData *pTileData);

	void StartPaintStroke();
	void OnPaintingStroke(AuxTileSet &auxTileSetRef, QPointF ptScenePos, Qt::MouseButtons uiMouseFlags); 
	void OnPaintStrokeRelease(AuxTileSet &auxTileSetRef, Qt::MouseButton eMouseBtn);

private:
	// Used during a drag operation, displace unselected tiles by the given grid delta
	// and assign their temp 'TileSetGfxItem::m_ptDraggingGridPos'
	void DisplaceTiles(QPoint vGridDelta);
};

#endif // TILESETSCENE_H
