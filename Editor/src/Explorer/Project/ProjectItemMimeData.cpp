/**************************************************************************
*	ProjectItemMimeData.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "ProjectItemMimeData.h"
#include "ProjectItemData.h"
#include "Project.h"
#include "IModel.h"
#include "AtlasFrame.h"
#include "TextModel.h"

ProjectItemMimeData::ProjectItemMimeData(QList<ExplorerItemData *> &itemListRef) :
	IMimeData(MIMETYPE_ProjectItems)
{
	QJsonArray rootArray; // An array of ProjectItems
	for(int i = 0; i < itemListRef.size(); ++i)
	{
		if(itemListRef[i]->IsProjectItem() == false)
			continue;

		QJsonObject itemObj;
		itemObj.insert("project", itemListRef[i]->GetProject().GetAbsPath().toLower());
		itemObj.insert("name", itemListRef[i]->GetName(true));
		itemObj.insert("type", HyGlobal::ItemName(itemListRef[i]->GetType(), false));


		// STANDARD INFO
		FileDataPair itemFileData;
		ProjectItemData *pProjectItem = static_cast<ProjectItemData *>(itemListRef[i]);
		pProjectItem->GetLatestFileData(itemFileData);
		itemObj.insert("metaObj", itemFileData.m_Meta);
		itemObj.insert("dataObj", itemFileData.m_Data);

		// ASSETS FROM MANAGERS
		for(int iAssetCount = 0; iAssetCount < NUMASSETTYPES; ++iAssetCount)
		{
			QList<AssetItemData *> assetList = pProjectItem->GetModel()->GetAssets(static_cast<AssetType>(iAssetCount));
			
			QJsonArray assetArray = MakeAssetJsonArray(assetList, static_cast<AssetType>(iAssetCount));
			itemObj.insert(HyGlobal::AssetName(static_cast<AssetType>(iAssetCount)), assetArray);
		}

		// FONT INFO
		if(pProjectItem->GetType() == ITEM_Text)
		{
			QStringList fontUrlList = static_cast<TextModel *>(pProjectItem->GetModel())->GetFontUrls();
			QJsonArray fontUrlArray;
			for(int i = 0; i < fontUrlList.size(); ++i)
				fontUrlArray.append(fontUrlList[i]);
			itemObj.insert("fonts", fontUrlArray);
		}
		
		rootArray.append(itemObj);
	}

	// Serialize the item info into json source
	m_Data = JsonValueToSrc(QJsonValue(rootArray));
	setData(HYGUI_MIMETYPE_ITEM, m_Data);
}

/*virtual*/ ProjectItemMimeData::~ProjectItemMimeData()
{ }
