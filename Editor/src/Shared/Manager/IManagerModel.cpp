/**************************************************************************
 *	IManagerModel.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "IManagerModel.h"
#include "Project.h"
#include "DlgAtlasGroupSettings.h"
#include "Harmony.h"

IManagerModel::IManagerModel(Project &projRef, HyGuiItemType eItemType) :
	ITreeModel(2, QStringList(), nullptr),
	m_BanksModel(*this),
	m_ProjectRef(projRef),
	m_eITEM_TYPE(eItemType),
	m_MetaDir(m_ProjectRef.GetMetaDataAbsPath() + HyGlobal::ItemName(eItemType, true)),
	m_DataDir(m_ProjectRef.GetAssetsAbsPath() + HyGlobal::ItemName(eItemType, true))
{
	if(m_MetaDir.exists() == false)
	{
		HyGuiLog(HyGlobal::ItemName(m_eITEM_TYPE, true) % " meta directory is missing, recreating", LOGTYPE_Info);
		m_MetaDir.mkpath(m_MetaDir.absolutePath());
	}
	if(m_DataDir.exists() == false)
	{
		HyGuiLog(HyGlobal::ItemName(m_eITEM_TYPE, true) % " data directory is missing, recreating", LOGTYPE_Info);
		m_DataDir.mkpath(m_DataDir.absolutePath());
	}

	QFile settingsFile(m_MetaDir.absoluteFilePath(HyGlobal::ItemName(m_eITEM_TYPE, false) % HYGUIPATH_MetaExt));
	if(settingsFile.exists())
	{
		if(!settingsFile.open(QIODevice::ReadOnly))
			HyGuiLog(QString("IManagerModel::IManagerModel() could not open ") % HyGlobal::ItemName(m_eITEM_TYPE, false) % HYGUIPATH_MetaExt, LOGTYPE_Error);

#ifdef HYGUI_UseBinaryMetaFiles
		QJsonDocument settingsDoc = QJsonDocument::fromBinaryData(settingsFile.readAll());
#else
		QJsonDocument settingsDoc = QJsonDocument::fromJson(settingsFile.readAll());
#endif
		settingsFile.close();

		QJsonObject settingsObj = settingsDoc.object();

		// TODO: rename to nextBankId
		m_uiNextBankId = JSONOBJ_TOINT(settingsObj, "startAtlasId");

		// TODO: rename to banks
		QJsonArray bankArray = settingsObj["groups"].toArray();
		for(int i = 0; i < bankArray.size(); ++i)
		{
			// TODO: rename to bankId
			QString sName = HyGlobal::MakeFileNameFromCounter(bankArray[i].toObject()["atlasGrpId"].toInt());
			m_BanksModel.AppendBank(m_DataDir.absoluteFilePath(sName), bankArray[i].toObject());
		}

		m_ExpandedFiltersArray = settingsObj["expanded"].toArray();

		//// Create all the filter items first, storing their actual path in their data (for now)
		//QJsonArray filtersArray = settingsObj["filters"].toArray();
		//for(int i = 0; i < filtersArray.size(); ++i)
		//{
		//	QDir filterPathDir(filtersArray.at(i).toString());

		//	AtlasTreeItem *pNewTreeItem = new AtlasTreeItem((QTreeWidgetItem *)nullptr, QTreeWidgetItem::Type);

		//	pNewTreeItem->setText(0, filterPathDir.dirName());
		//	pNewTreeItem->setIcon(0, HyGlobal::ItemIcon(ITEM_Filter, SUBICON_None));

		//	QVariant v(QString(filterPathDir.absolutePath()));
		//	pNewTreeItem->setData(0, Qt::UserRole, v);

		//	m_TopLevelTreeItemList.append(pNewTreeItem);
		//}

		//// Then place the filters correctly as a parent hierarchy using the path string stored in their data
		//QList<AtlasTreeItem *> atlasFiltersTreeItemList(m_TopLevelTreeItemList);
		//for(int i = 0; i < m_TopLevelTreeItemList.size(); ++i)
		//{
		//	AtlasTreeItem *pParentFilter = nullptr;

		//	QString sFilterPath = m_TopLevelTreeItemList[i]->data(0, Qt::UserRole).toString();
		//	sFilterPath.truncate(sFilterPath.lastIndexOf("/"));
		//	if(sFilterPath != "")
		//	{
		//		for(int j = 0; j < atlasFiltersTreeItemList.size(); ++j)
		//		{
		//			if(atlasFiltersTreeItemList[j]->data(0, Qt::UserRole).toString() == sFilterPath)
		//			{
		//				pParentFilter = atlasFiltersTreeItemList[j];
		//				break;
		//			}
		//		}
		//	}

		//	if(pParentFilter)
		//	{
		//		pParentFilter->addChild(m_TopLevelTreeItemList.takeAt(i));
		//		i = -1;
		//	}
		//}

		//// Finally go through all the filters and set the data string to the 'HYTREEWIDGETITEM_IsFilter' value to identify this QTreeWidgetItem as a filter
		//for(int i = 0; i < atlasFiltersTreeItemList.size(); ++i)
		//	atlasFiltersTreeItemList[i]->setData(0, Qt::UserRole, QVariant(QString(HYTREEWIDGETITEM_IsFilter)));

		// TODO: rename to assets
		QJsonArray frameArray = settingsObj["frames"].toArray();
		for(int i = 0; i < frameArray.size(); ++i)
		{
			QJsonObject assetObj = frameArray[i].toObject();
			AssetItemData *pAssetData = CreateAssetTreeItem(assetObj["filter"].toString(), assetObj["name"].toString(), assetObj);


			//QString sFilterPath = frameObj["filter"].toString();
			//AtlasTreeItem *pFrameParent = nullptr;
			//if(sFilterPath != "")
			//{
			//	for(int j = 0; j < atlasFiltersTreeItemList.size(); ++j)
			//	{
			//		if(atlasFiltersTreeItemList[j]->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter && HyGlobal::GetTreeWidgetItemPath(atlasFiltersTreeItemList[j]) == sFilterPath)
			//		{
			//			pFrameParent = atlasFiltersTreeItemList[j];
			//			break;
			//		}
			//	}
			//}

			// Check to see if the actual meta asset exists on disk
			if(QFile::exists(m_MetaDir.absoluteFilePath(pAssetData->ConstructMetaFileName())) == false)
				pAssetData->SetError(ATLASFRAMEERROR_CannotFindMetaImg);
			else
				pAssetData->ClearError(ATLASFRAMEERROR_CannotFindMetaImg);

			//if(pNewFrame->GetName().isEmpty() || pNewFrame->GetName()[0] != HyGuiInternalCharIndicator)
			//{
			//	if(pFrameParent)
			//		pFrameParent->addChild(pNewFrame->GetTreeItem());
			//	else
			//		m_TopLevelTreeItemList.append(pNewFrame->GetTreeItem());
			//}
		}
	}
	else
	{
		m_uiNextBankId = 0;
		CreateNewBank("Default");
	}
}

/*virtual*/ IManagerModel::~IManagerModel()
{
	
}

Project &IManagerModel::GetProjOwner()
{
	return m_ProjectRef;
}

QDir IManagerModel::GetMetaDir()
{
	return m_MetaDir;
}

QDir IManagerModel::GetDataDir()
{
	return m_DataDir;
}

int IManagerModel::GetNumBanks()
{
	return m_BanksModel.rowCount();
}

QString IManagerModel::GetBankName(uint uiBankIndex)
{
	return m_BanksModel.GetBank(uiBankIndex)->GetName();
}

QJsonObject IManagerModel::GetBankSettings(uint uiBankIndex)
{
	return m_BanksModel.GetBank(uiBankIndex)->m_Settings;
}

void IManagerModel::SetBankSettings(uint uiBankIndex, QJsonObject newSettingsObj)
{
	m_BanksModel.GetBank(uiBankIndex)->m_Settings = newSettingsObj;
}

QJsonArray IManagerModel::GetExpandedFiltersArray()
{
	return m_ExpandedFiltersArray;
}

QString IManagerModel::AssembleFilter(const AssetItemData *pAsset) const
{
	QStringList sPrefixParts;

	TreeModelItem *pTreeItem = GetItem(FindIndex<const AssetItemData *>(pAsset, 0))->GetParent();
	while(pTreeItem && pTreeItem != m_pRootItem)
	{
		TreeModelItemData *pItem = pTreeItem->data(0).value<TreeModelItemData *>();
		if(pItem->GetType() == ITEM_Filter)
			sPrefixParts.prepend(pItem->GetText());

		pTreeItem = pTreeItem->GetParent();
	}

	QString sPrefix;
	for(int i = 0; i < sPrefixParts.size(); ++i)
	{
		sPrefix += sPrefixParts[i];
		sPrefix += "/";
	}

	return sPrefix;
}

bool IManagerModel::RemoveLookup(AssetItemData *pAsset)
{
	m_AssetUuidMap.remove(pAsset->GetUuid());

	auto iter = m_AssetChecksumMap.find(pAsset->GetChecksum());
	if(iter == m_AssetChecksumMap.end())
		HyGuiLog("IManagerModel::RemoveLookup could not find asset with checksum: " % QString::number(pAsset->GetChecksum()), LOGTYPE_Error);

	iter.value().removeOne(pAsset);
	if(iter.value().size() == 0)
	{
		m_AssetChecksumMap.remove(pAsset->GetChecksum());
		return true;
	}

	return false;
}

AssetItemData *IManagerModel::FindById(QUuid uuid)
{
	auto iter = m_AssetUuidMap.find(uuid);
	if(iter == m_AssetUuidMap.end())
		return nullptr;
	else
		return iter.value();
}

QList<AssetItemData *> IManagerModel::FindByChecksum(quint32 uiChecksum)
{
	auto iter = m_AssetChecksumMap.find(uiChecksum);
	if(iter == m_AssetChecksumMap.end())
		return QList<AssetItemData *>();
	else
		return iter.value();
}

bool IManagerModel::DoesAssetExist(quint32 uiChecksum)
{
	return m_AssetChecksumMap.contains(uiChecksum);
}

void IManagerModel::RemoveAsset(AssetItemData *pAsset)
{
	if(RemoveLookup(pAsset))
		pAsset->DeleteMetaFile();

	for(int i = 0; i < m_BanksModel.rowCount(); ++i)
	{
		if(pAsset->GetBankId() == m_BanksModel.GetBank(i)->GetId())
		{
			m_BanksModel.GetBank(i)->m_AssetList.removeOne(pAsset);
			break;
		}
	}

	delete pAsset;
}

bool IManagerModel::TransferAsset(AssetItemData *pAsset, uint uiNewBankId)
{
	if(uiNewBankId == pAsset->GetBankId())
		return false;

	bool bValid = false;
	for(int i = 0; i < m_BanksModel.rowCount(); ++i)
	{
		if(pAsset->GetBankId() == m_BanksModel.GetBank(i)->GetId())
		{
			m_BanksModel.GetBank(i)->m_AssetList.removeOne(pAsset);
			bValid = true;
			break;
		}
	}

	if(bValid == false)
		return false;

	bValid = false;
	for(int i = 0; i < m_BanksModel.rowCount(); ++i)
	{
		if(uiNewBankId == m_BanksModel.GetBank(i)->GetId())
		{
			pAsset->SetBankId(uiNewBankId);

			m_BanksModel.GetBank(i)->m_AssetList.append(pAsset);
			bValid = true;
			break;
		}
	}

	return bValid;
}

//QList<AssetItemData *> IManagerModel::RequestAssets(ProjectItemData *pItem)
//{
//	QList<QTreeWidgetItem *> selectedItems;
//	if(m_pProjOwner->GetAtlasWidget())
//	{
//		selectedItems = m_pProjOwner->GetAtlasWidget()->GetFramesTreeWidget()->selectedItems();
//		qSort(selectedItems.begin(), selectedItems.end(), SortTreeWidgetsPredicate());
//
//		m_pProjOwner->GetAtlasWidget()->GetFramesTreeWidget()->clearSelection();
//	}
//
//	QList<AtlasFrame *> frameRequestList;
//	for(int i = 0; i < selectedItems.size(); ++i)
//	{
//		AtlasFrame *pFrame = selectedItems[i]->data(0, Qt::UserRole).value<AtlasFrame *>();
//		if(pFrame == nullptr)
//			continue;
//
//		frameRequestList.append(pFrame);
//	}
//
//	if(frameRequestList.empty())
//		return QList<AtlasFrame *>();
//
//	return RequestAssets(pItem, frameRequestList);
//}

QList<AssetItemData *> IManagerModel::RequestAssetsByUuid(ProjectItemData *pItem, QList<QUuid> requestList)
{
	if(requestList.empty())
		return QList<AssetItemData *>();

	QList<AssetItemData *> frameRequestList;
	for(int i = 0; i < requestList.size(); ++i)
	{
		AssetItemData *pFoundFrame = FindById(requestList[i]);
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

	return RequestAssets(pItem, frameRequestList);
}

QList<AssetItemData *> IManagerModel::RequestAssets(ProjectItemData *pItem, QList<AssetItemData *> requestList)
{
	if(requestList.empty())
		return requestList;//RequestAssets(pItem);

	QList<AssetItemData *> returnList;
	for(int i = 0; i < requestList.size(); ++i)
	{
		requestList[i]->InsertLink(pItem);
		returnList.append(requestList[i]);
	}

	return returnList;
}

void IManagerModel::RelinquishAssets(ProjectItemData *pItem, QList<AssetItemData *> relinquishList)
{
	for(int i = 0; i < relinquishList.size(); ++i)
		relinquishList[i]->RemoveLink(pItem);
}

QSet<AssetItemData *> IManagerModel::ImportNewAssets(QStringList sImportList, quint32 uiBankId, HyGuiItemType eType, QList<TreeModelItem *> correspondingParentList)
{
	if(correspondingParentList.size() != sImportList.size())
	{
		HyGuiLog("AtlasModel::ImportImages was passed a correspondingParentList that isn't the same size as the sImportImgList", LOGTYPE_Error);
		return QSet<AssetItemData *>(); // indicates error
	}

	// TODO: Error check all the imported assets before adding them, and cancel entire import if any fail (currently will import all passing assets)

	QSet<AssetItemData *> returnSet;
	for(int i = 0; i < sImportList.size(); ++i)
	{
		AssetItemData *pNewAsset = OnAllocateAssetData(sImportList[i], uiBankId, eType);
		if(pNewAsset)
		{
			RegisterAsset(pNewAsset);
			InsertTreeItem(pNewAsset, correspondingParentList[i]);

			returnSet.insert(pNewAsset);
		}
	}

	return returnSet;
}

bool IManagerModel::CreateNewFilter(QString sName, TreeModelItem *pParent)
{
	if(InsertTreeItem(new TreeModelItemData(ITEM_Filter, sName), pParent))
	{
		SaveMeta();
		return true;
	}

	return false;
}

uint IManagerModel::CreateNewBank(QString sName)
{
	//AtlasGrp *pNewAtlasGrp = new AtlasGrp());
	QJsonObject bankObj = DlgAtlasGroupSettings::GenerateDefaultSettingsObj();

	// TODO: rename to bankName
	bankObj.insert("txtName", sName);
	// TODO: rename to bankId
	bankObj.insert("atlasGrpId", QJsonValue(static_cast<qint64>(m_uiNextBankId)));

	m_BanksModel.AppendBank(m_DataDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(m_uiNextBankId)), bankObj);

	m_uiNextBankId++;
	SaveMeta();

	return static_cast<uint>(m_BanksModel.rowCount() - 1);
}

void IManagerModel::RemoveBank(quint32 uiBankId)
{
	if(uiBankId == 0)
	{
		HyGuiLog("IManagerModel::RemoveBank is trying to remove bank id: 0", LOGTYPE_Error);
		return;
	}

	for(int i = 0; i < m_BanksModel.rowCount(); ++i)
	{
		if(m_BanksModel.GetBank(i)->GetId() == uiBankId)
		{
			if(m_BanksModel.GetBank(i)->m_AssetList.empty())
			{
				m_BanksModel.RemoveBank(i);
				SaveMeta();
			}
			else {
				HyGuiLog("Cannot remove bank since it has " % QString::number(m_BanksModel.GetBank(i)->m_AssetList.size()) % " asset(s) that is/are still linked to it.", LOGTYPE_Info);
			}

			return;
		}
	}

	HyGuiLog("IManagerModel::RemoveBank could not find bank ID: " % QString::number(uiBankId), LOGTYPE_Error);
}

uint IManagerModel::GetBankIndexFromBankId(quint32 uiBankId)
{
	uint uiBankIndex = 0xFFFFFFFF;
	for(int i = 0; i < m_BanksModel.rowCount(); ++i)
	{
		if(m_BanksModel.GetBank(i)->GetId() == uiBankId)
		{
			uiBankIndex = i;
			break;
		}
	}
	if(0xFFFFFFFF == uiBankIndex) {
		HyGuiLog("IManagerModel::GetBankIndexFromBankId could not find bank index from ID: " % QString::number(uiBankId), LOGTYPE_Error);
	}

	return uiBankIndex;
}

quint32 IManagerModel::GetBankIdFromBankIndex(uint uiBankIndex)
{
	return m_BanksModel.GetBank(uiBankIndex)->GetId();
}

void IManagerModel::SaveMeta()
{
	// Assemble array with all the frames from every group
	QJsonArray assetsArray;
	QJsonArray groupsArray;
	for(int i = 0; i < m_BanksModel.rowCount(); ++i)
	{
		groupsArray.append(m_BanksModel.GetBank(i)->m_Settings);

		QList<AssetItemData *> &assetListRef = m_BanksModel.GetBank(i)->m_AssetList;
		for(int j = 0; j < assetListRef.size(); ++j)
		{
			QJsonObject assetObj;
			assetListRef[j]->GetJsonObj(assetObj);
			assetsArray.append(assetObj);
		}
	}

//	QJsonArray filtersArray;
//	m_ExpandedFiltersArray = QJsonArray();
//	if(m_pProjOwner->GetAtlasWidget())
//	{
//		QTreeWidgetItemIterator iter(m_pProjOwner->GetAtlasWidget()->GetFramesTreeWidget());
//		while(*iter)
//		{
//			if((*iter)->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter)
//			{
//				QTreeWidgetItem *pTreeWidgetItem = *iter;
//				QString sFilterName = pTreeWidgetItem->text(0);
//				bool bExpanded = pTreeWidgetItem->isExpanded();
//
//				QString sFilterPath = HyGlobal::GetTreeWidgetItemPath(*iter);
//				filtersArray.append(QJsonValue(sFilterPath));
//				m_ExpandedFiltersArray.append(bExpanded);
//			}
//
//			++iter;
//		}
//	}
//	else
//	{
//		QFile settingsFile(m_MetaDir.absoluteFilePath(HYMETA_AtlasFile));
//		if(settingsFile.exists())
//		{
//			if(!settingsFile.open(QIODevice::ReadOnly))
//				HyGuiLog(QString("WidgetAtlasGroup::WidgetAtlasGroup() could not open ") % HYMETA_AtlasFile, LOGTYPE_Error);
//
//#ifdef HYGUI_UseBinaryMetaFiles
//			QJsonDocument settingsDoc = QJsonDocument::fromBinaryData(settingsFile.readAll());
//#else
//			QJsonDocument settingsDoc = QJsonDocument::fromJson(settingsFile.readAll());
//#endif
//			settingsFile.close();
//
//			QJsonObject settingsObj = settingsDoc.object();
//			filtersArray = settingsObj["filters"].toArray();
//			m_ExpandedFiltersArray = settingsObj["expanded"].toArray();
//		}
//	}

	// Assemble the official QJsonObject for the write
	QJsonObject settingsObj;
	settingsObj.insert("$fileVersion", HYGUI_FILE_VERSION);
	//settingsObj.insert("expanded", m_ExpandedFiltersArray);
	//settingsObj.insert("filters", filtersArray);

	// TODO: rename to assets
	settingsObj.insert("frames", assetsArray);
	settingsObj.insert("groups", groupsArray);

	// TODO: rename to nextBankId
	settingsObj.insert("startAtlasId", QJsonValue(static_cast<qint64>(m_uiNextBankId)));

	QFile settingsFile(HyGlobal::ItemName(m_eITEM_TYPE, false) % HYGUIPATH_MetaExt);
	if(!settingsFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		HyGuiLog("Couldn't open meta file for writing", LOGTYPE_Error);
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
			HyGuiLog("Could not write to meta file: " % settingsFile.errorString(), LOGTYPE_Error);
		}

		settingsFile.close();
	}
}

void IManagerModel::SaveRuntime()
{
	SaveMeta();

	QJsonDocument runtimeDoc;
	runtimeDoc.setObject(GetSaveJson());

	QFile runtimeFile(m_DataDir.absoluteFilePath(HYASSETS_AtlasFile));
	if(runtimeFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
		HyGuiLog("Couldn't open atlas data info file for writing", LOGTYPE_Error);
	else
	{
		qint64 iBytesWritten = runtimeFile.write(runtimeDoc.toJson());
		if(0 == iBytesWritten || -1 == iBytesWritten)
			HyGuiLog("Could not write save file: " % runtimeFile.errorString(), LOGTYPE_Error);

		runtimeFile.close();
	}

	Harmony::Reload(&m_ProjectRef);
}

AssetItemData *IManagerModel::CreateAssetTreeItem(const QString sPrefix, const QString sName, QJsonObject metaObj)
{
	TreeModelItem *pCurTreeItem = m_pRootItem;
	if(sPrefix.isEmpty() == false)
	{
		QStringList sPathSplitList = sPrefix.split(QChar('/'));
		// Traverse down the tree and add any prefix TreeItem that doesn't exist, and finally adding this item's TreeItem
		for(int i = 0; i < sPathSplitList.size(); ++i)
		{
			bool bFound = false;
			for(int j = 0; j < pCurTreeItem->GetNumChildren(); ++j)
			{
				if(QString::compare(sPathSplitList[i], pCurTreeItem->GetChild(j)->data(0).value<TreeModelItemData *>()->GetText(), Qt::CaseInsensitive) == 0)
				{
					pCurTreeItem = pCurTreeItem->GetChild(j);
					bFound = true;
					break;
				}
			}

			if(bFound == false)
			{
				// Still more filters to dig thru, so this means we're at a filter. Add the prefix TreeModelItemData here and continue traversing down the tree
				InsertTreeItem(new TreeModelItemData(ITEM_Filter, sPathSplitList[i]), pCurTreeItem);
				pCurTreeItem = pCurTreeItem->GetChild(pCurTreeItem->GetNumChildren() - 1);
			}
		}
	}

	AssetItemData *pNewItemData = OnAllocateAssetData(metaObj);
	RegisterAsset(pNewItemData);

	InsertTreeItem(pNewItemData, pCurTreeItem);
	return pNewItemData;
}

void IManagerModel::RegisterAsset(AssetItemData *pAsset)
{
	for(int i = 0; i < m_BanksModel.rowCount(); ++i)
	{
		if(pAsset->GetBankId() == m_BanksModel.GetBank(i)->GetId())
		{
			m_BanksModel.GetBank(i)->m_AssetList.append(pAsset);
			break;
		}
	}

	m_AssetUuidMap[pAsset->GetUuid()] = pAsset;

	uint32 uiChecksum = pAsset->GetChecksum();
	if(m_AssetChecksumMap.contains(uiChecksum))
	{
		m_AssetChecksumMap.find(uiChecksum).value().append(pAsset);
		HyGuiLog("'" % pAsset->GetName() % "' is a duplicate of '" % m_AssetChecksumMap.find(uiChecksum).value()[0]->GetName() % "' with the checksum: " % QString::number(uiChecksum) % " totaling: " % QString::number(m_AssetChecksumMap.find(uiChecksum).value().size()), LOGTYPE_Info);
	}
	else
	{
		QList<AssetItemData *> newFrameList;
		newFrameList.append(pAsset);
		m_AssetChecksumMap[uiChecksum] = newFrameList;
	}
}
