/**************************************************************************
*	AssetMimeData.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "AssetMimeData.h"
#include "ProjectItemData.h"
#include "Project.h"
#include "IModel.h"
#include "AtlasFrame.h"
#include "IManagerModel.h"

AssetMimeData::AssetMimeData(Project &projRef, QList<TreeModelItemData *> &assetListRef, AssetType eAssetType) :
	IMimeData(MIMETYPE_AssetItems)
{
	for(uint32 i = 0; i < NUMASSETTYPES; ++i)
		m_AssetCounts[i] = 0;

	QJsonObject rootAssetObj;
	for(int iAssetCount = 0; iAssetCount < NUMASSETTYPES; ++iAssetCount)
	{
		QJsonArray assetArray;
		if(eAssetType == static_cast<AssetType>(iAssetCount))
		{
			assetArray = MakeAssetJsonArray(projRef, assetListRef, static_cast<AssetType>(iAssetCount));
			m_AssetCounts[static_cast<AssetType>(iAssetCount)] = assetArray.size();
		}
		
		rootAssetObj.insert(HyGlobal::AssetName(static_cast<AssetType>(iAssetCount)), assetArray);
	}

	// Serialize the asset info into json source
	m_Data = JsonValueToSrc(QJsonValue(rootAssetObj));
	setData(HyGlobal::MimeTypeString(m_eMIME_TYPE), m_Data);
}

/*virtual*/ AssetMimeData::~AssetMimeData()
{ }

uint32 AssetMimeData::GetNumAssetsOfType(AssetType eAssetType) const
{
	return m_AssetCounts[eAssetType];
}

QJsonArray AssetMimeData::GetAssetsArray(AssetType eAssetType) const
{
	QJsonDocument doc = QJsonDocument::fromJson(m_Data);
	QJsonObject rootAssetObj = doc.object();

	return rootAssetObj[HyGlobal::AssetName(eAssetType)].toArray();
}
