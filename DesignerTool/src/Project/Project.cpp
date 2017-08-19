/**************************************************************************
 *	Project.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Project.h"

#include "AtlasWidget.h"
#include "AudioWidgetManager.h"
#include "MainWindow.h"
#include "HyGuiGlobal.h"

#include "Harmony/HyEngine.h"

#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDirIterator>
#include <QTreeWidgetItemIterator>
#include <QMessageBox>

// Keep this commented out unless you want the entire project to save every item upon boot (used if 'Data.json' layout has changed and needs to propagate all its changes)
//#define RESAVE_ENTIRE_PROJECT

HarmonyInit g_DefaultInit;

Project::Project(const QString sProjectFilePath) :  DataExplorerItem(ITEM_Project, sProjectFilePath),
                                                    IHyApplication(g_DefaultInit),
                                                    m_pDraw(nullptr),
                                                    m_DlgProjectSettings(sProjectFilePath),
                                                    m_pAtlasModel(nullptr),
                                                    m_pAtlasWidget(nullptr),
                                                    m_pAudioMan(nullptr),
                                                    m_pTabBar(nullptr),
                                                    m_pCurOpenItem(nullptr),
                                                    m_bHasError(false)
{
    m_pTreeItemPtr->setText(0, GetGameName());
    m_Init.sGameName = GetGameName().toStdString();
    m_Init.sDataDir = GetAssetsAbsPath().toStdString();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Load game data items
    QFile dataFile(GetAssetsAbsPath() % HYGUIPATH_DataFile);
    if(dataFile.exists())
    {
        if(!dataFile.open(QIODevice::ReadOnly))
        {
            HyGuiLog("ItemProject::ItemProject() could not open " % sProjectFilePath % "'s " % HYGUIPATH_DataFile % " file for project: " % dataFile.errorString(), LOGTYPE_Error);
            m_bHasError = true;
            return;
        }

        QJsonDocument userDoc = QJsonDocument::fromJson(dataFile.readAll());
        dataFile.close();

        m_SaveDataObj = userDoc.object();
    }
    else
    {
        // Initialize the project by processing each type of sub dir
        QList<HyGuiItemType> subDirList = HyGlobal::SubDirList();
        for(int i = 0; i < subDirList.size(); ++i)
        {
            if(subDirList[i] == ITEM_DirAtlases || subDirList[i] == ITEM_DirAudioBanks)
                continue;

            QString sSubDirName = HyGlobal::ItemName(subDirList[i]);
            m_SaveDataObj.insert(sSubDirName, QJsonObject());
        }

        SaveGameData();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    m_pAtlasModel = new AtlasModel(this);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool bDefaultFontFound = false;

    // Initialize the project by processing each type of sub dir
    QList<HyGuiItemType> subDirList = HyGlobal::SubDirList();
    for(int i = 0; i < subDirList.size(); ++i)
    {
        if(subDirList[i] == ITEM_DirAtlases || subDirList[i] == ITEM_DirAudioBanks)
            continue;

        QString sSubDirPath = GetAssetsAbsPath() % HyGlobal::ItemName(subDirList[i]) % HyGlobal::ItemExt(subDirList[i]);
        DataExplorerItem *pSubDirItem = new DataExplorerItem(subDirList[i], sSubDirPath);

        // Adding sub dir tree item
        QTreeWidgetItem *pSubDirTreeItem = pSubDirItem->GetTreeItem();
        m_pTreeItemPtr->addChild(pSubDirTreeItem);

        QString sSubDirName = HyGlobal::ItemName(subDirList[i]);
        if(m_SaveDataObj.contains(sSubDirName) == false)
            m_SaveDataObj.insert(sSubDirName, QJsonObject());

        // Get the corresponding sub dir QJsonObject, and iterate through the objects (data items) within
        QJsonObject subDirObj = m_SaveDataObj[sSubDirName].toObject();
        QJsonObject::iterator objsInSubDirIter = subDirObj.begin();

        ///////////////////////////////////////
        /// ITEMS IN SUBDIR
        for(; objsInSubDirIter != subDirObj.end(); ++objsInSubDirIter)
        {
            QString sItemPath = objsInSubDirIter.key();

            // Create prefix folder tree items if they don't exist, and finally adding the tree item for the data itself
            QStringList sPathPartList = sItemPath.split("/");
            QString sCurPrefix = "";

            QTreeWidgetItem *pCurPrefixTreeItem = pSubDirTreeItem;
            ///////////////////////////////////////
            /// PATH PARTS
            for(int iPathPartIndex = 0; iPathPartIndex < sPathPartList.size(); ++iPathPartIndex)
            {
                if(iPathPartIndex != 0)
                    sCurPrefix += "/";

                // Not the last path part, must be a prefix
                if(iPathPartIndex != sPathPartList.size() - 1)
                {
                    sCurPrefix += sPathPartList[iPathPartIndex];

                    bool bPrefixFound = false;
                    for(int iChildIndex = 0; iChildIndex < pCurPrefixTreeItem->childCount(); ++iChildIndex)
                    {
                        if(sPathPartList[iPathPartIndex] == pCurPrefixTreeItem->child(iChildIndex)->text(0))
                        {
                            pCurPrefixTreeItem = pCurPrefixTreeItem->child(iChildIndex);
                            bPrefixFound = true;
                            break;
                        }
                    }

                    if(bPrefixFound == false)
                    {
                        DataExplorerItem *pPrefixItem = new DataExplorerItem(ITEM_Prefix, sSubDirName % "/" % sCurPrefix);
                        QTreeWidgetItem *pNewPrefixTreeWidget = pPrefixItem->GetTreeItem();

                        if(sCurPrefix[0] != HYDEFAULT_PrefixChar)
                            pCurPrefixTreeItem->addChild(pNewPrefixTreeWidget);

                        pCurPrefixTreeItem = pNewPrefixTreeWidget;
                    }
                }
                else // Last path part, so must be the actual data item
                {
                    DataExplorerItem *pNewDataItem = nullptr;
                    switch(subDirList[i])
                    {
                    case ITEM_DirAudio:
                        pNewDataItem = new ProjectItem(*this, ITEM_Audio, sCurPrefix, sPathPartList[iPathPartIndex], objsInSubDirIter.value(), false);
                        break;
                    case ITEM_DirFonts:
                        pNewDataItem = new ProjectItem(*this, ITEM_Font, sCurPrefix, sPathPartList[iPathPartIndex], objsInSubDirIter.value(), false);

                        if(sCurPrefix == "+Hy/" && sPathPartList[iPathPartIndex] == "+HyFont")
                            bDefaultFontFound = true;
                        break;
                    case ITEM_DirSprites:
                        pNewDataItem = new ProjectItem(*this, ITEM_Sprite, sCurPrefix, sPathPartList[iPathPartIndex], objsInSubDirIter.value(), false);
                        break;
                    case ITEM_DirParticles:
                    case ITEM_DirSpine:
                    case ITEM_DirShaders:
                    case ITEM_DirEntities:
                        pNewDataItem = new ProjectItem(*this, ITEM_Entity, sCurPrefix, sPathPartList[iPathPartIndex], objsInSubDirIter.value(), false);
                        break;
                    case ITEM_DirAtlases:
                    default:
                        { HyGuiLog("Unknown item type in ItemProject!", LOGTYPE_Error); }
                    }

                    if(sCurPrefix[0] != HYDEFAULT_PrefixChar)
                        pCurPrefixTreeItem->addChild(pNewDataItem->GetTreeItem());

#ifdef RESAVE_ENTIRE_PROJECT
                    static_cast<ProjectItem *>(pNewDataItem)->Save();
#endif

                }
            }
        }
    }

#ifdef RESAVE_ENTIRE_PROJECT
    SaveGameData();
#endif

    if(bDefaultFontFound == false)
    {
        QDir templateDataDir(MainWindow::EngineLocation() % "templates/data");
        QFile srcFile(templateDataDir.absoluteFilePath("src.json"));
        if(!srcFile.open(QFile::ReadOnly))
        {
            HyGuiLog("Error reading " % srcFile.fileName() % " when generating default font: " % srcFile.errorString(), LOGTYPE_Error);
            return;
        }

        QByteArray sContents = srcFile.readAll();
        srcFile.close();

        QByteArray sBefore("[HyHarmonyTemplateDataDir]");
        QByteArray sAfter(QString(MainWindow::EngineLocation() % "templates/data/").toLocal8Bit());
        sContents.replace(sBefore, sAfter);
        MainWindow::PasteItemSrc(sContents, this);
    }
}

/*virtual*/ Project::~Project()
{
    delete m_pDraw;
    delete m_pAtlasWidget;
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
    QFileInfo file(m_sPATH);
    return file.dir().absolutePath() + '/';
}

QString Project::GetGameName() const
{
    return GetSettingsObj()["GameName"].toString();
}

QString Project::GetAbsPath() const
{
    return m_sPATH;
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

AtlasModel &Project::GetAtlasModel()
{
    return *m_pAtlasModel;
}

AtlasWidget *Project::GetAtlasWidget()
{
    return m_pAtlasWidget;
}

AudioWidgetManager *Project::GetAudioWidget()
{
    return m_pAudioMan;
}

QTabBar *Project::GetTabBar()
{
    return m_pTabBar;
}

ProjectItem *Project::GetCurrentOpenItem()
{
    return m_pCurOpenItem;
}

void Project::OpenItem(ProjectItem *pItem)
{
    if(m_pCurOpenItem && m_pCurOpenItem != pItem)
        m_pCurOpenItem->DrawHide();

    if(m_pCurOpenItem == pItem)
        return;

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
        m_pCurOpenItem->DrawLoad(*this);

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

// IHyApplication override
/*virtual*/ bool Project::Initialize()
{
    m_pDraw = new ProjectDraw(*this);
    m_pDraw->Load();
    return true;
}

// IHyApplication override
/*virtual*/ bool Project::Update()
{
    if(m_pAtlasWidget)
        m_pAtlasWidget->DrawUpdate(*this);

    if(m_pTabBar->count() > 0)
    {
        m_pDraw->Hide();
        //m_pTabBar->tabData(m_pTabBar->currentIndex()).value<ProjectItem *>()->WidgetUpdate(*this);
    }
    else
        m_pDraw->Show();

    return true;
}

// IHyApplication override
/*virtual*/ void Project::Shutdown()
{
    delete m_pDraw;
    m_pDraw = nullptr;
}

void Project::SetRenderSize(int iWidth, int iHeight)
{
    Window().SetResolution(glm::ivec2(iWidth, iHeight));

    if(m_pTabBar && m_pTabBar->count() != 0)
    {
        if(m_pTabBar->currentIndex() >= 0)
            m_pTabBar->tabData(m_pTabBar->currentIndex()).value<ProjectItem *>()->GetDraw()->ResizeRenderer();
    }
}

void Project::OnHarmonyLoaded()
{
    // IHyApplication::Initialize() override
    Initialize();

    m_Init.sGameName = GetName(false).toStdString();
    m_Init.sDataDir = GetAssetsAbsPath().toStdString();
    
    if(m_pAtlasWidget)
        m_pAtlasWidget->StashTreeWidgets();

    delete m_pAtlasWidget;
    delete m_pAudioMan;
    m_pAtlasWidget = new AtlasWidget(m_pAtlasModel, this, nullptr);
    m_pAudioMan = new AudioWidgetManager(this, nullptr);
    
    if(m_pTabBar == nullptr)
    {
        m_pTabBar = new QTabBar(nullptr);
        m_pTabBar->setTabsClosable(true);
        m_pTabBar->setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
        m_pTabBar->connect(m_pTabBar, SIGNAL(currentChanged(int)), this, SLOT(OnTabBarCurrentChanged(int)));
        m_pTabBar->connect(m_pTabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(OnCloseTab(int)));
    }
    else
    {
        for(int i = 0; i < m_pTabBar->count(); ++i)
        {
            ProjectItem *pOpenItem = m_pTabBar->tabData(i).value<ProjectItem *>();
            pOpenItem->DrawLoad(*this);
        }

        if(m_pTabBar->currentIndex() >= 0)
            m_pTabBar->tabData(m_pTabBar->currentIndex()).value<ProjectItem *>()->DrawShow();
    }
    
    MainWindow::SetSelectedProjWidgets(this);
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

    MainWindow::SetSaveEnabled(bCurItemDirty, bAnyItemDirty);
}

void Project::SaveGameData(HyGuiItemType eType, QString sPath, QJsonValue itemVal)
{
    HyGuiItemType eSubDirType = HyGlobal::GetCorrespondingDirItem(eType);
    QString sSubDirName = HyGlobal::ItemName(eSubDirType);

    if(m_SaveDataObj.contains(sSubDirName) == false) {
        HyGuiLog("Could not find subdir: " % sSubDirName % " within ItemProject::SaveGameData", LOGTYPE_Error);
    }

    QJsonObject subDirObj = m_SaveDataObj[sSubDirName].toObject();

    subDirObj.remove(sPath);
    subDirObj.insert(sPath, itemVal);

    m_SaveDataObj.remove(sSubDirName);
    m_SaveDataObj.insert(sSubDirName, subDirObj);

#ifndef RESAVE_ENTIRE_PROJECT
    SaveGameData();
#endif
}

void Project::SaveGameData()
{
    QFile dataFile(GetAssetsAbsPath() % HYGUIPATH_DataFile);
    if(dataFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false) {
       HyGuiLog(QString("Couldn't open ") % HYGUIPATH_DataFile % " for writing: " % dataFile.errorString(), LOGTYPE_Error);
    }
    else
    {
        QJsonDocument userDoc;
        userDoc.setObject(m_SaveDataObj);
        qint64 iBytesWritten = dataFile.write(userDoc.toJson());
        if(0 == iBytesWritten || -1 == iBytesWritten)
        {
            HyGuiLog(QString("Could not write to ") % HYGUIPATH_DataFile % " file: " % dataFile.errorString(), LOGTYPE_Error);
        }

        dataFile.close();
    }
}

void Project::DeleteGameData(HyGuiItemType eType, QString sPath)
{
    HyGuiItemType eSubDirType = HyGlobal::GetCorrespondingDirItem(eType);
    QString sSubDirName = HyGlobal::ItemName(eSubDirType);

    if(m_SaveDataObj.contains(sSubDirName) == false) {
        HyGuiLog("Could not find subdir: " % sSubDirName % " within ItemProject::DeleteGameData", LOGTYPE_Error);
    }

    QJsonObject subDirObj = m_SaveDataObj[sSubDirName].toObject();

    subDirObj.remove(sPath);

    m_SaveDataObj.remove(sSubDirName);
    m_SaveDataObj.insert(sSubDirName, subDirObj);

    SaveGameData();
    
    // If open, make sure to close as it's been deleted from project
    for(int i = 0; i < m_pTabBar->count(); ++i)
    {
        QVariant v = m_pTabBar->tabData(i);
        ProjectItem *pItem = v.value<ProjectItem *>();
        
        if(pItem->GetName(true) == sPath)
            OnCloseTab(i);
    }
}

void Project::DeletePrefixAndContents(HyGuiItemType eSubDirType, QString sPrefix)
{
    HyGuiItemType eActualSubDir = HyGlobal::GetCorrespondingDirItem(eSubDirType);
    QString sSubDirName = HyGlobal::ItemName(eActualSubDir);
    
    if(m_SaveDataObj.contains(sSubDirName) == false) {
        HyGuiLog("Could not find subdir: " % sSubDirName % " within ItemProject::DeletePrefix", LOGTYPE_Error);
    }

    QJsonObject subDirObj = m_SaveDataObj[sSubDirName].toObject();
    for(auto iter = subDirObj.begin(); iter != subDirObj.end(); ++iter)
    {
        QFileInfo itemInfo;
        itemInfo.setFile(iter.key());
        QString sCurPrefix = itemInfo.path() % "/";
        
        if(sCurPrefix == sPrefix)
            DeleteGameData(eActualSubDir, iter.key());
    }
}

QJsonObject Project::GetSubDirObj(HyGuiItemType eType)
{
    return m_SaveDataObj[HyGlobal::ItemName(HyGlobal::GetCorrespondingDirItem(eType))].toObject();
}

bool Project::CloseAllTabs()
{
    int iNumTabsOpened = m_pTabBar->count();
    for(int i = iNumTabsOpened - 1; i >= 0; --i)
        OnCloseTab(i);
    
    return m_pTabBar->count() == 0;
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

    if(pItem->IsSaveClean() == false)
    {
        int iDlgReturn = QMessageBox::question(nullptr, "Save Changes", pItem->GetName(true) % " has unsaved changes. Do you want to save before closing?", QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if(iDlgReturn == QMessageBox::Save)
            pItem->Save();
        else if(iDlgReturn == QMessageBox::Discard)
            pItem->DiscardChanges();
        else if(iDlgReturn == QMessageBox::Cancel)
            return;
    }

    MainWindow::CloseItem(pItem);
    pItem->WidgetUnload();
    pItem->DrawUnload();

    if(pItem == m_pCurOpenItem)
        m_pCurOpenItem = nullptr;

    m_pTabBar->removeTab(iIndex);

    if(pItem->IsExistencePendingSave() && pItem->GetTreeItem()->parent() != nullptr)
        pItem->GetTreeItem()->parent()->removeChild(pItem->GetTreeItem());
}
