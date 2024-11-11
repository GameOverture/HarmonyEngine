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



TileData::TileData(const QJsonObject &tileDataObj)
{
	m_iAtlasIndex = tileDataObj["AtlasIndex"].toInt();
	m_TextureOffset = QPoint(tileDataObj["TextureOffsetX"].toInt(), tileDataObj["TextureOffsetY"].toInt());
	m_bIsFlippedHorz = tileDataObj["IsFlippedHorz"].toBool();
	m_bIsFlippedVert = tileDataObj["IsFlippedVert"].toBool();
	m_bIsRotated = tileDataObj["IsRotated"].toBool();


	// TODO
	m_iAnimFrame = tileDataObj["AnimFrame"].toInt();



	m_iProbability = tileDataObj["Probability"].toInt();

	QJsonArray autoTileArray = tileDataObj["AutoTileMap"].toArray();
	for(int i = 0; i < autoTileArray.size(); ++i)
	{
		QJsonObject autoTileObj = autoTileArray[i].toObject();
		m_AutoTileMap[autoTileObj["AutoTileHandle"].toInt()] = autoTileObj["PeeringBits"].toInt();
	}

	QJsonArray VertexArray = tileDataObj["VertexMap"].toArray();
	for(int i = 0; i < VertexArray.size(); ++i)
	{
		QJsonObject vertexObj = VertexArray[i].toObject();
		PhysicsLayerHandle layerHandle = vertexObj["LayerHandle"].toInt();
		QJsonArray polygonArray = vertexObj["PolygonList"].toArray();
		QList<QList<QPoint>> vertexList;
		for(int iPolygonIndex = 0; iPolygonIndex < polygonArray.size(); ++iPolygonIndex)
		{
			QJsonArray pointArray = polygonArray[iPolygonIndex].toArray();
			QList<QPoint> pointList;
			for(int k = 0; k < pointArray.size(); ++k)
			{
				QJsonObject pointObj = pointArray[k].toObject();
				pointList.push_back(QPoint(pointObj["x"].toInt(), pointObj["y"].toInt()));
			}
			vertexList.push_back(pointList);
		}
		m_VertexMap[layerHandle] = vertexList;
	}
}

void TileData::GetTileData(QJsonObject &tileDataObjOut)
{
	tileDataObjOut["AtlasIndex"] = m_iAtlasIndex;
	tileDataObjOut["TextureOffsetX"] = m_TextureOffset.x();
	tileDataObjOut["TextureOffsetY"] = m_TextureOffset.y();
	tileDataObjOut["IsFlippedHorz"] = m_bIsFlippedHorz;
	tileDataObjOut["IsFlippedVert"] = m_bIsFlippedVert;
	tileDataObjOut["IsRotated"] = m_bIsRotated;
	
	// TODO
	tileDataObjOut["AnimFrame"] = m_iAnimFrame;

	tileDataObjOut["Probability"] = m_iProbability;
	QJsonArray autoTileArray;
	for(auto it = m_AutoTileMap.begin(); it != m_AutoTileMap.end(); ++it)
	{
		QJsonObject autoTileObj;
		autoTileObj["AutoTileHandle"] = static_cast<int>(it.key());
		autoTileObj["PeeringBits"] = it.value();
		autoTileArray.push_back(autoTileObj);
	}
	tileDataObjOut["AutoTileMap"] = autoTileArray;
	QJsonArray VertexArray;
	for(auto it = m_VertexMap.begin(); it != m_VertexMap.end(); ++it)
	{
		QJsonObject vertexObj;
		vertexObj["LayerHandle"] = static_cast<int>(it.key());
		QJsonArray polygonArray;
		for(int iPolygonIndex = 0; iPolygonIndex < it.value().size(); ++iPolygonIndex)
		{
			QJsonArray pointArray;
			for(int k = 0; k < it.value()[iPolygonIndex].size(); ++k)
			{
				QJsonObject pointObj;
				pointObj["x"] = it.value()[iPolygonIndex][k].x();
				pointObj["y"] = it.value()[iPolygonIndex][k].y();
				pointArray.push_back(pointObj);
			}
			polygonArray.push_back(pointArray);
		}
		vertexObj["PolygonList"] = polygonArray;
		VertexArray.push_back(vertexObj);
	}
	tileDataObjOut["VertexMap"] = VertexArray;
}
