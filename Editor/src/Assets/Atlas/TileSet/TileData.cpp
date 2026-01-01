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

#include <QBitArray>

TileData::TileData(QPoint metaGridPos, QPixmap tilePixmap) :
	m_Uuid(QUuid::createUuid()),
	m_MetaGridPos(metaGridPos),
	m_TilePixmap(tilePixmap),
	m_TextureOffset(0, 0),
	m_bIsFlippedHorz(false),
	m_bIsFlippedVert(false),
	m_bIsRotated(false),
	m_iProbability(100)
{
}

TileData::TileData(const QJsonObject &tileDataObj, QPixmap tilePixmap) :
	m_Uuid(QUuid(tileDataObj["UUID"].toString())),
	m_MetaGridPos(QPoint(tileDataObj["MetaGridPosX"].toInt(), tileDataObj["MetaGridPosY"].toInt())),
	m_TilePixmap(tilePixmap),
	m_TextureOffset(QPoint(tileDataObj["TextureOffsetX"].toInt(), tileDataObj["TextureOffsetY"].toInt())),
	m_bIsFlippedHorz(tileDataObj["IsFlippedHorz"].toBool()),
	m_bIsFlippedVert(tileDataObj["IsFlippedVert"].toBool()),
	m_bIsRotated(tileDataObj["IsRotated"].toBool()),
	m_iProbability(tileDataObj["Probability"].toInt()),
	m_AnimationUuid(QUuid(tileDataObj["AnimationUUID"].toString())),
	m_TerrainSetUuid(QUuid(tileDataObj["TerrainSetUUID"].toString()))
{
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

	QJsonArray collisionArray = tileDataObj["CollisionMap"].toArray();
	for(int i = 0; i < collisionArray.size(); ++i)
	{
		QJsonObject collisionObj = collisionArray[i].toObject();
		QUuid collisionUuid = QUuid(collisionObj["CollisionUUID"].toString());
		QJsonArray pointArray = collisionObj["VertexList"].toArray();
		QList<QPointF> vertexList;
		for(int j = 0; j < pointArray.size(); ++j)
		{
			QJsonObject pointObj = pointArray[j].toObject();
			vertexList.push_back(QPointF(pointObj["x"].toDouble(), pointObj["y"].toDouble()));
		}
		m_CollisionMap[collisionUuid] = vertexList;
	}
}

TileData::TileData(const TileData &other) :
	m_TilePixmap(other.m_TilePixmap),
	m_TextureOffset(other.m_TextureOffset),
	m_bIsFlippedHorz(other.m_bIsFlippedHorz),
	m_bIsFlippedVert(other.m_bIsFlippedVert),
	m_bIsRotated(other.m_bIsRotated),
	m_iProbability(other.m_iProbability),
	m_AnimationUuid(other.m_AnimationUuid),
	m_TerrainSetUuid(other.m_TerrainSetUuid),
	m_TerrainMap(other.m_TerrainMap),
	m_CollisionMap(other.m_CollisionMap)
{
}

TileData &TileData::operator=(const TileData &other)
{
	if(this == &other)
		return *this;
	
	m_TilePixmap = other.m_TilePixmap;
	m_TextureOffset = other.m_TextureOffset;
	m_bIsFlippedHorz = other.m_bIsFlippedHorz;
	m_bIsFlippedVert = other.m_bIsFlippedVert;
	m_bIsRotated = other.m_bIsRotated;
	m_iProbability = other.m_iProbability;
	m_AnimationUuid = other.m_AnimationUuid;
	m_TerrainSetUuid = other.m_TerrainSetUuid;
	m_TerrainMap = other.m_TerrainMap;
	m_CollisionMap = other.m_CollisionMap;

	return *this;
}

TileData::~TileData()
{
}

QUuid TileData::GetUuid() const
{
	return m_Uuid;
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
	tileDataObjOut["MetaGridPosX"] = m_MetaGridPos.x();
	tileDataObjOut["MetaGridPosY"] = m_MetaGridPos.y();
	tileDataObjOut["TextureOffsetX"] = m_TextureOffset.x();
	tileDataObjOut["TextureOffsetY"] = m_TextureOffset.y();
	tileDataObjOut["IsFlippedHorz"] = m_bIsFlippedHorz;
	tileDataObjOut["IsFlippedVert"] = m_bIsFlippedVert;
	tileDataObjOut["IsRotated"] = m_bIsRotated;
	tileDataObjOut["Probability"] = m_iProbability;
	
	tileDataObjOut["AnimationUUID"] = m_AnimationUuid.toString(QUuid::WithoutBraces);
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
	for(auto it = m_CollisionMap.begin(); it != m_CollisionMap.end(); ++it)
	{
		QJsonObject collisionObj;
		collisionObj["CollisionUUID"] = it.key().toString(QUuid::WithoutBraces);
		QJsonArray pointArray;
		for(int i = 0; i < it.value().size(); ++i)
		{
			QJsonObject pointObj;
			pointObj["x"] = it.value()[i].x();
			pointObj["y"] = it.value()[i].y();
			pointArray.push_back(pointObj);
		}
		collisionObj["VertexList"] = pointArray;
		collisionArray.push_back(collisionObj);
	}
	tileDataObjOut["CollisionMap"] = collisionArray;

	return tileDataObjOut;
}

QPoint TileData::GetTextureOffset() const
{
	return m_TextureOffset;
}

QPixmap TileData::GetPixmap() const
{
	return m_TilePixmap;
}

QUuid TileData::GetAnimation() const
{
	return m_AnimationUuid;
}

void TileData::SetAnimation(QUuid animationUuid)
{
	m_AnimationUuid = animationUuid;
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

QList<QUuid> TileData::GetCollisionList() const
{
	return m_CollisionMap.keys();
}

QList<QPointF> TileData::GetCollisionVertices(QUuid uuid) const
{
	return m_CollisionMap.value(uuid);
}

void TileData::SetCollisionVertices(QUuid uuid, const QList<QPointF> &vertexList)
{
	m_CollisionMap[uuid] = vertexList;
}
