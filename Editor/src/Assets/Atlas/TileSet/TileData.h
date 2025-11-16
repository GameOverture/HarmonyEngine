/**************************************************************************
 *	TileData.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef TILEDATA_H
#define TILEDATA_H

#include "AtlasTileSet.h"

#include <QWidget>
#include <QSet>
#include <QJsonObject>
#include <QDataStream>

class TileData
{
	QPoint											m_MetaGridPos;			// The user defined position in the grid when setting up the TileSet - and not its location on the sub-atlas texture

	QPixmap											m_TilePixmap;
	QPoint											m_TextureOffset;

	bool 											m_bIsFlippedHorz;
	bool 											m_bIsFlippedVert;
	bool 											m_bIsRotated;			// Transpose

	int												m_iProbability;

	QUuid											m_TerrainUuid;		// The terrain autotile "center bit" assigned as
	enum AutotilePeeringBit
	{
		AUTOBIT_RightCorner			= 1 << 0,		// Hexagon-flat, Isometric
		AUTOBIT_RightSide			= 1 << 1,		// Hexagon-pointed, Square
		AUTOBIT_BottomRightSide		= 1 << 2,		// Hexagon-flat, Hexagon-pointed, Isometric
		AUTOBIT_BottomRightCorner	= 1 << 3,		// Hexagon-flat, Hexagon-pointed, Square
		AUTOBIT_BottomSide			= 1 << 4,		// Hexagon-flat, Square
		AUTOBIT_BottomCorner		= 1 << 5,		// Hexagon-pointed, Isometric
		AUTOBIT_BottomLeftSide		= 1 << 6,		// Hexagon-flat, Hexagon-pointed, Isometric
		AUTOBIT_BottomLeftCorner	= 1 << 7,		// Hexagon-flat, Hexagon-pointed, Square
		AUTOBIT_LeftCorner			= 1 << 8,		// Hexagon-flat, Isometric
		AUTOBIT_LeftSide			= 1 << 9,		// Hexagon-pointed, Square
		AUTOBIT_TopLeftSide			= 1 << 10,		// Hexagon-flat, Hexagon-pointed, Isometric
		AUTOBIT_TopLeftCorner		= 1 << 11,		// Hexagon-flat, Hexagon-pointed, Square
		AUTOBIT_TopCorner			= 1 << 12, 		// Hexagon-pointed, Isometric
		AUTOBIT_TopSide				= 1 << 13,		// Hexagon-flat, Square
		AUTOBIT_TopRightSide		= 1 << 14,		// Hexagon-flat, Hexagon-pointed, Isometric
		AUTOBIT_TopRightCorner		= 1 << 15,		// Hexagon-flat, Hexagon-pointed, Square
	};
	QMap<QUuid, uint16_t>							m_AutoTileMap;

	QMap<QUuid, QList<QList<QPoint>>>				m_VertexMap;

public:
	TileData(QPoint metaGridPos, QPixmap tilePixmap);
	TileData(const QJsonObject &tileDataObj, QPixmap tilePixmap);
	TileData(const TileData &other);
	TileData &operator=(const TileData &other);
	~TileData();

	QPoint GetMetaGridPos() const;
	void SetMetaGridPos(QPoint metaGridPos);

	QJsonObject GetTileData() const;
	QPoint GetTextureOffset() const;
	QPixmap GetPixmap() const;
};

#endif // TILEDATA_H
