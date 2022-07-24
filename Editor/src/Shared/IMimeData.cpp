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
#include "Project.h"

IMimeData::IMimeData(MimeType eMimeType) :
	m_eMIME_TYPE(eMimeType)
{
}

/*virtual*/ IMimeData::~IMimeData()
{
}

/*virtual*/ bool IMimeData::hasFormat(const QString &sMimeType) const /*override*/
{
	return ("application/json" == sMimeType.toLower() ||
		(m_eMIME_TYPE == MIMETYPE_ProjectItems && HYGUI_MIMETYPE_ITEM == sMimeType.toLower()) ||
		(m_eMIME_TYPE == MIMETYPE_Assets && HYGUI_MIMETYPE_ASSET == sMimeType.toLower()));
}

/*virtual*/ QStringList IMimeData::formats() const /*override*/
{
	QStringList sFormatList;
	if(m_eMIME_TYPE == MIMETYPE_ProjectItems)
		sFormatList << HYGUI_MIMETYPE_ITEM;
	else
		sFormatList << HYGUI_MIMETYPE_ASSET;
	sFormatList << "application/json";

	return sFormatList;
}

/*virtual*/ QVariant IMimeData::retrieveData(const QString &sMimeType, QVariant::Type type) const /*override*/
{
	if(type != QVariant::UserType && type != QVariant::ByteArray && type != QVariant::String)
		return QVariant();

	if(sMimeType.compare(HYGUI_MIMETYPE_ITEM, Qt::CaseInsensitive) == 0 ||
		sMimeType.compare(HYGUI_MIMETYPE_ASSET, Qt::CaseInsensitive) == 0 ||
		sMimeType.toLower() == "application/json")
	{
		return QVariant(m_Data);
	}

	return QVariant();
}

QJsonArray IMimeData::MakeAssetJsonArray(QList<AssetItemData *> assetList, AssetType eAssetType)
{
	QJsonArray assetArray;
	for(int i = 0; i < assetList.size(); ++i)
	{
		if(assetList[i] == nullptr)
			continue;

		QJsonObject assetObj;
		assetObj.insert("project", assetList[i]->GetProject().GetAbsPath().toLower());
		if(assetList[i]->GetType() == ITEM_Filter)
		{
			assetObj.insert("isFilter", true);
			assetObj.insert("filter", assetList[i]->GetFilter());
			assetObj.insert("name", QJsonValue(assetList[i]->GetText()));
		}
		else
		{
			assetObj.insert("isFilter", false);
			assetObj.insert("assetUUID", assetList[i]->GetUuid().toString(QUuid::WithoutBraces));
			assetObj.insert("checksum", QJsonValue(static_cast<qint64>(assetList[i]->GetChecksum())));
			assetObj.insert("filter", assetList[i]->GetFilter());
			assetObj.insert("name", QJsonValue(assetList[i]->GetName()));
			assetObj.insert("uri", QJsonValue(assetList[i]->GetAbsMetaFilePath()));
		}
		assetArray.append(assetObj);
	}

	return assetArray;
}
