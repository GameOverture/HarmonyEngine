/**************************************************************************
 *	AtlasModel.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AtlasModel.h"
#include "Harmony.h"
#include "Project.h"
#include "AtlasWidget.h"
#include "AtlasRepackThread.h"
#include "MainWindow.h"
#include "DlgAtlasGroupSettings.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void AtlasModel::FrameLookup::AddLookup(AtlasFrame *pFrame)
//{
//	m_FrameIdMap[pFrame->GetId()] = pFrame;
//
//	uint32 uiChecksum = pFrame->GetImageChecksum();
//	
//	if(m_FrameChecksumMap.contains(uiChecksum))
//	{
//		m_FrameChecksumMap.find(uiChecksum).value().append(pFrame);
//		HyGuiLog("'" % pFrame->GetName() % "' is a duplicate of '" % m_FrameChecksumMap.find(uiChecksum).value()[0]->GetName() % "' with the checksum: " % QString::number(uiChecksum) % " totaling: " % QString::number(m_FrameChecksumMap.find(uiChecksum).value().size()), LOGTYPE_Info);
//	}
//	else
//	{
//		QList<AtlasFrame *> newFrameList;
//		newFrameList.append(pFrame);
//		m_FrameChecksumMap[uiChecksum] = newFrameList;
//	}
//}
//bool AtlasModel::FrameLookup::RemoveLookup(AtlasFrame *pFrame)  // Returns true if no remaining duplicates exist
//{
//	m_FrameIdMap.remove(pFrame->GetId());
//
//	auto iter = m_FrameChecksumMap.find(pFrame->GetImageChecksum());
//	if(iter == m_FrameChecksumMap.end())
//		HyGuiLog("AtlasModel::RemoveLookup could not find frame", LOGTYPE_Error);
//	
//	iter.value().removeOne(pFrame);
//	if(iter.value().size() == 0)
//	{
//		m_FrameChecksumMap.remove(pFrame->GetImageChecksum());
//		return true;
//	}
//	
//	return false;
//}
//AtlasFrame *AtlasModel::FrameLookup::FindById(QUuid uuid)
//{
//	auto iter = m_FrameIdMap.find(uuid);
//	if(iter == m_FrameIdMap.end())
//		return nullptr;
//	else
//		return iter.value();
//}
//QList<AtlasFrame *> AtlasModel::FrameLookup::FindByChecksum(quint32 uiChecksum)
//{
//	auto iter = m_FrameChecksumMap.find(uiChecksum);
//	if(iter == m_FrameChecksumMap.end())
//		return QList<AtlasFrame *>();
//	else
//		return iter.value();
//}
//bool AtlasModel::FrameLookup::DoesImageExist(quint32 uiChecksum)
//{
//	return m_FrameChecksumMap.contains(uiChecksum);
//}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AtlasModel::AtlasModel(Project &projRef) :
	IManagerModel(projRef, ITEM_AtlasImage)
{

}

/*virtual*/ AtlasModel::~AtlasModel()
{

}

//void AtlasModel::StashTreeWidgets(QList<AtlasTreeItem *> treeItemList)
//{
//	m_TopLevelTreeItemList = treeItemList;
//}
//
//QList<AtlasTreeItem *> AtlasModel::GetTopLevelTreeItemList()
//{
//	return m_TopLevelTreeItemList;
//}

int AtlasModel::GetNumTextures(uint uiBankIndex)
{
	return m_AtlasGrpList[uiBankIndex]->GetExistingTextureInfoList().size();
}

QSize AtlasModel::GetAtlasDimensions(uint uiBankIndex)
{
	int iWidth = m_BanksModel.GetBank(uiBankIndex)->m_Settings["sbTextureWidth"].toInt();
	int iHeight = m_BanksModel.GetBank(uiBankIndex)->m_Settings["sbTextureHeight"].toInt();
	
	return QSize(iWidth, iHeight);
}

HyTextureFormat AtlasModel::GetAtlasTextureType(uint uiBankIndex)
{
	return static_cast<HyTextureFormat>(m_BanksModel.GetBank(uiBankIndex)->m_Settings["textureType"].toInt());
}

bool AtlasModel::IsImageValid(QImage &image, quint32 uiAtlasGrpId)
{
	return IsImageValid(image.width(), image.height(), uiAtlasGrpId);
}

bool AtlasModel::IsImageValid(int iWidth, int iHeight, quint32 uiAtlasGrpId)
{
	int uiAtlasGrpIndex = GetAtlasGrpIndexFromAtlasGrpId(uiAtlasGrpId);
	return IsImageValid(iWidth, iHeight, m_AtlasGrpList[uiAtlasGrpIndex]->m_PackerSettings);
}

bool AtlasModel::IsImageValid(int iWidth, int iHeight, const QJsonObject &atlasSettings)
{
	int iMarginWidth =  atlasSettings["sbFrameMarginLeft"].toInt();
	iMarginWidth +=     atlasSettings["sbFrameMarginRight"].toInt();
	int iMarginHeight = atlasSettings["sbFrameMarginBottom"].toInt();
	iMarginHeight +=    atlasSettings["sbFrameMarginTop"].toInt();

	QSize atlasMargins(iMarginWidth, iMarginHeight);
	QSize atlasDimensions(atlasSettings["sbTextureWidth"].toInt(), atlasSettings["sbTextureHeight"].toInt());

	if(iWidth >= (atlasDimensions.width() - atlasMargins.width()) ||
		iHeight >= (atlasDimensions.height() - atlasMargins.height()))
	{
		return false;
	}

	return true;
}

void AtlasModel::WriteMetaSettings()
{
	// Assemble array with all the frames from every group
	QJsonArray frameArray;
	QJsonArray groupsArray;
	for(int i = 0; i < m_AtlasGrpList.size(); ++i)
	{
		groupsArray.append(m_AtlasGrpList[i]->m_PackerSettings);

		QList<AtlasFrame *> &atlasFramesRef = m_AtlasGrpList[i]->m_FrameList;
		for(int j = 0; j < atlasFramesRef.size(); ++j)
		{
			QJsonObject frameObj;
			atlasFramesRef[j]->GetJsonObj(frameObj);
			frameArray.append(QJsonValue(frameObj));
		}
	}

	QJsonArray filtersArray;
	m_ExpandedFiltersArray = QJsonArray();
	if(m_pProjOwner->GetAtlasWidget())
	{
		QTreeWidgetItemIterator iter(m_pProjOwner->GetAtlasWidget()->GetFramesTreeWidget());
		while(*iter)
		{
			if((*iter)->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter)
			{
				QTreeWidgetItem *pTreeWidgetItem = *iter;
				QString sFilterName = pTreeWidgetItem->text(0);
				bool bExpanded = pTreeWidgetItem->isExpanded();

				QString sFilterPath = HyGlobal::GetTreeWidgetItemPath(*iter);
				filtersArray.append(QJsonValue(sFilterPath));
				m_ExpandedFiltersArray.append(bExpanded);
			}

			++iter;
		}
	}
	else
	{
		QFile settingsFile(m_MetaDir.absoluteFilePath(HYMETA_AtlasFile));
		if(settingsFile.exists())
		{
			if(!settingsFile.open(QIODevice::ReadOnly))
				HyGuiLog(QString("WidgetAtlasGroup::WidgetAtlasGroup() could not open ") % HYMETA_AtlasFile, LOGTYPE_Error);

#ifdef HYGUI_UseBinaryMetaFiles
			QJsonDocument settingsDoc = QJsonDocument::fromBinaryData(settingsFile.readAll());
#else
			QJsonDocument settingsDoc = QJsonDocument::fromJson(settingsFile.readAll());
#endif
			settingsFile.close();

			QJsonObject settingsObj = settingsDoc.object();
			filtersArray = settingsObj["filters"].toArray();
			m_ExpandedFiltersArray = settingsObj["expanded"].toArray();
		}
	}

	// Assemble the official QJsonObject for the write
	QJsonObject settingsObj;
	settingsObj.insert("$fileVersion", HYGUI_FILE_VERSION);
	settingsObj.insert("expanded", m_ExpandedFiltersArray);
	settingsObj.insert("filters", filtersArray);
	settingsObj.insert("frames", frameArray);
	settingsObj.insert("groups", groupsArray);
	settingsObj.insert("startAtlasId", QJsonValue(static_cast<qint64>(m_uiNextAtlasId)));

	QFile settingsFile(m_MetaDir.absoluteFilePath(HYMETA_AtlasFile));
	if(!settingsFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
	   HyGuiLog("Couldn't open atlas settings file for writing", LOGTYPE_Error);
	}
	else
	{
		QJsonDocument settingsDoc(settingsObj);

#ifdef HYGUI_UseBinaryMetaFiles
		qint64 iBytesWritten = settingsFile.write(settingsDoc.toBinaryData());
#else
		qint64 iBytesWritten = settingsFile.write(settingsDoc.toJson());
#endif
		if(0 == iBytesWritten || -1 == iBytesWritten) {
			HyGuiLog("Could not write to atlas settings file: " % settingsFile.errorString(), LOGTYPE_Error);
		}

		settingsFile.close();
	}
}

//AtlasFrame *AtlasModel::CreateFrame(QUuid uuid, quint32 uiChecksum, quint32 uiAtlasGrpId, QString sN, QRect rAlphaCrop, AtlasItemType eFrameType, int iW, int iH, int iX, int iY, int iTextureIndex, uint uiErrors)
//{
//
//}


AtlasFrame *AtlasModel::GenerateFrame(ProjectItemData *pItem, QString sName, QImage &newImage, quint32 uiAtlasGrpIndex, HyGuiItemType eType)
{
	if(IsImageValid(newImage, m_AtlasGrpList[uiAtlasGrpIndex]->GetId()) == false)
		return nullptr;

	// This will also create a meta image
	AtlasFrame *pFrame = ImportImage(sName, newImage, m_AtlasGrpList[uiAtlasGrpIndex]->GetId(), eType, nullptr);

	QSet<AtlasFrame *> newFrameSet;
	newFrameSet.insert(pFrame);
	Repack(uiAtlasGrpIndex, QSet<int>(), newFrameSet);

	// This retrieves the newly created AtlasFrame and links it to its ProjectItemData
	QList<QUuid> idList;
	idList.append(pFrame->GetId());
	QList<AtlasFrame *> returnList = RequestFramesById(pItem, idList);

	if(returnList.empty() == false)
		return returnList[0];

	return nullptr;
}

bool AtlasModel::ReplaceFrame(AtlasFrame *pFrame, QString sName, QImage &newImage, bool bDoAtlasGroupRepack)
{
	if(IsImageValid(newImage, pFrame->GetAtlasGrpId()) == false)
		return false;

	QSet<int> textureIndexToReplaceSet;
	if(pFrame->GetTextureIndex() >= 0)
		textureIndexToReplaceSet.insert(pFrame->GetTextureIndex());

	// First remove the frame from the map
	if(m_FrameLookup.RemoveLookup(pFrame))
		pFrame->DeleteMetaImage(m_MetaDir);

	// Determine the new checksum into the map
	quint32 uiChecksum = HyGlobal::CRCData(0, newImage.bits(), newImage.byteCount());
	pFrame->ReplaceImage(sName, uiChecksum, newImage, m_MetaDir);

	// Re-enter the frame into the map
	m_FrameLookup.AddLookup(pFrame);

	if(bDoAtlasGroupRepack)
	{
		uint uiAtlasGrpIndex = GetAtlasGrpIndexFromAtlasGrpId(pFrame->GetAtlasGrpId());
		Repack(uiAtlasGrpIndex, textureIndexToReplaceSet, QSet<AtlasFrame *>());
	}

	return true;
}

QList<AtlasFrame *> AtlasModel::RequestFrames(ProjectItemData *pItem)
{
	QList<QTreeWidgetItem *> selectedItems;
	if(m_pProjOwner->GetAtlasWidget())
	{
		selectedItems = m_pProjOwner->GetAtlasWidget()->GetFramesTreeWidget()->selectedItems();
		qSort(selectedItems.begin(), selectedItems.end(), SortTreeWidgetsPredicate());

		m_pProjOwner->GetAtlasWidget()->GetFramesTreeWidget()->clearSelection();
	}

	QList<AtlasFrame *> frameRequestList;
	for(int i = 0; i < selectedItems.size(); ++i)
	{
		AtlasFrame *pFrame = selectedItems[i]->data(0, Qt::UserRole).value<AtlasFrame *>();
		if(pFrame == nullptr)
			continue;

		frameRequestList.append(pFrame);
	}

	if(frameRequestList.empty())
		return QList<AtlasFrame *>();

	return RequestFrames(pItem, frameRequestList);
}

QList<AtlasFrame *> AtlasModel::RequestFramesById(ProjectItemData *pItem, QList<QUuid> requestList)
{
	if(requestList.empty())
		return QList<AtlasFrame *>();

	QList<AtlasFrame *> frameRequestList;
	for(int i = 0; i < requestList.size(); ++i)
	{
		AtlasFrame *pFoundFrame = m_FrameLookup.FindById(requestList[i]);
		if(pFoundFrame == nullptr)
		{
			// TODO: Support a "Yes to all" dialog functionality here. Also note that the request list will not == the return list
			HyGuiLog("Cannot find image with UUID: " % requestList[i].toString() % "\nIt may have been removed, or is invalid in the Atlas Manager.", LOGTYPE_Warning);
		}
		else
		{
			frameRequestList.append(pFoundFrame);
		}
	}

	return RequestFrames(pItem, frameRequestList);
}

QList<AtlasFrame *> AtlasModel::RequestFrames(ProjectItemData *pItem, QList<AtlasFrame *> requestList)
{
	if(requestList.empty())
		return RequestFrames(pItem);

	QList<AtlasFrame *> returnList;
	for(int i = 0; i < requestList.size(); ++i)
	{
		requestList[i]->m_DependencySet.insert(pItem);
		returnList.append(requestList[i]);
	}

	return returnList;
}

void AtlasModel::RelinquishFrames(ProjectItemData *pItem, QList<AtlasFrame *> relinquishList)
{
	for(int i = 0; i < relinquishList.size(); ++i)
		relinquishList[i]->m_DependencySet.remove(pItem);
}

QSet<AtlasFrame *> AtlasModel::ImportImages(QStringList sImportImgList, quint32 uiAtlasGrpId, HyGuiItemType eType, QList<AtlasTreeItem *> correspondingParentList)
{
	if(correspondingParentList.size() != sImportImgList.size())
	{
		HyGuiLog("AtlasModel::ImportImages was passed a correspondingParentList that isn't the same size as the sImportImgList", LOGTYPE_Error);
		return QSet<AtlasFrame *>(); // indicates error
	}

	QList<QImage *> newImageList;
	for(int i = 0; i < sImportImgList.size(); ++i)
	{
		QFileInfo fileInfo(sImportImgList[i]);

		QImage *pNewImage = new QImage(fileInfo.absoluteFilePath());
		newImageList.push_back(pNewImage);

		QSize atlasDimensions = GetAtlasDimensions(GetAtlasGrpIndexFromAtlasGrpId(uiAtlasGrpId));
		if(IsImageValid(*pNewImage, uiAtlasGrpId) == false)
		{
			HyGuiLog("Importing image " % fileInfo.fileName() % " will not fit in atlas group '" % QString::number(uiAtlasGrpId) % "' (" % QString::number(atlasDimensions.width()) % "x" % QString::number(atlasDimensions.height()) % ")", LOGTYPE_Warning);

			for(int j = 0; j < newImageList.size(); ++j)
				delete newImageList[j];

			return QSet<AtlasFrame *>(); // indicates error
		}
	}

	QSet<AtlasFrame *> returnSet;
	for(int i = 0; i < newImageList.size(); ++i)
		returnSet.insert(ImportImage(QFileInfo(sImportImgList[i]).baseName(), *newImageList[i], uiAtlasGrpId, eType, correspondingParentList[i]));

	for(int j = 0; j < newImageList.size(); ++j)
		delete newImageList[j];

	return returnSet;
}

AtlasTreeItem *AtlasModel::CreateFilter(QString sName, AtlasTreeItem *pParent)
{
	AtlasTreeItem *pNewTreeItem = nullptr;
	if(pParent == nullptr)
		pNewTreeItem = new AtlasTreeItem(m_pProjOwner->GetAtlasWidget()->GetFramesTreeWidget());
	else
		pNewTreeItem = new AtlasTreeItem(pParent);

	pNewTreeItem->setText(0, sName);
	pNewTreeItem->setIcon(0, HyGlobal::ItemIcon(ITEM_Filter, SUBICON_None));
	pNewTreeItem->setData(0, Qt::UserRole, QVariant(QString(HYTREEWIDGETITEM_IsFilter)));

	WriteMetaSettings();

	return pNewTreeItem;
}

bool AtlasModel::DoesImageExist(quint32 uiChecksum)
{
	return m_FrameLookup.DoesImageExist(uiChecksum);
}

void AtlasModel::SaveData()
{
	QJsonArray atlasGrpArray;
	for(int i = 0; i < m_AtlasGrpList.size(); ++i)
	{
		QJsonObject atlasGrpObj;
		atlasGrpObj.insert("width", m_AtlasGrpList[i]->m_PackerSettings["sbTextureWidth"].toInt());
		atlasGrpObj.insert("height", m_AtlasGrpList[i]->m_PackerSettings["sbTextureHeight"].toInt());
		atlasGrpObj.insert("atlasGrpId", m_AtlasGrpList[i]->m_PackerSettings["atlasGrpId"].toInt());
		atlasGrpObj.insert("textureType", m_AtlasGrpList[i]->m_PackerSettings["textureType"].toInt());
		
		QJsonArray textureArray;
		QList<QJsonArray> frameArrayList;
		QList<AtlasFrame *> &atlasGrpFrameListRef = m_AtlasGrpList[i]->m_FrameList;
		for(int i = 0; i < atlasGrpFrameListRef.size(); ++i)
		{
			if(atlasGrpFrameListRef[i]->GetTextureIndex() < 0)
				continue;
	
			while(frameArrayList.empty() || frameArrayList.size() <= atlasGrpFrameListRef[i]->GetTextureIndex())
				frameArrayList.append(QJsonArray());
	
			QJsonObject frameObj;
			frameObj.insert("checksum", QJsonValue(static_cast<qint64>(atlasGrpFrameListRef[i]->GetImageChecksum())));
			frameObj.insert("left", QJsonValue(atlasGrpFrameListRef[i]->GetX()));
			frameObj.insert("top", QJsonValue(atlasGrpFrameListRef[i]->GetY()));
			frameObj.insert("right", QJsonValue(atlasGrpFrameListRef[i]->GetX() + atlasGrpFrameListRef[i]->GetCrop().width()));
			frameObj.insert("bottom", QJsonValue(atlasGrpFrameListRef[i]->GetY() + atlasGrpFrameListRef[i]->GetCrop().height()));
	
			frameArrayList[atlasGrpFrameListRef[i]->GetTextureIndex()].append(frameObj);
		}
	
		for(int i = 0; i < frameArrayList.size(); ++i)
			textureArray.append(frameArrayList[i]);
	
		atlasGrpObj.insert("textures", textureArray);

		atlasGrpArray.append(atlasGrpObj);
	}

	QJsonObject atlasInfoObj;
	atlasInfoObj.insert("$fileVersion", HYGUI_FILE_VERSION);
	atlasInfoObj.insert("atlasGroups", atlasGrpArray);
	
	QJsonDocument atlasInfoDoc;
	atlasInfoDoc.setObject(atlasInfoObj);

	QFile atlasInfoFile(m_RootDataDir.absoluteFilePath(HYASSETS_AtlasFile));
	if(atlasInfoFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
		HyGuiLog("Couldn't open atlas data info file for writing", LOGTYPE_Error);
	else
	{
		qint64 iBytesWritten = atlasInfoFile.write(atlasInfoDoc.toJson());
		if(0 == iBytesWritten || -1 == iBytesWritten)
			HyGuiLog("Could not write to atlas settings file: " % atlasInfoFile.errorString(), LOGTYPE_Error);

		atlasInfoFile.close();
	}
}

uint AtlasModel::CreateNewAtlasGrp(QString sName)
{
	m_RootDataDir.mkdir(HyGlobal::MakeFileNameFromCounter(m_uiNextAtlasId));
	
	AtlasGrp *pNewAtlasGrp = new AtlasGrp(m_RootDataDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(m_uiNextAtlasId)));
	pNewAtlasGrp->m_PackerSettings = DlgAtlasGroupSettings::GenerateDefaultSettingsObj();
	pNewAtlasGrp->m_PackerSettings.insert("txtName", sName);
	pNewAtlasGrp->m_PackerSettings.insert("atlasGrpId", QJsonValue(static_cast<qint64>(m_uiNextAtlasId)));
	
	beginInsertRows(QModelIndex(), m_AtlasGrpList.count(), m_AtlasGrpList.count());
	m_AtlasGrpList.push_back(pNewAtlasGrp);
	endInsertRows();
	
	m_uiNextAtlasId++;
	WriteMetaSettings();

	return static_cast<uint>(m_AtlasGrpList.size() - 1);
}

void AtlasModel::RemoveAtlasGrp(quint32 uiAtlasGrpId)
{
	if(uiAtlasGrpId == 0) {
		HyGuiLog("AtlasModel::RemoveAtlasGrp is trying to remove atlas group id: 0", LOGTYPE_Error);
	}
	
	for(int i = 0; i < m_AtlasGrpList.size(); ++i)
	{
		if(m_AtlasGrpList[i]->GetId() == uiAtlasGrpId)
		{
			if(m_AtlasGrpList[i]->m_FrameList.empty())
			{
				AtlasGrp *pAtlasGrpToBeRemoved = m_AtlasGrpList[i];
				
				beginRemoveRows(QModelIndex(), i, i);
				m_AtlasGrpList.removeAt(i);
				endRemoveRows();
				
				delete pAtlasGrpToBeRemoved;
				m_RootDataDir.rmdir(HyGlobal::MakeFileNameFromCounter(uiAtlasGrpId));
				
				WriteMetaSettings();
			}
			else {
				HyGuiLog("Cannot remove atlas group since it has " % QString::number(m_AtlasGrpList[i]->m_FrameList.size()) % " image(s) that are still linked to this group.", LOGTYPE_Info);
			}
			
			return;
		}
	}
	
	HyGuiLog("AtlasModel::RemoveAtlasGrp could not find atlas group ID: " % QString::number(uiAtlasGrpId), LOGTYPE_Error);
}

uint AtlasModel::GetAtlasGrpIndexFromAtlasGrpId(quint32 uiAtlasGrpId)
{
	uint uiAtlasGrpIndex = 0xFFFFFFFF;
	for(int i = 0; i < m_AtlasGrpList.size(); ++i)
	{
		if(m_AtlasGrpList[i]->GetId() == uiAtlasGrpId)
		{
			uiAtlasGrpIndex = i;
			break;
		}
	}
	if(0xFFFFFFFF == uiAtlasGrpIndex) {
		HyGuiLog("AtlasModel::Repack could not find atlas group index from ID: " % QString::number(uiAtlasGrpId), LOGTYPE_Error);
	}
	
	return uiAtlasGrpIndex;
}

quint32 AtlasModel::GetAtlasGrpIdFromAtlasGrpIndex(uint uiAtlasGrpIndex)
{
	return m_AtlasGrpList[uiAtlasGrpIndex]->GetId();
}

void AtlasModel::RepackAll(uint uiAtlasGrpIndex)
{
	int iNumTotalTextures = m_AtlasGrpList[uiAtlasGrpIndex]->GetExistingTextureInfoList().size();
	
	QSet<int> textureIndexSet;
	for(int i = 0; i < iNumTotalTextures; ++i)
		textureIndexSet.insert(i);

	if(textureIndexSet.empty() == false)
		Repack(uiAtlasGrpIndex, textureIndexSet, QSet<AtlasFrame *>());
	else
		SaveAndReloadHarmony();
}

void AtlasModel::Repack(uint uiAtlasGrpIndex, QSet<int> repackTexIndicesSet, QSet<AtlasFrame *> newFramesSet)
{
	// Always repack the last texture to ensure it gets filled as much as it can
	QFileInfoList existingTexturesInfoList = m_AtlasGrpList[uiAtlasGrpIndex]->GetExistingTextureInfoList();
	for(int i = HyClamp(existingTexturesInfoList.size() - 1, 0, existingTexturesInfoList.size()); i < existingTexturesInfoList.size(); ++i)
		repackTexIndicesSet.insert(i);

	QList<int> textureIndexList = repackTexIndicesSet.toList();

	// Get all the affected frames into a list
	QList<AtlasFrame *> &atlasGrpFrameListRef = m_AtlasGrpList[uiAtlasGrpIndex]->m_FrameList;
	for(int i = 0; i < atlasGrpFrameListRef.size(); ++i)
	{
		for(int j = 0; j < textureIndexList.size(); ++j)
		{
			if(atlasGrpFrameListRef[i]->GetTextureIndex() == textureIndexList[j])
				newFramesSet.insert(atlasGrpFrameListRef[i]);
		}
	}

	QList<AtlasFrame *>newFramesList = newFramesSet.toList();


	AtlasRepackThread *pWorkerThread = new AtlasRepackThread(m_AtlasGrpList[uiAtlasGrpIndex], textureIndexList, newFramesList, m_MetaDir);
	connect(pWorkerThread, &AtlasRepackThread::finished, pWorkerThread, &QObject::deleteLater);
	connect(pWorkerThread, &AtlasRepackThread::LoadUpdate, this, &AtlasModel::OnLoadUpdate);
	connect(pWorkerThread, &AtlasRepackThread::RepackIsFinished, this, &AtlasModel::OnRepackFinished);

	MainWindow::SetLoading("Repacking Atlases", 0);
	pWorkerThread->start();
}

/*virtual*/ int AtlasModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return m_AtlasGrpList.size();
}

/*virtual*/ QVariant AtlasModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
	if (role == Qt::TextAlignmentRole)
		return Qt::AlignLeft;

	if(role == Qt::DisplayRole || role == Qt::EditRole)
		return "Id: " % QString::number(m_AtlasGrpList[index.row()]->GetId()) % " - " % m_AtlasGrpList[index.row()]->m_PackerSettings["txtName"].toString();

	return QVariant();
}

/*virtual*/ bool AtlasModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
	return QAbstractItemModel::setData(index, value, role);
}

/*virtual*/ QVariant AtlasModel::headerData(int iIndex, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
	return QVariant();
}

/*virtual*/ AssetItemData *AtlasModel::OnAllocateAssetData(QJsonObject metaObj) /*override*/
{
	QRect rAlphaCrop(QPoint(metaObj["cropLeft"].toInt(), metaObj["cropTop"].toInt()),
					 QPoint(metaObj["cropRight"].toInt(), metaObj["cropBottom"].toInt()));


	AtlasFrame *pNewFrame = new AtlasFrame(QUuid(metaObj["frameUUID"].toString()),
										   JSONOBJ_TOINT(metaObj, "checksum"),
										   JSONOBJ_TOINT(metaObj, "atlasGrpId"),
										   metaObj["name"].toString(),
										   rAlphaCrop,
										   static_cast<AtlasItemType>(metaObj["type"].toInt()),
										   metaObj["width"].toInt(),
										   metaObj["height"].toInt(),
										   metaObj["x"].toInt(),
										   metaObj["y"].toInt(),
										   metaObj["textureIndex"].toInt(),
										   metaObj["errors"].toInt(0));

	return pNewFrame;
}

void AtlasModel::SaveAndReloadHarmony()
{
	WriteMetaSettings();

	if(GetProjOwner() && GetProjOwner()->GetAtlasWidget())
		GetProjOwner()->GetAtlasWidget()->RefreshInfo();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// REGENERATE THE ATLAS DATA INFO FILE (HARMONY EXPORT)
	SaveData();

	// Reload Harmony
	Harmony::Reload(m_pProjOwner);

	HyGuiLog("Atlas Refresh finished", LOGTYPE_Normal);
}

/*slot*/ void AtlasModel::OnLoadUpdate(QString sMsg, int iPercComplete)
{
	MainWindow::SetLoading(sMsg, iPercComplete);
}

/*slot*/ void AtlasModel::OnRepackFinished()
{
	SaveAndReloadHarmony();
}

AtlasFrame *AtlasModel::ImportImage(QString sName, QImage &newImage, quint32 uiAtlasGrpId, HyGuiItemType eType, AtlasTreeItem *pParent)
{
	quint32 uiChecksum = HyGlobal::CRCData(0, newImage.bits(), newImage.byteCount());

	AtlasItemType eAtlasItemType = HyGlobal::GetAtlasItemFromItem(eType);

	QRect rAlphaCrop(0, 0, newImage.width(), newImage.height());
	if(eAtlasItemType == ATLASITEM_Image) // 'sub-atlases' should not be cropping their alpha because they rely on their own UV coordinates
		rAlphaCrop = ImagePacker::crop(newImage);

	AtlasFrame *pNewFrame = CreateFrame(QUuid::createUuid(), uiChecksum, uiAtlasGrpId, sName, rAlphaCrop, eAtlasItemType, newImage.width(), newImage.height(), -1, -1, -1, 0);
	if(pNewFrame)
	{
		newImage.save(m_MetaDir.absoluteFilePath(pNewFrame->ConstructImageFileName()));

		if(pParent == nullptr)
		{
			if(m_pProjOwner->GetAtlasWidget())
				m_pProjOwner->GetAtlasWidget()->GetFramesTreeWidget()->addTopLevelItem(pNewFrame->GetTreeItem());
		}
		else
		{
			if(pParent->data(0, Qt::UserRole).toString() != HYTREEWIDGETITEM_IsFilter)
				HyGuiLog("AtlasModel::ImportImage was passed parent that wasn't a filter", LOGTYPE_Error);

			pParent->addChild(pNewFrame->GetTreeItem());
		}
	}

	return pNewFrame;
}
