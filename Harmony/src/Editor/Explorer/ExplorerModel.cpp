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
#include "ExplorerItem.h"
#include "Project/Project.h"
#include "Project/ProjectItem.h"
#include "Atlas/AtlasWidget.h"
#include "Harmony/Harmony.h"
#include "Project/ProjectItemMimeData.h"

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

	for(int i = 0; i < m_pRootItem->childCount(); ++i)
	{
		ExplorerItem *pItem = m_pRootItem->child(i)->data(0).value<ExplorerItem *>();
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
		ExplorerItem *pItem = childrenVec[i]->data(0).value<ExplorerItem *>();
		if(pItem->GetType() == ITEM_Prefix && pItem->GetName(true)[0] != HyGuiInternalCharIndicator)
			sReturnPrefixList.append(pItem->GetName(true));
	}

	return sReturnPrefixList;
}

QList<ExplorerItem *> ExplorerModel::GetItemsRecursively(const QModelIndex &indexRef)
{
	QList<ExplorerItem *> returnList;
	
	QStack<TreeModelItem *> treeItemStack;
	treeItemStack.push(static_cast<TreeModelItem *>(indexRef.internalPointer()));
	while(!treeItemStack.isEmpty())
	{
		TreeModelItem *pItem = treeItemStack.pop();
		returnList.push_back(pItem->data(0).value<ExplorerItem *>());

		for(int i = 0; i < pItem->childCount(); ++i)
			treeItemStack.push(pItem->child(i));
	}

	return returnList;
}


ExplorerItem *ExplorerModel::FindItemByItemPath(Project *pProject, QString sPath)
{
	QModelIndex sourceIndex = FindIndexByItemPath(pProject, sPath);
	TreeModelItem *pSourceTreeItem = GetItem(sourceIndex);
	
	return pSourceTreeItem->data(0).value<ExplorerItem *>();
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

	InsertNewItem(pNewProject, m_pRootItem);
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

ExplorerItem *ExplorerModel::AddItem(Project *pProj, HyGuiItemType eNewItemType, const QString sPrefix, const QString sName, QJsonValue initValue, bool bIsPendingSave)
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

	TreeModelItem *pCurTreeItem = FindProjectTreeItem(pProj);
	if(sPrefix.isEmpty() == false)
	{
		QStringList sPathSplitList = sPrefix.split(QChar('/'));
		// Traverse down the tree and add any prefix TreeItem that doesn't exist, and finally adding this item's TreeItem
		for(int i = 0; i < sPathSplitList.size(); ++i)
		{
			bool bFound = false;
			for(int j = 0; j < pCurTreeItem->childCount(); ++j)
			{
				if(QString::compare(sPathSplitList[i], pCurTreeItem->child(j)->data(0).value<ExplorerItem *>()->GetName(false), Qt::CaseInsensitive) == 0)
				{
					pCurTreeItem = pCurTreeItem->child(j);
					bFound = true;
					break;
				}
			}

			if(bFound == false)
			{
				// Still more directories to dig thru, so this means we're at a prefix. Add the prefix ExplorerItem here and continue traversing down the tree
				InsertNewItem(new ExplorerItem(*pProj, ITEM_Prefix, sPathSplitList[i]), pCurTreeItem);
				pCurTreeItem = pCurTreeItem->child(pCurTreeItem->childCount() - 1);
			}
		}
	}

	ExplorerItem *pNewItem = nullptr;
	if(eNewItemType == ITEM_Prefix)
		pNewItem = new ExplorerItem(*pProj, ITEM_Prefix, sName);
	else
		pNewItem = new ProjectItem(*pProj, eNewItemType, sName, initValue, bIsPendingSave);

	InsertNewItem(pNewItem, pCurTreeItem);
	return pNewItem;
}

bool ExplorerModel::RemoveItem(ExplorerItem *pItem)
{
	QModelIndex index = FindIndex<ExplorerItem *>(pItem, 0);
	return removeRow(index.row(), index.parent());
}

QString ExplorerModel::AssemblePrefix(ExplorerItem *pItem) const
{
	QStringList sPrefixParts;

	TreeModelItem *pTreeItem = GetItem(FindIndex<ExplorerItem *>(pItem, 0))->parent();
	while(pTreeItem && pTreeItem != m_pRootItem)
	{
		ExplorerItem *pItem = pTreeItem->data(0).value<ExplorerItem *>();
		if(pItem->GetType() == ITEM_Prefix)
			sPrefixParts.prepend(pItem->GetName(false));

		pTreeItem = pTreeItem->parent();
	}

	QString sPrefix;
	for(int i = 0; i < sPrefixParts.size(); ++i)
	{
		sPrefix += sPrefixParts[i];
		sPrefix += "/";
	}

	return sPrefix;
}

bool ExplorerModel::PasteItemSrc(QByteArray sSrc, const QModelIndex &indexRef)
{
	// Error check destination index 'indexRef'
	TreeModelItem *pDestTreeItem = FindPrefixTreeItem(indexRef);
	if(pDestTreeItem == nullptr)
	{
		HyGuiLog("ExplorerModel::PasteItemSrc failed to get the TreeModelItem from index that was passed in", LOGTYPE_Error);
		return false;
	}

	ExplorerItem *pDestItem = pDestTreeItem->data(0).value<ExplorerItem *>();

	// Destination is known, get project information
	Project *pDestProject = &pDestItem->GetProject();
	QDir metaDir(pDestProject->GetMetaDataAbsPath());
	QDir metaTempDir = HyGlobal::PrepTempDir(pDestProject);

	// Import images into the selected atlas group, or default one
	QSet<AtlasFrame *> importedImageSet;
	quint32 uiAtlasGrpId = pDestProject->GetAtlasModel().GetAtlasGrpIdFromAtlasGrpIndex(0);
	if(pDestProject->GetAtlasWidget())
		uiAtlasGrpId = pDestProject->GetAtlasWidget()->GetSelectedAtlasGrpId();

	// Parse 'sSrc' for paste information
	QJsonDocument pasteDoc = QJsonDocument::fromJson(sSrc);
	QJsonArray pasteArray = pasteDoc.array();
	for(int iPasteIndex = 0; iPasteIndex < pasteArray.size(); ++iPasteIndex)
	{
		QJsonObject pasteObj = pasteArray[iPasteIndex].toObject();

		// If paste item is already in the destination project, move it to new location
		if(pasteObj["project"].toString().toLower() == pDestProject->GetAbsPath().toLower())
		{
			QString sItemPath = pasteObj["itemName"].toString();
			QModelIndex sourceIndex = FindIndexByItemPath(pDestProject, sItemPath);
			TreeModelItem *pSourceTreeItem = GetItem(sourceIndex);
			ExplorerItem *pSourceItem = pSourceTreeItem->data(0).value<ExplorerItem *>();

			// Move paste item to new prefix location within project
			QModelIndex destIndex = FindIndex<ExplorerItem *>(pDestItem, 0);
			if(sourceIndex.parent() != destIndex)
			{
				beginMoveRows(sourceIndex.parent(), pSourceTreeItem->childNumber(), pSourceTreeItem->childNumber(), destIndex, 0);

				pSourceItem->Rename(pDestItem->GetName(true), pSourceItem->GetName(false));
			
				pSourceTreeItem->parent()->removeChildren(pSourceTreeItem->childNumber(), 1);
				pDestTreeItem->insertChildren(0, 1, pDestTreeItem->columnCount());
			
				QVariant v;
				v.setValue<ExplorerItem *>(pSourceItem);
				pDestTreeItem->child(0)->setData(0, v);

				endMoveRows();
			}

			continue;
		}

#if 1
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Pasted item's assets needs to be imported into this project

		// Determine the pasted item type
		HyGuiItemType ePasteItemType = ITEM_Unknown;
		QString sItemType = pasteObj["itemType"].toString();
		QList<HyGuiItemType> typeList = HyGlobal::GetTypeList();
		for(int i = 0; i < typeList.size(); ++i)
		{
			if(sItemType == HyGlobal::ItemName(typeList[i], false))
			{
				ePasteItemType = typeList[i];
				break;
			}
		}

		// Import any missing fonts (.ttf)
		if(ePasteItemType == ITEM_Text)
		{
			QString sFontMetaDir = metaDir.absoluteFilePath(HyGlobal::ItemName(ITEM_Text, true));
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
		}
		// Copy images to meta-temp dir first
		QJsonArray imageArray = pasteObj["images"].toArray();
		for(int i = 0; i < imageArray.size(); ++i)
		{
			QJsonObject imageObj = imageArray[i].toObject();

			if(pDestProject->GetAtlasModel().DoesImageExist(JSONOBJ_TOINT(imageObj, "checksum")) == false)
			{
				// Ensure filename is its metadata name so it's used when imported.
				QFileInfo pasteImageFileInfo(imageObj["uri"].toString());
				QFile::copy(pasteImageFileInfo.absoluteFilePath(), metaTempDir.absolutePath() % "/" % imageObj["name"].toString() % "." % pasteImageFileInfo.suffix());
			}
		}
		// Get string list of the copied images paths
		QStringList importImageList;
		QFileInfoList importFileInfoList = metaTempDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
		for(int i = 0; i < importFileInfoList.size(); ++i)
			importImageList.append(importFileInfoList[i].absoluteFilePath());
		// TODO: Create filters that match the source of the pasted images
		QList<AtlasTreeItem *> correspondingParentList;
		for(int i = 0; i < importImageList.size(); ++i)
			correspondingParentList.push_back(nullptr);


		// Repack this atlas group with imported images
		HyGuiItemType eType;
		switch(ePasteItemType)
		{
		case ITEM_Prefab: eType = ITEM_Prefab; break;
		case ITEM_Text: eType = ITEM_Text; break;
		default:
			eType = ITEM_AtlasImage;
			break;
		}

		QSet<AtlasFrame *> newImportedImagesSet = pDestProject->GetAtlasModel().ImportImages(importImageList, uiAtlasGrpId, eType, correspondingParentList);
		importedImageSet += newImportedImagesSet;

		// Replace any image "id" with the newly imported frames' ids
		if(pasteObj["src"].isArray())
		{
			QJsonArray srcArray = pasteObj["src"].toArray();
			if(srcArray.empty() == false && srcArray[0].isObject() == false)
				HyGuiLog("DataExplorerWidget::PasteItemSrc - src array isn't of QJsonObjects", LOGTYPE_Error);

			// Copy everything into newSrcArray, while replacing "id" with proper value
			QJsonArray newSrcArray;
			for(int i = 0; i < srcArray.size(); ++i)
			{
				QJsonObject srcArrayObj = srcArray[i].toObject();

				srcArrayObj = ReplaceIdWithProperValue(srcArrayObj, importedImageSet);
				newSrcArray.append(srcArrayObj);
			}

			pasteObj["src"] = newSrcArray;
		}
		else if(pasteObj["src"].isObject())
		{
			QJsonObject srcObj = pasteObj["src"].toObject();
			srcObj = ReplaceIdWithProperValue(srcObj, importedImageSet);

			pasteObj["src"] = srcObj;
		}
		else
			HyGuiLog("DataExplorerWidget::PasteItemSrc - src isn't an object or array", LOGTYPE_Error);

		// Create a new project item representing the pasted item and save it
		QFileInfo itemNameFileInfo(pasteObj["itemName"].toString());
		QString sPrefix = itemNameFileInfo.path();
		QString sName = itemNameFileInfo.baseName();
	
		ProjectItem *pImportedProjItem = static_cast<ProjectItem *>(AddItem(pDestProject, ePasteItemType, sPrefix, sName, pasteObj["src"], false));
		pImportedProjItem->Save();
#endif
	}

	if(importedImageSet.empty() == false)
		pDestProject->GetAtlasModel().Repack(pDestProject->GetAtlasModel().GetAtlasGrpIndexFromAtlasGrpId(uiAtlasGrpId), QSet<int>(), importedImageSet);

	return true;
}

/*virtual*/ QVariant ExplorerModel::data(const QModelIndex &indexRef, int iRole /*= Qt::DisplayRole*/) const /*override*/
{
	TreeModelItem *pTreeItem = GetItem(indexRef);
	if(pTreeItem == m_pRootItem)
		return QVariant();

	if(iRole == Qt::UserRole)
		return ITreeModel::data(indexRef, iRole);

	ExplorerItem *pItem = pTreeItem->data(0).value<ExplorerItem *>();
	if(pItem == nullptr)
		return QVariant();

	switch(iRole)
	{
	case Qt::DisplayRole:		// The key data to be rendered in the form of text. (QString)
	case Qt::EditRole:			// The data in a form suitable for editing in an editor. (QString)
		if(pItem->GetType() == ITEM_Project)
			return QVariant(static_cast<Project *>(pItem)->GetGameName());
		else
			return QVariant(pItem->GetName(false));

	case Qt::DecorationRole:	// The data to be rendered as a decoration in the form of an icon. (QColor, QIcon or QPixmap)
		if(pItem->GetType() == ITEM_Project && Harmony::GetProject() == pItem)
			return QVariant(pItem->GetIcon(SUBICON_Activated));
		if(pItem->IsProjectItem())
		{
			ProjectItem *pProjItem = static_cast<ProjectItem *>(pItem);
			if(pProjItem->IsExistencePendingSave())
				return QVariant(pItem->GetIcon(SUBICON_New));
			else if(pProjItem->IsSaveClean() == false)
				return QVariant(pItem->GetIcon(SUBICON_Dirty));
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
	ExplorerItem *pItem = GetItem(indexRef)->data(0).value<ExplorerItem *>();

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
	QList<ExplorerItem *> itemList;
	for(int i = 0; i < indexes.size(); ++i)
	{
		ExplorerItem *pItem = GetItem(indexes[i])->data(0).value<ExplorerItem *>();
		if(pItem)
			itemList.push_back(pItem);
	}

	if(itemList.empty())
		return 0;

	return new ProjectItemMimeData(itemList);
}

/*virtual*/ QStringList ExplorerModel::mimeTypes() const /*override*/
{
	return QStringList() << HYGUI_MIMETYPE;
}

/*virtual*/ bool ExplorerModel::canDropMimeData(const QMimeData *pData, Qt::DropAction eAction, int iRow, int iColumn, const QModelIndex &parentRef) const /*override*/
{
	if(pData->hasFormat(HYGUI_MIMETYPE) == false)
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
	
	if(eAction == Qt::MoveAction)
		return PasteItemSrc(pData->data(HYGUI_MIMETYPE), parentRef);

	HyGuiLog("dropMimeData isn't MOVEACTION", LOGTYPE_Normal);
	return false;
}

/*virtual*/ void ExplorerModel::OnTreeModelItemRemoved(TreeModelItem *pTreeItem) /*override*/
{
	ExplorerItem *pItem = pTreeItem->data(0).value<ExplorerItem *>();
	pItem->DeleteFromProject();
}

bool ExplorerModel::InsertNewItem(ExplorerItem *pNewItem, TreeModelItem *pParentTreeItem, int iRow /*= -1*/)
{
	QModelIndex parentIndex = FindIndex<ExplorerItem *>(pParentTreeItem->data(0).value<ExplorerItem *>(), 0);
	iRow = (iRow == -1 ? pParentTreeItem->childCount() : iRow);

	if(insertRow(iRow, parentIndex) == false)
	{
		HyGuiLog("ExplorerModel::InsertNewItem() - insertRow failed", LOGTYPE_Error);
		return false;
	}

	QVariant v;
	v.setValue<ExplorerItem *>(pNewItem);
	if(setData(index(iRow, 0, parentIndex), v) == false)
		HyGuiLog("ExplorerModel::InsertNewItem() - setData failed", LOGTYPE_Error);

	return true;
}

TreeModelItem *ExplorerModel::FindProjectTreeItem(Project *pProject)
{
	for(int i = 0; i < m_pRootItem->childCount(); ++i)
	{
		if(m_pRootItem->child(i)->data(0).value<Project *>() == pProject)
			return m_pRootItem->child(i);
	}

	return nullptr;
}

TreeModelItem *ExplorerModel::FindPrefixTreeItem(const QModelIndex &indexRef) const
{
	// Error check destination index 'indexRef'
	TreeModelItem *pTreeItem = GetItem(indexRef);
	if(pTreeItem == m_pRootItem)
		return nullptr;

	ExplorerItem *pItem = pTreeItem->data(0).value<ExplorerItem *>();
	if(pItem == nullptr)
		return nullptr;

	// If the explorer item isn't a prefix or a project, go up one parent then and it should be.
	if(pItem->GetType() != ITEM_Prefix && pItem->GetType() != ITEM_Project)
	{
		pTreeItem = pTreeItem->parent();
		pItem = pTreeItem->data(0).value<ExplorerItem *>();
		if(pItem == nullptr || (pItem->GetType() != ITEM_Prefix && pItem->GetType() != ITEM_Project))
			return nullptr;
	}

	return pTreeItem;
}

QModelIndex ExplorerModel::FindIndexByItemPath(Project *pProject, QString sPath)
{
	TreeModelItem *pCurTreeItem = FindProjectTreeItem(pProject);
	if(pCurTreeItem == nullptr)
		return QModelIndex();

	QStringList sPathSplitList = sPath.split(QChar('/'));
	// Traverse down the tree and add any prefix TreeItem that doesn't exist, and finally adding this item's TreeItem
	for(int i = 0; i < sPathSplitList.size(); ++i)
	{
		bool bFound = false;
		for(int j = 0; j < pCurTreeItem->childCount(); ++j)
		{
			if(QString::compare(sPathSplitList[i], pCurTreeItem->child(j)->data(0).value<ExplorerItem *>()->GetName(false), Qt::CaseInsensitive) == 0)
			{
				if(i == sPathSplitList.size() - 1)
				{
					ExplorerItem *pItem = pCurTreeItem->child(j)->data(0).value<ExplorerItem *>();
					return FindIndex<ExplorerItem *>(pItem, 0);
				}

				pCurTreeItem = pCurTreeItem->child(j);
				bFound = true;
				break;
			}
		}

		if(bFound == false)
			return QModelIndex();
	}

	return QModelIndex();
}

QJsonObject ExplorerModel::ReplaceIdWithProperValue(QJsonObject srcObj, QSet<AtlasFrame *> importedFrames)
{
	QStringList srcObjKeyList = srcObj.keys();
	for(int j = 0; j < srcObjKeyList.size(); ++j)
	{
		if(srcObjKeyList[j] == "checksum")
		{
			for(auto iter = importedFrames.begin(); iter != importedFrames.end(); ++iter)
			{
				if((*iter)->GetImageChecksum() == JSONOBJ_TOINT(srcObj, "checksum"))
				{
					srcObj.insert("id", QJsonValue(static_cast<qint64>((*iter)->GetId())));
					break;
				}
			}
			break;
		}
	}

	return srcObj;
}
