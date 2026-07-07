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

#define TILEDATA_INVALID_ATLASINDEX 0xFFFF

class VectorModel;

class TileData
{
	QUuid											m_Uuid;
	uint16											m_uiAtlasIndex;			// The index location (row major) in the sub-atlas
	QPoint											m_MetaGridPos;			// The user defined position in the grid when setting up the TileSet - and not its location on the sub-atlas texture

	QPixmap											m_TilePixmap;
	quint32											m_uiChecksum;
	QPoint											m_TextureOffset;

	bool 											m_bIsFlippedHorz;
	bool 											m_bIsFlippedVert;
	bool 											m_bIsRotated;			// Transpose

	int												m_iProbability;

	QUuid											m_AnimationUuid;

	QUuid											m_TerrainSetUuid;		// The Terrain Set assigned to this tile
	QMap<QUuid, QBitArray>							m_TerrainMap;			// Key: Terrain Uuid (NOT TERRAIN SET), Value: QBitArray(NUM_AUTOTILEPARTS)

	QMap<QUuid, VectorModel *>						m_CollisionLayerMap;

public:
	TileData(QPoint metaGridPos, QPixmap tilePixmap);
	TileData(const QJsonObject &tileDataObj, QPixmap tilePixmap);
	TileData(TileData &&other) noexcept;
	TileData(const TileData &other) = delete;
	TileData &operator=(const TileData &other) = delete;
	~TileData();

	QUuid GetUuid() const;

	uint16 GetAtlasIndex() const;
	void SetAtlasIndex(uint16 uiAtlasIndex);

	QPoint GetMetaGridPos() const;
	void SetMetaGridPos(QPoint metaGridPos);

	QJsonObject GetTileData() const;
	QPoint GetTextureOffset() const;
	QPixmap GetPixmap() const;

	QUuid GetAnimation() const;
	void SetAnimation(QUuid animationUuid);

	QUuid GetTerrainSet() const;
	void SetTerrainSet(QUuid terrainSetUuid);
	QUuid GetTerrain(TileSetAutoTilePart ePart) const;
	void SetTerrain(QUuid terrainUuid, TileSetAutoTilePart ePart);
	void ClearTerrain(TileSetAutoTilePart ePart);
	const QMap<QUuid, QBitArray> &GetTerrainMap() const;
	void SetTerrainMap(const QMap<QUuid, QBitArray> &terrainMap);

	QList<QUuid> GetCollisionLayerList() const;
	VectorModel *GetCollisionLayerModel(QUuid uuid) const;
};

#endif // TILEDATA_H
