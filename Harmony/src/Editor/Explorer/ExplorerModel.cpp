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
	ITreeModel(new ExplorerItem(), nullptr)
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
		ExplorerItem *pItem = static_cast<ExplorerItem *>(m_pRootItem->GetChild(i));
		Project *pItemProject = static_cast<Project *>(pItem);
		sListOpenProjs.append(pItemProject->GetAbsPath());
	}

	return sListOpenProjs;
}

Project *ExplorerModel::AddProject(const QString sNewProjectFilePath)
{
	HyGuiLog("Opening project: " % sNewProjectFilePath, LOGTYPE_Info);
	Project *pNewProject = new Project(sNewProjectFilePath);
	if(pNewProject->HasError())
	{
		HyGuiLog("Project: " % sNewProjectFilePath % " had an error and will not be opened", LOGTYPE_Error);
		delete pNewProject;
		return nullptr;
	}

	InsertItem(0, pNewProject, m_pRootItem);
	return pNewProject;

	// BELOW BREAKS QTABBAR and UNDOSTACK SIGNAL/SLOT CONNECTIONS (I guess because QObject must be created on main thread?)
	//
	//MainWindow::StartLoading(MDI_Explorer);
	//ExplorerLoadThread *pNewLoadThread = new ExplorerLoadThread(sNewProjectFilePath, this);
	//connect(pNewLoadThread, &ExplorerLoadThread::LoadFinished, this, &DataExplorerWidget::OnProjectLoaded);
	//connect(pNewLoadThread, &ExplorerLoadThread::finished, pNewLoadThread, &QObject::deleteLater);
	//pNewLoadThread->start();
}

ExplorerItem *ExplorerModel::AddItem(Project *pProj, HyGuiItemType eNewItemType, const QString sPrefix, const QString sName, QJsonValue importValue)
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

	//QTreeWidgetItem *pParentTreeItem = pProj->GetTreeItem();
	ExplorerItem *pParentItem = pProj;
	if(sPrefix.isEmpty() == false)
	{
		QStringList sPathSplitList = sPrefix.split(QChar('/'));
		// Traverse down the tree and add any prefix TreeItem that doesn't exist, and finally adding this item's TreeItem
		for(int i = 0; i < sPathSplitList.size(); ++i)
		{
			bool bFound = false;
			for(int j = 0; j < pParentItem->GetNumChildren(); ++j)
			{
				if(QString::compare(sPathSplitList[i], static_cast<ExplorerItem *>(pParentItem->GetChild(j))->GetName(false), Qt::CaseInsensitive) == 0)
				{
					pParentItem = static_cast<ExplorerItem *>(pParentItem->GetChild(j));
					bFound = true;
					break;
				}
			}

			if(bFound == false)
			{
				// Still more directories to dig thru, so this means we're at a prefix. Add the prefix ExplorerItem here and continue traversing down the tree
				ExplorerItem *pNewPrefixItem = new ExplorerItem(*pProj, ITEM_Prefix, sPathSplitList[i]);
				InsertItem(pParentItem->GetNumChildren(), pNewPrefixItem, pParentItem);

				pParentItem = pNewPrefixItem;
			}
		}
	}

	ExplorerItem *pNewItem = nullptr;
	if(eNewItemType == ITEM_Prefix)
	{
		pNewItem = new ExplorerItem(*pProj, ITEM_Prefix, sName);
		InsertItem(pParentItem->GetNumChildren(), pNewItem, pParentItem);
	}
	else
	{
		pNewItem = new ProjectItem(*pProj, eNewItemType, sName, importValue, true);
		InsertItem(pParentItem->GetNumChildren(), pNewItem, pParentItem);

		// New items that are considered "imported" should be saved immediately since they have direct references into the atlas manager
		if(importValue.isNull() == false)
			static_cast<ProjectItem *>(pNewItem)->Save();
	}

	return pNewItem;
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
	AddItem(pProject, ePasteItemType, sPrefix, sName, pasteObj["src"]);
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
				return QVariant(pItem->GetIcon(SUBICON_Pending)); // SUBICON_New
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

///*virtual*/ void ExplorerModel::OnSave() /*override*/
//{
//}
//
///*virtual*/ QJsonObject ExplorerModel::PopStateAt(uint32 uiIndex) /*override*/
//{
//}
//
///*virtual*/ QJsonValue ExplorerModel::GetJson() const /*override*/
//{
//}
//
///*virtual*/ QList<AtlasFrame *> ExplorerModel::GetAtlasFrames() const /*override*/
//{
//}
//
///*virtual*/ QStringList ExplorerModel::GetFontUrls() const /*override*/
//{
//}
//
///*virtual*/ void ExplorerModel::Refresh() /*override*/
//{
//}

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
