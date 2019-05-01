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

ExplorerModel::ExplorerModel() :
	ITreeModel(QStringList(), nullptr)
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
		if(pItem->GetType() == ITEM_Prefix)
			sReturnPrefixList.append(pItem->GetName(true));
	}

	return sReturnPrefixList;
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
	TreeModelItem *pTreeItem = GetItem(FindIndex<ExplorerItem *>(pItem, 0));
	TreeModelItem *pParentTreeItem = pTreeItem->parent();
	return removeRow(pTreeItem->childNumber(), createIndex(pParentTreeItem->childNumber(), 0, pParentTreeItem));
}

QString ExplorerModel::AssemblePrefix(ExplorerItem *pItem)
{
	QStringList sPrefixParts;

	TreeModelItem *pTreeItem = GetItem(FindIndex<ExplorerItem *>(pItem, 0))->parent();
	while(pTreeItem)
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

void ExplorerModel::PasteItemSrc(QByteArray sSrc, Project *pProject, QString sPrefixOverride)
{
	QDir metaDir(pProject->GetMetaDataAbsPath());
	QDir metaTempDir = HyGlobal::PrepTempDir(pProject);

	QJsonDocument pasteDoc = QJsonDocument::fromJson(sSrc);
	QJsonObject pasteObj = pasteDoc.object();

	if(pasteObj["project"].toString().toLower() == pProject->GetAbsPath().toLower())
		return;

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
	if(ePasteItemType == ITEM_Font)
	{
		QString sFontMetaDir = metaDir.absoluteFilePath(HyGlobal::ItemName(ITEM_Font, true));
		QDir fontMetaDir(sFontMetaDir);
		fontMetaDir.mkdir(".");

		QJsonArray fontArray = pasteObj["fonts"].toArray();
		for(int i = 0; i < fontArray.size(); ++i)
		{
			QFileInfo pasteFontFileInfo(fontArray[i].toString());

			QString sAbsFilePath = pasteFontFileInfo.absoluteFilePath();
			if(QFile::copy(sAbsFilePath, sFontMetaDir % "/" % pasteFontFileInfo.fileName()))
				HyGuiLog("Paste Imported font: " % pasteFontFileInfo.fileName(), LOGTYPE_Normal);
		}
	}

	// Copy images to meta-temp dir first
	QJsonArray imageArray = pasteObj["images"].toArray();
	for(int i = 0; i < imageArray.size(); ++i)
	{
		QJsonObject imageObj = imageArray[i].toObject();

		if(pProject->GetAtlasModel().DoesImageExist(JSONOBJ_TOINT(imageObj, "checksum")) == false)
		{
			QFileInfo pasteImageFileInfo(imageObj["uri"].toString());
			QFile::copy(pasteImageFileInfo.absoluteFilePath(), metaTempDir.absolutePath() % "/" % imageObj["name"].toString() % "." % pasteImageFileInfo.suffix());
		}
	}
	// Get string list of the copied images paths
	QStringList importImageList;
	QFileInfoList importFileInfoList = metaTempDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
	for(int i = 0; i < importFileInfoList.size(); ++i)
		importImageList.append(importFileInfoList[i].absoluteFilePath());

	// Import images into the selected atlas group, or default one
	quint32 uiAtlasGrpId = pProject->GetAtlasModel().GetAtlasGrpIdFromAtlasGrpIndex(0);
	if(pProject->GetAtlasWidget())
		uiAtlasGrpId = pProject->GetAtlasWidget()->GetSelectedAtlasGrpId();

	// TODO: Create filters that match the source of the pasted images
	QList<AtlasTreeItem *> correspondingParentList;
	for(int i = 0; i < importImageList.size(); ++i)
		correspondingParentList.push_back(nullptr);


	// Repack this atlas group with imported images
	HyGuiItemType eType;
	switch(ePasteItemType)
	{
	case ITEM_Prefab: eType = ITEM_Prefab; break;
	case ITEM_Font: eType = ITEM_Font; break;
	default:
		eType = ITEM_AtlasImage;
		break;
	}

	QSet<AtlasFrame *> importedFramesSet = pProject->GetAtlasModel().ImportImages(importImageList, uiAtlasGrpId, eType, correspondingParentList);
	//if(importedFramesSet.empty() == false)
	//	pProject->GetAtlasModel().Repack(pProject->GetAtlasModel().GetAtlasGrpIndexFromAtlasGrpId(uiAtlasGrpId), QSet<int>(), importedFramesSet);

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

			srcArrayObj = ReplaceIdWithProperValue(srcArrayObj, importedFramesSet);
			newSrcArray.append(srcArrayObj);
		}

		pasteObj["src"] = newSrcArray;
	}
	else if(pasteObj["src"].isObject())
	{
		QJsonObject srcObj = pasteObj["src"].toObject();
		srcObj = ReplaceIdWithProperValue(srcObj, importedFramesSet);

		pasteObj["src"] = srcObj;
	}
	else
		HyGuiLog("DataExplorerWidget::PasteItemSrc - src isn't an object or array", LOGTYPE_Error);

	// Create a new project item representing the pasted item and save it
	QFileInfo itemNameFileInfo(pasteObj["itemName"].toString());
	QString sPrefix = sPrefixOverride.isEmpty() ? itemNameFileInfo.path() : sPrefixOverride;
	QString sName = itemNameFileInfo.baseName();
	
	ProjectItem *pImportedProjItem = static_cast<ProjectItem *>(AddItem(pProject, ePasteItemType, sPrefix, sName, pasteObj["src"], false));
	pImportedProjItem->Save();
}

/*virtual*/ Qt::DropActions ExplorerModel::supportedDropActions() const /*override*/
{
	return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}

/*virtual*/ QVariant ExplorerModel::headerData(int iSection, Qt::Orientation orientation, int role) const /*override*/
{
	return QVariant();
}

/*virtual*/ int ExplorerModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const /*override*/
{
	return 1;
}

/*virtual*/ QVariant ExplorerModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const /*override*/
{
	ExplorerItem *pItem = static_cast<ExplorerItem *>(index.internalPointer());
	if(pItem == nullptr)
		return QVariant();

	switch(role)
	{
	case Qt::DisplayRole:		// The key data to be rendered in the form of text. (QString)
	case Qt::EditRole:			// The data in a form suitable for editing in an editor. (QString)
		if(pItem->GetType() == ITEM_Project)
			return QVariant(static_cast<Project *>(pItem)->GetGameName());
		else
			return QVariant(pItem->GetName(false));

	case Qt::DecorationRole:	// The data to be rendered as a decoration in the form of an icon. (QColor, QIcon or QPixmap)
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
		return QVariant();

	case Qt::StatusTipRole:		// The data displayed in the status bar. (QString)
		return QVariant(pItem->GetName(true));

	default:
		return QVariant();
	}
}

/*virtual*/ Qt::ItemFlags ExplorerModel::flags(const QModelIndex& index) const /*override*/
{
	return QAbstractItemModel::flags(index);
}

bool ExplorerModel::InsertNewItem(ExplorerItem *pNewItem, TreeModelItem *pParentTreeItem, int iRow /*= -1*/)
{
	iRow = (iRow == -1 ? pParentTreeItem->childCount() : iRow);
	if(insertRow(iRow, createIndex(pParentTreeItem->childNumber(), 0, pParentTreeItem)) == false)
	{
		HyGuiLog("ExplorerModel::InsertNewItem() - insertRow failed", LOGTYPE_Error);
		return false;
	}

	QVariant v;
	v.setValue<ExplorerItem *>(pNewItem);
	if(setData(index(iRow, 0, createIndex(pParentTreeItem->childNumber(), 0, pParentTreeItem)), v) == false)
	{
		HyGuiLog("ExplorerModel::InsertNewItem() - setData failed", LOGTYPE_Error);
		return false;
	}

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
