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

	QPointF															m_vDraggingStartMousePos;

	QMap<QPoint, TileSetGfxItem*>									m_ImportTileMap;		// Pending import tiles
	QGraphicsRectItem												m_ImportBorderRect;		// A dash-line box that encompasses the working-portion of the 'import' tiles
	TileSetGfxItemGroup*											m_pModeImportGroup;
	QSize															m_vImportRegionSize;

	Qt::Edge														m_eImportAppendEdge;

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

	void SetImportAppendEdge(Qt::Edge eEdge);

	void OnMarqueeRelease(Qt::MouseButton eMouseBtn, bool bShiftHeld, QPointF ptStartDrag, QPointF ptEndDrag);
	void ClearSetupSelection();

	TileSetGfxItem* GetSetupTileAt(QPointF ptScenePos) const;

	void AddTile(TileSetMode eMode, TileData* pTileData, const QPolygonF& outlinePolygon, QPoint ptGridPos, QPixmap pixmap, bool bDefaultSelected);
	
	// Syncronizes the graphics items to match the data of m_pTileSet and current import tiles
	// Also repositions tiles during sorting operations
	void RefreshTiles(QPointF vDragDelta = QPointF());

	void ClearImportTiles();
	void ClearSetupTiles();

	void OnDraggingTilesMousePress(QPointF ptMouseScenePos);
	void OnDraggingTilesMouseMove(QPointF ptMouseScenePos);
	void OnDraggingTilesMouseRelease(QPointF ptMouseScenePos);

private:
	// Used during a drag operation, displace unselected tiles by the given grid delta
	// and assign their temp 'TileSetGfxItem::m_ptDraggingGridPos'
	void DisplaceTiles(QPoint vGridDelta);
};

#endif // TILESETSCENE_H
