/**************************************************************************
 *	AtlasModel.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "AtlasModel.h"

#include "Harmony.h"
#include "Project.h"
#include "AtlasWidget.h"
#include "AtlasRepackThread.h"
#include "MainWindow.h"
#include "Global.h"
#include "DlgAtlasGroupSettings.h"

#include "Harmony/HyEngine.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AtlasModel::FrameLookup::AddLookup(AtlasFrame *pFrame)
{
	m_FrameIdMap[pFrame->GetId()] = pFrame;

	uint32 uiChecksum = pFrame->GetImageChecksum();
	
	if(m_FrameChecksumMap.contains(uiChecksum))
	{
		m_FrameChecksumMap.find(uiChecksum).value().append(pFrame);
		HyGuiLog("'" % pFrame->GetName() % "' is a duplicate of '" % m_FrameChecksumMap.find(uiChecksum).value()[0]->GetName() % "' with the checksum: " % QString::number(uiChecksum) % " totaling: " % QString::number(m_FrameChecksumMap.find(uiChecksum).value().size()), LOGTYPE_Info);
	}
	else
	{
		QList<AtlasFrame *> newFrameList;
		newFrameList.append(pFrame);
		m_FrameChecksumMap[uiChecksum] = newFrameList;
	}
}
bool AtlasModel::FrameLookup::RemoveLookup(AtlasFrame *pFrame)  // Returns true if no remaining duplicates exist
{
	m_FrameIdMap.remove(pFrame->GetId());

	auto iter = m_FrameChecksumMap.find(pFrame->GetImageChecksum());
	if(iter == m_FrameChecksumMap.end())
		HyGuiLog("AtlasModel::RemoveLookup could not find frame", LOGTYPE_Error);
	
	iter.value().removeOne(pFrame);
	if(iter.value().size() == 0)
	{
		m_FrameChecksumMap.remove(pFrame->GetImageChecksum());
		return true;
	}
	
	return false;
}
AtlasFrame *AtlasModel::FrameLookup::FindById(quint32 uiId)
{
	auto iter = m_FrameIdMap.find(uiId);
	if(iter == m_FrameIdMap.end())
		return nullptr;
	else
		return iter.value();
}
QList<AtlasFrame *> AtlasModel::FrameLookup::FindByChecksum(quint32 uiChecksum)
{
	auto iter = m_FrameChecksumMap.find(uiChecksum);
	if(iter == m_FrameChecksumMap.end())
		return QList<AtlasFrame *>();
	else
		return iter.value();
}
bool AtlasModel::FrameLookup::DoesImageExist(quint32 uiChecksum)
{
	return m_FrameChecksumMap.contains(uiChecksum);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AtlasModel::AtlasModel(Project *pProjOwner) :	m_pProjOwner(pProjOwner),
												m_MetaDir(m_pProjOwner->GetMetaDataAbsPath() + HyGlobal::ItemName(ITEM_AtlasImage, true)),
												m_RootDataDir(m_pProjOwner->GetAssetsAbsPath() + HyGlobal::ItemName(ITEM_AtlasImage, true))
{
	if(m_MetaDir.exists() == false)
	{
		HyGuiLog("Meta atlas directory is missing, recreating", LOGTYPE_Info);
		m_MetaDir.mkpath(m_MetaDir.absolutePath());
	}
	if(m_RootDataDir.exists() == false)
	{
		HyGuiLog("Data atlas directory is missing, recreating", LOGTYPE_Info);
		m_RootDataDir.mkpath(m_RootDataDir.absolutePath());
	}

	QFile settingsFile(m_MetaDir.absoluteFilePath(HYGUIPATH_MetaSettings));
	if(settingsFile.exists())
	{
		if(!settingsFile.open(QIODevice::ReadOnly))
			HyGuiLog(QString("WidgetAtlasGroup::WidgetAtlasGroup() could not open ") % HYGUIPATH_MetaSettings, LOGTYPE_Error);

#ifdef HYGUI_UseBinaryMetaFiles
		QJsonDocument settingsDoc = QJsonDocument::fromBinaryData(settingsFile.readAll());
#else
		QJsonDocument settingsDoc = QJsonDocument::fromJson(settingsFile.readAll());
#endif
		settingsFile.close();

		QJsonObject settingsObj = settingsDoc.object();

		m_uiNextFrameId = JSONOBJ_TOINT(settingsObj, "startFrameId");
		m_uiNextAtlasId = JSONOBJ_TOINT(settingsObj, "startAtlasId");
		
		QJsonArray atlasGrpArray = settingsObj["groups"].toArray();
		for(int i = 0; i < atlasGrpArray.size(); ++i)
		{
			AtlasGrp *pNewAtlasGrp = new AtlasGrp(m_RootDataDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(atlasGrpArray[i].toObject()["atlasGrpId"].toInt())));
			pNewAtlasGrp->m_PackerSettings = atlasGrpArray[i].toObject();
			
			m_AtlasGrpList.push_back(pNewAtlasGrp);
			
			m_RootDataDir.mkdir(HyGlobal::MakeFileNameFromCounter(pNewAtlasGrp->GetId()));
		}
		//m_PackerSettings = settingsObj["settings"].toObject();

		// Create all the filter items first, storing their actual path in their data (for now)
		QJsonArray filtersArray = settingsObj["filters"].toArray();
		for(int i = 0; i < filtersArray.size(); ++i)
		{
			QDir filterPathDir(filtersArray.at(i).toString());

			AtlasTreeItem *pNewTreeItem = new AtlasTreeItem((QTreeWidgetItem *)nullptr, QTreeWidgetItem::Type);

			pNewTreeItem->setText(0, filterPathDir.dirName());
			pNewTreeItem->setIcon(0, HyGlobal::ItemIcon(ITEM_Filter, SUBICON_None));

			QVariant v(QString(filterPathDir.absolutePath()));
			pNewTreeItem->setData(0, Qt::UserRole, v);

			m_TopLevelTreeItemList.append(pNewTreeItem);
		}

		// Then place the filters correctly as a parent heirarchy using the path string stored in their data
		QList<AtlasTreeItem *> atlasFiltersTreeItemList(m_TopLevelTreeItemList);
		for(int i = 0; i < m_TopLevelTreeItemList.size(); ++i)
		{
			AtlasTreeItem *pParentFilter = NULL;

			QString sFilterPath = m_TopLevelTreeItemList[i]->data(0, Qt::UserRole).toString();
			sFilterPath.truncate(sFilterPath.lastIndexOf("/"));
			if(sFilterPath != "")
			{
				for(int j = 0; j < atlasFiltersTreeItemList.size(); ++j)
				{
					if(atlasFiltersTreeItemList[j]->data(0, Qt::UserRole).toString() == sFilterPath)
					{
						pParentFilter = atlasFiltersTreeItemList[j];
						break;
					}
				}
			}

			if(pParentFilter)
			{
				pParentFilter->addChild(m_TopLevelTreeItemList.takeAt(i));
				i = -1;
			}
		}

		// Finally go through all the filters and set the data string to the 'HYTREEWIDGETITEM_IsFilter' value to identify this QTreeWidgetItem as a filter
		for(int i = 0; i < atlasFiltersTreeItemList.size(); ++i)
			atlasFiltersTreeItemList[i]->setData(0, Qt::UserRole, QVariant(QString(HYTREEWIDGETITEM_IsFilter)));

		QJsonArray frameArray = settingsObj["frames"].toArray();
		for(int i = 0; i < frameArray.size(); ++i)
		{
			QJsonObject frameObj = frameArray[i].toObject();

			// TODO: Put this in parameter list, once snow white updated
			quint32 uiAtlasGrpId;
			if(false == frameObj.contains("atlasGrpId"))
				uiAtlasGrpId = 0; // 0 == Default
			else
				uiAtlasGrpId = JSONOBJ_TOINT(frameObj, "atlasGrpId");

			QRect rAlphaCrop(QPoint(frameObj["cropLeft"].toInt(), frameObj["cropTop"].toInt()), QPoint(frameObj["cropRight"].toInt(), frameObj["cropBottom"].toInt()));
			AtlasFrame *pNewFrame = CreateFrame(JSONOBJ_TOINT(frameObj, "id"),
												JSONOBJ_TOINT(frameObj, "checksum"),
												uiAtlasGrpId,
												frameObj["name"].toString(),
												rAlphaCrop,
												static_cast<AtlasItemType>(frameObj["type"].toInt()),
												frameObj["width"].toInt(),
												frameObj["height"].toInt(),
												frameObj["x"].toInt(),
												frameObj["y"].toInt(),
												frameObj["textureIndex"].toInt(),
												frameObj["errors"].toInt(0));

			QString sFilterPath = frameObj["filter"].toString();
			AtlasTreeItem *pFrameParent = NULL;
			if(sFilterPath != "")
			{
				for(int j = 0; j < atlasFiltersTreeItemList.size(); ++j)
				{
					if(atlasFiltersTreeItemList[j]->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter && HyGlobal::GetTreeWidgetItemPath(atlasFiltersTreeItemList[j]) == sFilterPath)
					{
						pFrameParent = atlasFiltersTreeItemList[j];
						break;
					}
				}
			}

			if(QFile::exists(m_MetaDir.absoluteFilePath(pNewFrame->ConstructImageFileName())) == false)
				pNewFrame->SetError(ATLASFRAMEERROR_CannotFindMetaImg);
			else
				pNewFrame->ClearError(ATLASFRAMEERROR_CannotFindMetaImg);

			if(pFrameParent)
				pFrameParent->addChild(pNewFrame->GetTreeItem());
			else
				m_TopLevelTreeItemList.append(pNewFrame->GetTreeItem());

		}
	}
	else
	{
		m_uiNextFrameId = 0;
		m_uiNextAtlasId = 0;
		
		CreateNewAtlasGrp("Default");
	}
}

/*virtual*/ AtlasModel::~AtlasModel()
{
	for(int i = 0; i < m_AtlasGrpList.size(); ++i)
	{
		QList<AtlasFrame *> &atlasFramesRef = m_AtlasGrpList[i]->m_FrameList;
		for(int j = 0; j < atlasFramesRef.size(); ++j)
			delete atlasFramesRef[j];
	}
}

Project *AtlasModel::GetProjOwner()
{
	return m_pProjOwner;
}

int AtlasModel::GetNumAtlasGroups()
{
	return m_AtlasGrpList.size();
}

QString AtlasModel::GetAtlasGroupName(uint uiAtlasGrpIndex)
{
	return m_AtlasGrpList[uiAtlasGrpIndex]->m_PackerSettings["txtName"].toString();
}

QList<AtlasFrame *> AtlasModel::GetFrames(uint uiAtlasGrpIndex)
{
	return m_AtlasGrpList[uiAtlasGrpIndex]->m_FrameList;
}

QJsonObject AtlasModel::GetPackerSettings(uint uiAtlasGrpIndex)
{
	return m_AtlasGrpList[uiAtlasGrpIndex]->m_PackerSettings;
}

void AtlasModel::SetPackerSettings(uint uiAtlasGrpIndex, QJsonObject newPackerSettingsObj)
{
	m_AtlasGrpList[uiAtlasGrpIndex]->m_PackerSettings = newPackerSettingsObj;
}

void AtlasModel::StashTreeWidgets(QList<AtlasTreeItem *> treeItemList)
{
	m_TopLevelTreeItemList = treeItemList;
}

QList<AtlasTreeItem *> AtlasModel::GetTopLevelTreeItemList()
{
	return m_TopLevelTreeItemList;
}

int AtlasModel::GetNumTextures(uint uiAtlasGrpIndex)
{
	return m_AtlasGrpList[uiAtlasGrpIndex]->GetExistingTextureInfoList().size();
}

QSize AtlasModel::GetAtlasDimensions(uint uiAtlasGrpIndex)
{
	int iWidth = m_AtlasGrpList[uiAtlasGrpIndex]->m_PackerSettings["sbTextureWidth"].toInt();
	int iHeight = m_AtlasGrpList[uiAtlasGrpIndex]->m_PackerSettings["sbTextureHeight"].toInt();
	
	return QSize(iWidth, iHeight);
}

HyTextureFormat AtlasModel::GetAtlasTextureType(uint uiAtlasGrpIndex)
{
	return static_cast<HyTextureFormat>(m_AtlasGrpList[uiAtlasGrpIndex]->m_PackerSettings["textureType"].toInt());
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
	if(m_pProjOwner->GetAtlasWidget())
	{
		QTreeWidgetItemIterator iter(m_pProjOwner->GetAtlasWidget()->GetFramesTreeWidget());
		while(*iter)
		{
			if((*iter)->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter)
			{
				QString sFilterPath = HyGlobal::GetTreeWidgetItemPath(*iter);
				filtersArray.append(QJsonValue(sFilterPath));
			}

			++iter;
		}
	}
	else
	{
		QFile settingsFile(m_MetaDir.absoluteFilePath(HYGUIPATH_MetaSettings));
		if(settingsFile.exists())
		{
			if(!settingsFile.open(QIODevice::ReadOnly))
				HyGuiLog(QString("WidgetAtlasGroup::WidgetAtlasGroup() could not open ") % HYGUIPATH_MetaSettings, LOGTYPE_Error);

#ifdef HYGUI_UseBinaryMetaFiles
			QJsonDocument settingsDoc = QJsonDocument::fromBinaryData(settingsFile.readAll());
#else
			QJsonDocument settingsDoc = QJsonDocument::fromJson(settingsFile.readAll());
#endif
			settingsFile.close();

			QJsonObject settingsObj = settingsDoc.object();
			filtersArray = settingsObj["filters"].toArray();
		}
	}

	// Assemble the offical QJsonObject for the write
	QJsonObject settingsObj;
	settingsObj.insert("frames", frameArray);
	settingsObj.insert("groups", groupsArray);
	settingsObj.insert("startFrameId", QJsonValue(static_cast<qint64>(m_uiNextFrameId)));
	settingsObj.insert("startAtlasId", QJsonValue(static_cast<qint64>(m_uiNextAtlasId)));
	settingsObj.insert("filters", filtersArray);

	QFile settingsFile(m_MetaDir.absoluteFilePath(HYGUIPATH_MetaSettings));
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

AtlasFrame *AtlasModel::CreateFrame(quint32 uiId, quint32 uiChecksum, quint32 uiAtlasGrpId, QString sN, QRect rAlphaCrop, AtlasItemType eFrameType, int iW, int iH, int iX, int iY, int iTextureIndex, uint uiErrors)
{
	if(uiId == ATLASFRAMEID_NotSet)
	{
		uiId = m_uiNextFrameId;
		m_uiNextFrameId++;
	}

	AtlasFrame *pNewFrame = new AtlasFrame(uiId, uiChecksum, uiAtlasGrpId, sN, rAlphaCrop, eFrameType, iW, iH, iX, iY, iTextureIndex, uiErrors);

	m_FrameLookup.AddLookup(pNewFrame);
	
	for(int i = 0; i < m_AtlasGrpList.size(); ++i)
	{
		if(m_AtlasGrpList[i]->m_PackerSettings.contains("atlasGrpId") == false) {
			HyGuiLog("AtlasModel::CreateFrame could not find atlasGrpId", LOGTYPE_Error);
		}
		else if(pNewFrame->GetAtlasGrpId() == m_AtlasGrpList[i]->GetId())
		{
			m_AtlasGrpList[i]->m_FrameList.append(pNewFrame);
			break;
		}
	}
	
	return pNewFrame;
}

void AtlasModel::RemoveFrame(AtlasFrame *pFrame)
{
	if(m_FrameLookup.RemoveLookup(pFrame))
		pFrame->DeleteMetaImage(m_MetaDir);
	
	for(int i = 0; i < m_AtlasGrpList.size(); ++i)
	{
		if(m_AtlasGrpList[i]->m_PackerSettings.contains("atlasGrpId") == false) {
			HyGuiLog("AtlasModel::CreateFrame could not find atlasGrpId", LOGTYPE_Error);
		}
		else if(pFrame->GetAtlasGrpId() == m_AtlasGrpList[i]->GetId())
		{
			m_AtlasGrpList[i]->m_FrameList.removeOne(pFrame);
			break;
		}
	}

	delete pFrame;
}

bool AtlasModel::TransferFrame(AtlasFrame *pFrame, quint32 uiNewAtlasGrpId)
{
	if(uiNewAtlasGrpId == pFrame->GetAtlasGrpId())
		return false;
	
	bool bValid = false;
	for(int i = 0; i < m_AtlasGrpList.size(); ++i)
	{
		if(pFrame->GetAtlasGrpId() == m_AtlasGrpList[i]->GetId())
		{
			m_AtlasGrpList[i]->m_FrameList.removeOne(pFrame);
			bValid = true;
			break;
		}
	}
	
	if(bValid == false)
		return false;
	
	bValid = false;
	for(int i = 0; i < m_AtlasGrpList.size(); ++i)
	{
		if(uiNewAtlasGrpId == m_AtlasGrpList[i]->GetId())
		{
			pFrame->SetAtlasGrpId(uiNewAtlasGrpId);
			
			m_AtlasGrpList[i]->m_FrameList.append(pFrame);
			bValid = true;
			break;
		}
	}
	
	return bValid;
}

AtlasFrame *AtlasModel::GenerateFrame(ProjectItem *pItem, QString sName, QImage &newImage, quint32 uiAtlasGrpIndex, HyGuiItemType eType)
{
	// This will also create a meta image
	AtlasFrame *pFrame = ImportImage(sName, newImage, m_AtlasGrpList[uiAtlasGrpIndex]->GetId(), eType, nullptr);

	QSet<AtlasFrame *> newFrameSet;
	newFrameSet.insert(pFrame);
	Repack(uiAtlasGrpIndex, QSet<int>(), newFrameSet);

	// This retrieves the newly created AtlasFrame and links it to its ProjectItem
	QList<quint32> idList;
	idList.append(pFrame->GetId());
	QList<AtlasFrame *> returnList = RequestFramesById(pItem, idList);

	if(returnList.empty() == false)
		return returnList[0];

	return nullptr;
}

void AtlasModel::ReplaceFrame(AtlasFrame *pFrame, QString sName, QImage &newImage, bool bDoAtlasGroupRepack)
{
	QSet<int> textureIndexToReplaceSet;
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
}

QList<AtlasFrame *> AtlasModel::RequestFrames(ProjectItem *pItem)
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
		if(pFrame == NULL)
			continue;

		frameRequestList.append(pFrame);
	}

	if(frameRequestList.empty())
		return QList<AtlasFrame *>();

	return RequestFrames(pItem, frameRequestList);
}

QList<AtlasFrame *> AtlasModel::RequestFramesById(ProjectItem *pItem, QList<quint32> requestList)
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
			HyGuiLog("Cannot find image with id: " % QString::number(requestList[i]) % "\nIt may have been removed, or is invalid in the Atlas Manager.", LOGTYPE_Warning);
		}
		else
		{
			frameRequestList.append(pFoundFrame);
		}
	}

	return RequestFrames(pItem, frameRequestList);
}

QList<AtlasFrame *> AtlasModel::RequestFrames(ProjectItem *pItem, QList<AtlasFrame *> requestList)
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

void AtlasModel::RelinquishFrames(ProjectItem *pItem, QList<AtlasFrame *> relinquishList)
{
	for(int i = 0; i < relinquishList.size(); ++i)
		relinquishList[i]->m_DependencySet.remove(pItem);
}

QSet<AtlasFrame *> AtlasModel::ImportImages(QStringList sImportImgList, quint32 uiAtlasGrpId, HyGuiItemType eType, QList<AtlasTreeItem *> correspondingParentList)
{
	if(correspondingParentList.size() != sImportImgList.size())
		HyGuiLog("AtlasModel::ImportImages was passed a correspondingParentList that isn't the same size as the sImportImgList", LOGTYPE_Error);

	QSet<AtlasFrame *> returnSet;
	for(int i = 0; i < sImportImgList.size(); ++i)
	{
		QFileInfo fileInfo(sImportImgList[i]);
		QImage newImage(fileInfo.absoluteFilePath());

		returnSet.insert(ImportImage(fileInfo.baseName(), newImage, uiAtlasGrpId, eType, correspondingParentList[i]));
	}

	return returnSet;
}

AtlasFrame *AtlasModel::ImportImage(QString sName, QImage &newImage, quint32 uiAtlasGrpId, HyGuiItemType eType, AtlasTreeItem *pParent)
{
	quint32 uiChecksum = HyGlobal::CRCData(0, newImage.bits(), newImage.byteCount());

	QRect rAlphaCrop(0, 0, newImage.width(), newImage.height());
	if(eType != ITEM_Font && eType != ITEM_Spine) // Cannot crop 'sub-atlases' because they rely on their own UV coordinates
		rAlphaCrop = ImagePacker::crop(newImage);

	AtlasFrame *pNewFrame = CreateFrame(ATLASFRAMEID_NotSet, uiChecksum, uiAtlasGrpId, sName, rAlphaCrop, HyGlobal::GetAtlasItemFromItem(eType), newImage.width(), newImage.height(), -1, -1, -1, 0);
	if(pNewFrame)
	{
		newImage.save(m_MetaDir.absoluteFilePath(pNewFrame->ConstructImageFileName()));

		if(pParent == nullptr)
			m_pProjOwner->GetAtlasWidget()->GetFramesTreeWidget()->addTopLevelItem(pNewFrame->GetTreeItem());
		else
		{
			if(pParent->data(0, Qt::UserRole).toString() != HYTREEWIDGETITEM_IsFilter)
				HyGuiLog("AtlasModel::ImportImage was passed parent that wasn't a filter", LOGTYPE_Error);

			pParent->addChild(pNewFrame->GetTreeItem());
		}
	}

	return pNewFrame;
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
	
	//GetAtlasInfoForGameData(atlasObj);

	QJsonDocument atlasInfoDoc;
	atlasInfoDoc.setArray(atlasGrpArray);

	QFile atlasInfoFile(m_RootDataDir.absolutePath() % "/" % HYGUIPATH_DataAtlases);
	if(atlasInfoFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false) {
	   HyGuiLog("Couldn't open atlas data info file for writing", LOGTYPE_Error);
	}
	else
	{
		qint64 iBytesWritten = atlasInfoFile.write(atlasInfoDoc.toJson());
		if(0 == iBytesWritten || -1 == iBytesWritten) {
			HyGuiLog("Could not write to atlas settings file: " % atlasInfoFile.errorString(), LOGTYPE_Error);
		}

		atlasInfoFile.close();
	}
}

//void AtlasModel::GetAtlasInfoForGameData(QJsonObject &atlasObjOut)
//{
//    atlasObjOut.insert("id", m_DataDir.dirName().toInt());
//    atlasObjOut.insert("width", m_PackerSettings["sbTextureWidth"].toInt());
//    atlasObjOut.insert("height", m_PackerSettings["sbTextureHeight"].toInt());
//    atlasObjOut.insert("num8BitClrChannels", 4);   // TODO: Actually make this configurable?

//    QJsonArray textureArray;
//    QList<QJsonArray> frameArrayList;
//    for(int i = 0; i < m_FrameList.size(); ++i)
//    {
//        if(m_FrameList[i]->GetTextureIndex() < 0)
//            continue;

//        while(frameArrayList.empty() || frameArrayList.size() <= m_FrameList[i]->GetTextureIndex())
//            frameArrayList.append(QJsonArray());

//        QJsonObject frameObj;
//        frameObj.insert("checksum", QJsonValue(static_cast<qint64>(m_FrameList[i]->GetImageChecksum())));
//        frameObj.insert("left", QJsonValue(m_FrameList[i]->GetX()));
//        frameObj.insert("top", QJsonValue(m_FrameList[i]->GetY()));
//        frameObj.insert("right", QJsonValue(m_FrameList[i]->GetX() + m_FrameList[i]->GetCrop().width()));
//        frameObj.insert("bottom", QJsonValue(m_FrameList[i]->GetY() + m_FrameList[i]->GetCrop().height()));

//        frameArrayList[m_FrameList[i]->GetTextureIndex()].append(frameObj);
//    }

//    for(int i = 0; i < frameArrayList.size(); ++i)
//        textureArray.append(frameArrayList[i]);

//    atlasObjOut.insert("textures", textureArray);
//}

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

void AtlasModel::SaveAndReloadHarmony()
{
	WriteMetaSettings();

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
