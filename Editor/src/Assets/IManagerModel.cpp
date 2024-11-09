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
#include "Harmony.h"
#include "MainWindow.h"
#include "ManagerWidget.h"
#include "ProjectItemData.h"
#include "AssetMimeData.h"
#include "AtlasImportThread.h"
#include "AudioImportThread.h"
#include "SourceImportThread.h"
#include "AtlasRepackThread.h"
#include "AudioRepackThread.h"
#include "AuxAssetInspector.h"
#include "SourceFile.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QMimeData>

IManagerModel::IManagerModel(Project &projRef, AssetManagerType eAssetType) :
	ITreeModel(2, QStringList(), nullptr),
	m_ProjectRef(projRef),
	m_eASSET_TYPE(eAssetType),
	m_bIsSingleBank(false),
	m_MetaDir(m_ProjectRef.GetMetaAbsPath() + HyGlobal::AssetName(eAssetType)),
	m_DataDir(m_ProjectRef.GetAssetsAbsPath() + HyGlobal::AssetName(eAssetType)),
	m_uiNextBankId(99999) // Should be properly initialized with Init()
{
}

/*virtual*/ IManagerModel::~IManagerModel()
{
}

// Init() exists because we need to construct using virtual functions (or after derived ctor initialization)
void IManagerModel::Init()
{
	// Ensure proper data directories exist
	if(m_MetaDir.exists() == false)
	{
		HyGuiLog(HyGlobal::AssetName(m_eASSET_TYPE) % " meta directory is missing, recreating", LOGTYPE_Info);
		m_MetaDir.mkpath(m_MetaDir.absolutePath());
	}
	if(m_DataDir.exists() == false)	// Check this second because some AssetManagers set DataDir to also be MetaDir
	{
		HyGuiLog(HyGlobal::AssetName(m_eASSET_TYPE) % " data directory is missing, recreating", LOGTYPE_Info);
		m_DataDir.mkpath(m_DataDir.absolutePath());
	}

	// Read this manager's banks and assets from existing file, or create a new default bank
	QFile settingsFile(m_MetaDir.absoluteFilePath(HyGlobal::AssetName(m_eASSET_TYPE) % HYGUIPATH_MetaExt));
	if(settingsFile.exists())
	{
		if(!settingsFile.open(QIODevice::ReadOnly))
			HyGuiLog(QString("IManagerModel::IManagerModel() could not open ") % HyGlobal::AssetName(m_eASSET_TYPE) % HYGUIPATH_MetaExt, LOGTYPE_Error);

#ifdef HYGUI_UseBinaryMetaFiles
		QJsonDocument settingsDoc = QJsonDocument::fromBinaryData(settingsFile.readAll());
#else
		QJsonDocument settingsDoc = QJsonDocument::fromJson(settingsFile.readAll());
#endif
		settingsFile.close();

		QJsonObject settingsObj = settingsDoc.object();
		m_uiNextBankId = JSONOBJ_TOINT(settingsObj, "nextBankId");

		QJsonArray bankArray = settingsObj["banks"].toArray();
		for(int i = 0; i < bankArray.size(); ++i)
		{
			QString sName = HyGlobal::MakeFileNameFromCounter(bankArray[i].toObject()["bankId"].toInt());
			BankData *pNewBank = m_BanksModel.AppendBank(m_DataDir.absoluteFilePath(sName), bankArray[i].toObject());

			if(m_eASSET_TYPE == ASSETMAN_Atlases || m_eASSET_TYPE == ASSETMAN_Audio)
				m_DataDir.mkdir(HyGlobal::MakeFileNameFromCounter(pNewBank->GetId()));
		}

		QJsonArray assetsArray = settingsObj["assets"].toArray();
		for(int i = 0; i < assetsArray.size(); ++i)
		{
			QJsonObject assetObj = assetsArray[i].toObject();
			IAssetItemData *pAssetData = CreateAssetTreeItem(assetObj["filter"].toString(), assetObj["name"].toString(), assetObj);

			// Check to see if the actual meta asset exists on disk
			QString sFilePath = m_MetaDir.absoluteFilePath(pAssetData->ConstructMetaFileName());
			if(QFile::exists(sFilePath) == false)
				pAssetData->SetError(ASSETERROR_CannotFindMetaFile);
			else
				pAssetData->ClearError(ASSETERROR_CannotFindMetaFile);
		}
	}
	else
	{
		m_uiNextBankId = 0;
		CreateNewBank("Default");
	}

	OnInit();
}

AssetManagerType IManagerModel::GetAssetType() const
{
	return m_eASSET_TYPE;
}

Project &IManagerModel::GetProjOwner() const
{
	return m_ProjectRef;
}

QAbstractListModel *IManagerModel::GetBanksModel()
{
	return &m_BanksModel;
}

bool IManagerModel::IsSingleBank() const
{
	return m_bIsSingleBank;
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
	return m_BanksModel.GetBank(uiBankIndex)->m_MetaObj;
}

void IManagerModel::SetBankSettings(uint uiBankIndex, QJsonObject newSettingsObj)
{
	m_BanksModel.GetBank(uiBankIndex)->m_MetaObj = newSettingsObj;
}

QList<IAssetItemData *> IManagerModel::GetBankAssets(uint uiBankIndex)
{
	return m_BanksModel.GetBank(uiBankIndex)->m_AssetList;
}

void IManagerModel::GenerateAssetsDlg(const QModelIndex &indexDestination)
{
	OnGenerateAssetsDlg(indexDestination);
}

bool IManagerModel::ImportNewAssets(QStringList sImportList, quint32 uiBankId, QVector<TreeModelItemData *> correspondingParentList, QVector<QUuid> correspondingUuidList)
{
	if(correspondingParentList.size() != sImportList.size() || correspondingUuidList.size() != sImportList.size())
	{
		HyGuiLog("AtlasModel::ImportNewAssets was passed a correspondingParentList/correspondingUuidList that isn't the same size as the sImportList", LOGTYPE_Error);
		return false;
	}
	
	// If there are no assets to import, then we're done
	if(sImportList.isEmpty())
		return true;

	m_ImportedCorrespondingParentList = correspondingParentList;

	IImportThread *pImportThread = nullptr;
	switch(m_eASSET_TYPE)
	{
		case ASSETMAN_Atlases:
			pImportThread = new AtlasImportThread(*this, sImportList, uiBankId, correspondingParentList, correspondingUuidList);
			break;

		case ASSETMAN_Audio:
			pImportThread = new AudioImportThread(*this, sImportList, uiBankId, correspondingParentList, correspondingUuidList);
			break;

		case ASSETMAN_Source:
			pImportThread = new SourceImportThread(*this, sImportList, uiBankId, correspondingParentList, correspondingUuidList);
			break;
			//QList<IAssetItemData *> returnList = OnImportAssets(sImportList, uiBankId, correspondingParentList, correspondingUuidList);
			//OnImportAssetsFinished(returnList, correspondingParentList);
			//return true;

		default:
			HyGuiLog("IManagerModel::ImportNewAssets() 'm_eASSET_TYPE' was unhandled", LOGTYPE_Error);
			return false;
	}

	connect(pImportThread, &QThread::finished, pImportThread, &QObject::deleteLater);
	connect(pImportThread, &IImportThread::ImportUpdate, this, &IManagerModel::OnImportAssetsUpdate);
	connect(pImportThread, &IImportThread::ImportIsFinished, this, &IManagerModel::OnImportAssetsFinished);
	MainWindow::SetLoading(LOADINGTYPE_ImportAssets, 0, 0);
	pImportThread->start();
	
	return true;
}

void IManagerModel::RemoveItems(QList<IAssetItemData *> assetsList, QList<TreeModelItemData *> filtersList, bool bAskToConfirm)
{
	// First loop through and check to see if any links are present, and abort if dependencies are found
	for(int i = 0; i < assetsList.count(); ++i)
	{
		QList<TreeModelItemData *> dependantList = assetsList[i]->GetDependants();// GetDependencies();
		if(dependantList.empty() == false)
		{
			QString sMessage = "'" % assetsList[i]->GetFilter() % "/" % assetsList[i]->GetName() % "' cannot be deleted because it is in use by the following items: \n\n";
			for(TreeModelItemData *pItemData : dependantList)
			{
				if(pItemData->IsProjectItem())
					sMessage.append(HyGlobal::ItemName(pItemData->GetType(), false) % " - " % static_cast<ProjectItemData *>(pItemData)->GetName(true) % "\n");
				else
					sMessage.append(HyGlobal::ItemName(pItemData->GetType(), false) % " - " % pItemData->GetText() % "\n");
			}

			HyGuiLog(sMessage, LOGTYPE_Warning);
			return;
		}
	}

	// Special Case: Close source files that are about to be deleted, abort if they don't close
	if(m_eASSET_TYPE == ASSETMAN_Source)
	{
		for(IAssetItemData *pAsset : assetsList)
		{
			SourceFile *pSrcFile = static_cast<SourceFile *>(pAsset);
			if(pSrcFile->TryCloseAllCodeEditors() == false)
			{
				HyGuiLog("Delete aborted because source file '" % pSrcFile->GetName() % "' is still open", LOGTYPE_Normal);
				return;
			}
		}
	}

	// No dependencies found, resume with deleting
	if(bAskToConfirm && assetsList.size() > 0)
	{
		QString sItemDesc;
		if(assetsList.size() > 1)
		{
			if(m_eASSET_TYPE == ASSETMAN_Source)
				sItemDesc = QString::number(assetsList.size()) % " files";
			else
				sItemDesc = QString::number(assetsList.size()) % " assets";
		}
		else
			sItemDesc = assetsList[0]->GetName();

		if(QMessageBox::No == QMessageBox::question(MainWindow::GetInstance(), "Confirm delete", "Do you want to delete " % sItemDesc % "?", QMessageBox::Yes, QMessageBox::No))
			return;
	}

	QStringList sRemovedFilterPaths;
	for(int i = 0; i < assetsList.size(); ++i)
	{
		sRemovedFilterPaths.push_back(assetsList[i]->GetFilter());

		QModelIndex index = FindIndex<IAssetItemData *>(assetsList[i], 0);
		if(removeRow(index.row(), index.parent()) == false)
			HyGuiLog("IManagerModel::removeRow returned false on: " % assetsList[i]->GetName(), LOGTYPE_Error);
	}
	for(int i = 0; i < filtersList.size(); ++i)
	{
		QModelIndex index = FindIndex<TreeModelItemData *>(filtersList[i], 0);
		if(removeRow(index.row(), index.parent()) == false)
			HyGuiLog("IManagerModel::removeRow returned false on: " % filtersList[i]->GetText(), LOGTYPE_Error);
	}

	// This must be called after the removal of the TreeModelItems or it'll crash within GetItemsRecursively()
	OnRemoveAssets(sRemovedFilterPaths, assetsList);
	FlushRepack();
}

bool IManagerModel::GetAffectedItems(QList<IAssetItemData *> assetsList, QList<ProjectItemData *> &affectedItemListOut) const
{
	ProjectTabBar *pTabBar = m_ProjectRef.GetTabBar();

	affectedItemListOut.clear();
	for(int i = 0; i < assetsList.count(); ++i)
	{
		QList<TreeModelItemData *> assetDependantList = assetsList[i]->GetDependants();
		for(TreeModelItemData *pItemData : assetDependantList)
		{
			if(pItemData->IsProjectItem() == false)
				continue;

			ProjectItemData *pLinkedItem = static_cast<ProjectItemData *>(pItemData);
			affectedItemListOut.append(pLinkedItem);

			// Abort if any of these linked items are currently opened & unsaved
			for(int iTabBarIndex = 0; iTabBarIndex < pTabBar->count(); ++iTabBarIndex)
			{
				QVariant v = pTabBar->tabData(iTabBarIndex);
				ProjectItemData *pOpenItem = v.value<ProjectItemData *>();

				if(pLinkedItem == pOpenItem && pTabBar->tabText(iTabBarIndex).contains('*', Qt::CaseInsensitive))
				{
					QString sMessage = "'" % assetsList[i]->GetName() % "' asset cannot be replaced because an item that references it is currently opened and unsaved:\n" % pOpenItem->GetName(true);
					HyGuiLog(sMessage, LOGTYPE_Warning);
						
					affectedItemListOut.clear();
						
					return false;
				}
			}
		}
	}

	return true;
}

void IManagerModel::ReplaceAssets(QList<IAssetItemData *> assetsList, bool bWithNewAssets)
{
	if(GetAffectedItems(assetsList, m_RepackAffectedItemList) == false)
		return;

	if(bWithNewAssets)
	{
		// Make sure no sub-atlases are being replaced
		for(IAssetItemData *pAsset : assetsList)
		{
			QString sReplaceResult = pAsset->OnReplaceAllowed();
			if(sReplaceResult.isEmpty() == false)
			{
				HyGuiLog(sReplaceResult, LOGTYPE_Warning);
				return;
			}
		}

		// Initialize file dialog to choose new assets as replacement(s)
		QFileDialog dlg(MainWindow::GetInstance());
		dlg.setDirectory("");
		if(assetsList.count() == 1)
		{
			dlg.setFileMode(QFileDialog::ExistingFile);
			dlg.setWindowTitle("Select an asset as the replacement");
		}
		else
		{
			dlg.setFileMode(QFileDialog::ExistingFiles);
			dlg.setWindowTitle("Select " % QString::number(assetsList.count()) % " assets as replacements");
		}
		dlg.setWindowModality(Qt::ApplicationModal);
		dlg.setModal(true);
		QStringList sFilterList = { "*" % assetsList[0]->GetMetaFileExt(), "*.*" };
		dlg.setNameFilters(sFilterList);
		QStringList sImportAssetList;
		do
		{
			if(dlg.exec() == QDialog::Rejected)
				return;

			sImportAssetList = dlg.selectedFiles();

			if(sImportAssetList.count() != assetsList.count())
				HyGuiLog("You must select " % QString::number(assetsList.count()) % " assets", LOGTYPE_Warning);
		} while(sImportAssetList.count() != assetsList.count());

		OnReplaceAssets(sImportAssetList, assetsList);
	}
	else
		OnUpdateAssets(assetsList);

	static_cast<AuxAssetInspector *>(MainWindow::GetAuxWidget(AUXTAB_AssetInspector))->Clear(m_eASSET_TYPE);

	FlushRepack();
}

void IManagerModel::Rename(TreeModelItemData *pItem, QString sNewName)
{
	pItem->SetText(sNewName);
	SaveMeta();
}

bool IManagerModel::TransferAssets(QList<IAssetItemData *> assetsList, uint uiNewBankId)
{
	// Remove any assets that are already in the specified bank
	for(int i = 0; i < assetsList.size();)
	{
		if(assetsList[i]->GetBankId() == uiNewBankId)
			assetsList.removeAt(i);
		else
			++i;
	}

	if(GetAffectedItems(assetsList, m_RepackAffectedItemList) == false)
		return false;

	bool bTransferSucceeded = OnMoveAssets(assetsList, uiNewBankId);
	FlushRepack();

	return bTransferSucceeded;
}

void IManagerModel::AddAssetsToRepack(BankData *pBankData)
{
	if(m_RepackAffectedAssetsMap.find(pBankData) == m_RepackAffectedAssetsMap.end())
		m_RepackAffectedAssetsMap.insert(pBankData, QSet<IAssetItemData *>(pBankData->m_AssetList.begin(), pBankData->m_AssetList.end()));
}

void IManagerModel::AddAssetsToRepack(BankData *pBankData, IAssetItemData *pAsset)
{
	if(m_RepackAffectedAssetsMap.find(pBankData) == m_RepackAffectedAssetsMap.end())
		m_RepackAffectedAssetsMap.insert(pBankData, QSet<IAssetItemData *>());
	
	m_RepackAffectedAssetsMap[pBankData].insert(pAsset);
}

void IManagerModel::AddAssetsToRepack(BankData *pBankData, QSet<IAssetItemData *> &assetsSet)
{
	if(m_RepackAffectedAssetsMap.find(pBankData) == m_RepackAffectedAssetsMap.end())
		m_RepackAffectedAssetsMap.insert(pBankData, assetsSet);
	else
		m_RepackAffectedAssetsMap[pBankData].unite(assetsSet);
}

void IManagerModel::FlushRepack()
{
	OnFlushRepack();

	switch(m_eASSET_TYPE)
	{
	case ASSETMAN_Atlases:
		StartRepackThread("Repacking Atlases", new AtlasRepackThread(m_RepackAffectedAssetsMap, m_MetaDir));
		break;
	case ASSETMAN_Audio:
		StartRepackThread("Repacking Audio", new AudioRepackThread(m_RepackAffectedAssetsMap, m_MetaDir));
		break;
	}
}

QString IManagerModel::AssembleFilter(const TreeModelItemData *pAsset, bool bIncludeSelfIfFilter) const
{
	if(pAsset == nullptr)
		return QString();

	QStringList sPrefixParts;

	if(bIncludeSelfIfFilter && pAsset->GetType() == ITEM_Filter)
		sPrefixParts.append(pAsset->GetText());

	TreeModelItem *pTreeItem = GetItem(FindIndex<TreeModelItemData *>(const_cast<TreeModelItemData *>(pAsset), 0))->GetParent();
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

		if(i != sPrefixParts.size() - 1)
			sPrefix += "/";
	}

	return sPrefix;
}

TreeModelItemData *IManagerModel::FindTreeItemFilter(TreeModelItemData *pItem) const
{
	if(pItem == nullptr)
		return nullptr;

	if(pItem->GetType() == ITEM_Filter)
		return pItem;

	TreeModelItem *pTreeItem = GetItem(FindIndex<TreeModelItemData *>(pItem, 0));
	pTreeItem = pTreeItem->GetParent();
	TreeModelItemData *pFilter = pTreeItem->data(0).value<TreeModelItemData *>();

	if(pFilter == nullptr || pFilter->GetType() != ITEM_Filter)
		return nullptr;
	
	return pFilter;
}

TreeModelItemData *IManagerModel::ReturnFilter(QString sFilterPath, bool bCreateNonExistingFilter /*= true*/)
{
	TreeModelItem *pCurTreeItem = m_pRootItem;
	if(sFilterPath.isEmpty() == false)
	{
		QStringList sPathSplitList = sFilterPath.split(QChar('/'));
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
				if(bCreateNonExistingFilter == false)
					return nullptr;

				// Still more filters to dig thru, so this means we're at a filter. Add the prefix TreeModelItemData here and continue traversing down the tree
				InsertTreeItem(m_ProjectRef, new TreeModelItemData(ITEM_Filter, QUuid(), sPathSplitList[i]), pCurTreeItem);
				pCurTreeItem = pCurTreeItem->GetChild(pCurTreeItem->GetNumChildren() - 1);
			}
		}
	}

	if(pCurTreeItem)
		return pCurTreeItem->data(0).value<TreeModelItemData *>();

	return nullptr;
}

bool IManagerModel::RemoveLookup(IAssetItemData *pAsset)
{
	m_ProjectRef.RemoveItemDataLookup(pAsset->GetUuid());

	for(int i = 0; i < m_BanksModel.rowCount(); ++i)
	{
		if(pAsset->GetBankId() == m_BanksModel.GetBank(i)->GetId())
		{
			m_BanksModel.GetBank(i)->m_AssetList.removeOne(pAsset);
			break;
		}
	}

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

QList<IAssetItemData *> IManagerModel::FindByChecksum(quint32 uiChecksum)
{
	auto iter = m_AssetChecksumMap.find(uiChecksum);
	if(iter == m_AssetChecksumMap.end())
		return QList<IAssetItemData *>();
	else
		return iter.value();
}

bool IManagerModel::DoesAssetExist(quint32 uiChecksum)
{
	return m_AssetChecksumMap.contains(uiChecksum);
}

TreeModelItemData *IManagerModel::CreateNewFilter(QString sName, TreeModelItemData *pParent, bool bSaveMeta)
{
	TreeModelItem *pTreeParent = pParent ? GetItem(FindIndex<TreeModelItemData *>(pParent, 0)) : nullptr;
	TreeModelItemData *pNewFilterData = new TreeModelItemData(ITEM_Filter, QUuid(), sName);
	if(InsertTreeItem(m_ProjectRef, pNewFilterData, pTreeParent))
	{
		if(bSaveMeta)
			SaveMeta();

		return pNewFilterData;
	}

	return nullptr;
}

void IManagerModel::CreateNewBank(QString sName)
{
	QJsonObject bankObj;
	bankObj.insert("bankId", QJsonValue(static_cast<qint64>(m_uiNextBankId)));
	bankObj.insert("bankName", sName);
	
	OnCreateNewBank(bankObj);

	BankData *pNewBank = m_BanksModel.AppendBank(m_DataDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(m_uiNextBankId)), bankObj);
	
	if(m_eASSET_TYPE == ASSETMAN_Atlases || m_eASSET_TYPE == ASSETMAN_Audio)
		m_DataDir.mkdir(HyGlobal::MakeFileNameFromCounter(pNewBank->GetId()));

	m_uiNextBankId++;
	SaveMeta();
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
				//OnDeleteBank(*m_BanksModel.GetBank(i));
				if(m_eASSET_TYPE == ASSETMAN_Atlases)
					m_DataDir.rmdir(HyGlobal::MakeFileNameFromCounter(m_BanksModel.GetBank(i)->GetId()));

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

uint IManagerModel::GetBankIndexFromBankId(quint32 uiBankId) const
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

quint32 IManagerModel::GetBankIdFromBankIndex(uint uiBankIndex) const
{
	return m_BanksModel.GetBank(uiBankIndex)->GetId();
}

void IManagerModel::SaveMeta()
{
	QJsonArray assetsArray;
	QJsonArray banksArray;
	for(int i = 0; i < m_BanksModel.rowCount(); ++i)
	{
		banksArray.append(m_BanksModel.GetBank(i)->m_MetaObj);

		QList<IAssetItemData *> &assetListRef = m_BanksModel.GetBank(i)->m_AssetList;
		for(int j = 0; j < assetListRef.size(); ++j)
		{
			QJsonObject assetObj;
			assetListRef[j]->GetJsonObj(assetObj);
			assetsArray.append(assetObj);
		}
	}

	// Assemble the root QJsonObject for the write
	QJsonObject settingsObj;
	settingsObj.insert("$fileVersion", HYGUI_FILE_VERSION);
	settingsObj.insert("assets", assetsArray);
	settingsObj.insert("banks", banksArray);
	settingsObj.insert("nextBankId", QJsonValue(static_cast<qint64>(m_uiNextBankId)));

	OnSaveMeta(settingsObj);

	QFile settingsFile(m_MetaDir.absoluteFilePath(HyGlobal::AssetName(m_eASSET_TYPE) % HYGUIPATH_MetaExt));
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

	QFile runtimeFile(m_DataDir.absoluteFilePath(HyGlobal::AssetName(m_eASSET_TYPE) % HYGUIPATH_DataExt));
	if(runtimeFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
		HyGuiLog("Couldn't open atlas data info file for writing", LOGTYPE_Error);
	else
	{
		qint64 iBytesWritten = runtimeFile.write(runtimeDoc.toJson());
		if(0 == iBytesWritten || -1 == iBytesWritten)
			HyGuiLog("Could not write save file: " % runtimeFile.errorString(), LOGTYPE_Error);

		runtimeFile.close();
	}
}

/*virtual*/ QVariant IManagerModel::data(const QModelIndex &indexRef, int iRole /*= Qt::DisplayRole*/) const /*override*/
{
	if(indexRef.isValid() == false)
		return QVariant();

	TreeModelItem *pTreeItem = GetItem(indexRef);
	if(pTreeItem == m_pRootItem)
		return QVariant();

	if(iRole == Qt::UserRole)
		return ITreeModel::data(indexRef, iRole);

	TreeModelItemData *pItemData = pTreeItem->data(0).value<TreeModelItemData *>();
	if(pItemData == nullptr)
		return QVariant();

	switch(iRole)
	{
	case Qt::DisplayRole:		// The key data to be rendered in the form of text. (QString)
	case Qt::EditRole:			// The data in a form suitable for editing in an editor. (QString)
		if(indexRef.column() == 0)
			return QVariant(pItemData->GetText());
		else
		{
			if(pItemData->GetType() != ITEM_Filter)
				return QVariant(static_cast<IAssetItemData *>(pItemData)->GetPropertyInfo());// "Bank: " % QString::number(GetBankIndexFromBankId(static_cast<AssetItemData *>(pItemData)->GetBankId())));
			else
			{
				QList<TreeModelItemData *> itemsInFilterList = GetItemsRecursively(indexRef);
				quint32 uiBankId = 0;
				bool bBankIdDetermined = false;
				for(auto pItem : itemsInFilterList)
				{
					if(pItem->GetType() != ITEM_Filter)
					{
						if(bBankIdDetermined == false)
						{
							uiBankId = static_cast<IAssetItemData *>(pItem)->GetBankId();
							bBankIdDetermined = true;
						}
						else if(uiBankId != static_cast<IAssetItemData *>(pItem)->GetBankId())
							return QVariant("Mixed");
					}
				}

				if(m_bIsSingleBank == false)
					return QVariant("Bank: " % QString::number(GetBankIndexFromBankId(uiBankId)));

				return QVariant();
			}
		}

	case Qt::DecorationRole:	// The data to be rendered as a decoration in the form of an icon. (QColor, QIcon or QPixmap)
		if(indexRef.column() == 0)
		{
			if(pItemData->GetType() != ITEM_Filter)
			{
				IAssetItemData *pAsset = static_cast<IAssetItemData *>(pItemData);

				if(pAsset->GetErrors() != 0)
					return QVariant(pItemData->GetIcon(SUBICON_Warning));
				else if(m_eASSET_TYPE == ASSETMAN_Atlases)
				{
					if(static_cast<AtlasFrame *>(pAsset)->GetSubAtlasType() == ITEM_None)
						return QVariant(static_cast<AtlasFrame *>(pAsset)->GetThumbnail());
					else
						return HyGlobal::ItemIcon(static_cast<AtlasFrame *>(pAsset)->GetSubAtlasType(), SUBICON_None);
				}
				else if(m_eASSET_TYPE == ASSETMAN_Source)
					return static_cast<SourceFile *>(pAsset)->GetSourceIcon();
			}
			else if(m_eASSET_TYPE == ASSETMAN_Source && pItemData->GetText() == HySrcEntityFilter)
				return QVariant(QIcon(":/icons16x16/entity-folder.png"));

			return QVariant(pItemData->GetIcon(SUBICON_None));
		}
		return QVariant();

	case Qt::ToolTipRole:		// The data displayed in the item's tooltip. (QString)
		if(pItemData->GetType() != ITEM_Filter)
		{
			IAssetItemData *pAsset = static_cast<IAssetItemData *>(pItemData);
			if(pAsset->GetErrors() != 0)
				return QVariant(HyGlobal::GetGuiFrameErrors(pAsset->GetErrors()));

			return QVariant(m_MetaDir.absoluteFilePath(pAsset->ConstructMetaFileName()));
		}
		return QVariant();

	case Qt::StatusTipRole:		// The data displayed in the status bar. (QString)
		return QVariant(pItemData->GetText());

	default:
		return QVariant();
	}

	return QVariant();
}

/*virtual*/ Qt::ItemFlags IManagerModel::flags(const QModelIndex& indexRef) const /*override*/
{
	TreeModelItemData *pItem = GetItem(indexRef)->data(0).value<TreeModelItemData *>();

	if(indexRef.isValid() == false || pItem == nullptr)
		return QAbstractItemModel::flags(indexRef) | Qt::ItemIsDropEnabled;

	return QAbstractItemModel::flags(indexRef) | Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled;
}

/*virtual*/ Qt::DropActions IManagerModel::supportedDragActions() const /*override*/
{
	return Qt::MoveAction | Qt::LinkAction;
}

/*virtual*/ Qt::DropActions IManagerModel::supportedDropActions() const /*override*/
{
	return Qt::MoveAction;
}

/*virtual*/ QStringList IManagerModel::mimeTypes() const /*override*/
{
	QStringList sMimeTypeList;
	sMimeTypeList << HyGlobal::MimeTypeString(MIMETYPE_AssetItems);

	switch(m_eASSET_TYPE)
	{
	case ASSETMAN_Atlases:
		sMimeTypeList << "image/png";
		break;
	case ASSETMAN_Audio:
		sMimeTypeList << "audio/wav";
		break;
	}

	return sMimeTypeList;
}

/*virtual*/ QMimeData *IManagerModel::mimeData(const QModelIndexList &indexes) const /*override*/
{
	//QList<TreeModelItemData *> assetList;
	//for(const auto &index : indexes)
	//{
	//	if(index.column() != 0)
	//		continue;

	//	assetList.push_back(data(index, Qt::UserRole).value<TreeModelItemData *>());
	//}
	ManagerWidget *pManagerWidget = nullptr;
	switch(m_eASSET_TYPE)
	{
	case ASSETMAN_Source:
		pManagerWidget = m_ProjectRef.GetSourceWidget();
		break;
	case ASSETMAN_Atlases:
		pManagerWidget = m_ProjectRef.GetAtlasWidget();
		break;
	case ASSETMAN_Audio:
		pManagerWidget = m_ProjectRef.GetAudioWidget();
		break;
	default:
		HyGuiLog("IManagerModel::mimeData() - Unknown asset type", LOGTYPE_Error);
		break;
	}
	
	QList<IAssetItemData *> selectedAssetsList; QList<TreeModelItemData *> selectedFiltersList;
	pManagerWidget->GetSelected(selectedAssetsList, selectedFiltersList, true);

	for(TreeModelItemData *pItem : selectedAssetsList)
		selectedFiltersList.append(pItem);

	QMimeData *pNewMimeData = new AssetMimeData(GetProjOwner(), selectedFiltersList, m_eASSET_TYPE);
	return pNewMimeData;
}

/*virtual*/ bool IManagerModel::canDropMimeData(const QMimeData *pData, Qt::DropAction eAction, int iRow, int iColumn, const QModelIndex &parentRef) const /*override*/
{
	if(pData->hasFormat(HyGlobal::MimeTypeString(MIMETYPE_AssetItems)) == false)
		return false;

	return true;
}

/*virtual*/ bool IManagerModel::dropMimeData(const QMimeData *pData, Qt::DropAction eAction, int iRow, int iColumn, const QModelIndex &parentRef) /*override*/
{
	if(eAction == Qt::IgnoreAction)
		return true;
	
	if(eAction != Qt::MoveAction)
	{
		HyGuiLog("dropMimeData isn't MOVEACTION", LOGTYPE_Normal);
		return false;
	}

	// Error check destination index 'indexRef'
	TreeModelItem *pDropItem = GetItem(parentRef);
	TreeModelItemData *pDestFilter = FindTreeItemFilter(pDropItem->data(0).value<TreeModelItemData *>());
	TreeModelItem *pDestFilterTreeItem = GetItem(FindIndex<TreeModelItemData *>(pDestFilter, 0));

	// Parse 'sSrc' for paste information
	QByteArray sSrc = pData->data(HyGlobal::MimeTypeString(MIMETYPE_AssetItems));
	QJsonDocument assetDoc = QJsonDocument::fromJson(sSrc);
	QJsonObject rootAssetObj = assetDoc.object();

	QList<TreeModelItemData *> processedItemDataList; // Used to ensure the same item isn't moved/processed twice (preserves the filter hierarchy)

	QJsonArray assetArray = rootAssetObj[HyGlobal::AssetName(m_eASSET_TYPE)].toArray();

	// NOTE: 'assetArray' is sorted so filters are processed first
	for(int iAssetIndex = 0; iAssetIndex < assetArray.size(); ++iAssetIndex)
	{
		QJsonObject assetObj = assetArray[iAssetIndex].toObject();

		// If asset item is already in the destination project, just simply move it to new filter location
		if(assetObj["project"].toString().toLower() == m_ProjectRef.GetAbsPath().toLower())
		{
			TreeModelItemData *pItemData = nullptr;

			if(assetObj["isFilter"].toBool())
			{
				QString sFilterPath = assetObj["filter"].toString();
				if(sFilterPath.isEmpty() == false)
					sFilterPath += "/";
				sFilterPath += assetObj["name"].toString();

				pItemData = ReturnFilter(sFilterPath, false);
				if(pItemData == nullptr)
				{
					// NOTE: A null returned filter here probably indicates it was a nested filter that was already moved
					continue;
				}
			}
			else
			{
				pItemData = m_ProjectRef.FindItemData(assetObj["assetUUID"].toString());
				if(pItemData == nullptr)
				{
					HyGuiLog("IManagerModel::dropMimeData - could not find by UUID: " % assetObj["assetUUID"].toString(), LOGTYPE_Warning);
					continue;
				}
			}

			// Don't move/process the same item twice or it will flatten the filter hierarchy at the destination
			if(processedItemDataList.contains(pItemData))
				continue;

			QModelIndex sourceIndex = FindIndex<TreeModelItemData *>(pItemData, 0);
			TreeModelItem *pSourceTreeItem = GetItem(sourceIndex);

			// Populate 'processedItemDataList' to indicate this item has been moved/processed
			// All children of 'pSourceTreeItem' will be considered moved/processed because the filter is being moved
			QList<TreeModelItemData *> movedItemsList = GetItemsRecursively(sourceIndex);
			for(int iChildIndex = 0; iChildIndex < movedItemsList.size(); ++iChildIndex)
				processedItemDataList.push_back(movedItemsList[iChildIndex]);

			// Move asset item to new filter location within manager
			QModelIndex destIndex = FindIndex<TreeModelItemData *>(pDestFilter, 0);
			if(sourceIndex.parent() != destIndex)
			{
				beginMoveRows(sourceIndex.parent(), pSourceTreeItem->GetIndex(), pSourceTreeItem->GetIndex(), destIndex, 0);
				pSourceTreeItem->GetParent()->MoveChild(pSourceTreeItem->GetIndex(), pDestFilterTreeItem, 0);
				endMoveRows();
			}

			continue;
		}
		else
		{
			// TODO: Import new assets if not from current project
		}
	}
	if(assetArray.isEmpty() == false)
		SaveMeta();

	return true;
}

void IManagerModel::RegisterAsset(IAssetItemData *pAsset)
{
	m_ProjectRef.AddItemDataLookup(pAsset);

	for(int i = 0; i < m_BanksModel.rowCount(); ++i)
	{
		if(pAsset->GetBankId() == m_BanksModel.GetBank(i)->GetId())
		{
			m_BanksModel.GetBank(i)->m_AssetList.append(pAsset);
			break;
		}
	}

	uint32 uiChecksum = pAsset->GetChecksum();
	if(m_AssetChecksumMap.contains(uiChecksum))
	{
		m_AssetChecksumMap.find(uiChecksum).value().append(pAsset);
		HyGuiLog("'" % pAsset->GetName() % "' is a duplicate of '" % m_AssetChecksumMap.find(uiChecksum).value()[0]->GetName() % "' with the checksum: " % QString::number(uiChecksum) % " totaling: " % QString::number(m_AssetChecksumMap.find(uiChecksum).value().size()), LOGTYPE_Debug);
	}
	else
	{
		QList<IAssetItemData *> newFrameList;
		newFrameList.append(pAsset);
		m_AssetChecksumMap[uiChecksum] = newFrameList;
	}
}

void IManagerModel::DeleteAsset(IAssetItemData *pAsset)
{
	pAsset->RelinquishDependees();

	if(RemoveLookup(pAsset))
		pAsset->DeleteMetaFile();

	delete pAsset;
}

void IManagerModel::MoveAsset(IAssetItemData *pAsset, quint32 uiNewBankId)
{
	for(int i = 0; i < m_BanksModel.rowCount(); ++i)
	{
		if(pAsset->GetBankId() == m_BanksModel.GetBank(i)->GetId())
		{
			m_BanksModel.GetBank(i)->m_AssetList.removeOne(pAsset);
			break;
		}
	}

	for(int i = 0; i < m_BanksModel.rowCount(); ++i)
	{
		if(uiNewBankId == m_BanksModel.GetBank(i)->GetId())
		{
			pAsset->SetBankId(uiNewBankId);
			m_BanksModel.GetBank(i)->m_AssetList.append(pAsset);
			break;
		}
	}
}

void IManagerModel::StartRepackThread(QString sLoadMessage, IRepackThread *pRepackThread)
{
	connect(pRepackThread, &QThread::finished, pRepackThread, &QObject::deleteLater);
	connect(pRepackThread, &IRepackThread::RepackUpdate, this, &IManagerModel::OnRepackUpdate);
	connect(pRepackThread, &IRepackThread::RepackIsFinished, this, &IManagerModel::OnRepackFinished);

	MainWindow::SetLoading(GetLoadingType(), 0, 0);
	pRepackThread->start();
}

IAssetItemData *IManagerModel::CreateAssetTreeItem(QString sPrefix, QString sName, QJsonObject metaObj)
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
				InsertTreeItem(m_ProjectRef, new TreeModelItemData(ITEM_Filter, QUuid(), sPathSplitList[i]), pCurTreeItem);
				pCurTreeItem = pCurTreeItem->GetChild(pCurTreeItem->GetNumChildren() - 1);
			}
		}
	}

	IAssetItemData *pNewItemData = OnAllocateAssetData(metaObj);
	RegisterAsset(pNewItemData);

	InsertTreeItem(m_ProjectRef, pNewItemData, pCurTreeItem);
	return pNewItemData;
}

LoadingType IManagerModel::GetLoadingType() const
{
	switch(m_eASSET_TYPE)
	{
	case ASSETMAN_Atlases:	return LOADINGTYPE_AtlasManager;
	case ASSETMAN_Audio:	return LOADINGTYPE_AudioManager;

	default:
		HyGuiLog("IManagerModel::OnRepackUpdate - unhandled asset type: " % QString::number(m_eASSET_TYPE), LOGTYPE_Error);
	}

	return LOADINGTYPE_Unknown;
}

/*slot*/ void IManagerModel::OnImportAssetsUpdate(int iAssetsLoaded, int iTotalAssets)
{
	MainWindow::SetLoading(LOADINGTYPE_ImportAssets, iAssetsLoaded, iTotalAssets);
}

/*slot*/ void IManagerModel::OnImportAssetsFinished(bool bImportOccured, QString sMsg)
{
	if(bImportOccured)
	{
		for(int i = 0; i < m_ImportedAssetList.size(); ++i)
			InsertTreeItem(m_ProjectRef, m_ImportedAssetList[i], GetItem(FindIndex<TreeModelItemData *>(m_ImportedCorrespondingParentList[i], 0)));

		FlushRepack();
		SaveMeta();
	}

	m_ImportedAssetList.clear();
	m_ImportedCorrespondingParentList.clear();
	MainWindow::ClearLoading(LOADINGTYPE_ImportAssets);

	if(sMsg.isEmpty() == false)
		HyGuiLog(sMsg, LOGTYPE_Warning);
}

/*slot*/ void IManagerModel::OnRepackUpdate(int iBlocksLoaded, int iTotalBlocks)
{
	MainWindow::SetLoading(GetLoadingType(), iBlocksLoaded, iTotalBlocks);
}

/*slot*/ void IManagerModel::OnRepackFinished()
{
	// Set 'LOADINGTYPE_ReloadHarmony' to be loading so that the user can't do anything else until Harmony is reloaded
	MainWindow::SetLoading(LOADINGTYPE_ReloadHarmony, 0, 0);

	m_RepackAffectedAssetsMap.clear();
	MainWindow::ClearLoading(GetLoadingType());

	m_ProjectRef.AddDirtyItems(this, m_RepackAffectedItemList);
	m_RepackAffectedItemList.clear();
	
	if(MainWindow::GetCurrentLoading().size() == 1)
	{
		// Only LOADINGTYPE_ReloadHarmony remains, can now proceed with:
		// - saving the affected items
		// - saving the runtime manifests
		// - reloading Harmony
		m_ProjectRef.ReloadHarmony();
	}
}
