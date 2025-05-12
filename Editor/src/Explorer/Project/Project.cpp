/**************************************************************************
 *	Project.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "Project.h"
#include "SourceModel.h"
#include "AudioManagerModel.h"
#include "MainWindow.h"
#include "ProjectItemMimeData.h"
#include "ExplorerModel.h"
#include "VersionPatcher.h"
#include "ManagerWidget.h"
#include "AtlasModel.h"
#include "GlobalUndoCmds.h"
#include "IAssetItemData.h"
#include "TextModel.h" // For Project::ReloadHarmony() hack
#include "SourceSettingsDlg.h"

#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDirIterator>
#include <QTreeWidgetItemIterator>
#include <QMessageBox>
#include <QStandardPaths>
#include <QProcess>
#include <QDragEnterEvent>
#include <QSettings>

ProjectTabBar::ProjectTabBar(Project *pProjectOwner) :
	m_pProjectOwner(pProjectOwner)
{
}

/*virtual*/ ProjectTabBar::~ProjectTabBar()
{
}

Project *ProjectTabBar::GetProjectOwner()
{
	return m_pProjectOwner;
}

QList<ProjectItemData *> ProjectTabBar::GetCycleOrder()
{
	return m_CycleOrderList;
}

void ProjectTabBar::OnTabBarProjItemDataChanged(ProjectItemData *pItem)
{
	// Insert into m_CycleOrderList if not already in there, or move to front if it is
	if(m_CycleOrderList.contains(pItem))
		m_CycleOrderList.move(m_CycleOrderList.indexOf(pItem), 0);
	else
		m_CycleOrderList.prepend(pItem);
}

void ProjectTabBar::OnTabBarProjItemDataRemoved(ProjectItemData *pItem)
{
	m_CycleOrderList.removeOne(pItem);
}

/*virtual*/ void ProjectTabBar::dragEnterEvent(QDragEnterEvent *pEvent) /*override*/
{
	const QMimeData *pMimeData = pEvent->mimeData();

	if(pMimeData->hasFormat(HyGlobal::MimeTypeString(MIMETYPE_ProjectItems)))
		pEvent->acceptProposedAction();
}

/*virtual*/ void ProjectTabBar::dropEvent(QDropEvent *pEvent) /*override*/
{
	if(pEvent->mimeData()->hasFormat(HyGlobal::MimeTypeString(MIMETYPE_ProjectItems)))
	{
		// Process the data from the event.
		QByteArray dragDataSrc = pEvent->mimeData()->data(HyGlobal::MimeTypeString(MIMETYPE_ProjectItems));
		QJsonDocument userDoc = QJsonDocument::fromJson(dragDataSrc);

		bool bValidDrop = false;
		QJsonArray projItemArray = userDoc.array();
		for(int i = 0; i < projItemArray.size(); ++i)
		{
			QJsonObject itemObj = projItemArray[i].toObject();
			if(itemObj["isPrefix"].toBool())
				continue;

			TreeModelItemData *pTreeModelItemData = m_pProjectOwner->FindItemData(QUuid(itemObj["UUID"].toString()));
			if(pTreeModelItemData && pTreeModelItemData->IsProjectItem() == false)
				continue;

			MainWindow::OpenItem(static_cast<ProjectItemData *>(pTreeModelItemData));
			bValidDrop = true;
		}
			
		if(bValidDrop)
			pEvent->acceptProposedAction();
		else
			pEvent->ignore();
	}
	else
		pEvent->ignore();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Project::Project(const QString sProjectFilePath, ExplorerModel &modelRef) :
	ExplorerItemData(*this, ITEM_Project, QUuid(), HyIO::CleanPath(sProjectFilePath.toStdString(), HyGlobal::ItemExt(ITEM_Project).toStdString()).c_str()),
	m_pDraw(nullptr),
	m_pSourceModel(nullptr),
	m_pSourceWidget(nullptr),
	m_pAtlasModel(nullptr),
	m_pAtlasWidget(nullptr),
	m_pAudioModel(nullptr),
	m_pAudioWidget(nullptr),
	m_pTabBar(nullptr),
	m_pCurOpenItem(nullptr),
	m_bHasError(false),
	m_bExplorerModelLoaded(false)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Parse/Load .hyproj file
	m_SettingsObj = ReadProjFile();
	if(m_bHasError)
		return;

	bool bFilesPatched = VersionPatcher::Run(this);
	m_SettingsObj = ReadProjFile();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	m_pTabBar = new ProjectTabBar(this);
	m_pTabBar->setTabsClosable(true);
	m_pTabBar->setShape(QTabBar::TriangularNorth);
	m_pTabBar->setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
	m_pTabBar->setAcceptDrops(true);
	m_pTabBar->setMovable(true);
	m_pTabBar->connect(m_pTabBar, SIGNAL(currentChanged(int)), this, SLOT(OnTabBarCurrentChanged(int)));
	m_pTabBar->connect(m_pTabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(OnCloseTab(int)));

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	m_pSourceModel = new SourceModel(*this);
	m_pSourceModel->Init();
	if(bFilesPatched)
		m_pSourceModel->SaveMeta();
	SourceSettingsDlg *pDlg = new SourceSettingsDlg(*this, static_cast<BanksModel *>(m_pSourceModel->GetBanksModel())->GetBank(0)->m_MetaObj);
	if(pDlg->IsError())
		HyGuiLog("Project " % GetName(false) % " has invalid build settings.\n\n" % pDlg->GetError() % "\n\nPlease activate project, and resolve in Build -> Build Settings", LOGTYPE_Error);
	delete pDlg;

	m_pAtlasModel = new AtlasModel(*this);
	m_pAtlasModel->Init();

	m_pAudioModel = new AudioManagerModel(*this);
	m_pAudioModel->Init();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	QStringList sFilterList(HYMETA_FontFilterList);
	QMap<QString,QString> fontsMap;

	QStringList sSystemFontPaths = QStandardPaths::standardLocations(QStandardPaths::FontsLocation);
	for(int i = 0; i < sSystemFontPaths.count(); ++i)
	{
		QDir fontDir(sSystemFontPaths[i]);
		QFileInfoList fontFileInfoList = fontDir.entryInfoList(sFilterList);

		for(int i = 0; i < fontFileInfoList.count(); ++i)
			fontsMap[fontFileInfoList[i].fileName()] = fontFileInfoList[i].absoluteFilePath();
	}

	for(auto iter = fontsMap.begin(); iter != fontsMap.end(); ++iter)
	{
		QStandardItem *pFontItem = new QStandardItem(iter.key());
		pFontItem->setData(iter.value());

		m_FontListModel.appendRow(pFontItem);
	}

	QObject::connect(&m_OpenFileWatcher, SIGNAL(fileChanged(const QString &)), this, SLOT(OnFileChanged(const QString &)));

	ScanMetaFontDir();
	HyGlobal::CleanAllTempDirs(*this);
}

/*virtual*/ Project::~Project()
{
	Harmony::OnProjectDestructor(this); // Order matters because this calls Project::HarmonyShutdown()
	delete m_pSourceWidget;
	delete m_pAtlasWidget;
	delete m_pAudioWidget;

	delete m_pDraw;
	
	delete m_pSourceModel;
	delete m_pAtlasModel;
	delete m_pAudioModel;
}

QString Project::GetName() const
{
	QFileInfo fileInfo(m_sName);
	return fileInfo.baseName();
}

/*virtual*/ QString Project::GetName(bool bWithPrefix) const /*override*/
{
	return GetName();
}

void Project::LoadExplorerModel()
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Initialize 'm_ProjectFileData' from the project files on disk
	if(LoadDataObj(GetMetaAbsPath() % HYGUIPATH_ItemsFileName % HYGUIPATH_MetaExt, m_ProjectFileData.m_Meta))
		WriteMetaData();

	if(LoadDataObj(GetAssetsAbsPath() % HYASSETS_DataFile, m_ProjectFileData.m_Data))
		WriteGameData();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool bSystemFontFound = false;

	QStringList sItemTypeList = m_ProjectFileData.m_Data.keys();
	for(int i = 0; i < sItemTypeList.size(); ++i)
	{
		if(sItemTypeList[i] == "$fileVersion")
			continue;

		if(m_ProjectFileData.m_Meta.contains(sItemTypeList[i]) == false ||
		   m_ProjectFileData.m_Data.contains(sItemTypeList[i]) == false)
		{
			HyGuiLog("Item category " % sItemTypeList[i] % " is missing data or metadata", LOGTYPE_Error);
			continue;
		}

		// Determine which enum type of 'HyGuiItemType' - if not found in HyGlobal::GetItemTypeList_Project skip it
		ItemType eItemType = ITEM_Unknown;
		QList<ItemType> typeList = HyGlobal::GetItemTypeList_Project();
		for(int j = 0; j < typeList.size(); ++j)
		{
			ItemType eTmpType = typeList[j];
			if(sItemTypeList[i] == HyGlobal::ItemName(eTmpType, true))
			{
				eItemType = eTmpType;
				break;
			}
		}

		// Skip any items not found in HyGlobal::GetProjItemTypeList()
		if(eItemType == ITEM_Unknown)
			continue;

		QJsonObject metaItemsOfType = m_ProjectFileData.m_Meta[sItemTypeList[i]].toObject();
		QJsonObject dataItemsOfType = m_ProjectFileData.m_Data[sItemTypeList[i]].toObject();

		// Process every item within current type 'eItemType'
		QStringList sItemsList = dataItemsOfType.keys();
		for(int j = 0; j < sItemsList.size(); ++j)
		{
			QString sItemPath = sItemsList[j];

			if(metaItemsOfType.contains(sItemPath) == false ||
			   dataItemsOfType.contains(sItemPath) == false)
			{
				HyGuiLog(sItemPath % " is missing data or metadata", LOGTYPE_Warning);
			}

			FileDataPair itemFileData;
			itemFileData.m_Meta = metaItemsOfType[sItemPath].toObject();
			itemFileData.m_Data = dataItemsOfType[sItemPath].toObject();

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

			if(sPrefix == "+HyInternal" && sName == "+SystemText")
				bSystemFontFound = true;

			MainWindow::GetExplorerModel().AddItem(this, eItemType, sPrefix, sName, itemFileData, false);
		}
	}

	// LoadModel on each project item now needs to be invoked after all the project items have been created,
	// so that any dependency will be able found by Project::FindItemData()
	QModelIndexList indexList = MainWindow::GetExplorerModel().GetAllIndices();
	for(QModelIndex index : indexList)
	{
		TreeModelItemData *pItemData = MainWindow::GetExplorerModel().data(index, Qt::UserRole).value<TreeModelItemData *>();
		if(pItemData->IsProjectItem())
			static_cast<ProjectItemData *>(pItemData)->LoadModel();
	}

	//if(bSystemFontFound == false)
	//{
	//	QDir projGenDataDir(MainWindow::EngineSrcLocation() % HYGUIPATH_ProjGenDir % "data/");
	//	QFile srcFile(projGenDataDir.absoluteFilePath("src.json"));
	//	if(!srcFile.open(QFile::ReadOnly))
	//	{
	//		HyGuiLog("Error reading " % srcFile.fileName() % " when generating default font: " % srcFile.errorString(), LOGTYPE_Error);
	//		m_bHasError = true;
	//	}
	//	else
	//	{
	//		QByteArray sContents = srcFile.readAll();
	//		srcFile.close();

	//		QByteArray sBefore("[HyHarmonyTemplateDataDir]");
	//		QByteArray sAfter(QString(MainWindow::EngineSrcLocation() % HYGUIPATH_ProjGenDir % "data/").toLocal8Bit());
	//		sContents.replace(sBefore, sAfter);

	//		MainWindow::GetExplorerModel().PasteItemSrc(sContents, MainWindow::GetExplorerModel().FindIndex<ExplorerItemData *>(this, 0));
	//	}
	//}

	m_bExplorerModelLoaded = true;
}

QJsonObject Project::ReadProjFile()
{
	QFile projFile(GetAbsPath());
	if(projFile.exists())
	{
		if(!projFile.open(QIODevice::ReadOnly))
		{
			HyGuiLog("Could not open project " % GetAbsPath() % ": " % projFile.errorString(), LOGTYPE_Error);
			m_bHasError = true;
		}
	}
	else
	{
		HyGuiLog("Could not find the project file: " % GetAbsPath(), LOGTYPE_Error);
		m_bHasError = true;
	}

	QJsonDocument settingsDoc = QJsonDocument::fromJson(projFile.readAll());
	projFile.close();

	return settingsDoc.object();
}

void Project::WriteGameData()
{
	QFile dataFile(GetAssetsAbsPath() % HYASSETS_DataFile);
	if(dataFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
		HyGuiLog(QString("Couldn't open ") % HYASSETS_DataFile % " for writing: " % dataFile.errorString(), LOGTYPE_Error);
	else
	{
		m_ProjectFileData.m_Data.insert("$fileVersion", HYGUI_FILE_VERSION);

		QJsonDocument userDoc;
		userDoc.setObject(m_ProjectFileData.m_Data);
		qint64 iBytesWritten = dataFile.write(userDoc.toJson());
		if(0 == iBytesWritten || -1 == iBytesWritten)
		{
			HyGuiLog(QString("Could not write to ") % HYASSETS_DataFile % " file: " % dataFile.errorString(), LOGTYPE_Error);
		}

		dataFile.close();
	}
}

void Project::WriteMetaData()
{
	QFile metaFile(GetMetaAbsPath() % HYGUIPATH_ItemsFileName % HYGUIPATH_MetaExt);
	if(metaFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
		HyGuiLog(QString("Couldn't open ") % HYGUIPATH_ItemsFileName % HYGUIPATH_MetaExt % " for writing: " % metaFile.errorString(), LOGTYPE_Error);
	else
	{
		m_ProjectFileData.m_Meta.insert("$fileVersion", HYGUI_FILE_VERSION);

		QJsonDocument metaDoc;
		metaDoc.setObject(m_ProjectFileData.m_Meta);

#ifdef HYGUI_UseBinaryMetaFiles
		qint64 iBytesWritten = metaFile.write(metaDoc.toBinaryData());
#else
		qint64 iBytesWritten = metaFile.write(metaDoc.toJson());
#endif
		if(0 == iBytesWritten || -1 == iBytesWritten) {
			HyGuiLog("Could not write to meta data file: " % metaFile.errorString(), LOGTYPE_Error);
		}

		metaFile.close();
	}
}

bool Project::HasError() const
{
	return m_bHasError;
}

QJsonObject Project::GetSettingsObj() const
{
	return m_SettingsObj;
}

void Project::SaveSettingsObj(const QJsonObject newSettingsObj)
{
	m_SettingsObj = newSettingsObj;

	QFile settingsFile(GetAbsPath());
	if(settingsFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
	{
		HyGuiLog(QString("Couldn't open ") % GetAbsPath() % " for writing: " % settingsFile.errorString(), LOGTYPE_Error);
		m_bHasError = true;
	}
	else
	{
		QJsonDocument settingsDoc;
		settingsDoc.setObject(m_SettingsObj);
		qint64 iBytesWritten = settingsFile.write(settingsDoc.toJson());
		if(0 == iBytesWritten || -1 == iBytesWritten)
		{
			HyGuiLog(QString("Could not write to ") % GetAbsPath() % " file: " % settingsFile.errorString(), LOGTYPE_Error);
			m_bHasError = true;
		}

		settingsFile.close();
	}
}

QString Project::GetDirPath() const
{
	QFileInfo file(m_sName);
	return file.dir().absolutePath() + '/';
}

QString Project::GetTitle() const
{
	return GetSettingsObj()["Title"].toString();
}

QString Project::GetAbsPath() const
{
	return m_sName;
}

QString Project::GetAssetsAbsPath() const
{
	return QDir::cleanPath(GetDirPath() + '/' + GetSettingsObj()["DataPath"].toString()) + '/';
}

QString Project::GetAssetsRelPath() const
{
	return QDir::cleanPath(GetSettingsObj()["DataPath"].toString()) + '/';
}

QString Project::GetMetaAbsPath() const
{
	return QDir::cleanPath(GetDirPath() + '/' + GetSettingsObj()["MetaPath"].toString()) + '/';
}

QString Project::GetMetaRelPath() const
{
	return QDir::cleanPath(GetSettingsObj()["MetaPath"].toString()) + '/';
}

QString Project::GetSourceAbsPath() const
{
	return QDir::cleanPath(GetDirPath() + '/' + GetSettingsObj()["SourcePath"].toString()) + '/';
}

QString Project::GetSourceRelPath() const
{
	return QDir::cleanPath(GetSettingsObj()["SourcePath"].toString()) + '/';
}

QString Project::GetBuildAbsPath() const
{
	return QDir::cleanPath(GetDirPath() + '/' + GetSettingsObj()["BuildPath"].toString()) + '/';
}

QString Project::GetBuildRelPath() const
{
	return QDir::cleanPath(GetSettingsObj()["BuildPath"].toString()) + '/';
}

QString Project::GetUserAbsPath() const
{
	QDir settingsDir(GetDirPath());
	QFileInfo projFileInfo(GetAbsPath());
	
	return settingsDir.absoluteFilePath(GetName(false) % HYGUIPATH_UserExt);
}

IManagerModel *Project::GetManagerModel(AssetManagerType eManagerType)
{
	switch(eManagerType)
	{
	case ASSETMAN_Source:
		return m_pSourceModel;

	case ASSETMAN_Atlases:
		return m_pAtlasModel;

	case ASSETMAN_Audio:
		return m_pAudioModel;

	default:
		HyGuiLog("Project::GetManagerModel was passed invalid eManagerType", LOGTYPE_Error);
		return nullptr;
	}
}

SourceModel &Project::GetSourceModel()
{
	return *m_pSourceModel;
}

ManagerWidget *Project::GetSourceWidget()
{
	return m_pSourceWidget;
}

AtlasModel &Project::GetAtlasModel()
{
	return *m_pAtlasModel;
}

ManagerWidget *Project::GetAtlasWidget()
{
	return m_pAtlasWidget;
}

AudioManagerModel &Project::GetAudioModel()
{
	return *m_pAudioModel;
}

ManagerWidget *Project::GetAudioWidget()
{
	return m_pAudioWidget;
}

void Project::AddDirtyItems(IManagerModel *pDirtyManager, QList<ProjectItemData *> dirtyProjItemList)
{
	if(pDirtyManager)
		m_DirtyManagerSet.insert(pDirtyManager);

	m_DirtyProjItemSet.unite(QSet<ProjectItemData *>(dirtyProjItemList.begin(), dirtyProjItemList.end()));
}

void Project::ReloadHarmony()
{
	QList<ProjectItemData *> dirtyItemList = m_DirtyProjItemSet.values();
	for(int i = 0; i < dirtyItemList.size(); ++i)
	{
		bool bWriteToDisk = (i == (dirtyItemList.size() - 1));

		dirtyItemList[i]->LoadModel();

		// HACK: Text sub-atlases sometimes need to be rebuilt
		if(dirtyItemList[i]->GetType() == ITEM_Text)
			static_cast<TextModel *>(dirtyItemList[i]->GetModel())->SetRuntimeAtlasDirty();

		if(dirtyItemList[i]->Save(bWriteToDisk) == false)
			HyGuiLog(dirtyItemList[i]->GetName(true) % " failed to save during Project::ReloadHarmony", LOGTYPE_Error);
	}
	m_DirtyProjItemSet.clear();

	QList<IManagerModel *> dirtyManagerList = m_DirtyManagerSet.values();
	for(int i = 0; i < dirtyManagerList.size(); ++i)
	{
		dirtyManagerList[i]->SaveMeta();
		dirtyManagerList[i]->SaveData();
	}

	Harmony::Reload(this);
}

bool Project::PasteAssets(QJsonArray &assetArrayRef, AssetManagerType eAssetType, int &iNumImportedAssetsOut)
{
	iNumImportedAssetsOut = 0;
	if(assetArrayRef.count() == 0)
		return true;

	IManagerModel *pManager = nullptr;
	quint32 uiBankId = 0;
	switch(eAssetType)
	{
	case ASSETMAN_Source:
		pManager = m_pSourceModel;
		uiBankId = 0;
		break;
	case ASSETMAN_Atlases:
		pManager = m_pAtlasModel;
		uiBankId = m_pAtlasWidget ? m_pAtlasWidget->GetSelectedBankId() : 0;
		break;
	case ASSETMAN_Audio:
		pManager = m_pAudioModel;
		uiBankId = m_pAudioWidget ? m_pAudioWidget->GetSelectedBankId() : 0;
		break;
	default:
		HyGuiLog("Project::PasteAssets - Unknown eAssetType: " % QString::number(eAssetType), LOGTYPE_Error);
		break;
	}
	if(pManager == nullptr)
		return false;

	// Copy all valid assets to temp directory, then import them in with corresponding filter and UUID list
	QDir metaTempDir = HyGlobal::PrepTempDir(*this, HYGUIPATH_TEMPSUBDIR_PasteAssets);

	QStringList						importAssetList;
	QVector<TreeModelItemData *>	correspondingParentList;
	QVector<QUuid>					correspondingUuidList;

	if(eAssetType == ASSETMAN_Source)
		static_cast<SourceModel *>(pManager)->m_ImportBaseClassList.clear();

	for(int i = 0; i < assetArrayRef.size(); ++i)
	{
		QJsonObject assetObj = assetArrayRef[i].toObject();

		if(FindItemData(assetObj["assetUUID"].toString()) == nullptr)
		{
			QFileInfo assetFileInfo(assetObj["uri"].toString());

			// Ensure sFilePath is its metadata name so it's used when imported.
			QString sFilePath = metaTempDir.absolutePath() % "/" % assetObj["name"].toString() % "." % assetFileInfo.suffix();
			QFile::copy(assetFileInfo.absoluteFilePath(), sFilePath);

			importAssetList.push_back(sFilePath);
			correspondingParentList.push_back(pManager->ReturnFilter(assetObj["filter"].toString()));
			correspondingUuidList.push_back(QUuid(assetObj["assetUUID"].toString())); // The UUID has already been re-created for this imported asset if moving to another project (so it doesn't conflict with its old project)

			if(eAssetType == ASSETMAN_Source)
				static_cast<SourceModel *>(pManager)->m_ImportBaseClassList.push_back(assetObj["baseClass"].toString());
		}
	}

	iNumImportedAssetsOut = importAssetList.size();
	pManager->ImportNewAssets(importAssetList, uiBankId, correspondingParentList, correspondingUuidList);

	return true;
}

//void Project::SetAudioModel(QJsonObject audioObj)
//{
//	QString sItemTypeName = HyGlobal::ItemName(ITEM_Audio, true);
//	if(m_ProjectFileData.m_Meta.contains(sItemTypeName) == false)
//	{
//		HyGuiLog("Project::SetAudioModel could not find item type: " % sItemTypeName, LOGTYPE_Error);
//		return;
//	}
//
//	m_ProjectFileData.m_Meta.remove(sItemTypeName);
//	m_ProjectFileData.m_Meta.insert(sItemTypeName, audioObj);
//
//	WriteMetaData();
//}

QStandardItemModel *Project::GetFontListModel()
{
	return &m_FontListModel;
}

void Project::ScanMetaFontDir()
{
	QStringList sFilterList(HYMETA_FontFilterList);
	QDir metaDir(GetMetaAbsPath() % HYMETA_FontsDir);
	QFileInfoList metaFontFileInfoList = metaDir.entryInfoList(sFilterList);

	QMap<QString,QString> metaFontsMap;
	for(int i = 0; i < metaFontFileInfoList.count(); ++i)
		metaFontsMap[metaFontFileInfoList[i].fileName()] = metaFontFileInfoList[i].absoluteFilePath();

	for(auto iter = metaFontsMap.begin(); iter != metaFontsMap.end(); ++iter)
	{
		QStandardItem *pFontItem = new QStandardItem(HyGlobal::ItemIcon(ITEM_Text, SUBICON_None), iter.key());
		pFontItem->setData(iter.value());

		// TODO: Prepend this item, not append
		m_FontListModel.appendRow(pFontItem);
		//m_FontListModel.setItem(0, pFontItem);
	}
}

ProjectTabBar *Project::GetTabBar()
{
	return m_pTabBar;
}

ProjectItemData *Project::GetCurrentOpenItem()
{
	return m_pCurOpenItem;
}

ProjectDraw *Project::GetProjDraw()
{
	return m_pDraw;
}

void Project::SetRenderSize(int iWidth, int iHeight)
{
	if(m_pTabBar && m_pTabBar->count() != 0)
	{
		if(m_pTabBar->currentIndex() >= 0)
		{
			IDraw *pDraw = m_pTabBar->tabData(m_pTabBar->currentIndex()).value<ProjectItemData *>()->GetDraw();
			if(pDraw)
				pDraw->ResizeRenderer();
		}
	}

	if(m_pDraw)
		m_pDraw->ResizeRenderer();
}

void Project::ShowGridBackground(bool bShow)
{
	if(m_pDraw == nullptr)
		return;

	m_pDraw->EnableGridBackground(bShow);
}

void Project::ShowGridOrigin(bool bShow)
{
	if(m_pDraw == nullptr)
		return;

	m_pDraw->EnableGridOrigin(bShow);
}

void Project::ShowGridOverlay(bool bShow)
{
	if(m_pDraw == nullptr)
		return;

	m_pDraw->EnableGridOverlay(bShow);
}

uint32 Project::GetSnappingSettings() const
{
	return m_uiSnappingSettings;
}

void Project::SetSnappingSettings(uint32 uiSnappingSettings, bool bSaveUserData)
{
	m_uiSnappingSettings = uiSnappingSettings;
	if(bSaveUserData)
		SaveUserData();
}

void Project::SaveItemData(ItemType eType, QString sPath, const FileDataPair &itemFileDataRef, bool bWriteToDisk)
{
	QString sItemTypeName = HyGlobal::ItemName(eType, true);
	if(m_ProjectFileData.m_Data.contains(sItemTypeName) == false) {
		HyGuiLog("Project::SaveItemData could not find item type: " % sItemTypeName, LOGTYPE_Error);
	}

	QJsonObject metaItemTypeObj = m_ProjectFileData.m_Meta[sItemTypeName].toObject();
	metaItemTypeObj.remove(sPath);
	metaItemTypeObj.insert(sPath, itemFileDataRef.m_Meta);
	m_ProjectFileData.m_Meta.remove(sItemTypeName);
	m_ProjectFileData.m_Meta.insert(sItemTypeName, metaItemTypeObj);

	QJsonObject dataItemTypeObj = m_ProjectFileData.m_Data[sItemTypeName].toObject();
	dataItemTypeObj.remove(sPath);
	dataItemTypeObj.insert(sPath, itemFileDataRef.m_Data);
	m_ProjectFileData.m_Data.remove(sItemTypeName);
	m_ProjectFileData.m_Data.insert(sItemTypeName, dataItemTypeObj);

	if(bWriteToDisk)
	{
		WriteMetaData();
		WriteGameData();
	}
}

void Project::DeleteItemData(ItemType eType, QString sPath, bool bWriteToDisk)
{
	DeleteItemInDataObj(eType, sPath, m_ProjectFileData.m_Meta);
	DeleteItemInDataObj(eType, sPath, m_ProjectFileData.m_Data);

	if(bWriteToDisk)
	{
		WriteMetaData();
		WriteGameData();
	}
	
	// If open, make sure to close as it's been deleted from project
	for(int i = 0; i < m_pTabBar->count(); ++i)
	{
		QVariant v = m_pTabBar->tabData(i);
		ProjectItemData *pItem = v.value<ProjectItemData *>();
		
		if(pItem->GetName(true) == sPath)
		{
			ProjectItemData *pItem = m_pTabBar->tabData(i).value<ProjectItemData *>();
			MainWindow::CloseItem(pItem);
			break;
		}
	}
}

// Return 'true' if the data obj needs to save to disk
bool Project::LoadDataObj(QString sFilePath, QJsonObject &dataObjRef)
{
	QFile dataFile(sFilePath);
	if(dataFile.exists())
	{
		if(!dataFile.open(QIODevice::ReadOnly))
		{
			HyGuiLog("Project::LoadDataObj() could not open " % m_sName % "'s " % QFileInfo(sFilePath).fileName() % " file for project: " % dataFile.errorString(), LOGTYPE_Error);
			m_bHasError = true;
			return false; // Don't write with invalid object
		}

		QJsonDocument userDoc = QJsonDocument::fromJson(dataFile.readAll());
		dataFile.close();

		dataObjRef = userDoc.object();
	}

	// Ensure save object has all the valid types in map
	bool bTypeNotFound = false;
	QList<ItemType> typeList = HyGlobal::GetItemTypeList_Project();
	for(int i = 0; i < typeList.size(); ++i)
	{
		QString sTypeName = HyGlobal::ItemName(typeList[i], true);
		if(dataObjRef.contains(sTypeName) == false)
		{
			bTypeNotFound = true;
			dataObjRef.insert(sTypeName, QJsonObject());
		}
	}

	return dataFile.exists() == false || bTypeNotFound;
}

void Project::DeleteItemInDataObj(ItemType eType, QString sPath, QJsonObject &dataObjRef)
{
	QString sItemTypeName = HyGlobal::ItemName(eType, true);
	if(dataObjRef.contains(sItemTypeName) == false)
		HyGuiLog("Project::DeleteItemInDataObj could not find item type: " % sItemTypeName, LOGTYPE_Error);

	QJsonObject itemTypeObject = dataObjRef[sItemTypeName].toObject();

	// If this item is "existence pending" it won't be found in commented code below
	//if(itemTypeObject.contains(sPath) == false)
	//	HyGuiLog("Project::DeleteItemInDataObj could not find item: " % sPath, LOGTYPE_Error);

	itemTypeObject.remove(sPath);
	dataObjRef[sItemTypeName] = itemTypeObject;
}

void Project::DeletePrefixAndContents(QString sPrefix, bool bWriteToDisk)
{
	bool bItemsDeleted = false;

	QList<ItemType> typeList = HyGlobal::GetItemTypeList_Project();
	for(auto itemTypeIter = m_ProjectFileData.m_Data.begin(); itemTypeIter != m_ProjectFileData.m_Data.end(); ++itemTypeIter)
	{
		ItemType eType = ITEM_Unknown;
		for(int i = 0; i < typeList.size(); ++i)
		{
			if(itemTypeIter.key() == HyGlobal::ItemName(typeList[i], true))
			{
				eType = typeList[i];
				break;
			}
		}
		if(eType == ITEM_Unknown)
			continue;

		QJsonObject itemObj = itemTypeIter.value().toObject();
		for(auto iter = itemObj.begin(); iter != itemObj.end(); ++iter)
		{
			QFileInfo itemInfo;
			itemInfo.setFile(iter.key());
			QString sCurPrefix = itemInfo.path();

			if(sCurPrefix == sPrefix)
			{
				DeleteItemData(eType, iter.key(), false);
				bItemsDeleted = true;
			}
		}
	}

	if(bWriteToDisk && bItemsDeleted)
	{
		WriteMetaData();
		WriteGameData();
	}
}

QString Project::RenameItem(ItemType eType, QString sOldPath, QString sNewPath)
{
	if(eType == ITEM_Prefix)
	{
		HyGuiLog("Project::RenameItem invoked on a Prefix", LOGTYPE_Error);
		return sOldPath.section('/', -1);
	}

	QString sItemTypeName = HyGlobal::ItemName(eType, true);
	if(m_ProjectFileData.m_Data.contains(sItemTypeName) == false)
	{
		HyGuiLog("Project::RenameItemInDataObj could not find item type: " % sItemTypeName, LOGTYPE_Error);
		return sOldPath.section('/', -1);
	}
	QJsonObject itemTypeDataObj = m_ProjectFileData.m_Data[sItemTypeName].toObject();

	if(m_ProjectFileData.m_Meta.contains(sItemTypeName) == false)
	{
		HyGuiLog("Project::RenameItemInDataObj could not find item type (in meta): " % sItemTypeName, LOGTYPE_Error);
		return sOldPath.section('/', -1);
	}
	QJsonObject itemTypeMetaObj = m_ProjectFileData.m_Meta[sItemTypeName].toObject();

	// Ensure there are no name conflicts
	QString sUniqueNewPath = sNewPath;
	int iConflictCount = 0;
	bool bConflicts = false;
	do
	{
		bConflicts = false;

		for(auto iter = itemTypeDataObj.begin(); iter != itemTypeDataObj.end(); ++iter)
		{
			if(sUniqueNewPath.compare(iter.key(), Qt::CaseInsensitive) == 0)
			{
				sUniqueNewPath = sNewPath;
				sUniqueNewPath += "_Copy";
				if(iConflictCount++ > 0)
					sUniqueNewPath += QString::number(iConflictCount);

				bConflicts = true;
				break;
			}
		}

		// Nothing should clash below - optional check
		for(auto iter = itemTypeMetaObj.begin(); iter != itemTypeMetaObj.end(); ++iter)
		{
			if(sUniqueNewPath.compare(iter.key(), Qt::CaseInsensitive) == 0)
			{
				sUniqueNewPath = sNewPath;
				sUniqueNewPath += "_Copy";
				if(iConflictCount++ > 0)
					sUniqueNewPath += QString::number(iConflictCount);

				bConflicts = true;
				break;
			}
		}
	} while(bConflicts);

	if(iConflictCount > 0)
		HyGuiLog("Item was renamed to avoid conflict: " % sUniqueNewPath, LOGTYPE_Info);

	RenameItemInDataObj(eType, sOldPath, sUniqueNewPath, m_ProjectFileData.m_Meta);
	RenameItemInDataObj(eType, sOldPath, sUniqueNewPath, m_ProjectFileData.m_Data);

	RefreshNamesOnTabs();
	WriteGameData();
	WriteMetaData();

	HyGuiLog(sOldPath % " is now known as: " % sUniqueNewPath, LOGTYPE_Normal);

	return sUniqueNewPath.section('/', -1);
}

QString Project::RenamePrefix(QString sOldPath, QString sNewPath)
{
	RenamePrefixInDataObj(sOldPath, sNewPath, m_ProjectFileData.m_Meta);
	RenamePrefixInDataObj(sOldPath, sNewPath, m_ProjectFileData.m_Data);

	RefreshNamesOnTabs();
	WriteGameData();
	WriteMetaData();

	return sNewPath.section('/', -1);
}

bool Project::DoesItemExist(ItemType eType, QString sPath) const
{
	QJsonObject subDirObj = m_ProjectFileData.m_Data;
	for(auto objsInSubDirIter = subDirObj.begin(); objsInSubDirIter != subDirObj.end(); ++objsInSubDirIter)
	{
		QString sItemPath = objsInSubDirIter.key();
		if(0 == sPath.compare(sItemPath, Qt::CaseInsensitive))
			return true;
	}

	return false;
}

const QMap<QUuid, TreeModelItemData *> &Project::GetItemMap() const
{
	return m_ItemDataUuidMap;
}

void Project::SaveUserData() const
{
	// Save asset manager widgets expanded/selected state
	QSettings settings(GetUserAbsPath(), QSettings::IniFormat);
	settings.beginGroup("AssetManagers");
	{
		settings.setValue("TabIndex", MainWindow::GetAssetManagerTabIndex());

		settings.setValue(HyGlobal::AssetName(ASSETMAN_Source), m_pSourceWidget->GetExpandedFilters());
		settings.setValue(HyGlobal::AssetName(ASSETMAN_Atlases), m_pAtlasWidget->GetExpandedFilters());
		settings.setValue(HyGlobal::AssetName(ASSETMAN_Atlases) + "BankIndex", m_pAtlasWidget->GetSelectedBankIndex());
		settings.setValue(HyGlobal::AssetName(ASSETMAN_Atlases) + "ShowAllBanks", m_pAtlasWidget->IsShowAllBanksChecked());
		settings.setValue(HyGlobal::AssetName(ASSETMAN_Audio), m_pAudioWidget->GetExpandedFilters());
		settings.setValue(HyGlobal::AssetName(ASSETMAN_Audio) + "BankIndex", m_pAudioWidget->GetSelectedBankIndex());
		settings.setValue(HyGlobal::AssetName(ASSETMAN_Audio) + "ShowAllBanks", m_pAudioWidget->IsShowAllBanksChecked());
	}
	settings.endGroup();
	settings.beginGroup("ProjectSettings");
	{
		bool bShowGridBackground, bShowGridOrigin, bShowGridOverlay;
		MainWindow::GetGridStatus(bShowGridBackground, bShowGridOrigin, bShowGridOverlay);
		settings.setValue("ShowGridBackground", bShowGridBackground);
		settings.setValue("ShowGridOrigin", bShowGridOrigin);
		settings.setValue("ShowGridOverlay", bShowGridOverlay);

		settings.setValue("SnappingSettings", m_uiSnappingSettings);
	}
	settings.endGroup();
}

void Project::AddItemDataLookup(TreeModelItemData *pItemData)
{
	m_ItemDataUuidMap[pItemData->GetUuid()] = pItemData;
}

void Project::RemoveItemDataLookup(const QUuid &uuid)
{
	m_ItemDataUuidMap.remove(uuid);
}

TreeModelItemData *Project::FindItemData(const QUuid &uuid)
{
	auto iter = m_ItemDataUuidMap.find(uuid);
	if(iter == m_ItemDataUuidMap.end())
		return nullptr;

	return iter.value();
}

QList<TreeModelItemData *> Project::IncrementDependencies(TreeModelItemData *pItemDepender, QList<QUuid> dependeeList)
{
	QList<TreeModelItemData *> dependeeItemDataList;
	for(auto uuid : dependeeList)
		dependeeItemDataList.push_back(FindItemData(uuid));

	return IncrementDependencies(pItemDepender, dependeeItemDataList);
}

QList<TreeModelItemData *> Project::IncrementDependencies(TreeModelItemData *pItemDepender, QList<TreeModelItemData *> dependeeItemDataList)
{
	if(dependeeItemDataList.empty())
		return dependeeItemDataList;

	QList<TreeModelItemData *> returnList;
	for(int i = 0; i < dependeeItemDataList.size(); ++i)
	{
		if(dependeeItemDataList[i] == nullptr)
			continue;

		dependeeItemDataList[i]->AddDependantRef(pItemDepender);
		returnList.append(dependeeItemDataList[i]);
	}

	return returnList;
}

void Project::DecrementDependencies(TreeModelItemData *pItemDepender, QList<QUuid> dependeeList)
{
	QList<TreeModelItemData *> dependeeItemDataList;
	for(auto uuid : dependeeList)
	{
		if(uuid.isNull())
			continue;
		dependeeItemDataList.push_back(FindItemData(uuid));
	}

	DecrementDependencies(pItemDepender, dependeeItemDataList);
}

void Project::DecrementDependencies(TreeModelItemData *pItemDepender, QList<TreeModelItemData *> dependeeItemDataList)
{
	for(int i = 0; i < dependeeItemDataList.size(); ++i)
		dependeeItemDataList[i]->SubtractDependantRef(pItemDepender);
}

void Project::OpenTab(ProjectItemData *pItem)
{
	if(m_pCurOpenItem == pItem)
		return;

	if(m_pCurOpenItem)
		m_pCurOpenItem->DrawHide();

	m_pCurOpenItem = pItem;
	m_pTabBar->OnTabBarProjItemDataChanged(m_pCurOpenItem);

	bool bAlreadyLoaded = false;
	// Search for existing tab
	for(int i = 0; i < m_pTabBar->count(); ++i)
	{
		if(m_pTabBar->tabData(i).value<ProjectItemData *>() == m_pCurOpenItem)
		{
			bAlreadyLoaded = true;

			m_pTabBar->blockSignals(true);
			m_pTabBar->setCurrentIndex(i);
			m_pTabBar->blockSignals(false);

			m_pCurOpenItem->DrawShow();
			break;
		}
	}

	// Add tab, otherwise
	if(bAlreadyLoaded == false)
	{
		m_pCurOpenItem->WidgetLoad();
		m_pCurOpenItem->DrawLoad();

		m_pTabBar->blockSignals(true);
		int iIndex = m_pTabBar->addTab(m_pCurOpenItem->GetIcon(m_pCurOpenItem->IsExistencePendingSave() ? SUBICON_New : SUBICON_None), m_pCurOpenItem->GetName(false));
		QVariant v;
		v.setValue(m_pCurOpenItem);
		m_pTabBar->setTabData(iIndex, v);
		m_pTabBar->setCurrentIndex(iIndex);
		m_pTabBar->blockSignals(false);

		m_pCurOpenItem->DrawShow();
	}

	ApplySaveEnables();
}

void Project::CloseTab(ProjectItemData *pItem)
{
	if(pItem == m_pCurOpenItem)
		m_pCurOpenItem = nullptr;

	for(int i = 0; i < m_pTabBar->count(); ++i)
	{
		if(m_pTabBar->tabData(i).value<ProjectItemData *>() == pItem)
		{
			m_pTabBar->removeTab(i);
			break;
		}
	}

	m_pTabBar->OnTabBarProjItemDataRemoved(pItem);

	pItem->WidgetUnload();
	pItem->DrawUnload();
}

bool Project::CloseAllTabs()
{
	int iNumTabsOpened = m_pTabBar->count();
	for(int i = iNumTabsOpened - 1; i >= 0; --i)
		OnCloseTab(i);

	return m_pTabBar->count() == 0;
}

void Project::UnloadAllTabs()
{
	for(int i = 0; i < m_pTabBar->count(); ++i)
		m_pTabBar->tabData(i).value<ProjectItemData *>()->DrawUnload();
}

void Project::ApplySaveEnables()
{
	bool bCurItemDirty = false;
	bool bAnyItemDirty = false;
	for(int i = 0; i < m_pTabBar->count(); ++i)
	{
		ProjectItemData *pItem = m_pTabBar->tabData(i).value<ProjectItemData *>();
		if(pItem->IsSaveClean() == false)
		{
			bAnyItemDirty = true;
			if(m_pTabBar->currentIndex() == i)
				bCurItemDirty = true;
		}
	}

	MainWindow::ApplySaveEnables(bCurItemDirty, bAnyItemDirty);
}

bool Project::IsUnsavedOpenItems()
{
	for(int i = 0; i < m_pTabBar->count(); ++i)
	{
		if(m_pTabBar->tabData(i).value<ProjectItemData *>()->IsSaveClean() == false)
			return true;
	}

	return false;
}

void Project::RunCMakeGui()
{
#if defined(Q_OS_WIN)
	QString sCMakeApp = "cmake-gui.exe";
	QStringList sArgList;
	sArgList << "-S" << GetDirPath() << "-B" << QDir::cleanPath(GetDirPath()+QDir::separator()+"build");

	QProcess *pCMakeProcess = new QProcess(this);
	pCMakeProcess->start(sCMakeApp, sArgList);
#else
	HyGuiLog("RunCMake not implemented for this platform", LOGTYPE_Warning);
#endif
}

bool Project::HarmonyInitialize()
{
	m_pDraw = new ProjectDraw();
	m_pDraw->Load();

	delete m_pSourceWidget;
	delete m_pAtlasWidget;
	delete m_pAudioWidget;
	m_pSourceWidget = new ManagerWidget(m_pSourceModel, nullptr);
	m_pAtlasWidget = new ManagerWidget(m_pAtlasModel, nullptr);
	m_pAudioWidget = new ManagerWidget(m_pAudioModel, nullptr);

	for(int i = 0; i < m_pTabBar->count(); ++i)
	{
		ProjectItemData *pOpenItem = m_pTabBar->tabData(i).value<ProjectItemData *>();
		pOpenItem->DrawLoad();
	}

	if(m_pTabBar->currentIndex() >= 0)
		m_pTabBar->tabData(m_pTabBar->currentIndex()).value<ProjectItemData *>()->DrawShow();

	return true;
}

// IHyEngine
bool Project::HarmonyUpdate()
{
	return true;
}

// IHyEngine
void Project::HarmonyShutdown()
{
	delete m_pDraw;
	m_pDraw = nullptr;

	SaveUserData();
}

void Project::OnTabBarCurrentChanged(int iIndex)
{
	if(iIndex < 0)
		return;

	int iCurIndex = m_pTabBar->currentIndex();
	QVariant v = m_pTabBar->tabData(iCurIndex);
	ProjectItemData *pItem = v.value<ProjectItemData *>();

	MainWindow::OpenItem(pItem);
}

void Project::OnCloseTab(int iIndex)
{
	ProjectItemData *pItem = m_pTabBar->tabData(iIndex).value<ProjectItemData *>();
	
	MainWindow::CloseItem(pItem);
}

void Project::RenameItemInDataObj(ItemType eType, QString sOldPath, QString sNewPath, QJsonObject &dataObjRef)
{
	QString sItemTypeName = HyGlobal::ItemName(eType, true);
	QJsonObject subDirObj = dataObjRef[sItemTypeName].toObject();

	QJsonValue itemVal = subDirObj[sOldPath];
	subDirObj.remove(sOldPath);
	subDirObj.insert(sNewPath, itemVal);

	dataObjRef.remove(sItemTypeName);
	dataObjRef.insert(sItemTypeName, subDirObj);
}

void Project::RenamePrefixInDataObj(QString sOldPath, QString sNewPath, QJsonObject &dataObjRef)
{
	if(sOldPath.endsWith('/', Qt::CaseInsensitive) == false)
		sOldPath += '/';
	if(sNewPath.endsWith('/', Qt::CaseInsensitive) == false)
		sNewPath += '/';

	QList<ItemType> typeList = HyGlobal::GetItemTypeList_Project();
	for(auto itemTypeIter = dataObjRef.begin(); itemTypeIter != dataObjRef.end(); ++itemTypeIter)
	{
		if(itemTypeIter.key().compare("$fileVersion") == 0)
			continue;

		ItemType eType = ITEM_Unknown;
		for(int i = 0; i < typeList.size(); ++i)
		{
			if(itemTypeIter.key() == HyGlobal::ItemName(typeList[i], true))
			{
				eType = typeList[i];
				break;
			}
		}
		if(eType == ITEM_Unknown)
			continue;

		QJsonObject itemTypeObj = itemTypeIter.value().toObject();
		QStringList itemsTypeKeysList = itemTypeObj.keys();
		for(int i = 0; i < itemsTypeKeysList.size(); ++i)
		{
			if(itemsTypeKeysList[i].startsWith(sOldPath, Qt::CaseInsensitive))
			{
				QString sNewKey = itemsTypeKeysList[i];
				sNewKey.replace(sOldPath, sNewPath, Qt::CaseInsensitive);
				
				QJsonValue data = itemTypeObj[itemsTypeKeysList[i]];
				itemTypeObj.remove(itemsTypeKeysList[i]);
				itemTypeObj.insert(sNewKey, data);

				HyGuiLog(itemsTypeKeysList[i] % " is now known as: " % sNewKey, LOGTYPE_Normal);
			}
		}

		QString sItemTypeName = HyGlobal::ItemName(eType, true);
		if(dataObjRef.contains(sItemTypeName) == false) {
			HyGuiLog("Could not find item type: " % sItemTypeName % " within ItemProject::RenamePrefix", LOGTYPE_Error);
		}

		dataObjRef.remove(sItemTypeName);
		dataObjRef.insert(sItemTypeName, itemTypeObj);
	}
}

void Project::RefreshNamesOnTabs()
{
	for(int i = 0; i < m_pTabBar->count(); ++i)
		m_pTabBar->setTabText(i, m_pTabBar->tabData(i).value<ProjectItemData *>()->GetName(false));

	// By opening the already open item, it will refresh its name
	MainWindow::OpenItem(m_pCurOpenItem);
}

//bool Project::RegisterMetaData(ProjectItem *pProjectItem)
//{
//	QString sItemTypeName = HyGlobal::ItemName(pProjectItem->GetType(), true);
//	auto iter = m_MetaDataObj.find(sItemTypeName);
//	if(iter->isUndefined() || iter->isObject() == false)
//	{
//		HyGuiLog("RegisterMetaData could not find item type: " % sItemTypeName, LOGTYPE_Error);
//		return false;
//	}
//
//	QJsonObject typeObj = m_MetaDataObj[sItemTypeName].toObject();
//
//	if(typeObj.find(pProjectItem->GetName(true)) == typeObj.end())
//	{
//		QJsonObject metaItemObj;
//		metaItemObj.insert("UUID", QUuid::createUuid().toString());
//		metaItemObj.insert("CameraPos", QJsonArray { 0, 0 });
//		metaItemObj.insert("CameraZoom", 1.0);
//
//		typeObj.insert(pProjectItem->GetName(true), metaItemObj);
//
//		m_MetaDataObj.remove(sItemTypeName);
//		m_MetaDataObj.insert(sItemTypeName, typeObj);
//		return true;
//	}
//
//	return false;
//}
