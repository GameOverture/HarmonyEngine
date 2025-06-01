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

class AtlasTileSet;

enum TileSetMode
{
	TILESETMODE_Importing,
	TILESETMODE_TileSet
};
class TileSetScene : public QGraphicsScene
{
	Q_OBJECT

	AtlasTileSet &						m_TileSetRef;

	TileSetMode							m_eMode;
	QGraphicsItemGroup *				m_pModeImportGroup;
	QGraphicsItemGroup *				m_pModeTileSetGroup;

	QGraphicsRectItem					m_ImportRect;				// A dash-line box that encompasses the entire import scene
	QGraphicsTextItem					m_ImportLabel;				// Main label/title of the import scene

	QMap<QPoint, QPair<QGraphicsRectItem *, QGraphicsPixmapItem *>>	m_ImportTileMap;
	QSize								m_vImportTileSize;

	QVector<QGraphicsPixmapItem *>		m_TileSetPixmapItem;		// The tile set pixmap item that is displayed in the tiles scene

public:
	TileSetScene(AtlasTileSet &tileSetRef);
	~TileSetScene();

	void SetDisplayMode(TileSetMode eMode);

	int GetNumImportPixmaps() const;
	QSize GetImportTileSize() const;
	QMap<QPoint, QPixmap> AssembleImportMap();

	void ClearImport();
	void AddImport(QPoint ptGridPos, QPixmap pixmap);
	void SyncImport();

	void SyncTileSet(); // Slow, deletes/reallocates all graphics items
};

#endif // TILESETSCENE_H
