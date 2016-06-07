/**************************************************************************
 *	WidgetTabsManager.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetTabsManager.h"
#include "ui_WidgetTabsManager.h"

#include "MainWindow.h"

WidgetTabsManager::WidgetTabsManager(QWidget *parent) : QWidget(parent),
                                                        ui(new Ui::WidgetTabsManager),
                                                        IHyApplication(HarmonyInit()),
                                                        m_pProjOwner(NULL)
{
    ui->setupUi(this);
    
    // NOTE: THIS CONSTRUCTOR IS INVALID TO USE. IT EXISTS FOR QT TO ALLOW Q_OBJECT TO WORK
    //HyGuiLog("WidgetTabsManager::WidgetTabsManager() invalid constructor used", LOGTYPE_Error);
}

WidgetTabsManager::WidgetTabsManager(ItemProject *pProjOwner, QWidget *parent /*= 0*/) :    QWidget(parent),
                                                                                            ui(new Ui::WidgetTabsManager),
                                                                                            IHyApplication(HarmonyInit(pProjOwner->GetName().toStdString(), pProjOwner->GetAssetsAbsPath().toStdString())),
                                                                                            m_pProjOwner(pProjOwner)
{
    ui->setupUi(this);
    ui->tabWidget->clear();

    m_ActionQueue.clear();
}

WidgetTabsManager::~WidgetTabsManager()
{
    delete ui;
}

void WidgetTabsManager::OpenItem(Item *pItem)
{
    m_ActionQueue.enqueue(std::pair<Item *, eQueuedAction>(pItem, QUEUEDITEM_Open));
}

void WidgetTabsManager::CloseItem(Item *pItem)
{
    m_ActionQueue.enqueue(std::pair<Item *, eQueuedAction>(pItem, QUEUEDITEM_Close));
}

// IHyApplication override
/*virtual*/ bool WidgetTabsManager::Initialize()
{
    return true;
}

// IHyApplication override
/*virtual*/ bool WidgetTabsManager::Update()
{
    while(m_ActionQueue.empty() == false)
    {
        std::pair<Item *, eQueuedAction> action = m_ActionQueue.dequeue();
        Item *pItem = action.first;
        eQueuedAction eActionToTake = action.second;

        Item *pNewItemToShow = NULL;

        switch(eActionToTake)
        {
        case QUEUEDITEM_Open:
            if(pItem->GetType() == ITEM_Project)
            {
                pItem->DrawOpen(*this);
                pNewItemToShow = pItem;
            }
            else
            {
                bool bIsAlreadyOpen = false;
                for(int i = 0; i < ui->tabWidget->count(); ++i)
                {
                    // Determine if already opened
                    if(reinterpret_cast<TabPage *>(ui->tabWidget->widget(i))->GetItem() == pItem)
                    {
                        ui->tabWidget->setCurrentIndex(i);
                        bIsAlreadyOpen = true;
                        break;
                    }
                }

                if(bIsAlreadyOpen == false)
                {
                    // Below should invoke callback 'on_tabWidget_currentChanged' which will enqueue a QUEUEDITEM_Show action
                    TabPage *pNewTab = new TabPage(pItem, this);
                    pItem->DrawOpen(*this);
                    ui->tabWidget->setCurrentIndex(ui->tabWidget->addTab(pNewTab, pItem->GetIcon(), pItem->GetName()));
                }
            }
            break;

        case QUEUEDITEM_Show:
            pNewItemToShow = pItem;
            break;

        case QUEUEDITEM_Close:
            if(m_pProjOwner == pItem)
            {
                m_pProjOwner->DrawClose(*this);

                pNewItemToShow = GetItem();
                //ShowItem(GetItem());
            }
            else
            {
                for(int i = 0; i < ui->tabWidget->count(); ++i)
                {
                    TabPage *pTabPage = reinterpret_cast<TabPage *>(ui->tabWidget->widget(i));
                    if(pTabPage->GetItem() == pItem)
                    {
                        pItem->DrawClose(*this);
                        ui->tabWidget->removeTab(i);
                        break;
                    }
                }
            }
            break;
        }

        if(pNewItemToShow)
        {
            // Hide everything
            if(m_pProjOwner->IsOverrideDraw())
                m_pProjOwner->DrawHide(*this);

            for(int i = 0; i < ui->tabWidget->count(); ++i)
                GetItem(i)->DrawHide(*this);

            // Then show
            if(pNewItemToShow->GetType() == ITEM_Project)
            {
                if(pItem != m_pProjOwner)
                    HyGuiLog("WidgetTabsManager::ShowItem() passed an ItemProj, but of a different project", LOGTYPE_Error);
            }

            MainWindow::SetCurrentItem(pNewItemToShow);

            pNewItemToShow->DrawShow(*this);
        }
    }

    if(GetItem())
        GetItem()->DrawUpdate(*this);

    return true;
}

// IHyApplication override
/*virtual*/ bool WidgetTabsManager::Shutdown()
{
    return true;
}

Item *WidgetTabsManager::GetItem(int iIndex /*= -1*/)
{
    if(m_pProjOwner->IsOverrideDraw())   // Overrides any Item in the current open TabPage
        return m_pProjOwner;

    if(ui->tabWidget->currentWidget() == NULL)
        return NULL;

    if(iIndex < 0)
        return static_cast<TabPage *>(ui->tabWidget->currentWidget())->GetItem();
    else
        return static_cast<TabPage *>(ui->tabWidget->widget(iIndex))->GetItem();
}

void WidgetTabsManager::on_tabWidget_currentChanged(int index)
{
    m_ActionQueue.enqueue(std::pair<Item *, eQueuedAction>(GetItem(index), QUEUEDITEM_Show));
}
