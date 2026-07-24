/**************************************************************************
 *	TileMapGfxScene.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "TileMapGfxScene.h"

const float g_fSceneMargins = 7000.0f;

TileMapGfxScene::TileMapGfxScene() :
	QGraphicsScene()
{
	QRectF sceneRect(-g_fSceneMargins, -g_fSceneMargins, g_fSceneMargins * 2.0f, g_fSceneMargins * 2.0f);
	setSceneRect(sceneRect);
}

/*virtual*/ TileMapGfxScene::~TileMapGfxScene()
{
}

void TileMapGfxScene::Initialize(const QJsonObject &tileSetMetaObj)
{

}
