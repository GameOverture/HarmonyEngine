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

ProjectItemMimeData::ProjectItemMimeData(QList<ExplorerItemData *> &itemListRef)
{
	QJsonArray clipboardArray;

	for(int i = 0; i < itemListRef.size(); ++i)
	{
		if(itemListRef[i]->IsProjectItem() == false)
			continue;

		ProjectItemData *pProjectItem = static_cast<ProjectItemData *>(itemListRef[i]);

		QJsonObject clipboardObj;

		// STANDARD INFO
		clipboardObj.insert("project", pProjectItem->GetProject().GetAbsPath());
		clipboardObj.insert("itemType", HyGlobal::ItemName(pProjectItem->GetType(), false));
		clipboardObj.insert("itemName", pProjectItem->GetName(true));

		FileDataPair itemFileData;
		pProjectItem->GetLatestFileData(itemFileData);
		clipboardObj.insert("metaObj", itemFileData.m_Meta);
		clipboardObj.insert("dataObj", itemFileData.m_Data);

		// IMAGE INFO
		QJsonArray imagesArray = GetAssetsArray(ITEM_AtlasImage, pProjectItem);
		clipboardObj.insert("images", imagesArray);

		// SOUND INFO
		QJsonArray soundsArray = GetAssetsArray(ITEM_Audio, pProjectItem);
		clipboardObj.insert("sounds", soundsArray);

		// FONT INFO
		QStringList fontUrlList = pProjectItem->GetModel()->GetFontUrls();
		QJsonArray fontUrlArray;
		for(int i = 0; i < fontUrlList.size(); ++i)
			fontUrlArray.append(fontUrlList[i]);
		clipboardObj.insert("fonts", fontUrlArray);

		clipboardArray.append(clipboardObj);
	}

	// Serialize the item info into json source
	m_Data = JsonValueToSrc(QJsonValue(clipboardArray));
	setData(HYGUI_MIMETYPE_ITEM, m_Data);
}

ProjectItemMimeData::ProjectItemMimeData(const QVariant &data) :
	m_Data(data.value<QByteArray>())
{
	setData(HYGUI_MIMETYPE_ITEM, m_Data);
}

/*virtual*/ ProjectItemMimeData::~ProjectItemMimeData()
{ }

/*virtual*/ bool ProjectItemMimeData::hasFormat(const QString &sMimeType) const /*override*/
{
	if(HYGUI_MIMETYPE_ITEM == sMimeType.toLower() || "application/json" == sMimeType.toLower())
		return true;

	return false;
}

/*virtual*/ QStringList ProjectItemMimeData::formats() const /*override*/
{
	QStringList sFormatList;
	sFormatList << HYGUI_MIMETYPE_ITEM;
	sFormatList << "application/json";

	return sFormatList;
}

/*virtual*/ QVariant ProjectItemMimeData::retrieveData(const QString &mimeType, QVariant::Type type) const /*override*/
{
	if((mimeType.toLower() == HYGUI_MIMETYPE_ITEM || mimeType.toLower() == "application/json") &&
		(type == QVariant::UserType || type == QVariant::ByteArray || type == QVariant::String))
	{
		QByteArray dataSrc = QByteArray(m_Data);
		return QVariant(dataSrc);
	}

	return QVariant();
}

QJsonArray ProjectItemMimeData::GetAssetsArray(HyGuiItemType eManagerType, ProjectItemData *pProjectItem)
{
	QList<AssetItemData *> assetList = pProjectItem->GetModel()->GetAssets(eManagerType);
	QJsonArray assetArray;
	for(int i = 0; i < assetList.size(); ++i)
	{
		QJsonObject assetObj;
		assetObj.insert("assetUUID", assetList[i]->GetUuid().toString(QUuid::WithoutBraces));
		assetObj.insert("checksum", QJsonValue(static_cast<qint64>(assetList[i]->GetChecksum())));
		assetObj.insert("filter", assetList[i]->GetFilter());
		assetObj.insert("name", QJsonValue(assetList[i]->GetName()));
		assetObj.insert("uri", QJsonValue(pProjectItem->GetProject().GetMetaDataAbsPath() % HyGlobal::ItemName(eManagerType, true) % "/" % assetList[i]->ConstructMetaFileName()));
		assetArray.append(assetObj);
	}

	return assetArray;
}
