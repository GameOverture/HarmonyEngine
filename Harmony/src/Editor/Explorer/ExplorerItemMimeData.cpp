/**************************************************************************
*	ExplorerItemMimeData.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "ExplorerItemMimeData.h"
#include "ExplorerItem.h"
#include "Project.h"
#include "IModel.h"

ExplorerItemMimeData::ExplorerItemMimeData(ExplorerItem *pExplorerItem) :
	m_pExplorerItem(pExplorerItem)
{
	QJsonValue itemValue;

	if(m_pExplorerItem->IsProjectItem() == false)
	{
		// TODO:
		return;
	}

	ProjectItem *pProjectItem = static_cast<ProjectItem *>(m_pExplorerItem);
	pProjectItem->GetModel()->Refresh();
	itemValue = pProjectItem->GetModel()->GetJson();

	if(pExplorerItem->GetType() == ITEM_Sprite)
	{

	}

	// STANDARD INFO
	QJsonObject clipboardObj;
	clipboardObj.insert("project", m_pExplorerItem->GetProject().GetAbsPath());
	clipboardObj.insert("itemType", HyGlobal::ItemName(m_pExplorerItem->GetType(), false));
	clipboardObj.insert("itemName", m_pExplorerItem->GetName(true));
	clipboardObj.insert("src", itemValue);

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

	// TODO: AUDIO INFO
	//clipboardObj.insert("audio", GetAudioWavs())

	// Serialize the item info into json source
	m_Data = JsonValueToSrc(QJsonValue(clipboardObj));
	setData(HYGUI_MIMETYPE, m_Data);
}

/*virtual*/ ExplorerItemMimeData::~ExplorerItemMimeData()
{
}

HyGuiItemType ExplorerItemMimeData::GetType() const
{
	return m_pExplorerItem->GetType();
}

ExplorerItem *ExplorerItemMimeData::GetItem() const
{
	return m_pExplorerItem;
}

/*virtual*/ bool ExplorerItemMimeData::hasFormat(const QString &sMimeType) const /*override*/
{
	if(HYGUI_MIMETYPE == sMimeType.toLower() || "application/json" == sMimeType.toLower())
		return true;

	return false;
}

/*virtual*/ QStringList ExplorerItemMimeData::formats() const /*override*/
{
	QStringList sFormatList;
	sFormatList << HYGUI_MIMETYPE;
	sFormatList << "application/json";

	return sFormatList;
}

/*virtual*/ QVariant ExplorerItemMimeData::retrieveData(const QString &mimeType, QVariant::Type type) const /*override*/
{
	if((mimeType.toLower() == HYGUI_MIMETYPE || mimeType.toLower() == "application/json") &&
		(type == QVariant::UserType || type == QVariant::ByteArray || type == QVariant::String))
	{
		QByteArray dataSrc = QByteArray(m_Data);
		return QVariant(dataSrc);
	}

	return QVariant();
}
