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

#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

ItemProject::ItemProject(const QString sNewProjectFilePath) :   ItemWidget(ITEM_Project, sNewProjectFilePath, NULL),
                                                                IHyApplication(HarmonyInit()),
                                                                m_eDrawState(PROJDRAWSTATE_Nothing),
                                                                m_ePrevDrawState(PROJDRAWSTATE_Nothing),
                                                                m_bHasError(false)
{
    m_pWidget = new QTabBar();
    connect(m_pWidget, SIGNAL(currentChanged(int)), this, SLOT(on_tabBar_currentChanged(int)));

    for(int i = 0; i < NUMPROJDRAWSTATE; ++i)
        m_bDrawStateLoaded[i] = false;
    
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

    m_pAtlasMan = new WidgetAtlasManager(this);

    sm_Init.sGameName = GetName(false).toStdString();
    sm_Init.sDataDir = GetAssetsAbsPath().toStdString();

    //m_pTabsManager = new WidgetTabsManager(this);
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

/*virtual*/ void ItemProject::OnDraw_Open(IHyApplication &hyApp)
{
    if(m_eDrawState == PROJDRAWSTATE_AtlasManager)
        AtlasManager_DrawOpen(hyApp, *m_pAtlasMan);
}

/*virtual*/ void ItemProject::OnDraw_Close(IHyApplication &hyApp)
{
    if(m_eDrawState == PROJDRAWSTATE_AtlasManager)
        AtlasManager_DrawClose(hyApp, *m_pAtlasMan);
}

/*virtual*/ void ItemProject::OnDraw_Show(IHyApplication &hyApp)
{
    if(m_eDrawState == PROJDRAWSTATE_AtlasManager)
        AtlasManager_DrawShow(hyApp, *m_pAtlasMan);
}

/*virtual*/ void ItemProject::OnDraw_Hide(IHyApplication &hyApp)
{
    if(m_ePrevDrawState == PROJDRAWSTATE_AtlasManager)
        AtlasManager_DrawHide(hyApp, *m_pAtlasMan);
}

/*virtual*/ void ItemProject::OnDraw_Update(IHyApplication &hyApp)
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
                DrawHide(hyApp);
            
            m_ePrevDrawState = PROJDRAWSTATE_Nothing;
        }
        else
        {
            if(m_bDrawStateLoaded[m_eDrawState] == false)
            {
                DrawOpen(hyApp);
                m_bDrawStateLoaded[m_eDrawState] = true;
            }

            DrawShow(hyApp);
        }
    }
    
    if(m_eDrawState == PROJDRAWSTATE_AtlasManager)
        AtlasManager_DrawUpdate(hyApp, *m_pAtlasMan);
}

// IHyApplication override
/*virtual*/ bool ItemProject::Initialize()
{
    return true;
}

// IHyApplication override
/*virtual*/ bool ItemProject::Update()
{
    QTabBar *pTabBar = static_cast<QTabBar *>(m_pWidget);

    while(m_ActionQueue.empty() == false)
    {
        std::pair<ItemWidget *, eQueuedAction> action = m_ActionQueue.dequeue();
        ItemWidget *pItem = action.first;
        eQueuedAction eActionToTake = action.second;

        switch(eActionToTake)
        {
        case QUEUEDITEM_Open:
            for(int i = 0; i < pTabBar->count(); ++i)
            {
                // Determine if already opened
                if(pTabBar->tabData(i).value<ItemWidget *>() == pItem)
                {
                    pTabBar->setCurrentIndex(i);
                    pItem = NULL;   // Signifies below to not create a new tab as this item is already open
                    break;
                }
            }

            if(pItem)
            {
                pItem->DrawOpen(*this);

                pTabBar->blockSignals(true);
                int iIndex = pTabBar->addTab(pItem->GetIcon(), pItem->GetName(false));
                QVariant v;
                v.setValue(pItem);
                pTabBar->setTabData(iIndex, v);
                pTabBar->setCurrentIndex(iIndex);
                pTabBar->blockSignals(false);

                m_ActionQueue.enqueue(std::pair<ItemWidget *, eQueuedAction>(pItem, QUEUEDITEM_Show));
            }
            break;

        case QUEUEDITEM_Show:
            // Hide everything
            if(IsOverrideDraw())
            {
                SetOverrideDrawState(PROJDRAWSTATE_Nothing);

                while(IsOverrideDraw())
                    DrawUpdate(*this);
            }

            for(int i = 0; i < pTabBar->count(); ++i)
                pTabBar->tabData(i).value<ItemWidget *>()->DrawHide(*this);

            // Then show
            MainWindow::SetCurrentItem(pItem);
            pItem->DrawShow(*this);
            break;

        case QUEUEDITEM_Close:
            for(int i = 0; i < pTabBar->count(); ++i)
            {
                //TabPage *pTabPage = static_cast<TabPage *>(ui->tabWidget->widget(i));
                if(pTabBar->tabData(i).value<ItemWidget *>() == pItem)
                {
                    pItem->DrawClose(*this);
                    pTabBar->removeTab(i);
                    break;
                }
            }
            break;
        }
    }

    if(IsOverrideDraw())
        DrawUpdate(*this);
    else if(pTabBar->count() > 0)
        pTabBar->tabData(pTabBar->currentIndex()).value<ItemWidget *>()->DrawUpdate(*this);

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

void ItemProject::Reset()
{
    for(int i = 0; i < NUMPROJDRAWSTATE; ++i)
        m_bDrawStateLoaded[i] = false;
}

void ItemProject::OpenItem(ItemWidget *pItem)
{
    m_ActionQueue.enqueue(std::pair<ItemWidget *, eQueuedAction>(pItem, QUEUEDITEM_Open));
}

void ItemProject::CloseItem(ItemWidget *pItem)
{
    m_ActionQueue.enqueue(std::pair<ItemWidget *, eQueuedAction>(pItem, QUEUEDITEM_Close));
}

void ItemProject::on_tabBar_currentChanged(int index)
{
    if(index < 0)
        return;

    QTabBar *pTabBar = static_cast<QTabBar *>(m_pWidget);

    int iIndex = pTabBar->currentIndex();
    QVariant v = pTabBar->tabData(iIndex);
    ItemWidget *pItem = v.value<ItemWidget *>();
    m_ActionQueue.enqueue(std::pair<ItemWidget *, eQueuedAction>(pItem, QUEUEDITEM_Show));
}
