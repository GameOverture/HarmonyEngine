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

AssetMimeData::AssetMimeData(Project &projRef, HyGuiItemType eManagerType, QList<TreeModelItemData *> &itemListRef)
{
	QJsonArray clipboardArray;

	for(int i = 0; i < itemListRef.size(); ++i)
	{
		QJsonObject itemObj;
		itemObj.insert("project", projRef.GetAbsPath().toLower());

		if(itemListRef[i]->GetType() == ITEM_Filter)
		{
			itemObj.insert("isFilter", true);

			IManagerModel *pManager = projRef.GetManagerModel(eManagerType);
			if(pManager == nullptr)
				continue;

			itemObj.insert("filter", pManager->AssembleFilter(itemListRef[i]));
			itemObj.insert("name", QJsonValue(itemListRef[i]->GetText()));
		}
		else
		{
			itemObj.insert("isFilter", false);

			AssetItemData *pAssetItem = static_cast<AssetItemData *>(itemListRef[i]);
			itemObj.insert("assetUUID", pAssetItem->GetUuid().toString(QUuid::WithoutBraces));
			itemObj.insert("checksum", QJsonValue(static_cast<qint64>(pAssetItem->GetChecksum())));
			itemObj.insert("filter", pAssetItem->GetFilter());
			itemObj.insert("name", QJsonValue(pAssetItem->GetName()));
			itemObj.insert("uri", QJsonValue(projRef.GetMetaDataAbsPath() % HyGlobal::ItemName(eManagerType, true) % "/" % pAssetItem->ConstructMetaFileName()));
		}

		clipboardArray.append(itemObj);
	}

	// Serialize the item info into json source
	m_Data = JsonValueToSrc(QJsonValue(clipboardArray));
	setData(HYGUI_MIMETYPE_ASSET, m_Data);
}

AssetMimeData::AssetMimeData(const QVariant &data) :
	m_Data(data.value<QByteArray>())
{
	setData(HYGUI_MIMETYPE_ASSET, m_Data);
}

/*virtual*/ AssetMimeData::~AssetMimeData()
{ }

/*virtual*/ bool AssetMimeData::hasFormat(const QString &sMimeType) const /*override*/
{
	if(HYGUI_MIMETYPE_ASSET == sMimeType.toLower() || "application/json" == sMimeType.toLower())
		return true;

	return false;
}

/*virtual*/ QStringList AssetMimeData::formats() const /*override*/
{
	QStringList sFormatList;
	sFormatList << HYGUI_MIMETYPE_ASSET;
	sFormatList << "application/json";

	return sFormatList;
}

/*virtual*/ QVariant AssetMimeData::retrieveData(const QString &mimeType, QVariant::Type type) const /*override*/
{
	if((mimeType.toLower() == HYGUI_MIMETYPE_ASSET || mimeType.toLower() == "application/json") &&
		(type == QVariant::UserType || type == QVariant::ByteArray || type == QVariant::String))
	{
		QByteArray dataSrc = QByteArray(m_Data);
		return QVariant(dataSrc);
	}

	return QVariant();
}
