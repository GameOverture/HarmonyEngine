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
#include "Project.h"

AssetMimeData::AssetMimeData(Project &projRef, QList<TreeModelItemData *> &assetListRef, AssetManagerType eAssetType) :
	IMimeData(MIMETYPE_AssetItems)
{
	for(uint32 i = 0; i < NUM_ASSETMANTYPES; ++i)
		m_AssetCounts[i] = 0;

	// Sort 'assetListRef' so that root filters are listed first
	std::sort(assetListRef.begin(), assetListRef.end(), 
		[&projRef, eAssetType](const TreeModelItemData *pLhs, const TreeModelItemData *pRhs) -> bool
		{
			if(pLhs->GetType() == ITEM_Filter && pRhs->GetType() == ITEM_Filter)
			{
				IManagerModel *pManager = projRef.GetManagerModel(eAssetType);
				QString sLhsFilter = pManager->AssembleFilter(pLhs, true);
				QString sRhsFilter = pManager->AssembleFilter(pRhs, true);
				if(sLhsFilter.contains(sRhsFilter))
					return false;
				else if(sRhsFilter.contains(sLhsFilter))
					return true;
				else
					return sLhsFilter < sRhsFilter;
			}
			else if(pLhs->GetType() == ITEM_Filter && pRhs->GetType() != ITEM_Filter)
				return true;
			else if(pLhs->GetType() != ITEM_Filter && pRhs->GetType() == ITEM_Filter)
				return false;

			return pLhs->GetText() < pRhs->GetText();
		});

	QJsonObject rootAssetObj;
	for(int iAssetCount = 0; iAssetCount < NUM_ASSETMANTYPES; ++iAssetCount)
	{
		QJsonArray assetArray;
		if(eAssetType == static_cast<AssetManagerType>(iAssetCount))
		{
			assetArray = MakeAssetJsonArray(projRef, assetListRef, static_cast<AssetManagerType>(iAssetCount));
			m_AssetCounts[static_cast<AssetManagerType>(iAssetCount)] = assetArray.size();
		}
		
		rootAssetObj.insert(HyGlobal::AssetName(static_cast<AssetManagerType>(iAssetCount)), assetArray);
	}

	// Serialize the asset info into json source
	m_Data = JsonValueToSrc(QJsonValue(rootAssetObj));
	setData(HyGlobal::MimeTypeString(m_eMIME_TYPE), m_Data);
}

/*virtual*/ AssetMimeData::~AssetMimeData()
{ }

uint32 AssetMimeData::GetNumAssetsOfType(AssetManagerType eAssetType) const
{
	return m_AssetCounts[eAssetType];
}

QJsonArray AssetMimeData::GetAssetsArray(AssetManagerType eAssetType) const
{
	QJsonDocument doc = QJsonDocument::fromJson(m_Data);
	QJsonObject rootAssetObj = doc.object();

	return rootAssetObj[HyGlobal::AssetName(eAssetType)].toArray();
}
