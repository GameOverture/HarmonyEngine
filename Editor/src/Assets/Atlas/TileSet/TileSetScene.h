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

enum TileSetMode
{
	TILESETMODE_Importing,
	TILESETMODE_Setup
};

class TileSetGroupItem : public QGraphicsItemGroup {
public:
	TileSetGroupItem(QGraphicsItem *parent = nullptr)
		: QGraphicsItemGroup(parent) {
		setHandlesChildEvents(false);
		setAcceptedMouseButtons(Qt::AllButtons);
	}
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *pEvent) override {
		pEvent->ignore(); // Pass the event to child items
	}
};

class TileSetScene : public QGraphicsScene
{
	Q_OBJECT

	AtlasTileSet *													m_pTileSet;

	TileSetMode														m_eMode;
	TileSetGroupItem *												m_pModeImportGroup;
	TileSetGroupItem *												m_pModeTileSetGroup;

	QMap<QPoint, TileSetGfxItem*>									m_ImportTileMap;
	QSize															m_vImportRegionSize;
	QGraphicsRectItem												m_ImportBoundsRect;		// A dash-line box that encompasses the entire import scene

	QVector<TileSetGfxItem*>											m_TileSetPixmapItem;// The tile set pixmap item that is displayed in the tiles scene

public:
	TileSetScene();
	~TileSetScene();

	void Initialize(AtlasTileSet *pTileSet);

	void SetDisplayMode(TileSetMode eMode);

	int GetNumImportPixmaps() const;
	QSize GetImportRegionSize() const;
	QMap<QPoint, QPixmap> AssembleImportMap();

	void OnMarqueeRelease(Qt::MouseButton eMouseBtn, QPointF ptStartDrag, QPointF ptEndDrag);

	// IMPORT
	void ClearImport();
	void AddImport(const QPolygonF &outlinePolygon, QPoint ptGridPos, QPixmap pixmap, bool bDefaultSelected);
	void SyncImport();

	// SETUP
	void SyncTileSet(); // Slow, deletes/reallocates all graphics items
};

#endif // TILESETSCENE_H
