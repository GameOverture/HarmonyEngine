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
#include "ProjectItem.h"
#include "Project.h"
#include "IModel.h"
#include "AtlasFrame.h"

ProjectItemMimeData::ProjectItemMimeData(QList<ExplorerItem *> &itemListRef)
{
	QJsonArray clipboardArray;

	for(int i = 0; i < itemListRef.size(); ++i)
	{
		if(itemListRef[i]->IsProjectItem() == false)
			continue;

		ProjectItem *pProjectItem = static_cast<ProjectItem *>(itemListRef[i]);

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
		QList<AtlasFrame *> atlasFrameList = pProjectItem->GetModel()->GetAtlasFrames();
		QJsonArray imagesArray;
		for(int i = 0; i < atlasFrameList.size(); ++i)
		{
			QJsonObject atlasFrameObj;
			atlasFrameObj.insert("checksum", QJsonValue(static_cast<qint64>(atlasFrameList[i]->GetImageChecksum())));
			atlasFrameObj.insert("name", QJsonValue(atlasFrameList[i]->GetName()));
			atlasFrameObj.insert("uri", QJsonValue(pProjectItem->GetProject().GetMetaDataAbsPath() % HyGlobal::ItemName(ITEM_AtlasImage, true) % "/" % atlasFrameList[i]->ConstructImageFileName()));
			imagesArray.append(atlasFrameObj);
		}
		clipboardObj.insert("images", imagesArray);

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
	setData(HYGUI_MIMETYPE, m_Data);
}

ProjectItemMimeData::ProjectItemMimeData(const QVariant &data) :
	m_Data(data.value<QByteArray>())
{
	setData(HYGUI_MIMETYPE, m_Data);
}

/*virtual*/ ProjectItemMimeData::~ProjectItemMimeData()
{ }

/*virtual*/ bool ProjectItemMimeData::hasFormat(const QString &sMimeType) const /*override*/
{
	if(HYGUI_MIMETYPE == sMimeType.toLower() || "application/json" == sMimeType.toLower())
		return true;

	return false;
}

/*virtual*/ QStringList ProjectItemMimeData::formats() const /*override*/
{
	QStringList sFormatList;
	sFormatList << HYGUI_MIMETYPE;
	sFormatList << "application/json";

	return sFormatList;
}

/*virtual*/ QVariant ProjectItemMimeData::retrieveData(const QString &mimeType, QVariant::Type type) const /*override*/
{
	if((mimeType.toLower() == HYGUI_MIMETYPE || mimeType.toLower() == "application/json") &&
		(type == QVariant::UserType || type == QVariant::ByteArray || type == QVariant::String))
	{
		QByteArray dataSrc = QByteArray(m_Data);
		return QVariant(dataSrc);
	}

	return QVariant();
}
