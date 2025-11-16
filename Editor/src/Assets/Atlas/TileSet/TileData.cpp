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

TileData::TileData(QPoint metaGridPos, QPixmap tilePixmap) :
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
	m_MetaGridPos(QPoint(tileDataObj["MetaGridPosX"].toInt(), tileDataObj["MetaGridPosY"].toInt())),
	m_TilePixmap(tilePixmap),
	m_TextureOffset(QPoint(tileDataObj["TextureOffsetX"].toInt(), tileDataObj["TextureOffsetY"].toInt())),
	m_bIsFlippedHorz(tileDataObj["IsFlippedHorz"].toBool()),
	m_bIsFlippedVert(tileDataObj["IsFlippedVert"].toBool()),
	m_bIsRotated(tileDataObj["IsRotated"].toBool()),
	m_iProbability(tileDataObj["Probability"].toInt())
{
	//QJsonArray autoTileArray = tileDataObj["AutoTileMap"].toArray();
	//for(int i = 0; i < autoTileArray.size(); ++i)
	//{
	//	QJsonObject autoTileObj = autoTileArray[i].toObject();
	//	m_AutoTileMap[autoTileObj["AutoTileHandle"].toInt()] = autoTileObj["PeeringBits"].toInt();
	//}

	//QJsonArray VertexArray = tileDataObj["VertexMap"].toArray();
	//for(int i = 0; i < VertexArray.size(); ++i)
	//{
	//	QJsonObject vertexObj = VertexArray[i].toObject();
	//	PhysicsLayerHandle layerHandle = vertexObj["LayerHandle"].toInt();
	//	QJsonArray polygonArray = vertexObj["PolygonList"].toArray();
	//	QList<QList<QPoint>> vertexList;
	//	for(int iPolygonIndex = 0; iPolygonIndex < polygonArray.size(); ++iPolygonIndex)
	//	{
	//		QJsonArray pointArray = polygonArray[iPolygonIndex].toArray();
	//		QList<QPoint> pointList;
	//		for(int k = 0; k < pointArray.size(); ++k)
	//		{
	//			QJsonObject pointObj = pointArray[k].toObject();
	//			pointList.push_back(QPoint(pointObj["x"].toInt(), pointObj["y"].toInt()));
	//		}
	//		vertexList.push_back(pointList);
	//	}
	//	m_VertexMap[layerHandle] = vertexList;
	//}
}

TileData::TileData(const TileData &other) :
	m_TilePixmap(other.m_TilePixmap),
	m_TextureOffset(other.m_TextureOffset),
	m_bIsFlippedHorz(other.m_bIsFlippedHorz),
	m_bIsFlippedVert(other.m_bIsFlippedVert),
	m_bIsRotated(other.m_bIsRotated),
	m_iProbability(other.m_iProbability),
	m_AutoTileMap(other.m_AutoTileMap),
	m_VertexMap(other.m_VertexMap)
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
	m_AutoTileMap = other.m_AutoTileMap;
	m_VertexMap = other.m_VertexMap;

	return *this;
}

TileData::~TileData()
{
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

	tileDataObjOut["MetaGridPosX"] = m_MetaGridPos.x();
	tileDataObjOut["MetaGridPosY"] = m_MetaGridPos.y();
	tileDataObjOut["TextureOffsetX"] = m_TextureOffset.x();
	tileDataObjOut["TextureOffsetY"] = m_TextureOffset.y();
	tileDataObjOut["IsFlippedHorz"] = m_bIsFlippedHorz;
	tileDataObjOut["IsFlippedVert"] = m_bIsFlippedVert;
	tileDataObjOut["IsRotated"] = m_bIsRotated;
	tileDataObjOut["Probability"] = m_iProbability;
	
	//tileDataObjOut["AnimFrame"] = m_iAnimFrame;
	//QJsonArray autoTileArray;
	//for(auto it = m_AutoTileMap.begin(); it != m_AutoTileMap.end(); ++it)
	//{
	//	QJsonObject autoTileObj;
	//	autoTileObj["AutoTileHandle"] = it.key().toString(QUuid::WithoutBraces);
	//	autoTileObj["PeeringBits"] = it.value();
	//	autoTileArray.push_back(autoTileObj);
	//}
	//tileDataObjOut["AutoTileMap"] = autoTileArray;
	//QJsonArray VertexArray;
	//for(auto it = m_VertexMap.begin(); it != m_VertexMap.end(); ++it)
	//{
	//	QJsonObject vertexObj;
	//	vertexObj["LayerHandle"] = static_cast<int>(it.key());
	//	QJsonArray polygonArray;
	//	for(int iPolygonIndex = 0; iPolygonIndex < it.value().size(); ++iPolygonIndex)
	//	{
	//		QJsonArray pointArray;
	//		for(int k = 0; k < it.value()[iPolygonIndex].size(); ++k)
	//		{
	//			QJsonObject pointObj;
	//			pointObj["x"] = it.value()[iPolygonIndex][k].x();
	//			pointObj["y"] = it.value()[iPolygonIndex][k].y();
	//			pointArray.push_back(pointObj);
	//		}
	//		polygonArray.push_back(pointArray);
	//	}
	//	vertexObj["PolygonList"] = polygonArray;
	//	VertexArray.push_back(vertexObj);
	//}
	//tileDataObjOut["VertexMap"] = VertexArray;

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
