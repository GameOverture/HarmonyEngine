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

AssetMimeData::AssetMimeData(Project &projRef, HyGuiItemType eManagerType, QList<AssetItemData *> &assetListRef)
{
	QJsonArray clipboardArray;
	for(int i = 0; i < assetListRef.size(); ++i)
	{
		QJsonObject assetObj;
		assetObj.insert("project", projRef.GetAbsPath().toLower());
		assetObj.insert("assetUUID", assetListRef[i]->GetUuid().toString(QUuid::WithoutBraces));
		assetObj.insert("checksum", QJsonValue(static_cast<qint64>(assetListRef[i]->GetChecksum())));
		assetObj.insert("filter", assetListRef[i]->GetFilter());
		assetObj.insert("name", QJsonValue(assetListRef[i]->GetName()));
		assetObj.insert("uri", QJsonValue(projRef.GetMetaDataAbsPath() % HyGlobal::ItemName(eManagerType, true) % "/" % assetListRef[i]->ConstructMetaFileName()));
		clipboardArray.append(assetObj);
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
