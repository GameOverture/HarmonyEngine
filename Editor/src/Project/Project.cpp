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
#include "AtlasWidget.h"
#include "GltfWidget.h"
#include "AudioAssetsWidget.h"
#include "MainWindow.h"
#include "ProjectItemMimeData.h"
#include "ExplorerModel.h"
#include "VersionPatcher.h"

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

ProjectTabBar::ProjectTabBar(Project *pProjectOwner) :
	m_pProjectOwner(pProjectOwner)
{
}

/*virtual*/ ProjectTabBar::~ProjectTabBar()
{
}

/*virtual*/ void ProjectTabBar::dragEnterEvent(QDragEnterEvent *pEvent) /*override*/
{
	const QMimeData *pMimeData = pEvent->mimeData();

	if(pMimeData->hasFormat(HYGUI_MIMETYPE))
		pEvent->acceptProposedAction();
}

/*virtual*/ void ProjectTabBar::dropEvent(QDropEvent *pEvent) /*override*/
{
	if(pEvent->proposedAction() == Qt::LinkAction && pEvent->mimeData()->hasFormat(HYGUI_MIMETYPE))
	{
		// Process the data from the event.
		QByteArray dragDataSrc = pEvent->mimeData()->data(HYGUI_MIMETYPE);
		QJsonDocument userDoc = QJsonDocument::fromJson(dragDataSrc);

		QJsonObject dragObj = userDoc.object();
		if(dragObj["project"].toString().toLower() != m_pProjectOwner->GetAbsPath().toLower())
		{
			pEvent->ignore();
			return;
		}

		ProjectItemData *pProjItem = static_cast<ProjectItemData *>(pEvent->source());
		MainWindow::OpenItem(pProjItem);

		pEvent->acceptProposedAction();
	}
	else
		pEvent->ignore();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Project::Project(const QString sProjectFilePath, ExplorerModel &modelRef) :
	ExplorerItemData(*this, ITEM_Project, HyIO::CleanPath(sProjectFilePath.toStdString().c_str(), HyGlobal::ItemExt(ITEM_Project).toStdString().c_str(), false).c_str()),
	m_ModelRef(modelRef),
	m_pDraw(nullptr),
	m_DlgProjectSettings(sProjectFilePath),
	m_pAtlasModel(nullptr),
	m_pAtlasWidget(nullptr),
	m_pGltfModel(nullptr),
	m_pGltfWidget(nullptr),
	m_pAudioWidget(nullptr),
	m_pTabBar(nullptr),
	m_pCurOpenItem(nullptr),
	m_bHasError(false)
{
	VersionPatcher::Run(this);

	if(m_DlgProjectSettings.HasError())
	{
		m_bHasError = true;
		return;
	}

	m_pTabBar = new ProjectTabBar(this);
	m_pTabBar->setTabsClosable(true);
	m_pTabBar->setShape(QTabBar::TriangularNorth);
	m_pTabBar->setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
	m_pTabBar->setAcceptDrops(true);
	m_pTabBar->setMovable(true);
	m_pTabBar->connect(m_pTabBar, SIGNAL(currentChanged(int)), this, SLOT(OnTabBarCurrentChanged(int)));
	m_pTabBar->connect(m_pTabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(OnCloseTab(int)));

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	m_pAtlasModel = new AtlasModel(this);
	m_pGltfModel = new GltfModel(this);

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

	ScanMetaFontDir();
}

/*virtual*/ Project::~Project()
{
	delete m_pAtlasWidget;
	delete m_pGltfWidget;

	Harmony::OnProjectDestructor(this); // Order matters because this calls Project::HarmonyShutdown()
	delete m_pDraw;
}

/*virtual*/ QString Project::GetName(bool bWithPrefix) const /*override*/
{
	return "";
}

void Project::LoadExplorerModel()
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Initialize 'm_ProjectFileData' from the project files on disk
	if(LoadDataObj(GetMetaDataAbsPath() % HYMETA_DataFile, m_ProjectFileData.m_Meta))
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
			HyGuiLog("Missing " % sItemTypeList[i] % " between data and meta", LOGTYPE_Error);
			continue;
		}

		// Determine which enum type of 'HyGuiItemType'
		HyGuiItemType eItemType = ITEM_Unknown;
		QList<HyGuiItemType> typeList = HyGlobal::GetTypeList();
		for(int j = 0; j < typeList.size(); ++j)
		{
			if(sItemTypeList[i] == HyGlobal::ItemName(typeList[j], true))
			{
				eItemType = typeList[j];
				break;
			}
		}
		if(eItemType == ITEM_Unknown)
		{
			HyGuiLog("Project ctor eType == TYPE_Unknown", LOGTYPE_Error);
			continue;
		}

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
				HyGuiLog("Missing " % sItemPath % " between data and meta", LOGTYPE_Error);
				continue;
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

			m_ModelRef.AddItem(this, eItemType, sPrefix, sName, itemFileData, false);
		}
	}

	if(bSystemFontFound == false)
	{
		QDir projGenDataDir(MainWindow::EngineSrcLocation() % HYGUIPATH_ProjGenDir % "data/");
		QFile srcFile(projGenDataDir.absoluteFilePath("src.json"));
		if(!srcFile.open(QFile::ReadOnly))
		{
			HyGuiLog("Error reading " % srcFile.fileName() % " when generating default font: " % srcFile.errorString(), LOGTYPE_Error);
			m_bHasError = true;
		}
		else
		{
			QByteArray sContents = srcFile.readAll();
			srcFile.close();

			QByteArray sBefore("[HyHarmonyTemplateDataDir]");
			QByteArray sAfter(QString(MainWindow::EngineSrcLocation() % HYGUIPATH_ProjGenDir % "data/").toLocal8Bit());
			sContents.replace(sBefore, sAfter);

			m_ModelRef.PasteItemSrc(sContents, m_ModelRef.FindIndex<ExplorerItemData *>(this, 0));
		}
	}
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
	QFile metaFile(GetMetaDataAbsPath() % HYMETA_DataFile);
	if(metaFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
		HyGuiLog(QString("Couldn't open ") % HYMETA_DataFile % " for writing: " % metaFile.errorString(), LOGTYPE_Error);
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

void Project::ExecProjSettingsDlg()
{
	if(m_DlgProjectSettings.exec() == QDialog::Accepted)
		m_DlgProjectSettings.SaveSettings();
}

QJsonObject Project::GetSettingsObj() const
{
	return m_DlgProjectSettings.GetSettingsObj();
}

QString Project::GetDirPath() const
{
	QFileInfo file(m_sText);
	return file.dir().absolutePath() + '/';
}

QString Project::GetGameName() const
{
	return GetSettingsObj()["GameName"].toString();
}

QString Project::GetAbsPath() const
{
	return m_sText;
}

QString Project::GetAssetsAbsPath() const
{
	return QDir::cleanPath(GetDirPath() + '/' + GetSettingsObj()["DataPath"].toString()) + '/';
}

QString Project::GetAssetsRelPath() const
{
	return QDir::cleanPath(GetSettingsObj()["DataPath"].toString()) + '/';
}

QString Project::GetMetaDataAbsPath() const
{
	return QDir::cleanPath(GetDirPath() + '/' + GetSettingsObj()["MetaDataPath"].toString()) + '/';
}

QString Project::GetMetaDataRelPath() const
{
	return QDir::cleanPath(GetSettingsObj()["MetaDataPath"].toString()) + '/';
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

ExplorerModel &Project::GetExplorerModel()
{
	return m_ModelRef;
}

AtlasModel &Project::GetAtlasModel()
{
	return *m_pAtlasModel;
}

AtlasWidget *Project::GetAtlasWidget()
{
	return m_pAtlasWidget;
}

GltfModel *Project::GetGltfModel()
{
	return m_pGltfModel;
}

GltfWidget *Project::GetGltfWidget()
{
	return m_pGltfWidget;
}

void Project::SetAudioModel(QJsonObject audioObj)
{
	QString sItemTypeName = HyGlobal::ItemName(ITEM_Audio, true);
	if(m_ProjectFileData.m_Meta.contains(sItemTypeName) == false)
	{
		HyGuiLog("Project::SetAudioModel could not find item type: " % sItemTypeName, LOGTYPE_Error);
		return;
	}

	m_ProjectFileData.m_Meta.remove(sItemTypeName);
	m_ProjectFileData.m_Meta.insert(sItemTypeName, audioObj);

	WriteMetaData();
}

AudioAssetsWidget *Project::GetAudioWidget()
{
	return m_pAudioWidget;
}

QStandardItemModel *Project::GetFontListModel()
{
	return &m_FontListModel;
}

void Project::ScanMetaFontDir()
{
	QStringList sFilterList(HYMETA_FontFilterList);
	QDir metaDir(GetMetaDataAbsPath() % HYMETA_FontsDir);
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

void Project::SetRenderSize(int iWidth, int iHeight)
{
	Hy_Window().SetWindowSize(glm::ivec2(iWidth, iHeight));

	if(m_pTabBar && m_pTabBar->count() != 0)
	{
		if(m_pTabBar->currentIndex() >= 0)
		{
			IDraw *pDraw = m_pTabBar->tabData(m_pTabBar->currentIndex()).value<ProjectItemData *>()->GetDraw();
			if(pDraw)
				pDraw->ResizeRenderer();
		}
	}
}

void Project::SaveItemData(HyGuiItemType eType, QString sPath, const FileDataPair &itemFileDataRef, bool bWriteToDisk)
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

void Project::DeleteItemData(HyGuiItemType eType, QString sPath, bool bWriteToDisk)
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

bool Project::LoadDataObj(QString sFilePath, QJsonObject &dataObjRef)
{
	QFile dataFile(sFilePath);
	if(dataFile.exists())
	{
		if(!dataFile.open(QIODevice::ReadOnly))
		{
			HyGuiLog("Project::LoadExplorerModel() could not open " % m_sText % "'s " % QFileInfo(sFilePath).fileName() % " file for project: " % dataFile.errorString(), LOGTYPE_Error);
			m_bHasError = true;
			return false; // Don't write with invalid object
		}

		QJsonDocument userDoc = QJsonDocument::fromJson(dataFile.readAll());
		dataFile.close();

		dataObjRef = userDoc.object();
	}

	// Ensure save object has all the valid types in map
	bool bTypeNotFound = false;
	QList<HyGuiItemType> typeList = HyGlobal::GetTypeList();
	for(int i = 0; i < typeList.size(); ++i)
	{
		if(typeList[i] == ITEM_Project || typeList[i] == ITEM_Filter || typeList[i] == ITEM_AtlasImage)
			continue;

		QString sTypeName = HyGlobal::ItemName(typeList[i], true);
		if(dataObjRef.contains(sTypeName) == false)
		{
			bTypeNotFound = true;
			dataObjRef.insert(sTypeName, QJsonObject());
		}
	}

	return dataFile.exists() == false || bTypeNotFound;
}

void Project::DeleteItemInDataObj(HyGuiItemType eType, QString sPath, QJsonObject &dataObjRef)
{
	QString sItemTypeName = HyGlobal::ItemName(eType, true);
	if(dataObjRef.contains(sItemTypeName) == false)
		HyGuiLog("Project::DeleteItemInDataObj could not find item type: " % sItemTypeName, LOGTYPE_Error);

	QJsonObject itemTypeObject = dataObjRef[sItemTypeName].toObject();
	if(itemTypeObject.contains(sPath) == false)
		HyGuiLog("Project::DeleteItemInDataObj could not find item: " % sPath, LOGTYPE_Error);

	itemTypeObject.remove(sPath);
	dataObjRef[sItemTypeName] = itemTypeObject;
}

void Project::DeletePrefixAndContents(QString sPrefix, bool bWriteToDisk)
{
	bool bItemsDeleted = false;

	QList<HyGuiItemType> typeList = HyGlobal::GetTypeList();
	for(auto itemTypeIter = m_ProjectFileData.m_Data.begin(); itemTypeIter != m_ProjectFileData.m_Data.end(); ++itemTypeIter)
	{
		HyGuiItemType eType = ITEM_Unknown;
		for(int i = 0; i < typeList.size(); ++i)
		{
			if(itemTypeIter.key() == HyGlobal::ItemName(typeList[i], true))
			{
				eType = typeList[i];
				break;
			}
		}
		if(eType == ITEM_Unknown)
			HyGuiLog("DeletePrefixAndContents bad", LOGTYPE_Error);

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

QString Project::RenameItem(HyGuiItemType eType, QString sOldPath, QString sNewPath)
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

bool Project::DoesItemExist(HyGuiItemType eType, QString sPath) const
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

QList<ProjectItemData *> Project::RegisterItemsById(ProjectItemData *pItemOwner, QList<QUuid> requestList)
{
	QList<ProjectItemData *> itemList;
	for(auto uuid : requestList)
		itemList.append(m_ModelRef.FindByUuid(uuid));

	return RegisterItems(pItemOwner, itemList);
}

QList<ProjectItemData *> Project::RegisterItems(ProjectItemData *pItemOwner, QList<ProjectItemData *> requestList)
{
	for(auto item : requestList)
	{
		if(m_ItemOwnerMap.contains(item) == false)
			m_ItemOwnerMap[item] = QSet<ProjectItemData *>();

		m_ItemOwnerMap[item].insert(pItemOwner);
	}

	return requestList;
}

void Project::RelinquishItems(ProjectItemData *pItemOwner, QList<ProjectItemData *> relinquishList)
{
	for(auto item : relinquishList)
	{
		if(m_ItemOwnerMap.contains(item) == false)
			continue;

		m_ItemOwnerMap[item].remove(pItemOwner);
	}
}

QList<ProjectItemData *> Project::GetItemOwners(ProjectItemData *pItem)
{
	if(m_ItemOwnerMap.contains(pItem) == false)
		return QList<ProjectItemData *>();

	return m_ItemOwnerMap[pItem].toList();
}

void Project::OpenTab(ProjectItemData *pItem)
{
	if(m_pCurOpenItem == pItem)
		return;

	if(m_pCurOpenItem)
		m_pCurOpenItem->DrawHide();

	m_pCurOpenItem = pItem;

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
	pItem->WidgetUnload();
	pItem->DrawUnload();

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

	if(m_pAtlasWidget)
		m_pAtlasWidget->StashTreeWidgets();

	delete m_pAtlasWidget;
	delete m_pGltfWidget;
	delete m_pAudioWidget;
	m_pAtlasWidget = new AtlasWidget(m_pAtlasModel, nullptr);
	m_pGltfWidget = new GltfWidget(m_pGltfModel, nullptr);
	m_pAudioWidget = new AudioAssetsWidget(this, nullptr);

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
	if(m_pAtlasWidget)
		m_pAtlasWidget->DrawUpdate();

	if(m_pTabBar->count() > 0)
	{
		m_pDraw->Hide();
		//m_pTabBar->tabData(m_pTabBar->currentIndex()).value<ProjectItemData *>()->WidgetUpdate(*this);
	}
	else
		m_pDraw->Show();

	return true;
}

// IHyEngine
void Project::HarmonyShutdown()
{
	delete m_pDraw;
	m_pDraw = nullptr;
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

void Project::RenameItemInDataObj(HyGuiItemType eType, QString sOldPath, QString sNewPath, QJsonObject &dataObjRef)
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

	QList<HyGuiItemType> typeList = HyGlobal::GetTypeList();
	for(auto itemTypeIter = dataObjRef.begin(); itemTypeIter != dataObjRef.end(); ++itemTypeIter)
	{
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
			}
		}

		HyGuiItemType eType = ITEM_Unknown;
		for(int i = 0; i < typeList.size(); ++i)
		{
			if(itemTypeIter.key() == HyGlobal::ItemName(typeList[i], true))
			{
				eType = typeList[i];
				break;
			}
		}
		if(eType == ITEM_Unknown)
			HyGuiLog("RenamePrefix bad", LOGTYPE_Error);

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
