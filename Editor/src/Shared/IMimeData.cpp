/**************************************************************************
 *	IMimeData.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2022 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "IMimeData.h"
#include "IAssetItemData.h"
#include "IManagerModel.h"
#include "Project.h"

IMimeData::IMimeData(MimeType eMimeType) :
	m_eMIME_TYPE(eMimeType)
{
}

/*virtual*/ IMimeData::~IMimeData()
{
}

MimeType IMimeData::GetMimeType() const
{
	return m_eMIME_TYPE;
}

/*virtual*/ bool IMimeData::hasFormat(const QString &sMimeType) const /*override*/
{
	return ("application/json" == sMimeType.toLower() || HyGlobal::MimeTypeString(m_eMIME_TYPE) == sMimeType.toLower());
}

/*virtual*/ QStringList IMimeData::formats() const /*override*/
{
	QStringList sFormatList;
	sFormatList << HyGlobal::MimeTypeString(m_eMIME_TYPE);
	sFormatList << "application/json";

	return sFormatList;
}

/*virtual*/ QVariant IMimeData::retrieveData(const QString &sMimeType, QVariant::Type type) const /*override*/
{
	if(type != QVariant::UserType && type != QVariant::ByteArray && type != QVariant::String)
		return QVariant();

	if(hasFormat(sMimeType))
		return QVariant(m_Data);

	return QVariant();
}

QJsonArray IMimeData::MakeAssetJsonArray(Project &projRef, QList<TreeModelItemData *> assetList, AssetManagerType eAssetType)
{
	QJsonArray assetArray;
	for(int i = 0; i < assetList.size(); ++i)
	{
		if(assetList[i] == nullptr)
			continue;

		QJsonObject assetObj;
		if(assetList[i]->GetType() == ITEM_Filter)
		{
			IManagerModel *pManager = projRef.GetManagerModel(eAssetType);
			if(pManager == nullptr)
				continue;

			assetObj.insert("isFilter", true);
			assetObj.insert("project", projRef.GetAbsPath().toLower());
			assetObj.insert("filter", pManager->AssembleFilter(assetList[i], false));
			assetObj.insert("name", QJsonValue(assetList[i]->GetText()));
		}
		else
		{
			AssetItemData *pAssetItem = static_cast<AssetItemData *>(assetList[i]);

			assetObj.insert("isFilter", false);
			assetObj.insert("project", pAssetItem->GetProject().GetAbsPath().toLower());
			assetObj.insert("assetUUID", pAssetItem->GetUuid().toString(QUuid::WithoutBraces));
			assetObj.insert("checksum", QJsonValue(static_cast<qint64>(pAssetItem->GetChecksum())));
			assetObj.insert("filter", pAssetItem->GetFilter());
			assetObj.insert("name", QJsonValue(pAssetItem->GetName()));
			assetObj.insert("uri", QJsonValue(pAssetItem->GetAbsMetaFilePath()));
		}
		assetArray.append(assetObj);
	}

	return assetArray;
}
