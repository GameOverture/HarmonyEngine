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
class TileSetGfxItem;
class TileData;

enum TileSetMode
{
	TILESETMODE_Importing,
	TILESETMODE_Setup
};

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

	TileSetMode														m_eMode;
	AtlasTileSet *													m_pTileSet;

	QMap<TileData*, TileSetGfxItem*>								m_SetupTileMap;			// Keys are pointing to Actual concrete tiles from AtlasTileSet::m_TileDataMap
	QGraphicsRectItem												m_SetupBorderRect;		// A dash-line box that encompasses the working-portion of the 'setup' tiles
	TileSetGfxItemGroup*											m_pModeSetupGroup;

	QPointF															m_vSortingStartMousePos;
	QMap<TileSetGfxItem*, QPointF>									m_SortingInitialPosMap;

	QMap<QPoint, TileSetGfxItem*>									m_ImportTileMap;		// Pending import tiles
	QGraphicsRectItem												m_ImportBorderRect;		// A dash-line box that encompasses the working-portion of the 'import' tiles
	TileSetGfxItemGroup*											m_pModeImportGroup;
	QSize															m_vImportRegionSize;


public:
	TileSetScene();
	virtual ~TileSetScene();

	void Initialize(AtlasTileSet *pTileSet);

	TileSetMode GetDisplayMode() const;
	void SetDisplayMode(TileSetMode eMode);
	
	QPointF GetFocusPt() const;

	int GetNumImportPixmaps() const;
	QSize GetImportRegionSize() const;
	QMap<QPoint, QPixmap> AssembleImportMap();

	void OnMarqueeRelease(Qt::MouseButton eMouseBtn, bool bShiftHeld, QPointF ptStartDrag, QPointF ptEndDrag);
	void ClearSetupSelection();

	TileSetGfxItem* GetSetupTileAt(QPointF ptScenePos) const;

	void AddTile(TileSetMode eMode, TileData* pTileData, const QPolygonF& outlinePolygon, QPoint ptGridPos, QPixmap pixmap, bool bDefaultSelected);
	void RefreshTiles(Qt::Edge eImportAppendEdge); // Syncronizes the graphics items to match the data of m_pTileSet and current import tiles

	void ClearImportTiles();
	void ClearSetupTiles();

	void OnSortingTilesMousePress(QPointF ptMouseScenePos);
	void OnSortingTilesMouseMove(QPointF ptMouseScenePos);
	void OnSortingTilesMouseRelease(QPointF ptMouseScenePos);

//private:
//	void SetGfxItemTilePos(TileSetGfxItem* pGfxItem, QPoint ptGridPos);
};

#endif // TILESETSCENE_H
