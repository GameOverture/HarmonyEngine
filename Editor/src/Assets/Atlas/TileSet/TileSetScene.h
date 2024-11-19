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

	QGraphicsTextItem					m_ImportLabel;
	QVector<QGraphicsPixmapItem *>		m_ImportTilePixmapList;

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

	void RemoveImportPixmaps();
	void AddImportPixmap(QPixmap pixmap);
	void ConstructImportScene(QPoint vTileSize, int iNumColumns, int iNumRows);
};

#endif // TILESETSCENE_H
