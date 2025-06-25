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
class TileGfxItem;

enum TileSetMode
{
	TILESETMODE_Importing,
	TILESETMODE_TileSet
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

	QGraphicsRectItem												m_ImportRect;		// A dash-line box that encompasses the entire import scene
	QGraphicsTextItem												m_ImportLabel;		// Main label/title of the import scene
	struct ImportTileItem
	{
		QGraphicsRectItem *											m_pRectItem;
		QGraphicsPixmapItem *										m_pPixmapItem;
		QGraphicsPolygonItem *										m_pOutlineItem;		// This is the outline of the tile as it sits in a grid, especially helpful for isometric and hexagon

		//ImportTileItem() :
		//	m_pRectItem(nullptr),
		//	m_pPixmapItem(nullptr),
		//	m_pOutlineItem(nullptr)
		//{ }
		ImportTileItem(QGraphicsRectItem *pRect, QGraphicsPixmapItem *pPixmap, QGraphicsPolygonItem *pOutline) :
			m_pRectItem(pRect),
			m_pPixmapItem(pPixmap),
			m_pOutlineItem(pOutline)
		{ }
	};
	QMap<QPoint, ImportTileItem>									m_ImportTileMap;
	QSize															m_vImportTileSize;

	QVector<TileGfxItem *>											m_TileSetPixmapItem;// The tile set pixmap item that is displayed in the tiles scene

public:
	TileSetScene();
	~TileSetScene();

	void Initialize(AtlasTileSet *pTileSet);

	void SetDisplayMode(TileSetMode eMode);

	int GetNumImportPixmaps() const;
	QSize GetImportTileSize() const;
	QMap<QPoint, QPixmap> AssembleImportMap();

	void ClearImport();
	void AddImport(const QPolygonF &outlinePolygon, QPoint ptGridPos, QPixmap pixmap);
	void SyncImport();

	void SyncTileSet(); // Slow, deletes/reallocates all graphics items

	void OnMarqueeRelease(QPoint ptStartDrag, QPoint ptEndDrag);
};

#endif // TILESETSCENE_H
