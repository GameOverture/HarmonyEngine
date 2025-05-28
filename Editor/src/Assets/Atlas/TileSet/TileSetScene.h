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

class TileSetScene : public QGraphicsScene
{
	Q_OBJECT

	QGraphicsRectItem					m_ImportRect;				// A dash-line box that encompasses the entire import scene
	QGraphicsTextItem					m_ImportLabel;				// Main label/title of the import scene
	QVector<QGraphicsRectItem *>		m_ImportTileRectList;
	QVector<QGraphicsPixmapItem *>		m_ImportTilePixmapList;
	QSize								m_vImportTileSize;

public:
	enum SceneType
	{
		SCENETYPE_Importing,
		SCENETYPE_TileSet
	};
	SceneType							m_eSceneType;

public:
	TileSetScene();
	~TileSetScene();

	void Setup(AtlasTileSet *pTileSet);

	int GetNumImportPixmaps() const;
	QVector<QGraphicsPixmapItem *> &GetImportPixmapList();
	QSize GetImportTileSize() const;

	void RemoveImportPixmaps();
	void AddImportPixmap(QPixmap pixmap);
	void ConstructImportScene();
	void ConstructImportScene(int iNumColumns, int iNumRows);
};

#endif // TILESETSCENE_H
