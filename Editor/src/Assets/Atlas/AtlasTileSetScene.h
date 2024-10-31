/**************************************************************************
 *	AtlasTileSetScene.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ATLASTILESETSCENE_H
#define ATLASTILESETSCENE_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>

class AtlasTileSetScene : public QGraphicsScene
{
	Q_OBJECT

public:
	AtlasTileSetScene();
	~AtlasTileSetScene();
};

#endif // ATLASTILESETSCENE_H
