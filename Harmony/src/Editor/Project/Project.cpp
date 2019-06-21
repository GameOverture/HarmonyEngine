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
#include "AudioAssetsWidget.h"
#include "MainWindow.h"
#include "ProjectItemMimeData.h"
#include "ExplorerModel.h"

#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDirIterator>
#include <QTreeWidgetItemIterator>
#include <QMessageBox>
#include <QStandardPaths>

// Keep this commented out unless you want the entire project to save every item upon boot (used if 'Data.json' layout has changed and needs to propagate all its changes)
//#define RESAVE_ENTIRE_PROJECT

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

		ProjectItem *pProjItem = static_cast<ProjectItem *>(pEvent->source());
		MainWindow::OpenItem(pProjItem);

		pEvent->acceptProposedAction();
	}
	else
		pEvent->ignore();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Project::Project(const QString sProjectFilePath, ExplorerModel &modelRef) :
	ExplorerItem(*this, ITEM_Project, HyStr::MakeStringProperPath(sProjectFilePath.toStdString().c_str(), HyGlobal::ItemExt(ITEM_Project).toStdString().c_str(), false).c_str()),
	m_ModelRef(modelRef),
	m_pDraw(nullptr),
	m_DlgProjectSettings(sProjectFilePath),
	m_pAtlasModel(nullptr),
	m_pAtlasWidget(nullptr),
	m_pAudioMan(nullptr),
	m_pTabBar(nullptr),
	m_pCurOpenItem(nullptr),
	m_bHasError(false)
{
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

	int iRow = 0;
	for(auto iter = fontsMap.begin(); iter != fontsMap.end(); ++iter, ++iRow)
	{
		QStandardItem *pFontItem = new QStandardItem(iter.key());
		pFontItem->setData(iter.value());

		m_FontListModel.appendRow(pFontItem);
		//m_FontListModel.setItem(iRow, pFontItem);
	}

	ScanMetaFontDir();
}

/*virtual*/ Project::~Project()
{
	delete m_pAtlasWidget;

	Harmony::OnProjectDestructor(this); // Order matters because this calls Project::HarmonyShutdown()
	delete m_pDraw;
}

/*virtual*/ QString Project::GetName(bool bWithPrefix) const /*override*/
{
	return "";
}

void Project::LoadExplorerModel()
{
	// Load game data items
	QFile dataFile(GetAssetsAbsPath() % HYASSETS_DataFile);
	if(dataFile.exists())
	{
		if(!dataFile.open(QIODevice::ReadOnly))
		{
			HyGuiLog("ItemProject::ItemProject() could not open " % m_sName % "'s " % HYASSETS_DataFile % " file for project: " % dataFile.errorString(), LOGTYPE_Error);
			m_bHasError = true;
			return;
		}

		QJsonDocument userDoc = QJsonDocument::fromJson(dataFile.readAll());
		dataFile.close();

		m_SaveDataObj = userDoc.object();
	}

	// Ensure save object has all the valid types in map
	QList<HyGuiItemType> typeList = HyGlobal::GetTypeList();
	for(int i = 0; i < typeList.size(); ++i)
	{
		if(typeList[i] == ITEM_Project || typeList[i] == ITEM_Prefix || typeList[i] == ITEM_Filter || typeList[i] == ITEM_AtlasImage)
			continue;

		QString sTypeName = HyGlobal::ItemName(typeList[i], true);
		if(m_SaveDataObj.contains(sTypeName) == false)
			m_SaveDataObj.insert(sTypeName, QJsonObject());
	}

	if(dataFile.exists() == false)
		WriteGameData();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool bSystemFontFound = false;

	// Initialize the project by processing each type
	for(auto typeIterator = m_SaveDataObj.begin(); typeIterator != m_SaveDataObj.end(); ++typeIterator)
	{
		QString sKey = typeIterator.key();

		HyGuiItemType eType = ITEM_Unknown;
		for(int i = 0; i < typeList.size(); ++i)
		{

			if(sKey == HyGlobal::ItemName(typeList[i], true))
			{
				eType = typeList[i];
				break;
			}
		}

		if(eType == ITEM_Unknown)
			HyGuiLog("Project ctor eType == TYPE_Unknown", LOGTYPE_Error);

		// Get the current (type) iterator, and iterate through its objects (items) within
		QJsonObject subDirObj = typeIterator.value().toObject();

		///////////////////////////////////////
		/// ITEMS IN SUBDIR
		for(auto objsInSubDirIter = subDirObj.begin(); objsInSubDirIter != subDirObj.end(); ++objsInSubDirIter)
		{
			QString sItemPath = objsInSubDirIter.key();

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

			if(sName == "+HyFont")
				bSystemFontFound = true;

			m_ModelRef.AddItem(this, eType, sPrefix, sName, objsInSubDirIter.value(), false);
#ifdef RESAVE_ENTIRE_PROJECT
			pNewDataItem->Save();
#endif
		}
	}

#ifdef RESAVE_ENTIRE_PROJECT
	SaveGameData();
#endif

	if(bSystemFontFound == false)
	{
		QDir templateDataDir(MainWindow::EngineSrcLocation() % HYGUIPATH_TemplateDir % "data/");
		QFile srcFile(templateDataDir.absoluteFilePath("src.json"));
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
			QByteArray sAfter(QString(MainWindow::EngineSrcLocation() % HYGUIPATH_TemplateDir % "data/").toLocal8Bit());
			sContents.replace(sBefore, sAfter);

			m_ModelRef.PasteItemSrc(sContents, m_ModelRef.FindIndex<ExplorerItem *>(this, 0));
		}
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
	QFileInfo file(m_sName);
	return file.dir().absolutePath() + '/';
}

QString Project::GetGameName() const
{
	return GetSettingsObj()["GameName"].toString();
}

QString Project::GetAbsPath() const
{
	return m_sName;
}

QString Project::GetAssetsAbsPath() const
{
	return QDir::cleanPath(GetDirPath() + '/' + GetSettingsObj()["DataPath"].toString() /*m_sRelativeAssetsLocation*/) + '/';
}

QString Project::GetAssetsRelPath() const
{
	return QDir::cleanPath(GetSettingsObj()["DataPath"].toString()/*m_sRelativeAssetsLocation*/) + '/';
}

QString Project::GetMetaDataAbsPath() const
{
	return QDir::cleanPath(GetDirPath() + '/' + GetSettingsObj()["MetaDataPath"].toString()/*m_sRelativeMetaDataLocation*/) + '/';
}

QString Project::GetMetaDataRelPath() const
{
	return QDir::cleanPath(GetSettingsObj()["MetaDataPath"].toString()/*m_sRelativeMetaDataLocation*/) + '/';
}

QString Project::GetSourceAbsPath() const
{
	return QDir::cleanPath(GetDirPath() + '/' + GetSettingsObj()["SourcePath"].toString() /*m_sRelativeSourceLocation*/) + '/';
}

QString Project::GetSourceRelPath() const
{
	return QDir::cleanPath(GetSettingsObj()["SourcePath"].toString()/*m_sRelativeSourceLocation*/) + '/';
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

void Project::SetAudioModel(QJsonObject audioObj)
{
	QString sItemTypeName = HyGlobal::ItemName(ITEM_Audio, true);
	if(m_SaveDataObj.contains(sItemTypeName) == false)
	{
		HyGuiLog("Project::SetAudioModel could not find item type: " % sItemTypeName, LOGTYPE_Error);
		return;
	}

	m_SaveDataObj.remove(sItemTypeName);
	m_SaveDataObj.insert(sItemTypeName, audioObj);

	WriteGameData();
}

AudioAssetsWidget *Project::GetAudioWidget()
{
	return m_pAudioMan;
}

QStandardItemModel *Project::GetFontListModel()
{
	return &m_FontListModel;
}

ProjectTabBar *Project::GetTabBar()
{
	return m_pTabBar;
}

ProjectItem *Project::GetCurrentOpenItem()
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
			IDraw *pDraw = m_pTabBar->tabData(m_pTabBar->currentIndex()).value<ProjectItem *>()->GetDraw();
			if(pDraw)
				pDraw->ResizeRenderer();
		}
	}
}

void Project::SaveGameData(HyGuiItemType eType, QString sPath, QJsonValue itemVal)
{
	QString sItemTypeName = HyGlobal::ItemName(eType, true);
	if(m_SaveDataObj.contains(sItemTypeName) == false) {
		HyGuiLog("Project::SaveGameData could not find item type: " % sItemTypeName, LOGTYPE_Error);
	}

	QJsonObject subDirObj = m_SaveDataObj[sItemTypeName].toObject();

	subDirObj.remove(sPath);
	subDirObj.insert(sPath, itemVal);

	m_SaveDataObj.remove(sItemTypeName);
	m_SaveDataObj.insert(sItemTypeName, subDirObj);

#ifndef RESAVE_ENTIRE_PROJECT
	WriteGameData();
#endif
}

void Project::DeleteGameData(HyGuiItemType eType, QString sPath)
{
	QString sItemTypeName = HyGlobal::ItemName(eType, true);
	if(m_SaveDataObj.contains(sItemTypeName) == false) {
		HyGuiLog("Project::DeleteGameData could not find item type: " % sItemTypeName, LOGTYPE_Error);
	}

	QJsonObject subDirObj = m_SaveDataObj[sItemTypeName].toObject();

	subDirObj.remove(sPath);

	m_SaveDataObj.remove(sItemTypeName);
	m_SaveDataObj.insert(sItemTypeName, subDirObj);

	WriteGameData();
	
	// If open, make sure to close as it's been deleted from project
	for(int i = 0; i < m_pTabBar->count(); ++i)
	{
		QVariant v = m_pTabBar->tabData(i);
		ProjectItem *pItem = v.value<ProjectItem *>();
		
		if(pItem->GetName(true) == sPath)
		{
			ProjectItem *pItem = m_pTabBar->tabData(i).value<ProjectItem *>();
			MainWindow::CloseItem(pItem);
			break;
		}
	}
}

void Project::DeletePrefixAndContents(QString sPrefix)
{
	QList<HyGuiItemType> typeList = HyGlobal::GetTypeList();
	for(auto itemTypeIter = m_SaveDataObj.begin(); itemTypeIter != m_SaveDataObj.end(); ++itemTypeIter)
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
				DeleteGameData(eType, iter.key());
		}
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
	if(m_SaveDataObj.contains(sItemTypeName) == false)
	{
		HyGuiLog("Project::RenameItem could not find item type: " % sItemTypeName, LOGTYPE_Error);
		return sOldPath.section('/', -1);
	}

	QJsonObject subDirObj = m_SaveDataObj[sItemTypeName].toObject();
	
	QJsonValue itemVal = subDirObj[sOldPath];
	subDirObj.remove(sOldPath);

	// Ensure there are no name conflicts
	QString sUniqueNewPath = sNewPath;
	int iConflictCount = 0;
	bool bConflicts = false;
	do
	{
		bConflicts = false;
		for(auto iter = subDirObj.begin(); iter != subDirObj.end(); ++iter)
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

	subDirObj.insert(sUniqueNewPath, itemVal);

	m_SaveDataObj.remove(sItemTypeName);
	m_SaveDataObj.insert(sItemTypeName, subDirObj);

	RefreshNamesOnTabs();
	WriteGameData();

	return sUniqueNewPath.section('/', -1);
}

QString Project::RenamePrefix(QString sOldPath, QString sNewPath)
{
	QList<HyGuiItemType> typeList = HyGlobal::GetTypeList();
	for(auto itemTypeIter = m_SaveDataObj.begin(); itemTypeIter != m_SaveDataObj.end(); ++itemTypeIter)
	{
		QJsonObject itemTypeObj = itemTypeIter.value().toObject();
		for(auto iter = itemTypeObj.begin(); iter != itemTypeObj.end(); ++iter)
		{
			if(iter.key().startsWith(sOldPath, Qt::CaseInsensitive))
			{
				QString sNewKey = iter.key();
				sNewKey.replace(sOldPath, sNewPath, Qt::CaseInsensitive);
				QJsonValue data = iter.value();
				itemTypeObj.remove(iter.key());
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
		if(m_SaveDataObj.contains(sItemTypeName) == false) {
			HyGuiLog("Could not find item type: " % sItemTypeName % " within ItemProject::RenamePrefix", LOGTYPE_Error);
		}

		m_SaveDataObj.remove(sItemTypeName);
		m_SaveDataObj.insert(sItemTypeName, itemTypeObj);
	}

	RefreshNamesOnTabs();
	WriteGameData();

	return sNewPath.section('/', -1);
}

void Project::RefreshNamesOnTabs()
{
	for(int i = 0; i < m_pTabBar->count(); ++i)
		m_pTabBar->setTabText(i, m_pTabBar->tabData(i).value<ProjectItem *>()->GetName(false));

	// By opening the already open item, it will refresh its name
	MainWindow::OpenItem(m_pCurOpenItem);
}

void Project::WriteGameData()
{
	QFile dataFile(GetAssetsAbsPath() % HYASSETS_DataFile);
	if(dataFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false) {
	   HyGuiLog(QString("Couldn't open ") % HYASSETS_DataFile % " for writing: " % dataFile.errorString(), LOGTYPE_Error);
	}
	else
	{
		QJsonDocument userDoc;
		userDoc.setObject(m_SaveDataObj);
		qint64 iBytesWritten = dataFile.write(userDoc.toJson());
		if(0 == iBytesWritten || -1 == iBytesWritten)
		{
			HyGuiLog(QString("Could not write to ") % HYASSETS_DataFile % " file: " % dataFile.errorString(), LOGTYPE_Error);
		}

		dataFile.close();
	}
}

QJsonObject Project::GetSavedItemsObj(HyGuiItemType eType)
{
	return m_SaveDataObj[HyGlobal::ItemName(eType, true)].toObject();
}

void Project::OpenTab(ProjectItem *pItem)
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
		if(m_pTabBar->tabData(i).value<ProjectItem *>() == m_pCurOpenItem)
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

void Project::CloseTab(ProjectItem *pItem)
{
	pItem->WidgetUnload();
	pItem->DrawUnload();

	if(pItem == m_pCurOpenItem)
		m_pCurOpenItem = nullptr;

	for(int i = 0; i < m_pTabBar->count(); ++i)
	{
		if(m_pTabBar->tabData(i).value<ProjectItem *>() == pItem)
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
		m_pTabBar->tabData(i).value<ProjectItem *>()->DrawUnload();
}

void Project::ApplySaveEnables()
{
	bool bCurItemDirty = false;
	bool bAnyItemDirty = false;
	for(int i = 0; i < m_pTabBar->count(); ++i)
	{
		ProjectItem *pItem = m_pTabBar->tabData(i).value<ProjectItem *>();
		if(pItem->IsSaveClean() == false)
		{
			bAnyItemDirty = true;
			if(m_pTabBar->currentIndex() == i)
				bCurItemDirty = true;
		}
	}

	MainWindow::ApplySaveEnables(bCurItemDirty, bAnyItemDirty);
}

bool Project::HarmonyInitialize()
{
	m_pDraw = new ProjectDraw();
	m_pDraw->Load();

	if(m_pAtlasWidget)
		m_pAtlasWidget->StashTreeWidgets();

	delete m_pAtlasWidget;
	delete m_pAudioMan;
	m_pAtlasWidget = new AtlasWidget(m_pAtlasModel, nullptr);
	m_pAudioMan = new AudioAssetsWidget(this, nullptr);

	for(int i = 0; i < m_pTabBar->count(); ++i)
	{
		ProjectItem *pOpenItem = m_pTabBar->tabData(i).value<ProjectItem *>();
		pOpenItem->DrawLoad();
	}

	if(m_pTabBar->currentIndex() >= 0)
		m_pTabBar->tabData(m_pTabBar->currentIndex()).value<ProjectItem *>()->DrawShow();

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
		//m_pTabBar->tabData(m_pTabBar->currentIndex()).value<ProjectItem *>()->WidgetUpdate(*this);
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
	ProjectItem *pItem = v.value<ProjectItem *>();

	MainWindow::OpenItem(pItem);
}

void Project::OnCloseTab(int iIndex)
{
	ProjectItem *pItem = m_pTabBar->tabData(iIndex).value<ProjectItem *>();
	MainWindow::CloseItem(pItem);
}

void Project::ScanMetaFontDir()
{
	QStringList sFilterList(HYMETA_FontFilterList);
	QDir metaDir(GetMetaDataAbsPath() % HYMETA_FontsDir);
	QFileInfoList metaFontFileInfoList = metaDir.entryInfoList(sFilterList);

	QMap<QString,QString> metaFontsMap;
	for(int i = 0; i < metaFontFileInfoList.count(); ++i)
	{
		QString metaFontFileName = metaFontFileInfoList[i].fileName();
		int iNumFonts = m_FontListModel.rowCount();
		for(int j = 0; j < iNumFonts; ++j)
		{
			if(m_FontListModel.item(j)->text().compare(metaFontFileName, Qt::CaseInsensitive) == 0)
			{
				m_FontListModel.removeRow(m_FontListModel.item(j)->row()); // Remove the font since we're gonna add it again as a meta dir font
				break;
			}
		}

		metaFontsMap[metaFontFileInfoList[i].fileName()] = metaFontFileInfoList[i].absoluteFilePath();
	}

	for(auto iter = metaFontsMap.begin(); iter != metaFontsMap.end(); ++iter)
	{
		QStandardItem *pFontItem = new QStandardItem(HyGlobal::ItemIcon(ITEM_Font, SUBICON_None), iter.key());
		pFontItem->setData(iter.value());

		m_FontListModel.setItem(0, pFontItem);
	}
}
