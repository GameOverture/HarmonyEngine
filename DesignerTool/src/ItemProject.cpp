/**************************************************************************
 *	ItemProject.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "ItemProject.h"

#include "WidgetAtlasManager.h"
#include "MainWindow.h"
#include "ItemSprite.h"
#include "HyGlobal.h"

#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDirIterator>

ItemProject::ItemProject(const QString sNewProjectFilePath) :   Item(ITEM_Project, sNewProjectFilePath),
                                                                IHyApplication(HarmonyInit()),
                                                                m_eDrawState(PROJDRAWSTATE_Nothing),
                                                                m_ePrevDrawState(PROJDRAWSTATE_Nothing),
                                                                m_ActionSave(0),
                                                                m_ActionSaveAll(0),
                                                                m_bHasError(false)
{
    for(int i = 0; i < NUMPROJDRAWSTATE; ++i)
        m_bDrawStateLoaded[i] = false;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QFile projFile(sNewProjectFilePath);
    if(projFile.exists())
    {
        if(!projFile.open(QIODevice::ReadOnly))
        {
            HyGuiLog("ItemProject::ItemProject() could not open the project file: " % sNewProjectFilePath, LOGTYPE_Error);
            m_bHasError = true;
        }
    }
    else
    {
        HyGuiLog("ItemProject::ItemProject() could not find the project file: " % sNewProjectFilePath, LOGTYPE_Error);
        m_bHasError = true;
    }

    if(m_bHasError)
        return;

    QJsonDocument settingsDoc = QJsonDocument::fromJson(projFile.readAll());
    projFile.close();

    QJsonObject projPathsObj = settingsDoc.object();

    m_sRelativeAssetsLocation = projPathsObj["AssetsPath"].toString();
    m_sRelativeMetaDataLocation = projPathsObj["MetaDataPath"].toString();
    m_sRelativeSourceLocation = projPathsObj["SourcePath"].toString();


    sm_Init.sGameName = GetName(false).toStdString();
    sm_Init.sDataDir = GetAssetsAbsPath().toStdString();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    m_pAtlasMan = new WidgetAtlasManager(this);

    m_pTabBar = new QTabBar();
    m_pTabBar->setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
    connect(m_pTabBar, SIGNAL(currentChanged(int)), this, SLOT(on_tabBar_currentChanged(int)));

    m_ActionSave.setText("&Save");
    m_ActionSave.setIcon(QIcon(":/icons16x16/file-save.png"));
    m_ActionSave.setShortcuts(QKeySequence::Save);
    m_ActionSave.setShortcutContext(Qt::ApplicationShortcut);
    m_ActionSave.setEnabled(false);
    connect(&m_ActionSave, SIGNAL(triggered(bool)), this, SLOT(on_save_triggered()));

    m_ActionSaveAll.setText("Save &All");
    m_ActionSaveAll.setIcon(QIcon(":/icons16x16/file-saveAll.png"));
    m_ActionSaveAll.setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));
    m_ActionSaveAll.setShortcutContext(Qt::ApplicationShortcut);
    m_ActionSaveAll.setEnabled(false);
    connect(&m_ActionSaveAll, SIGNAL(triggered(bool)), this, SLOT(on_saveAll_triggered()));
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    QList<eItemType> subDirList = HyGlobal::SubDirList();
    foreach(eItemType eType, subDirList)
    {
        if(eType == ITEM_DirAtlases)
            continue;

        QString sSubDirPath = GetAssetsAbsPath() % HyGlobal::ItemName(eType) % HyGlobal::ItemExt(eType);
        Item *pSubDirItem = new Item(eType, sSubDirPath);

        QTreeWidgetItem *pCurTreeItem = pSubDirItem->GetTreeItem();
        m_pTreeItemPtr->addChild(pCurTreeItem);

        QDirIterator dirIter(sSubDirPath, QDirIterator::Subdirectories);
        while(dirIter.hasNext())
        {
            QString sCurPath = dirIter.next();

            if(sCurPath.endsWith(QChar('.')))
                continue;

            // Ensure pCurParentTreeItem is correct. If not keep moving up the tree until found.
            QFileInfo curFileInfo(sCurPath);
            QString sCurFileParentBaseName = curFileInfo.dir().dirName();
            QString sCurTreeItemName = pCurTreeItem->text(0);
            while(QString::compare(sCurFileParentBaseName, sCurTreeItemName, Qt::CaseInsensitive) != 0)
            {
                pCurTreeItem = pCurTreeItem->parent();
                sCurTreeItemName = pCurTreeItem->text(0);
            }

            Item *pPrefixItem;
            if(dirIter.fileInfo().isDir())
            {
                pPrefixItem = new Item(ITEM_Prefix, sCurPath);
                QTreeWidgetItem *pPrefixTreeWidget = pPrefixItem->GetTreeItem();

                pCurTreeItem->addChild(pPrefixTreeWidget);
                pCurTreeItem = pPrefixTreeWidget;
            }
            else if(dirIter.fileInfo().isFile())
            {
                eItemType eType;
                for(int i = 0; i < NUMITEM; ++i)
                {
                    if(sCurPath.endsWith(HyGlobal::ItemExt(i)))
                    {
                        eType = static_cast<eItemType>(i);
                        break;
                    }
                }

                switch(eType)
                {
                case ITEM_Sprite:   pPrefixItem = new ItemSprite(sCurPath, *m_pAtlasMan); break;
                }

                pCurTreeItem->addChild(pPrefixItem->GetTreeItem());
            }
        }
    }
}

ItemProject::~ItemProject()
{
    delete m_pAtlasMan;
}

QString ItemProject::GetDirPath() const
{
    QFileInfo file(m_sPATH);
    return file.dir().absolutePath() + '/';
}

QList<QAction *> ItemProject::GetSaveActions()
{
    QList<QAction *> actionList;
    actionList.append(&m_ActionSave);
    actionList.append(&m_ActionSaveAll);
    
    return actionList;
}

void ItemProject::SetSaveEnabled(bool bSaveEnabled, bool bSaveAllEnabled)
{
    m_ActionSave.setEnabled(bSaveEnabled);
    m_ActionSaveAll.setEnabled(bSaveAllEnabled);
}

// IHyApplication override
/*virtual*/ bool ItemProject::Initialize()
{
    return true;
}

// IHyApplication override
/*virtual*/ bool ItemProject::Update()
{
    while(m_ShowQueue.empty() == false)
    {
        ItemWidget *pItem = m_ShowQueue.dequeue();
        if(pItem == NULL)
            continue;

        if(pItem->IsDrawLoaded() == false)
        {
            pItem->DrawLoad(*this);

            m_pTabBar->blockSignals(true);
            int iIndex = m_pTabBar->addTab(pItem->GetIcon(), pItem->GetName(false));
            QVariant v;
            v.setValue(pItem);
            m_pTabBar->setTabData(iIndex, v);
            m_pTabBar->setCurrentIndex(iIndex);
            m_pTabBar->blockSignals(false);
        }
        else
        {
            for(int i = 0; i < m_pTabBar->count(); ++i)
            {
                if(m_pTabBar->tabData(i).value<ItemWidget *>() == pItem)
                {
                    m_pTabBar->blockSignals(true);
                    m_pTabBar->setCurrentIndex(i);
                    m_pTabBar->blockSignals(false);
                    break;
                }
            }
        }

        // Hide everything
        if(IsOverrideDraw())
        {
            SetOverrideDrawState(PROJDRAWSTATE_Nothing);

            while(IsOverrideDraw())
                OverrideDraw();
        }
        for(int i = 0; i < m_pTabBar->count(); ++i)
            m_pTabBar->tabData(i).value<ItemWidget *>()->DrawHide(*this);

        // Then show
        pItem->DrawShow(*this);
    }

    while(m_KillQueue.empty() == false)
    {
        ItemWidget *pItem = m_KillQueue.dequeue();
        if(pItem == NULL)
            continue;

        for(int i = 0; i < m_pTabBar->count(); ++i)
        {
            if(m_pTabBar->tabData(i).value<ItemWidget *>() == pItem)
            {
                pItem->DrawUnload(*this);
                m_pTabBar->removeTab(i);
                break;
            }
        }
    }

    if(IsOverrideDraw())
        OverrideDraw();
    else if(m_pTabBar->count() > 0)
        m_pTabBar->tabData(m_pTabBar->currentIndex()).value<ItemWidget *>()->DrawUpdate(*this);

    return true;
}

// IHyApplication override
/*virtual*/ bool ItemProject::Shutdown()
{
    return true;
}

void ItemProject::SetOverrideDrawState(eProjDrawState eDrawState)
{
    m_DrawStateQueue.append(eDrawState);
}

bool ItemProject::IsOverrideDraw()
{
    return (m_eDrawState != PROJDRAWSTATE_Nothing || m_ePrevDrawState != PROJDRAWSTATE_Nothing || m_DrawStateQueue.empty() == false);
}

void ItemProject::OverrideDraw()
{
    while(m_DrawStateQueue.empty() == false)
    {
        m_ePrevDrawState = m_eDrawState;
        m_eDrawState = m_DrawStateQueue.dequeue();

        // This shouldn't happen, but if it did it would break logic below it
        if(m_eDrawState == PROJDRAWSTATE_Nothing && m_ePrevDrawState == PROJDRAWSTATE_Nothing)
            continue;

        // If our current state is 'nothing', then Hide() what was previous if it was loaded
        if(m_eDrawState == PROJDRAWSTATE_Nothing)
        {
            if(m_bDrawStateLoaded[m_ePrevDrawState])
            {
                m_pCamera->SetEnabled(false);

                if(m_ePrevDrawState == PROJDRAWSTATE_AtlasManager)
                    AtlasManager_DrawHide(*this, *m_pAtlasMan);
            }

            // Show the selected tab since we're done with override draw
            if(m_pTabBar->currentIndex() != -1)
                m_pTabBar->tabData(m_pTabBar->currentIndex()).value<ItemWidget *>()->DrawShow(*this);
            
            m_ePrevDrawState = PROJDRAWSTATE_Nothing;
        }
        else
        {
            if(m_bDrawStateLoaded[m_eDrawState] == false)
            {
                // A non NULL camera signifies that this has been loaded already
                if(m_pCamera != NULL)
                {
                    m_pCamera = Window().CreateCamera2d();
                    m_pCamera->SetEnabled(false);
                }

                if(m_eDrawState == PROJDRAWSTATE_AtlasManager)
                    AtlasManager_DrawOpen(*this, *m_pAtlasMan);

                m_bDrawStateLoaded[m_eDrawState] = true;
            }
            
            // Hide any currently shown items, since we're being draw override
            for(int i = 0; i < m_pTabBar->count(); ++i)
                m_pTabBar->tabData(i).value<ItemWidget *>()->DrawHide(*this);

            m_pCamera->SetEnabled(true);

            if(m_eDrawState == PROJDRAWSTATE_AtlasManager)
                AtlasManager_DrawShow(*this, *m_pAtlasMan);
        }
    }

    if(m_eDrawState == PROJDRAWSTATE_AtlasManager)
        AtlasManager_DrawUpdate(*this, *m_pAtlasMan);
}

void ItemProject::Reset()
{
    for(int i = 0; i < NUMPROJDRAWSTATE; ++i)
        m_bDrawStateLoaded[i] = false;
}

void ItemProject::OpenItem(ItemWidget *pItem)
{
    m_ShowQueue.enqueue(pItem);
}

void ItemProject::CloseItem(ItemWidget *pItem)
{
    m_KillQueue.enqueue(pItem);
}

void ItemProject::on_tabBar_currentChanged(int index)
{
    if(index < 0)
        return;

    int iIndex = m_pTabBar->currentIndex();
    QVariant v = m_pTabBar->tabData(iIndex);
    ItemWidget *pItem = v.value<ItemWidget *>();

    MainWindow::OpenItem(pItem);
}

void ItemProject::on_save_triggered()
{
    int iIndex = m_pTabBar->currentIndex();
    QVariant v = m_pTabBar->tabData(iIndex);
    ItemWidget *pItem = v.value<ItemWidget *>();
    pItem->Save();
    
    HyGuiLog(pItem->GetName(true) % " was saved", LOGTYPE_Normal);
}

void ItemProject::on_saveAll_triggered()
{
    for(int i = 0; i < m_pTabBar->count(); ++i)
    {
        if(m_pTabBar->tabText(i).contains('*', Qt::CaseInsensitive))
        {
            ItemWidget *pItem = m_pTabBar->tabData(i).value<ItemWidget *>();
            pItem->Save();
            
            HyGuiLog(pItem->GetName(true) % " was saved", LOGTYPE_Normal);
        }
        
        
    }
}
