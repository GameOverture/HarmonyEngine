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
#include "PropertiesTreeModel.h"

#include <QWidget>
#include <QSet>
#include <QJsonObject>
#include <QDataStream>

class VectorModel;

class TileData
{
	QUuid											m_Uuid;
	quint32											m_uiTileChecksum;
	QPoint											m_MetaGridPos;			// The user defined position in the grid when setting up the TileSet - and not its location on the sub-atlas texture

	PropertiesTreeModel *							m_pSetupPropertiesModel;

	QMap<QUuid, QList<int>>							m_AnimationMap;				// Key: Animation Uuid, Value: Associated frame indices (usually just one)

	QUuid											m_TerrainSetUuid;		// The Terrain Set assigned to this tile
	QMap<QUuid, QBitArray>							m_TerrainMap;			// Key: Terrain Uuid (NOT TERRAIN SET), Value: QBitArray(NUM_AUTOTILEPARTS)

	QMap<QUuid, VectorModel *>						m_CollisionLayerMap;

public:
	TileData(quint32 uiTileChecksum, QPoint metaGridPos);
	TileData(const QJsonObject &tileDataObj);
	TileData(TileData &&other) noexcept;
	TileData(const TileData &other) = delete;
	TileData &operator=(const TileData &other) = delete;
	~TileData();

	void InitPropertiesModel();

	QUuid GetUuid() const;

	quint32 GetTileChecksum() const;

	uint16 GetTileId() const;
	void SetTileId(uint16 uiTileId);

	QPoint GetMetaGridPos() const;
	void SetMetaGridPos(QPoint metaGridPos);

	QJsonObject GetTileData() const;

	PropertiesTreeModel *GetSetupPropertiesModel() const;

	QMap<QUuid, QList<int>> GetAnimationMap() const;
	void SetAnimationMap(QMap<QUuid, QList<int>> animMap);
	void SetAnimationFrame(QUuid animationUuid, int iFrameIndex);
	void RemoveAnimationFrame(QUuid animationUuid, int iFrameIndex);

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
