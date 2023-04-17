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

ExplorerItemData *ExplorerModel::FindItemByItemPath(Project *pProject, QString sPath, HyGuiItemType eType)
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

ExplorerItemData *ExplorerModel::AddItem(Project *pProj, HyGuiItemType eNewItemType, const QString sPrefix, const QString sName, FileDataPair initItemFileData, bool bIsPendingSave)
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
				if(QString::compare(sPathSplitList[i], pCurTreeItem->GetChild(j)->data(0).value<ExplorerItemData *>()->GetName(false), Qt::CaseInsensitive) == 0)
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

	ExplorerItemData *pNewItem = nullptr;
	if(eNewItemType == ITEM_Prefix)
		pNewItem = new ExplorerItemData(*pProj, ITEM_Prefix, QUuid(), sName);
	else
	{
		pNewItem = new ProjectItemData(*pProj, eNewItemType, sName, initItemFileData, bIsPendingSave);
		//m_ItemUuidMap[static_cast<ProjectItemData *>(pNewItem)->GetUuid()] = static_cast<ProjectItemData *>(pNewItem);
	}

	InsertTreeItem(*pProj, pNewItem, pCurTreeItem);
	return pNewItem;
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

bool ExplorerModel::PasteItemSrc(const ProjectItemMimeData *pProjMimeData, const QModelIndex &indexRef)
{
	// Error check destination index 'indexRef'
	TreeModelItem *pDestTreeItem = FindPrefixTreeItem(indexRef);
	if(pDestTreeItem == nullptr)
	{
		HyGuiLog("ExplorerModel::PasteItemSrc failed to get the TreeModelItem from index that was passed in", LOGTYPE_Error);
		return false;
	}

	ExplorerItemData *pDestItem = pDestTreeItem->data(0).value<ExplorerItemData *>();

	// Destination is known, get project information
	Project *pDestProject = &pDestItem->GetProject();

	// Parse 'pProjMimeData' for paste information
	QJsonDocument pasteDoc = QJsonDocument::fromJson(pProjMimeData->data(HyGlobal::MimeTypeString(MIMETYPE_ProjectItems)));
	QJsonArray pasteArray = pasteDoc.array();
	for(int iPasteIndex = 0; iPasteIndex < pasteArray.size(); ++iPasteIndex)
	{
		QJsonObject pasteObj = pasteArray[iPasteIndex].toObject();

		HyGuiItemType ePasteItemType = HyGlobal::GetTypeFromString(pasteObj["type"].toString());

		// If paste item is already in the destination project, just simply move it to new location
		if(pasteObj["project"].toString().toLower() == pDestProject->GetAbsPath().toLower())
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
		
		for(int iAssetCount = 0; iAssetCount < NUMASSETTYPES; ++iAssetCount)
		{
			QJsonArray assetArray = pasteObj[HyGlobal::AssetName(static_cast<AssetType>(iAssetCount))].toArray();
			if(pDestProject->PasteAssets(ePasteItemType, assetArray, static_cast<AssetType>(iAssetCount)) == false)
				HyGuiLog("Paste failed to import assets of type: " % QString::number(iAssetCount), LOGTYPE_Error);
		}
		
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
						HyGuiLog("Paste imported font: " % pasteFontFileInfo.fileName(), LOGTYPE_Normal);
					else
						HyGuiLog("Paste failed to imported font: " % pasteFontFileInfo.fileName(), LOGTYPE_Error);
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

		FileDataPair initFileItemData;
		initFileItemData.m_Meta = pasteObj["metaObj"].toObject();
		initFileItemData.m_Meta.insert("UUID", QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces)); // Create new UUID for imported item, so it doesn't conflict with its old project
		initFileItemData.m_Data = pasteObj["dataObj"].toObject();
		ProjectItemData *pImportedProjItem = static_cast<ProjectItemData *>(AddItem(pDestProject,
																			ePasteItemType,
																			sPrefix,
																			sName,
																			initFileItemData,
																			false));
		
		if(pImportedProjItem->Save(true) == false)
			return false;
	}

	return true;
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
		return PasteItemSrc(static_cast<const ProjectItemMimeData *>(pData), parentRef);

	HyGuiLog("dropMimeData isn't MOVEACTION", LOGTYPE_Normal);
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

QModelIndex ExplorerModel::FindIndexByItemPath(Project *pProject, QString sPath, HyGuiItemType eType)
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
