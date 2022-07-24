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

AssetMimeData::AssetMimeData(QList<AssetItemData *> &assetListRef) :
	IMimeData(MIMETYPE_Assets)
{
	QJsonArray assetArray;
	for(int i = 0; i < assetListRef.size(); ++i)
	{
		if(assetListRef[i] == nullptr)
			continue;

		QJsonObject itemObj = MakeAssetJsonObj(*assetListRef[i]);
		assetArray.append(itemObj);
	}

	// Serialize the asset info into json source
	m_Data = JsonValueToSrc(QJsonValue(assetArray));
	setData(HYGUI_MIMETYPE_ASSET, m_Data);
}

/*virtual*/ AssetMimeData::~AssetMimeData()
{ }
