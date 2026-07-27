/**************************************************************************
 *	TileMapGfxScene.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef TILEMAPGFXSCENE_H
#define TILEMAPGFXSCENE_H

#include <QGraphicsScene>

class AtlasTileSet;

class TileMapGfxScene : public QGraphicsScene
{
	Q_OBJECT

	const AtlasTileSet *			m_pTileSet;
	QUuid							m_TerrainUuid;

	QList<QGraphicsPixmapItem *>	m_TileMapItemsList;

public:
	TileMapGfxScene(QObject *pParent = nullptr);
	virtual ~TileMapGfxScene();

	void Initialize(const AtlasTileSet *pTileSet, QUuid terrainUuid);
};

#endif // TILEMAPGFXSCENE_H
