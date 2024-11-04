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

#include <QWidget>
#include <QSet>
#include <QJsonObject>
#include <QDataStream>

// NOTE: TileSet atlases are always "square"
//       COLUMNS = static_cast<int>(std::floor(std::sqrt(n)))
//       ROWS = static_cast<int>(std::ceil(static_cast<double>(n) / columns))
struct TileData
{
	// Render Buffer Data:
	int												m_iAtlasIndex;			// Row major order: (Y * NumColumns) + X
	QPoint											m_Offset;

	enum TileAnimType
	{
		TILEANIM_None = -1,
	};
	int												m_iAnimFrame;			// -1 indicates no animation (frames are laid out in row major order in the atlas). NOTE: A tile on the atlas may only be apart of one animation

	// Meta Data:
	int												m_iProbability;

	enum AutoTilePeeringBit
	{
		AUTOBIT_TopLeft								= 1 << 0,
		AUTOBIT_Top									= 1 << 1,
		AUTOBIT_TopRight							= 1 << 2,
		AUTOBIT_Left								= 1 << 3,
		AUTOBIT_Right								= 1 << 4,
		AUTOBIT_BottomLeft							= 1 << 5,
		AUTOBIT_Bottom								= 1 << 6,
		AUTOBIT_BottomRight							= 1 << 7
	};
	QMap<AutoTileHandle, uint8_t>					m_AutoTileMap;

	QMap<PhysicsLayerHandle, QList<QList<QPoint>>>	m_PhysicsVertMap;
};

#endif // TILEDATA_H
