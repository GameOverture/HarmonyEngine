/**************************************************************************
 *	AtlasTileSet.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ATLASTILESET_H
#define ATLASTILESET_H

#include "Global.h"
#include "AtlasFrame.h"
#include "TileSetScene.h"
#include "vendor/libtiled/tileset.h"

#include <QWidget>
#include <QSet>
#include <QJsonObject>
#include <QUndoStack>

#define TILEDATA_INVALID_ID 0xFFFF

#define NUM_COLS_TILESET(numTiles) static_cast<int>(std::floor(std::sqrt(numTiles)))
#define NUM_ROWS_TILESET(numTiles, numCols) (static_cast<int>(numTiles) + static_cast<int>(numCols) - 1) / static_cast<int>(numCols);
#define TILESET_TILE_PADDING 2 // Padding between tiles in the sub-atlas texture to avoid texture bleeding

bool operator<(const QPoint &a, const QPoint &b);

class TileData;

class AtlasTileSet : public AtlasFrame
{
	Q_OBJECT

	QJsonObject					m_TileSetMetaObj;				// The currently 'saved to disk' data of the TileSet
	bool						m_bExistencePendingSave;
	bool						m_bSubAtlasDirty;
	
	QUndoStack *				m_pUndoStack;
	QAction *					m_pActionUndo;
	QAction *					m_pActionRedo;
	
	TileSetScene				m_GfxScene;

	TileSetShape				m_eTileShape;
	QSize						m_RegionSize;					// The size of the atlas cutout dimensions (not necessarily the size of the tile)
	QSize						m_TileSize;						// User specified size that is used in conjunction with m_eTileType to create m_TilePolygon
	QPoint						m_TileOffset;
	QPolygonF					m_TilePolygon;					// Represents the actual tile, that is able to be arranged in a TileMap (grid)

	struct AnimationSet
	{
		QUuid						m_uuid;
		QString						m_sName;
		HyColor						m_Color;
		QSize						m_SizeInTiles;
		quint16						m_uiFrameDurationMs;
		bool						m_bGlobalSync;
		bool						m_bBounceAnim;	// AKA Ping-pong
		bool						m_bReverseAnim;
		bool						m_bLooping;
		bool						m_bEnabled;
		int							m_iNumFrames;

		QList<QList<TileData *>>	m_TempTileList;	// Used during AtlasTileSet::RegenerateSubAtlas() to help assemble animation tiles consecutively
													// size[m_iNumFrames][m_SizeInTiles.width * m_SizeInTiles.height] (the 2nd dimension to be sorted by the tiles' meta-grid location)

		QList<int>					m_TempStartingAtlasIndexList; // size[m_SizeInTiles.width * m_SizeInTiles.height]

		AnimationSet(QString sName, HyColor color) :
			m_uuid(QUuid::createUuid()),
			m_sName(sName),
			m_Color(color),
			m_SizeInTiles(1, 1),
			m_uiFrameDurationMs(33),
			m_bGlobalSync(true),
			m_bBounceAnim(false),
			m_bReverseAnim(false),
			m_bLooping(true),
			m_bEnabled(true),
			m_iNumFrames(0)
		{
		}
		AnimationSet(const QJsonObject &initObj)
		{
			m_uuid = QUuid(initObj["UUID"].toString());
			m_sName = initObj["name"].toString();
			m_Color = HyColor(initObj["color"].toVariant().toLongLong());
			m_SizeInTiles.setWidth(initObj["width"].toInt());
			m_SizeInTiles.setHeight(initObj["height"].toInt());
			m_uiFrameDurationMs = initObj["frameDuration"].toInt();
			m_bGlobalSync = initObj["globalSync"].toBool();
			m_bBounceAnim = initObj["bounceAnim"].toBool();
			m_bReverseAnim = initObj["reverseAnim"].toBool();
			m_bLooping = initObj["looping"].toBool();
			m_bEnabled = initObj["enabled"].toBool();
			m_iNumFrames = initObj["numFrames"].toInt();
			
			//m_AnimationList.clear();
			//QJsonArray tileAnimArray = initObj["tileAnims"].toArray();
			//for(QJsonValue tileAnimVal : tileAnimArray)
			//{
			//	QJsonObject animObj = tileAnimVal.toObject();
			//	m_AnimationList.push_back(TileAnimation(animObj));
			//}
		}

		QJsonObject ToJsonObject() const
		{
			QJsonObject animationSetObj;
			animationSetObj["UUID"] = m_uuid.toString(QUuid::WithoutBraces);
			animationSetObj["name"] = m_sName;
			animationSetObj["color"] = static_cast<qint64>(m_Color.GetAsHexCode());
			animationSetObj["width"] = m_SizeInTiles.width();
			animationSetObj["height"] = m_SizeInTiles.height();
			animationSetObj["frameDuration"] = m_uiFrameDurationMs;
			animationSetObj["globalSync"] = m_bGlobalSync;
			animationSetObj["bounceAnim"] = m_bBounceAnim;
			animationSetObj["reverseAnim"] = m_bReverseAnim;
			animationSetObj["looping"] = m_bLooping;
			animationSetObj["enabled"] = m_bEnabled;
			animationSetObj["numFrames"] = m_iNumFrames;

			//QJsonArray tileAnimsArray;
			//for(const TileAnimation &animRef : m_AnimationList)
			//	tileAnimsArray.append(animRef.ToJsonObject());
			//animationSetObj["tileAnims"] = tileAnimsArray;

			return animationSetObj;
		}
	};
	QList<AnimationSet>			m_AnimationSetList;

	struct TerrainSet
	{
		QUuid					m_uuid;
		AutoTileType			m_eType;

		struct Terrain
		{
			QUuid				m_uuid;
			QUuid				m_TerrainSetUuid;
			QString				m_sName;
			HyColor				m_Color;

			Terrain(QUuid terrainSetUuid, QString sName, HyColor color) :
				m_uuid(QUuid::createUuid()),
				m_TerrainSetUuid(terrainSetUuid),
				m_sName(sName),
				m_Color(color)
			{
			}
			Terrain(const QJsonObject &initObj)
			{
				m_uuid = QUuid(initObj["UUID"].toString());
				m_TerrainSetUuid = QUuid(initObj["terrainSetUUID"].toString());
				m_sName = initObj["name"].toString();
				m_Color = HyColor(initObj["color"].toVariant().toLongLong());
			}

			QJsonObject ToJsonObject() const
			{
				QJsonObject terrainObj;
				terrainObj["UUID"] = m_uuid.toString(QUuid::WithoutBraces);
				terrainObj["terrainSetUUID"] = m_TerrainSetUuid.toString(QUuid::WithoutBraces);
				terrainObj["name"] = m_sName;
				terrainObj["color"] = static_cast<qint64>(m_Color.GetAsHexCode());
				return terrainObj;
			}
		};
		QList<Terrain>			m_TerrainList;

		TerrainSet() :
			m_uuid(QUuid::createUuid()),
			m_eType(AUTOTILETYPE_MatchCornerSides)
		{
		}
		TerrainSet(const QJsonObject &initObj)
		{
			m_uuid = QUuid(initObj["UUID"].toString());
			m_eType = static_cast<AutoTileType>(initObj["terrainSetMode"].toInt());
			
			QJsonArray terrainArray = initObj["terrains"].toArray();
			for(QJsonValue terrainVal : terrainArray)
			{
				QJsonObject terrainObj = terrainVal.toObject();
				Terrain newTerrain(terrainObj);
				m_TerrainList.push_back(newTerrain);
			}
		}

		QJsonObject ToJsonObject() const
		{
			QJsonObject terrainSetObj;
			terrainSetObj["UUID"] = m_uuid.toString(QUuid::WithoutBraces);
			terrainSetObj["terrainSetMode"] = static_cast<int>(m_eType);
			QJsonArray terrainArray;
			for(const Terrain &terrainRef : m_TerrainList)
				terrainArray.append(terrainRef.ToJsonObject());
			terrainSetObj["terrains"] = terrainArray;
			return terrainSetObj;
		}
	};
	QList<TerrainSet>			m_TerrainSetList;

	struct CollisionLayer
	{
		QUuid					m_uuid;
		HyColor					m_Color;
		b2Filter				m_Filter;
		QUuid					m_SurfaceMaterialUuid;
		bool					m_bIsSensor;

		CollisionLayer(HyColor color) :
			m_uuid(QUuid::createUuid()),
			m_Color(color),
			m_Filter(b2DefaultFilter()),
			m_SurfaceMaterialUuid(),
			m_bIsSensor(false)
		{
		}

		CollisionLayer(const QJsonObject &initObj)
		{
			m_uuid = QUuid(initObj["UUID"].toString());
			m_Color = HyColor(initObj["color"].toVariant().toLongLong());
			QJsonObject filterObj = initObj["filter"].toObject();
			m_Filter.categoryBits = static_cast<uint64_t>(filterObj["categoryBits"].toVariant().toLongLong());
			m_Filter.maskBits = static_cast<uint64_t>(filterObj["maskBits"].toVariant().toLongLong());
			m_Filter.groupIndex = static_cast<int32>(filterObj["groupIndex"].toInt());
			m_SurfaceMaterialUuid = QUuid(initObj["surfaceMaterialUUID"].toString());
			m_bIsSensor = initObj["isSensor"].toBool();
		}

		QJsonObject ToJsonObject() const
		{
			QJsonObject collisionLayerObj;
			collisionLayerObj.insert("UUID", m_uuid.toString(QUuid::WithoutBraces));
			collisionLayerObj.insert("color", static_cast<qint64>(m_Color.GetAsHexCode()));
			QJsonObject filterObj;
			filterObj["categoryBits"] = static_cast<qint64>(m_Filter.categoryBits);
			filterObj["maskBits"] = static_cast<qint64>(m_Filter.maskBits);
			filterObj["groupIndex"] = static_cast<qint64>(m_Filter.groupIndex);
			collisionLayerObj.insert("filter", filterObj);
			collisionLayerObj.insert("surfaceMaterialUUID", m_SurfaceMaterialUuid.toString(QUuid::WithoutBraces));
			collisionLayerObj.insert("isSensor", m_bIsSensor);
			return collisionLayerObj;
		}
	};
	QList<CollisionLayer>		m_CollisionLayerList;

	// Tile Image Indices are row-major order
	// TileSet texture sub-atlas' rows and columns is made based on the total # of tiles
	//     COLUMNS = static_cast<int>(std::floor(std::sqrt(n)))
	//     ROWS    = static_cast<int>(std::ceil(static_cast<double>(n) / columns))
	QMap<quint32, QPixmap>		m_TileImageMap;
	int							m_iNumSubAtlasTiles;	// NOTE: This value may be different from m_TileImageMap.size() because there can be duplicate tiles due to animations requiring consecutive placement

	// Tile IDs must be sequential with no gaps, as the ID is used to sample the row-major texel in the ShaderDescriptor data texture
	QVector<TileData *>			m_TileDataList;

	Tiled::SharedTileset		m_pTiledTileSet;	// Dummy tileset ptr to reference this AtlasTileSet in the Tiled API

public:
	AtlasTileSet(IManagerModel &modelRef,
				 QUuid uuid,
				 quint32 uiChecksum,
				 quint32 uiBankId,
				 QString sName,
				 HyTextureInfo texInfo,
				 quint16 uiW, quint16 uiH, quint16 uiX, quint16 uiY,
				 int iTextureIndex,
				 const QJsonObject &tileSetMetaData,
				 bool bIsPendingSave,
				 uint uiErrors);
	virtual ~AtlasTileSet();

	const QJsonObject &GetSavedTileSetMeta() const;
	int GetNumTiles() const;
	
	TileSetShape GetTileShape() const;
	void SetTileShape(TileSetShape eTileSetShape);

	QSize GetAtlasRegionSize() const;
	void SetAtlasRegionSize(QSize size);

	QSize GetTileSize() const;
	void SetTileSize(QSize size);

	QPoint GetTileOffset() const;
	void SetTileOffset(QPoint offset);

	QPolygonF GetTilePolygon() const;
	void SetTilePolygon(const QPolygonF &polygonRef);

	QString GetTileSetInfo() const;
	QIcon GetTileSetIcon() const;

	static QJsonObject GenerateNewAnimationJsonObject(QString sName, HyColor color);
	static QJsonObject GenerateNewTerrainSetJsonObject();
	static QJsonObject GenerateNewTerrainJsonObject(QUuid terrainSetUuid, QString sName, HyColor color);
	static QJsonObject GenerateNewCollisionJsonObject(HyColor color);
	QVector<QJsonObject> GetAnimations() const;
	QVector<QJsonObject> GetTerrainSets() const;
	QVector<QJsonObject> GetCollisionLayers() const;
	QJsonObject GetJsonItem(QUuid uuid) const;
	HyColor GetAnimationColor(QUuid animationUuid) const;
	AutoTileType GetTerrainSetType(QUuid terrainSetUuid) const;
	HyColor GetTerrainColor(QUuid terrainUuid) const;
	HyColor GetCollisionLayerColor(QUuid collisionLayerUuid) const;

	TileData *FindTileData(QUuid uuid) const;
	QVector<TileData *> GetTileDataList() const;

	QPixmap GetTilePixmap(const TileData *pTile) const;
	QPixmap GetTilePixmap(quint32 uiChecksum) const;

	TileSetScene *GetGfxScene();

	// Cmd functions are the only functions that change the data (via Undo/Redo)
	QList<QPair<QPoint, TileData *>> Cmd_AppendNewTiles(QSize vRegionSize, const QMap<QPoint, QPixmap> &importBatchMap, Qt::Edge eAppendEdge);
	QList<QPair<QPoint, TileData *>> Cmd_RemoveTiles(QVector<TileData *> tileDataList);
	void Cmd_ReaddTiles(QList<QPair<QPoint, TileData *>> tileDataList);
	void Cmd_MoveTiles(QList<TileData*> tileDataList, QList<QPoint> newGridPosList);
	void Cmd_AllocateJsonItem(TileSetWgtType eType, QJsonObject data);
	void Cmd_SetJsonItem(QUuid uuid, const QJsonObject &itemDataObj);
	void Cmd_RemoveJsonItem(QUuid uuid);

	QUndoStack *GetUndoStack();
	QAction *GetUndoAction();
	QAction *GetRedoAction();

	void SetSubAtlasDirty();
	void UpdateTileSetMeta();
	bool Save();
	bool IsExistencePendingSave() const;
	bool IsSaveClean() const;
	void DiscardChanges();

	virtual void InsertUniqueJson(QJsonObject &frameObj) override;

	Tiled::SharedTileset GetTiledTileSet() const;

protected:
	void UpdateTilePolygon();
	bool RegenerateSubAtlas(); // Sorts m_TileDataList. This will regenerate the sub-atlas texture and into the atlas manager
};

#endif // ATLASTILESET_H
