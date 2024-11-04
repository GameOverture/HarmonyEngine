/**************************************************************************
 *	TileData.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "TileData.h"

QDataStream &operator<<(QDataStream &, const TileData::TilePhysics &)
{
}

QDataStream &operator>>(QDataStream &, TileData::TilePhysics &)
{
}

// DATA FORMAT (data segment, internal to AtlasTileSet):
// [int] AtlasIndex
// [QPoint] Offset
// [int] AnimFrame
// [int] Probability
// 
// [int] Size of AutoTileMap
// [int, uint8_t]...[int, uint8_t]...etc...
// 
// [TileData::TilePhysics] Tile Physics struct (is serializable)
QDataStream &operator<<(QDataStream &leftSide, const TileData &rightSide)
{
	leftSide << rightSide.m_iAtlasIndex;
	leftSide << rightSide.m_Offset;
	leftSide << rightSide.m_iAnimFrame;
	leftSide << rightSide.m_iProbability;

	leftSide << rightSide.m_AutoTileMap.size();
	for(auto iter = rightSide.m_AutoTileMap.begin(); iter != rightSide.m_AutoTileMap.end(); ++iter)
	{
		leftSide << iter.key();
		leftSide << iter.value();
	}

	leftSide << rightSide.m_TilePhysics;

	return leftSide;
}

QDataStream &operator>>(QDataStream &leftSide, TileData &rightSide)
{
	leftSide >> rightSide.m_iAtlasIndex;
	leftSide >> rightSide.m_Offset;
	leftSide >> rightSide.m_iAnimFrame;
	leftSide >> rightSide.m_iProbability;
	leftSide >> rightSide.m_AutoTileMap;
	leftSide >> rightSide.m_PhysicsVertList;

	return leftSide;
}
