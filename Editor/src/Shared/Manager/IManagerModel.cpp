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

IManagerModel::IManagerModel(Project &projRef, HyGuiItemType eItemType) :
	ITreeModel(2, QStringList(), nullptr),
	m_BanksModel(*this),
	m_ProjectRef(projRef),
	m_MetaDir(m_ProjectRef.GetMetaDataAbsPath() + HyGlobal::ItemName(eItemType, true)),
	m_DataDir(m_ProjectRef.GetAssetsAbsPath() + HyGlobal::ItemName(eItemType, true))
{
	if(m_MetaDir.exists() == false)
	{
		HyGuiLog(HyGlobal::ItemName(eItemType, true) % " meta directory is missing, recreating", LOGTYPE_Info);
		m_MetaDir.mkpath(m_MetaDir.absolutePath());
	}
	if(m_DataDir.exists() == false)
	{
		HyGuiLog(HyGlobal::ItemName(eItemType, true) % " data directory is missing, recreating", LOGTYPE_Info);
		m_DataDir.mkpath(m_DataDir.absolutePath());
	}

	QFile settingsFile(m_MetaDir.absoluteFilePath(HyGlobal::ItemName(eItemType, false) % HYGUIPATH_MetaExt));
	if(settingsFile.exists())
	{
		if(!settingsFile.open(QIODevice::ReadOnly))
			HyGuiLog(QString("IManagerModel::IManagerModel() could not open ") % HyGlobal::ItemName(eItemType, false) % HYGUIPATH_MetaExt, LOGTYPE_Error);

#ifdef HYGUI_UseBinaryMetaFiles
		QJsonDocument settingsDoc = QJsonDocument::fromBinaryData(settingsFile.readAll());
#else
		QJsonDocument settingsDoc = QJsonDocument::fromJson(settingsFile.readAll());
#endif
		settingsFile.close();

		QJsonObject settingsObj = settingsDoc.object();

		// TODO: rename to nextFileNameId
		m_uiNextBankId = JSONOBJ_TOINT(settingsObj, "startAtlasId");

		// TODO: rename to banks
		QJsonArray bankArray = settingsObj["groups"].toArray();
		for(int i = 0; i < bankArray.size(); ++i)
		{
			// TODO: rename to bankId
			QString sName = HyGlobal::MakeFileNameFromCounter(bankArray[i].toObject()["atlasGrpId"].toInt());
			m_BanksModel.CreateBank(m_DataDir.absoluteFilePath(sName), bankArray[i].toObject());

			m_RootDataDir.mkdir(sName);
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

			AssetItemData *pAssetData = static_cast<AssetItemData *>(AddTreeItem(false, assetObj["filter"].toString(), assetObj["name"].toString(), assetObj));


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
	for(int i = 0; i < m_AtlasGrpList.size(); ++i)
	{
		QList<AtlasFrame *> &atlasFramesRef = m_AtlasGrpList[i]->m_FrameList;
		for(int j = 0; j < atlasFramesRef.size(); ++j)
			delete atlasFramesRef[j];
	}
}

Project &IManagerModel::GetProjOwner()
{
	return m_ProjectRef;
}

int IManagerModel::GetNumBanks()
{
	return m_BanksModel.rowCount();
}

QString IManagerModel::GetBankName(uint uiBankIndex)
{
	return m_BanksModel.GetBank(uiBankIndex)->m_Settings["txtName"].toString();
}

QJsonObject IManagerModel::GetBankSettings(uint uiBankIndex)
{
	return m_BanksModel.GetBank(uiBankIndex)->m_Settings;
}

void IManagerModel::SetBankSettings(uint uiBankIndex, QJsonObject newSettingsObj)
{
	m_BanksModel.GetBank(uiBankIndex)->m_Settings = newSettingsObj;
}

QList<AssetItemData *> IManagerModel::GetAssets(uint uiBankIndex)
{
	return m_BanksModel.GetBank(uiBankIndex)->m_AssetList;
}

QJsonArray IManagerModel::GetExpandedFiltersArray()
{
	return m_ExpandedFiltersArray;
}

QString IManagerModel::AssembleFilter(const AssetItemData *pAsset) const
{
	QStringList sPrefixParts;

	TreeModelItem *pTreeItem = GetItem(FindIndex<ExplorerItemData *>(pItem, 0))->GetParent();
	while(pTreeItem && pTreeItem != m_pRootItem)
	{
		ExplorerItemData *pItem = pTreeItem->data(0).value<ExplorerItemData *>();
		if(pItem->GetType() == ITEM_Prefix)
			sPrefixParts.prepend(pItem->GetName(false));

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

void IManagerModel::RemoveAsset(AssetItemData *pAsset)
{
	if(m_FrameLookup.RemoveLookup(pFrame))
		pFrame->DeleteMetaImage(m_MetaDir);

	for(int i = 0; i < m_AtlasGrpList.size(); ++i)
	{
		if(m_AtlasGrpList[i]->m_PackerSettings.contains("atlasGrpId") == false) {
			HyGuiLog("AtlasModel::RemoveFrame could not find atlasGrpId", LOGTYPE_Error);
		}
		else if(pFrame->GetAtlasGrpId() == m_AtlasGrpList[i]->GetId())
		{
			m_AtlasGrpList[i]->m_FrameList.removeOne(pFrame);
			break;
		}
	}

	delete pFrame;
}

bool IManagerModel::TransferAsset(AssetItemData *pAsset, uint uiBankIndex)
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

TreeModelItemData *IManagerModel::AddTreeItem(bool bIsFilter, const QString sPrefix, const QString sName, QJsonObject metaObj)
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

	TreeModelItemData *pNewItem = nullptr;
	if(bIsFilter)
		pNewItem = new TreeModelItemData(ITEM_Filter, sName);
	else
	{
		pNewItem = OnAllocateAssetData(metaObj);
		m_AssetLookup.AddLookup(static_cast<AssetItemData *>(pNewItem));

		for(int i = 0; i < m_BanksModel.rowCount(); ++i)
		{
			// TODO: rename to bankId
			if(m_BanksModel.GetBank(i)->m_Settings.contains("atlasGrpId") == false) {
				HyGuiLog("IManagerModel::AddTreeItem could not find 'bankId' in bank's settings", LOGTYPE_Error);
			}
			// TODO: rename to bankId
			quint32 uiBankId = m_BanksModel.GetBank(i)->m_Settings["atlasGrpId"].toInt();
			
			if(static_cast<AssetItemData *>(pNewItem)->GetBankId() == uiBankId)
			{
				m_BanksModel.GetBank(i)->m_AssetList.append(static_cast<AssetItemData *>(pNewItem));
				break;
			}
		}

	}

	InsertTreeItem(pNewItem, pCurTreeItem);
	return pNewItem;
}

bool IManagerModel::InsertTreeItem(TreeModelItemData *pNewItem, TreeModelItem *pParentTreeItem, int iRow /*= -1*/)
{
	QModelIndex parentIndex = FindIndex<TreeModelItemData *>(pParentTreeItem->data(0).value<TreeModelItemData *>(), 0);
	iRow = (iRow == -1 ? pParentTreeItem->GetNumChildren() : iRow);

	if(insertRow(iRow, parentIndex) == false)
	{
		HyGuiLog("ExplorerModel::InsertNewItem() - insertRow failed", LOGTYPE_Error);
		return false;
	}

	QVariant v;
	v.setValue<TreeModelItemData *>(pNewItem);
	if(setData(index(iRow, 0, parentIndex), v, Qt::UserRole) == false)
		HyGuiLog("IManagerModel::InsertNewItem() - setData failed", LOGTYPE_Error);

	return true;
}
