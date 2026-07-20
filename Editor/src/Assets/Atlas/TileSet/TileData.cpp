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
#include "VectorModel.h"

#include <QBitArray>

TileData::TileData(quint32 uiTileChecksum, QPoint metaGridPos) :
	m_Uuid(QUuid::createUuid()),
	m_uiTileChecksum(uiTileChecksum),
	m_MetaGridPos(metaGridPos),
	m_pSetupPropertiesModel(nullptr)
{
	InitPropertiesModel();
}

TileData::TileData(const QJsonObject &tileDataObj) :
	m_Uuid(QUuid(tileDataObj["UUID"].toString())),
	m_uiTileChecksum(static_cast<quint32>(tileDataObj["TileChecksum"].toVariant().toLongLong())),
	m_MetaGridPos(QPoint(tileDataObj["MetaGridPosX"].toInt(), tileDataObj["MetaGridPosY"].toInt())),
	m_pSetupPropertiesModel(nullptr),
	m_TerrainSetUuid(QUuid(tileDataObj["TerrainSetUUID"].toString()))
{
	InitPropertiesModel();
	m_pSetupPropertiesModel->DeserializeJson(tileDataObj["SetupProperties"].toObject());

	QJsonObject animObj = tileDataObj["Animation"].toObject();
	for(QString sKey : animObj.keys())
	{
		QList<int> frameIndexList;
		QJsonArray frameIndexArray = animObj[sKey].toArray();
		for(QJsonValue val : frameIndexArray)
			frameIndexList.push_back(val.toInt());
		m_AnimationMap.insert(QUuid(sKey), frameIndexList);
	}

	QJsonArray terrainArray = tileDataObj["TerrainMap"].toArray();
	for(int i = 0; i < terrainArray.size(); ++i)
	{
		QJsonObject terrainObj = terrainArray[i].toObject();
		QUuid terrainUuid = QUuid(terrainObj["TerrainUUID"].toString());
		int iPartFlags = terrainObj["PeeringBits"].toInt();
		QBitArray peeringBits(NUM_AUTOTILEPARTS, false);
		for(int i = 0; i < NUM_AUTOTILEPARTS; ++i)
		{
			if(0 != (iPartFlags & (1 << i)))
				peeringBits.setBit(i);
		}
		m_TerrainMap[terrainUuid] = peeringBits;
	}

	QJsonArray collisionArray = tileDataObj["CollisionLayerMap"].toArray();
	for(int i = 0; i < collisionArray.size(); ++i)
	{
		QJsonObject collisionObj = collisionArray[i].toObject();
		QUuid collisionUuid = QUuid(collisionObj["CollisionLayerUUID"].toString());
		QJsonArray dataArray = collisionObj["Data"].toArray();
		QList<float> vertexList;
		for(int j = 0; j < dataArray.size(); ++j)
			vertexList.push_back(static_cast<float>(dataArray[j].toDouble()));

		m_CollisionLayerMap[collisionUuid] = new VectorModel(EDITMODETYPE_FixtureShape, HyGlobal::GetEditorColor(EDITORCOLOR_Fixtures));
	}
}

TileData::TileData(TileData &&other) noexcept :
	m_Uuid(other.m_Uuid),
	m_uiTileChecksum(other.m_uiTileChecksum),
	m_MetaGridPos(other.m_MetaGridPos),
	m_pSetupPropertiesModel(nullptr),
	m_AnimationMap(other.m_AnimationMap),
	m_TerrainSetUuid(other.m_TerrainSetUuid),
	m_TerrainMap(std::move(other.m_TerrainMap)),
	m_CollisionLayerMap(std::move(other.m_CollisionLayerMap))
{
	PropertiesTreeModel *pOldPropModel = m_pSetupPropertiesModel;
	InitPropertiesModel();
	m_pSetupPropertiesModel->DeserializeJson(pOldPropModel->SerializeJson());
	delete pOldPropModel;

	other.m_Uuid = QUuid();
	other.m_uiTileChecksum = 0;

	HyGuiLog("TileData move ctor invoked", LOGTYPE_Warning); // Determining if this is ever called
}

TileData::~TileData()
{
	delete m_pSetupPropertiesModel;
}

void TileData::InitPropertiesModel()
{
	m_pSetupPropertiesModel = new PropertiesTreeModel(nullptr, -1, QVariant());

	m_pSetupPropertiesModel->InsertCategory(-1, "Info");
	m_pSetupPropertiesModel->AppendProperty("Info", "Tile ID", PROPERTIESTYPE_int, TILEDATA_INVALID_ID, "This tile's assigned ID used in a TileMap. This ID may change when the TileSet's atlas is repacked, tiles are added/removed, or tile animations are set. Changed IDs will automatically update existing TileMaps", PROPERTIESACCESS_ReadOnly);
	//m_pSetupPropertiesModel->AppendProperty("Info", "Tile Checksum", PROPERTIESTYPE_int, 0, "The tile's image checksum", PROPERTIESACCESS_ReadOnly); // The row-major index of the tile image packed in the sub-atlas. Tiles with duplicate images, or Tile Variants will share the same Atlas Index
	m_pSetupPropertiesModel->AppendProperty("Info", "Is Variant Tile", PROPERTIESTYPE_bool, false, "False indicates this is a standard imported tile. If true, this tile references a standard imported tile but may have different properties set", PROPERTIESACCESS_ReadOnly);

	m_pSetupPropertiesModel->InsertCategory(-1, "Rendering");
	m_pSetupPropertiesModel->AppendProperty("Rendering", "Texture Origin Offset", PROPERTIESTYPE_ivec2, QPoint(0, 0), "Tiles are placed centered at their grid location. This property can be used to visually offset the tile", PROPERTIESACCESS_Mutable, 0, 0xFFFF, 1);
	m_pSetupPropertiesModel->AppendProperty("Rendering", "Flip Horz", PROPERTIESTYPE_bool, false, "If true, the tile is flipped horizontally", PROPERTIESACCESS_Mutable);
	m_pSetupPropertiesModel->AppendProperty("Rendering", "Flip Vert", PROPERTIESTYPE_bool, false, "If true, the tile is flipped vertically", PROPERTIESACCESS_Mutable);
	m_pSetupPropertiesModel->AppendProperty("Rendering", "Transpose", PROPERTIESTYPE_bool, false, "If true, the tile is rotated 90 degrees counter-clockwise and then flipped vertically. If you want to roate a tile by 90 degrees clockwise without flipping it, you would enable both 'Flip Horz' and 'Transpose'. To rotate a tile by 180 degrees clockwise, enable 'Flip Horz' and 'Flip Vert'. To rotate a tile by 270 degrees clockwise, enable 'Flip Vert' and 'Transpose'", PROPERTIESACCESS_Mutable);
	m_pSetupPropertiesModel->AppendProperty("Rendering", "Color Tint", PROPERTIESTYPE_Color, QRect(255, 255, 255, 0), "A color to alpha blend this tile with", PROPERTIESACCESS_Mutable);
	m_pSetupPropertiesModel->AppendProperty("Rendering", "Alpha", PROPERTIESTYPE_double, 1.0, "A value from 0.0 to 1.0 that indicates how opaque/transparent this tile is", PROPERTIESACCESS_Mutable, 0.0, 1.0, 0.05);
	
	m_pSetupPropertiesModel->InsertCategory(-1, "Randomization");
	m_pSetupPropertiesModel->AppendProperty("Randomization", "Probability", PROPERTIESTYPE_double, 1.0, "The relative probability of this tile appearing when painting with \"Place Random Tile\" enabled", PROPERTIESACCESS_Mutable, 0.0, 1.0, 0.05);
}

QUuid TileData::GetUuid() const
{
	return m_Uuid;
}

quint32 TileData::GetTileChecksum() const
{
	return m_uiTileChecksum;
}

uint16 TileData::GetTileId() const
{
	return static_cast<uint16>(m_pSetupPropertiesModel->FindPropertyValue("Info", "Tile ID").toInt());
}

void TileData::SetTileId(uint16 uiTileId)
{
	m_pSetupPropertiesModel->SetPropertyValue("Info", "Tile ID", QVariant(uiTileId));
}

QPoint TileData::GetMetaGridPos() const
{
	return m_MetaGridPos;
}

void TileData::SetMetaGridPos(QPoint metaGridPos)
{
	m_MetaGridPos = metaGridPos;
}

QJsonObject TileData::GetTileData() const
{
	QJsonObject tileDataObjOut;

	tileDataObjOut["UUID"] = m_Uuid.toString(QUuid::WithoutBraces);
	tileDataObjOut["TileChecksum"] = static_cast<qint64>(m_uiTileChecksum);
	tileDataObjOut["MetaGridPosX"] = m_MetaGridPos.x();
	tileDataObjOut["MetaGridPosY"] = m_MetaGridPos.y();
	tileDataObjOut["SetupProperties"] = m_pSetupPropertiesModel->SerializeJson();

	QJsonObject animObj;
	for(QUuid uuid : m_AnimationMap.keys())
	{
		QJsonArray frameIndexArray;
		for(int iFrameIndex : m_AnimationMap[uuid])
			frameIndexArray.append(iFrameIndex);
		animObj.insert(uuid.toString(QUuid::WithoutBraces), frameIndexArray);
	}
	tileDataObjOut["Animation"] = animObj;
	tileDataObjOut["TerrainSetUUID"] = m_TerrainSetUuid.toString(QUuid::WithoutBraces);
	QJsonArray terrainMapArray;
	for(QMap<QUuid, QBitArray>::const_iterator iter = m_TerrainMap.begin(); iter != m_TerrainMap.end(); ++iter)
	{
		QJsonObject terrainObj;
		terrainObj.insert("TerrainUUID", iter.key().toString(QUuid::WithoutBraces));
		bool bConvertResult = false;
		int iPeeringBits = iter.value().toUInt32(QSysInfo::Endian::LittleEndian, &bConvertResult);
		if(bConvertResult)
		{
			terrainObj.insert("PeeringBits", iPeeringBits);
			terrainMapArray.push_back(terrainObj);
		}
		else
			HyGuiLog("Failed to convert terrain peering bits", LOGTYPE_Error);
	}
	tileDataObjOut["TerrainMap"] = terrainMapArray;

	QJsonArray collisionArray;
	for(auto it = m_CollisionLayerMap.begin(); it != m_CollisionLayerMap.end(); ++it)
	{
		QJsonObject collisionObj;
		collisionObj["CollisionLayerUUID"] = it.key().toString(QUuid::WithoutBraces);

		//QJsonArray floatArray;
		//QList<float> floatList = it.value()->Serialize();
		//for(int j = 0; j < floatList.size(); ++j)
		//	floatArray.push_back(floatList[j]);
		QJsonObject serialziedObj = it.value()->Serialize();
		collisionObj["Data"] = serialziedObj["data"].toArray();

		collisionArray.push_back(collisionObj);
	}
	tileDataObjOut["CollisionLayerMap"] = collisionArray;

	return tileDataObjOut;
}

PropertiesTreeModel *TileData::GetSetupPropertiesModel() const
{
	return m_pSetupPropertiesModel;
}

const QMap<QUuid, QList<int>> &TileData::GetAnimationMap() const
{
	return m_AnimationMap;
}

void TileData::SetAnimationMap(QMap<QUuid, QList<int>> animMap)
{
	m_AnimationMap = animMap;
}

void TileData::SetAnimationFrame(QUuid animationUuid, int iFrameIndex)
{
	if(m_AnimationMap.contains(animationUuid) == false)
		m_AnimationMap.insert(animationUuid, QList<int>());
	if(m_AnimationMap[animationUuid].contains(iFrameIndex) == false)
		m_AnimationMap[animationUuid].push_back(iFrameIndex);
}

void TileData::RemoveAnimationFrame(QUuid animationUuid, int iFrameIndex)
{
	if(m_AnimationMap.contains(animationUuid) == false)
		return;

	m_AnimationMap[animationUuid].removeAll(iFrameIndex);
	if(m_AnimationMap[animationUuid].isEmpty())
		m_AnimationMap.remove(animationUuid);
}

QUuid TileData::GetTerrainSet() const
{
	return m_TerrainSetUuid;
}

void TileData::SetTerrainSet(QUuid terrainSetUuid)
{
	if(m_TerrainSetUuid == terrainSetUuid)
		return;

	m_TerrainSetUuid = terrainSetUuid;
	m_TerrainMap.clear();
}

QUuid TileData::GetTerrain(TileSetAutoTilePart ePart) const
{
	for(auto it = m_TerrainMap.begin(); it != m_TerrainMap.end(); ++it)
	{
		if(it.value().testBit(static_cast<int>(ePart)))
			return it.key();
	}
	
	return QUuid(); // Indicates no terrain assigned
}

void TileData::SetTerrain(QUuid terrainUuid, TileSetAutoTilePart ePart)
{
	ClearTerrain(ePart);

	if(m_TerrainMap.contains(terrainUuid))
		m_TerrainMap[terrainUuid].setBit(static_cast<int>(ePart), true);
	else
	{
		QBitArray bitArray(NUM_AUTOTILEPARTS, false);
		bitArray.setBit(static_cast<int>(ePart), true);
		m_TerrainMap[terrainUuid] = bitArray;
	}
}

void TileData::ClearTerrain(TileSetAutoTilePart ePart)
{
	for(auto it = m_TerrainMap.begin(); it != m_TerrainMap.end();)
	{
		if(it.value().testBit(static_cast<int>(ePart)))
		{
			it.value().setBit(static_cast<int>(ePart), false);
			// If no more bits are set, remove the entry altogether
			if(it.value().count(true) == 0)
				it = m_TerrainMap.erase(it);
			else
				++it;
		}
		else
			++it;
	}
}

const QMap<QUuid, QBitArray> &TileData::GetTerrainMap() const
{
	return m_TerrainMap;
}

void TileData::SetTerrainMap(const QMap<QUuid, QBitArray> &terrainMap)
{
	m_TerrainMap = terrainMap;
}

QList<QUuid> TileData::GetCollisionLayerList() const
{
	return m_CollisionLayerMap.keys();
}

VectorModel *TileData::GetCollisionLayerModel(QUuid uuid) const
{
	return m_CollisionLayerMap.value(uuid);
}

