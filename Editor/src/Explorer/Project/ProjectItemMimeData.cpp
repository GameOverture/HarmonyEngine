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
#include "IManagerModel.h"

ProjectItemMimeData::ProjectItemMimeData(QList<ExplorerItemData *> &itemListRef) :
	IMimeData(MIMETYPE_ProjectItems)
{
	// TODO: Find projItem dependencies for each item and add them to the rootArray


	QJsonArray rootArray; // An array of ProjectItems
	for(int i = 0; i < itemListRef.size(); ++i)
	{
		QJsonObject itemObj;
		itemObj.insert("project", itemListRef[i]->GetProject().GetAbsPath().toLower());
		itemObj.insert("name", itemListRef[i]->GetName(true));
		itemObj.insert("type", HyGlobal::ItemName(itemListRef[i]->GetType(), false));

		// If ProjectItem (aka not a prefix)
		if(itemListRef[i]->IsProjectItem())
		{
			itemObj.insert("isPrefix", false);

			FileDataPair itemFileData;
			ProjectItemData *pProjectItem = static_cast<ProjectItemData *>(itemListRef[i]);
			pProjectItem->GetSavedFileData(itemFileData);
			itemObj.insert("metaObj", itemFileData.m_Meta);
			itemObj.insert("dataObj", itemFileData.m_Data);

			itemObj.insert("UUID", pProjectItem->GetUuid().toString(QUuid::WithoutBraces));

			// ASSETS FROM MANAGERS
			for(int iAssetCount = 0; iAssetCount < NUM_ASSETMANTYPES; ++iAssetCount)
			{
				QList<TreeModelItemData *> treeModelList;
				QList<IAssetItemData *> assetsItemList = pProjectItem->GetModel()->GetAssetDependencies(static_cast<AssetManagerType>(iAssetCount));
				for(auto *pItem : assetsItemList)
					treeModelList.push_back(pItem);

				QJsonArray assetArray = MakeAssetJsonArray(pProjectItem->GetProject(), treeModelList, static_cast<AssetManagerType>(iAssetCount));
				itemObj.insert(HyGlobal::AssetName(static_cast<AssetManagerType>(iAssetCount)), assetArray);
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
		}
		else
			itemObj.insert("isPrefix", true);
		
		rootArray.append(itemObj);
	}

	// Serialize the item info into json source
	m_Data = JsonValueToSrc(QJsonValue(rootArray));
	setData(HyGlobal::MimeTypeString(m_eMIME_TYPE), m_Data);
}

/*virtual*/ ProjectItemMimeData::~ProjectItemMimeData()
{ }

/*static*/ void ProjectItemMimeData::RegenUuids(Project *pDestProject, QByteArray &jsonDataOut)
{
	QMap<QString, QString> uuidReplacementMap;

	QJsonDocument mimeDoc = QJsonDocument::fromJson(jsonDataOut);
	QJsonArray rootArray = mimeDoc.array();

	for(int i = 0; i < rootArray.size(); ++i)
	{
		QJsonObject itemObj = rootArray[i].toObject();

		if(itemObj["project"].toString().toLower() == pDestProject->GetAbsPath().toLower())
			continue;

		if(itemObj["isPrefix"].toBool() == false)
			uuidReplacementMap.insert(itemObj["UUID"].toString(), QUuid::createUuid().toString(QUuid::WithoutBraces));

		for(int iAssetCount = 0; iAssetCount < NUM_ASSETMANTYPES; ++iAssetCount)
		{
			AssetManagerType eAssetManType = static_cast<AssetManagerType>(iAssetCount);

			QJsonArray assetArray = itemObj[HyGlobal::AssetName(eAssetManType)].toArray();
			for(int i = 0; i < assetArray.size(); ++i)
			{
				QJsonObject assetObj = assetArray[i].toObject();
				if(assetObj["isFilter"].toBool() == false)
				{
					QList<IAssetItemData *> existingAssetList = pDestProject->GetManagerModel(eAssetManType)->FindByChecksum(assetObj["checksum"].toVariant().toLongLong());
					
					if(existingAssetList.empty()) // Create a new uuid
						uuidReplacementMap.insert(assetObj["assetUUID"].toString(), QUuid::createUuid().toString(QUuid::WithoutBraces));
					else // Use the existing asset's uuid
						uuidReplacementMap.insert(assetObj["assetUUID"].toString(), existingAssetList[0]->GetUuid().toString(QUuid::WithoutBraces));
				}
			}
		}
	}

	// Now that we have a map of old uuids to new uuids, replace them in the 'jsonDataOut'
	for(auto iter = uuidReplacementMap.begin(); iter != uuidReplacementMap.end(); ++iter)
	{
		QString sOldUuid = iter.key();
		QString sNewUuid = iter.value();
		jsonDataOut.replace(sOldUuid.toUtf8(), sNewUuid.toUtf8());
	}
}
