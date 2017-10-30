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
#include "Global.h"
#include "ProjectItemMimeData.h"

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

ProjectTabBar::ProjectTabBar(Project *pProjectOwner) :  m_pProjectOwner(pProjectOwner)
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
Project::Project(ExplorerWidget *pProjWidget, const QString sProjectFilePath) : ExplorerTreeItem(ITEM_Project, sProjectFilePath),
                                                                                IHyApplication(g_DefaultInit),
                                                                                m_pWidget(pProjWidget),
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
            if(subDirList[i] == DIR_Atlases || subDirList[i] == DIR_AudioBanks)
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
        if(subDirList[i] == DIR_Atlases || subDirList[i] == DIR_AudioBanks)
            continue;

        QString sSubDirPath = GetAssetsAbsPath() % HyGlobal::ItemName(subDirList[i]) % HyGlobal::ItemExt(subDirList[i]);
        ExplorerTreeItem *pSubDirItem = new ExplorerTreeItem(subDirList[i], sSubDirPath);

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
                        ExplorerTreeItem *pPrefixItem = new ExplorerTreeItem(ITEM_Prefix, sSubDirName % "/" % sCurPrefix);
                        QTreeWidgetItem *pNewPrefixTreeWidget = pPrefixItem->GetTreeItem();

                        if(sCurPrefix[0] != HYDEFAULT_PrefixChar)
                            pCurPrefixTreeItem->addChild(pNewPrefixTreeWidget);

                        pCurPrefixTreeItem = pNewPrefixTreeWidget;
                    }
                }
                else // Last path part, so must be the actual data item
                {
                    ExplorerTreeItem *pNewDataItem = nullptr;
                    switch(subDirList[i])
                    {
                    case DIR_Audio:
                        pNewDataItem = new ProjectItem(*this, ITEM_Audio, sCurPrefix, sPathPartList[iPathPartIndex], objsInSubDirIter.value(), false);
                        break;
                    case DIR_Fonts:
                        pNewDataItem = new ProjectItem(*this, ITEM_Font, sCurPrefix, sPathPartList[iPathPartIndex], objsInSubDirIter.value(), false);

                        if(sCurPrefix == "+Hy/" && sPathPartList[iPathPartIndex] == "+HyFont")
                            bDefaultFontFound = true;
                        break;
                    case DIR_Sprites:
                        pNewDataItem = new ProjectItem(*this, ITEM_Sprite, sCurPrefix, sPathPartList[iPathPartIndex], objsInSubDirIter.value(), false);
                        break;
                    case DIR_Entities:
                        pNewDataItem = new ProjectItem(*this, ITEM_Entity, sCurPrefix, sPathPartList[iPathPartIndex], objsInSubDirIter.value(), false);
                        break;
                    case DIR_Particles:
                    case DIR_Spine:
                    case DIR_Shaders:
                    case DIR_Atlases:
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
        QDir templateDataDir(MainWindow::EngineSrcLocation() % "templates/data");
        QFile srcFile(templateDataDir.absoluteFilePath("src.json"));
        if(!srcFile.open(QFile::ReadOnly))
        {
            HyGuiLog("Error reading " % srcFile.fileName() % " when generating default font: " % srcFile.errorString(), LOGTYPE_Error);
            m_bHasError = true;
            return;
        }

        QByteArray sContents = srcFile.readAll();
        srcFile.close();

        QByteArray sBefore("[HyHarmonyTemplateDataDir]");
        QByteArray sAfter(QString(MainWindow::EngineSrcLocation() % "templates/data/").toLocal8Bit());
        sContents.replace(sBefore, sAfter);
        MainWindow::PasteItemSrc(sContents, this);
    }
}

/*virtual*/ Project::~Project()
{
    delete m_pDraw;
    delete m_pAtlasWidget;
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

ProjectTabBar *Project::GetTabBar()
{
    return m_pTabBar;
}

ProjectItem *Project::GetCurrentOpenItem()
{
    return m_pCurOpenItem;
}

ExplorerWidget *Project::GetExplorerWidget()
{
    return m_pWidget;
}

void Project::SetRenderSize(int iWidth, int iHeight)
{
    Window().SetWindowSize(glm::ivec2(iWidth, iHeight));

    if(m_pTabBar && m_pTabBar->count() != 0)
    {
        if(m_pTabBar->currentIndex() >= 0)
            m_pTabBar->tabData(m_pTabBar->currentIndex()).value<ProjectItem *>()->GetDraw()->ResizeRenderer();
    }
}

void Project::SaveGameData(HyGuiItemType eType, QString sPath, QJsonValue itemVal)
{
    HyGuiItemType eSubDirType = HyGlobal::GetDirFromItem(eType);
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
    HyGuiItemType eSubDirType = HyGlobal::GetDirFromItem(eType);
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
        {
            ProjectItem *pItem = m_pTabBar->tabData(i).value<ProjectItem *>();
            MainWindow::CloseItem(pItem);
            break;
        }
    }
}

void Project::DeletePrefixAndContents(HyGuiItemType eSubDirType, QString sPrefix)
{
    HyGuiItemType eActualSubDir = HyGlobal::GetDirFromItem(eSubDirType);
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
    return m_SaveDataObj[HyGlobal::ItemName(HyGlobal::GetDirFromItem(eType))].toObject();
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

void Project::CloseTab(ProjectItem *pItem)
{
    pItem->WidgetUnload();
    pItem->DrawUnload();

    if(pItem == m_pCurOpenItem)
        m_pCurOpenItem = nullptr;

    for(int i = 0; i < m_pTabBar->count(); ++i)
    {
        if(m_pTabBar->tabData(i).value<ProjectItem *>() == m_pCurOpenItem)
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

void Project::ReloadHarmony()
{
    // TODO: handle this
//    Project *pCurItemProj = sm_pInstance->m_pCurSelectedProj;
//    sm_pInstance->m_pCurSelectedProj = nullptr;    // Set m_pCurSelectedProj to 'nullptr' so SetSelectedProj() doesn't imediately return

//    SetSelectedProj(pCurItemProj);
}

// IHyApplication override
/*virtual*/ bool Project::Initialize()
{
    m_pDraw = new ProjectDraw(*this);
    m_pDraw->Load();

    if(m_pAtlasWidget)
        m_pAtlasWidget->StashTreeWidgets();

    delete m_pAtlasWidget;
    delete m_pAudioMan;
    m_pAtlasWidget = new AtlasWidget(m_pAtlasModel, this, nullptr);
    m_pAudioMan = new AudioWidgetManager(this, nullptr);

    if(m_pTabBar == nullptr)
    {
        m_pTabBar = new ProjectTabBar(this);
        m_pTabBar->setTabsClosable(true);
        m_pTabBar->setShape(QTabBar::TriangularNorth);
        m_pTabBar->setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
        m_pTabBar->setAcceptDrops(true);
        m_pTabBar->setMovable(true);
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
