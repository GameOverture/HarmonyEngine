/**************************************************************************
*	ExplorerModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "ExplorerModel.h"
#include "ExplorerItemData.h"
#include "Project/Project.h"
#include "Project/ProjectItemData.h"
#include "Harmony/Harmony.h"
#include "Project/ProjectItemMimeData.h"
#include "AtlasModel.h"

ExplorerModel::ExplorerModel() :
	ITreeModel(1, QStringList(), nullptr)
{
}


ExplorerModel::~ExplorerModel()
{
}

QStringList ExplorerModel::GetOpenProjectPaths()
{
	QStringList sListOpenProjs;
	sListOpenProjs.clear();

	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		ExplorerItemData *pItem = m_pRootItem->GetChild(i)->data(0).value<ExplorerItemData *>();
		Project *pItemProject = static_cast<Project *>(pItem);
		sListOpenProjs.append(pItemProject->GetAbsPath());
	}

	return sListOpenProjs;
}

QStringList ExplorerModel::GetPrefixList(Project *pProject)
{
	TreeModelItem *pProjectTreeItem = FindProjectTreeItem(pProject);
	if(pProjectTreeItem == nullptr)
		return QStringList();

	QStringList sReturnPrefixList;
	QVector<TreeModelItem *> childrenVec = pProjectTreeItem->GetChildren();
	for(int i = 0; i < childrenVec.size(); ++i)
	{
		ExplorerItemData *pItem = childrenVec[i]->data(0).value<ExplorerItemData *>();
		if(pItem->GetType() == ITEM_Prefix && pItem->GetName(true)[0] != HyGuiInternalCharIndicator)
			sReturnPrefixList.append(pItem->GetName(true));
	}

	return sReturnPrefixList;
}

ExplorerItemData *ExplorerModel::FindItemByItemPath(Project *pProject, QString sPath, ItemType eType)
{
	QModelIndex sourceIndex = FindIndexByItemPath(pProject, sPath, eType);
	TreeModelItem *pSourceTreeItem = GetItem(sourceIndex);
	
	return pSourceTreeItem->data(0).value<ExplorerItemData *>();
}

Project *ExplorerModel::AddProject(const QString sNewProjectFilePath)
{
	HyGuiLog("Opening project: " % sNewProjectFilePath, LOGTYPE_Info);
	Project *pNewProject = new Project(sNewProjectFilePath, *this);
	if(pNewProject->HasError())
	{
		HyGuiLog("Project: " % sNewProjectFilePath % " had an error and will not be opened", LOGTYPE_Error);
		delete pNewProject;
		return nullptr;
	}

	InsertTreeItem(*pNewProject, pNewProject, m_pRootItem);
	pNewProject->LoadExplorerModel();

	return pNewProject;

	// BELOW BREAKS QTABBAR and UNDOSTACK SIGNAL/SLOT CONNECTIONS (I guess because QObject must be created on main thread?)
	//
	//MainWindow::StartLoading(MDI_Explorer);
	//ExplorerLoadThread *pNewLoadThread = new ExplorerLoadThread(sNewProjectFilePath, this);
	//connect(pNewLoadThread, &ExplorerLoadThread::LoadFinished, this, &DataExplorerWidget::OnProjectLoaded);
	//connect(pNewLoadThread, &ExplorerLoadThread::finished, pNewLoadThread, &QObject::deleteLater);
	//pNewLoadThread->start();
}

ExplorerItemData *ExplorerModel::AddItem(Project *pProj, ItemType eNewItemType, const QString sPrefix, const QString sName, FileDataPair initItemFileData, bool bIsPendingSave)
{
	if(pProj == nullptr)
	{
		HyGuiLog("Could not find associated project for item: " % sPrefix % "/" % sName, LOGTYPE_Error);
		return nullptr;
	}
	if(eNewItemType == ITEM_Project)
	{
		HyGuiLog("Do not use WidgetExplorer::AddItem for projects... use AddProjectItem instead", LOGTYPE_Error);
		return nullptr;
	}
	if(HyGlobal::IsItemFileDataValid(initItemFileData) == false)
	{
		HyGuiLog("ExplorerModel::AddItem was passed an invalid FileDataPair", LOGTYPE_Warning);
	}

	TreeModelItem *pCurTreeItem = FindProjectTreeItem(pProj);
	if(sPrefix.isEmpty() == false)
	{

		QStringList sPathSplitList = sPrefix.split(QChar('/'));
		// Traverse down the tree and add any prefix TreeItem that doesn't exist, and finally adding this item's TreeItem
		for(int i = 0; i < sPathSplitList.size(); ++i)
		{
			bool bFound = false;
			for(int j = 0; j < pCurTreeItem->GetNumChildren(); ++j)
			{
				ExplorerItemData *pChildItem = pCurTreeItem->GetChild(j)->data(0).value<ExplorerItemData *>();
				if(QString::compare(sPathSplitList[i], pChildItem->GetName(false), Qt::CaseInsensitive) == 0 &&
					pChildItem->GetType() == ITEM_Prefix)
				{
					pCurTreeItem = pCurTreeItem->GetChild(j);
					bFound = true;
					break;
				}
			}

			if(bFound == false)
			{
				// Still more directories to dig thru, so this means we're at a prefix. Add the prefix ExplorerItemData here and continue traversing down the tree
				InsertTreeItem(*pProj, new ExplorerItemData(*pProj, ITEM_Prefix, QUuid(), sPathSplitList[i]), pCurTreeItem);
				pCurTreeItem = pCurTreeItem->GetChild(pCurTreeItem->GetNumChildren() - 1);
			}
		}
	}

	// Ensure the name is unique at its destination 'pCurTreeItem'
	QString sUniqueName = sName;
	int iConflictCount = 0;
	bool bConflicts = false;
	do 
	{
		bConflicts = false;

		for(int i = 0; i < pCurTreeItem->GetNumChildren(); ++i)
		{
			ExplorerItemData *pChildItem = pCurTreeItem->GetChild(i)->data(0).value<ExplorerItemData *>();
			if(eNewItemType == pChildItem->GetType() && QString::compare(sUniqueName, pChildItem->GetName(false), Qt::CaseInsensitive) == 0)
			{
				bConflicts = true;
				
				sUniqueName = sName;
				sUniqueName += "_Copy";
				if(iConflictCount++ > 0)
					sUniqueName += QString::number(iConflictCount);
			}
		}
	} while(bConflicts);

	ExplorerItemData *pNewItem = nullptr;
	if(eNewItemType == ITEM_Prefix)
		pNewItem = new ExplorerItemData(*pProj, ITEM_Prefix, QUuid(), sUniqueName);
	else
		pNewItem = new ProjectItemData(*pProj, eNewItemType, sUniqueName, initItemFileData, bIsPendingSave);

	InsertTreeItem(*pProj, pNewItem, pCurTreeItem);
	return pNewItem;
}

bool ExplorerModel::AddMimeItem(const ProjectItemMimeData *pProjMimeData, Qt::DropAction eDropAction, const QModelIndex &parentRef)
{
	// Error check parameters
	TreeModelItem *pDestTreeItem = FindPrefixTreeItem(parentRef);
	if(pDestTreeItem == nullptr)
	{
		HyGuiLog("ExplorerModel::AddMimeItem failed to get the TreeModelItem from index that was passed in", LOGTYPE_Error);
		return false;
	}
	switch(eDropAction)
	{
	case Qt::CopyAction:
		HyGuiLog("Pasting items...", LOGTYPE_Normal);
		break;

	case Qt::MoveAction:
		HyGuiLog("Moving items...", LOGTYPE_Normal);
		break;

	case Qt::LinkAction:
		HyGuiLog("Linking items...", LOGTYPE_Normal);
		break;

	default:
		HyGuiLog("ExplorerModel::AddMimeItem was passed an invalid Qt::DropAction", LOGTYPE_Error);
		return false;
	}

	ExplorerItemData *pDestItem = pDestTreeItem->data(0).value<ExplorerItemData *>();
	Project *pDestProject = &pDestItem->GetProject();

	QByteArray jsonData = pProjMimeData->data(HyGlobal::MimeTypeString(MIMETYPE_ProjectItems));
	ProjectItemMimeData::RegenUuids(pDestProject, eDropAction, jsonData); // If from another project, OR eDropAction is COPY, it will modify 'jsonDataOut' and regenerate all UUIDs

	// Parse 'pProjMimeData' for paste information
	QJsonDocument pasteDoc = QJsonDocument::fromJson(jsonData);
	QJsonArray pasteArray = pasteDoc.array();
	
	QJsonArray assetManagerImportList[NUM_ASSETMANTYPES];
	bool bNewItemsCreated = false;
	for(int iPasteIndex = 0; iPasteIndex < pasteArray.size(); ++iPasteIndex)
	{
		QJsonObject pasteObj = pasteArray[iPasteIndex].toObject();
		ItemType ePasteItemType = HyGlobal::GetTypeFromString(pasteObj["type"].toString());

		// If MoveAction AND if paste item is already in the destination project, just simply move it to new location
		if(eDropAction == Qt::MoveAction && pasteObj["project"].toString().toLower() == pDestProject->GetAbsPath().toLower())
		{
			QString sItemPath = pasteObj["name"].toString();
			QModelIndex sourceIndex = FindIndexByItemPath(pDestProject, sItemPath, ePasteItemType);
			TreeModelItem *pSourceTreeItem = GetItem(sourceIndex);
			ExplorerItemData *pSourceItem = pSourceTreeItem->data(0).value<ExplorerItemData *>();

			// Move paste item to new prefix location within project
			QModelIndex destIndex = FindIndex<ExplorerItemData *>(pDestItem, 0);
			if(sourceIndex.parent() != destIndex)
			{
				beginMoveRows(sourceIndex.parent(), pSourceTreeItem->GetIndex(), pSourceTreeItem->GetIndex(), destIndex, 0);

				QString sNewPrefix;
				if(pDestItem->GetType() != ITEM_Project)
					sNewPrefix = pDestItem->GetName(true);

				pSourceItem->Rename(sNewPrefix, pSourceItem->GetName(false));
				pSourceTreeItem->GetParent()->MoveChild(pSourceTreeItem->GetIndex(), pDestTreeItem, 0);

				endMoveRows();
			}

			continue;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Pasted item's assets needs to be imported into this project

		// TODO: if it's a prefix, grab all the items within it and import them?
		if(pasteObj["isPrefix"].toBool())
			continue;

		// Import any missing fonts (.ttf)
		if(pasteObj.contains("fonts"))
		{
			QDir metaDir(pDestProject->GetMetaAbsPath());
			QString sFontMetaDir = metaDir.absoluteFilePath(HYMETA_FontsDir);
			QDir fontMetaDir(sFontMetaDir);
			fontMetaDir.mkdir(".");

			QJsonArray fontArray = pasteObj["fonts"].toArray();
			for(int i = 0; i < fontArray.size(); ++i)
			{
				QFileInfo pasteFontFileInfo(fontArray[i].toString());

				QString sAbsFilePath = pasteFontFileInfo.absoluteFilePath();
				QString sAbsDestPath = sFontMetaDir % "/" % pasteFontFileInfo.fileName();
				if(QFile::exists(sAbsDestPath) == false)
				{
					if(QFile::copy(sAbsFilePath, sAbsDestPath))
						HyGuiLog("Imported font: " % pasteFontFileInfo.fileName(), LOGTYPE_Normal);
					else
						HyGuiLog("Failed to import font: " % pasteFontFileInfo.fileName(), LOGTYPE_Error);
				}
			}
			if(fontArray.empty() == false)
				pDestProject->ScanMetaFontDir();
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Create a new project item representing the pasted item and save it
		QString sItemPath = pasteObj["name"].toString();
		QString sPrefix, sName;
		int iSplitIndex = sItemPath.lastIndexOf('/');
		if(-1 == iSplitIndex)
		{
			sPrefix = "";
			sName = sItemPath;
		}
		else
		{
			sPrefix = sItemPath.left(iSplitIndex);
			sName = sItemPath.right(sItemPath.length() - iSplitIndex - 1);
		}

		// Only preserve the sPrefix if eDropAction is LINK. Otherwise use the destination item's prefix
		if(eDropAction != Qt::LinkAction)
			sPrefix = pDestItem->GetName(true);

		FileDataPair initFileItemData;
		initFileItemData.m_Meta = pasteObj["metaObj"].toObject();
		initFileItemData.m_Data = pasteObj["dataObj"].toObject();
		ProjectItemData *pImportedProjItem = static_cast<ProjectItemData *>(AddItem(pDestProject,
																					ePasteItemType,
																					sPrefix,
																					sName, // NOTE: AddItem() ensures unique name
																					initFileItemData,
																					false));
		pDestProject->AddDirtyItems(nullptr, QList<ProjectItemData *>() << pImportedProjItem);
		HyGuiLog(HyGlobal::ItemName(ePasteItemType, false) % " Created: " % pImportedProjItem->GetName(true), LOGTYPE_Normal);
		bNewItemsCreated = true;

		for(int iAssetType = 0; iAssetType < NUM_ASSETMANTYPES; ++iAssetType)
		{
			QJsonArray assetArray = pasteObj[HyGlobal::AssetName(static_cast<AssetManagerType>(iAssetType))].toArray();
			for(int i = 0; i < assetArray.size(); ++i)
			{
				QJsonObject assetObj = assetArray[i].toObject();
				assetManagerImportList[iAssetType].append(assetObj);
			}
		}
	}

	// Finally import any assets that were part of the paste item - Once packing has finished 'pImportedProjItem' will be saved in the OnRepackFinished() callback
	bool bNewAssetsImported = false;
	for(int iAssetType = 0; iAssetType < NUM_ASSETMANTYPES; ++iAssetType)
	{
		AssetManagerType eAssetType = static_cast<AssetManagerType>(iAssetType);

		int iNumAssetsImported = 0;
		if(pDestProject->PasteAssets(assetManagerImportList[eAssetType], eAssetType, iNumAssetsImported) == false)
			HyGuiLog("Paste failed to import assets of type: " % HyGlobal::AssetName(eAssetType), LOGTYPE_Error);
		else if(iNumAssetsImported > 0)
			bNewAssetsImported = true;
	}

	// If no assets are being imported, then we can save any newly imported dirty project items now
	if(bNewItemsCreated && bNewAssetsImported == false)
		pDestProject->ReloadHarmony();

	return true;
}



bool ExplorerModel::RemoveItem(ExplorerItemData *pItem)
{
	QModelIndex index = FindIndex<ExplorerItemData *>(pItem, 0);
	return removeRow(index.row(), index.parent());
}

QString ExplorerModel::AssemblePrefix(ExplorerItemData *pItem) const
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

/*virtual*/ QVariant ExplorerModel::data(const QModelIndex &indexRef, int iRole /*= Qt::DisplayRole*/) const /*override*/
{
	TreeModelItem *pTreeItem = GetItem(indexRef);
	if(pTreeItem == m_pRootItem)
		return QVariant();

	if(iRole == Qt::UserRole)
		return ITreeModel::data(indexRef, iRole);

	ExplorerItemData *pItem = pTreeItem->data(0).value<ExplorerItemData *>();
	if(pItem == nullptr)
		return QVariant();

	switch(iRole)
	{
	case Qt::DisplayRole:		// The key data to be rendered in the form of text. (QString)
	case Qt::EditRole:			// The data in a form suitable for editing in an editor. (QString)
		if(pItem->GetType() == ITEM_Project)
			return QVariant(static_cast<Project *>(pItem)->GetTitle());
		else
			return QVariant(pItem->GetName(false));

	case Qt::DecorationRole:	// The data to be rendered as a decoration in the form of an icon. (QColor, QIcon or QPixmap)
		if(pItem->GetType() == ITEM_Project && Harmony::GetProject() == pItem)
			return QVariant(pItem->GetIcon(SUBICON_Activated));
		if(pItem->IsProjectItem())
		{
			ProjectItemData *pProjItem = static_cast<ProjectItemData *>(pItem);
			if(pProjItem->IsExistencePendingSave())
				return QVariant(pItem->GetIcon(SUBICON_New));
			else if(pProjItem->IsSaveClean() == false)
				return QVariant(pItem->GetIcon(SUBICON_Dirty));
			else if(pProjItem->HasError())
				return QVariant(pItem->GetIcon(SUBICON_Warning));
		}
		return QVariant(pItem->GetIcon(SUBICON_None));

	case Qt::ToolTipRole:		// The data displayed in the item's tooltip. (QString)
		return QVariant(pItem->GetName(true));

	case Qt::StatusTipRole:		// The data displayed in the status bar. (QString)
		return QVariant(pItem->GetName(true));

	default:
		return QVariant();
	}
}

/*virtual*/ Qt::ItemFlags ExplorerModel::flags(const QModelIndex& indexRef) const /*override*/
{
	ExplorerItemData *pItem = GetItem(indexRef)->data(0).value<ExplorerItemData *>();

	if(indexRef.isValid() == false || pItem == nullptr)
		return QAbstractItemModel::flags(indexRef);

	return QAbstractItemModel::flags(indexRef) | Qt::ItemIsDropEnabled | (pItem->GetType() == ITEM_Project ? 0 : Qt::ItemIsDragEnabled);
}

/*virtual*/ Qt::DropActions ExplorerModel::supportedDragActions() const /*override*/
{
	return Qt::MoveAction | Qt::LinkAction;
}

/*virtual*/ Qt::DropActions ExplorerModel::supportedDropActions() const /*override*/
{
	return Qt::MoveAction;
}

/*virtual*/ QMimeData *ExplorerModel::mimeData(const QModelIndexList &indexes) const /*override*/
{
	QList<ExplorerItemData *> itemList;
	for(const auto &index : indexes)
	{
		if(index.column() != 0)
			continue;

		itemList.push_back(data(index, Qt::UserRole).value<ExplorerItemData *>());
	}

	//RemoveRedundantItems(ITEM_Prefix, itemList);
	if(itemList.empty())
		return nullptr;

	for(auto *pItem : itemList)
	{
		if(pItem->IsProjectItem() && static_cast<ProjectItemData *>(pItem)->IsSaveClean() == false)
		{
			HyGuiLog("Cannot drag project items that are unsaved: " % pItem->GetName(true), LOGTYPE_Warning);
			return nullptr;
		}
	}

	return new ProjectItemMimeData(itemList);
}

/*virtual*/ QStringList ExplorerModel::mimeTypes() const /*override*/
{
	return QStringList() << HyGlobal::MimeTypeString(MIMETYPE_ProjectItems);
}

/*virtual*/ bool ExplorerModel::canDropMimeData(const QMimeData *pData, Qt::DropAction eAction, int iRow, int iColumn, const QModelIndex &parentRef) const /*override*/
{
	if(pData->hasFormat(HyGlobal::MimeTypeString(MIMETYPE_ProjectItems)) == false)
		return false;

	TreeModelItem *pParentTreeItem = FindPrefixTreeItem(parentRef);
	if(pParentTreeItem == nullptr)
		return false;

	return true;
}

/*virtual*/ bool ExplorerModel::dropMimeData(const QMimeData *pData, Qt::DropAction eAction, int iRow, int iColumn, const QModelIndex &parentRef) /*override*/
{
	if(eAction == Qt::IgnoreAction)
		return true;
	
	if(eAction == Qt::MoveAction && pData->hasFormat(HyGlobal::MimeTypeString(MIMETYPE_ProjectItems)))
		return AddMimeItem(static_cast<const ProjectItemMimeData *>(pData), eAction, parentRef);

	HyGuiLog("dropMimeData isn't MOVEACTION", LOGTYPE_Error);
	return false;
}

/*virtual*/ void ExplorerModel::OnTreeModelItemRemoved(TreeModelItem *pTreeItem) /*override*/
{
	ExplorerItemData *pItem = pTreeItem->data(0).value<ExplorerItemData *>();
	
	if(pItem->GetType() != ITEM_Project)
		pItem->DeleteFromProject();
}

//void ExplorerModel::CloseProject(TreeModelItem *pProjectTreeItem)
//{
//	QStack<TreeModelItem *> treeItemStack;
//	QVector<TreeModelItem *> childrenVec = pProjectTreeItem->GetChildren();
//	for(int i = 0; i < childrenVec.size(); ++i)
//	{
//		ExplorerItemData *pItem = childrenVec[i]->data(0).value<ExplorerItemData *>();
//		delete pItem;
//	}
//
//	ExplorerItemData *pProject = pProjectTreeItem->data(0).value<ExplorerItemData *>();
//	delete pProject;
//}

TreeModelItem *ExplorerModel::FindProjectTreeItem(Project *pProject)
{
	for(int i = 0; i < m_pRootItem->GetNumChildren(); ++i)
	{
		if(m_pRootItem->GetChild(i)->data(0).value<Project *>() == pProject)
			return m_pRootItem->GetChild(i);
	}

	return nullptr;
}

TreeModelItem *ExplorerModel::FindPrefixTreeItem(const QModelIndex &indexRef) const
{
	// Error check destination index 'indexRef'
	TreeModelItem *pTreeItem = GetItem(indexRef);
	if(pTreeItem == m_pRootItem)
		return nullptr;

	ExplorerItemData *pItem = pTreeItem->data(0).value<ExplorerItemData *>();
	if(pItem == nullptr)
		return nullptr;

	// If the explorer item isn't a prefix or a project, go up one parent then and it should be.
	if(pItem->GetType() != ITEM_Prefix && pItem->GetType() != ITEM_Project)
	{
		pTreeItem = pTreeItem->GetParent();
		pItem = pTreeItem->data(0).value<ExplorerItemData *>();
		if(pItem == nullptr || (pItem->GetType() != ITEM_Prefix && pItem->GetType() != ITEM_Project))
			return nullptr;
	}

	return pTreeItem;
}

QModelIndex ExplorerModel::FindIndexByItemPath(Project *pProject, QString sPath, ItemType eType)
{
	TreeModelItem *pCurTreeItem = FindProjectTreeItem(pProject);
	if(pCurTreeItem == nullptr)
		return QModelIndex();

	QStringList sPathSplitList = sPath.split(QChar('/'));
	// Traverse down the tree
	for(int i = 0; i < sPathSplitList.size(); ++i)
	{
		bool bFound = false;
		for(int j = 0; j < pCurTreeItem->GetNumChildren(); ++j)
		{
			if(QString::compare(sPathSplitList[i], pCurTreeItem->GetChild(j)->data(0).value<ExplorerItemData *>()->GetName(false), Qt::CaseInsensitive) == 0)
			{
				if(i == sPathSplitList.size() - 1)
				{
					ExplorerItemData *pItem = pCurTreeItem->GetChild(j)->data(0).value<ExplorerItemData *>();
					if(pItem->GetType() == eType)
						return FindIndex<ExplorerItemData *>(pItem, 0);
					else
						continue;
				}

				pCurTreeItem = pCurTreeItem->GetChild(j);
				bFound = true;
				break;
			}
		}

		if(bFound == false)
			return QModelIndex();
	}

	return QModelIndex();
}
