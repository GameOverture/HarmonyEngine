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

class TileMapGfxScene : public QGraphicsScene
{
	Q_OBJECT

public:
	TileMapGfxScene();
	virtual ~TileMapGfxScene();

	void Initialize(const QJsonObject &tileSetMetaObj);
};

#endif // TILEMAPGFXSCENE_H
