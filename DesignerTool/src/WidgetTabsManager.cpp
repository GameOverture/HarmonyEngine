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
                                                                                            IHyApplication(HarmonyInit(pProjOwner->GetName(false).toStdString(), pProjOwner->GetAssetsAbsPath().toStdString())),
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

        switch(eActionToTake)
        {
        case QUEUEDITEM_Open:
            for(int i = 0; i < ui->tabWidget->count(); ++i)
            {
                // Determine if already opened
                if(static_cast<TabPage *>(ui->tabWidget->widget(i))->GetItem() == pItem)
                {
                    ui->tabWidget->setCurrentIndex(i);
                    pItem = NULL;   // Signifies below to not create a new tab as this item is already open
                    break;
                }
            }

            if(pItem)
            {
                // Below should invoke callback 'on_tabWidget_currentChanged' which will enqueue a QUEUEDITEM_Show action
                TabPage *pNewTab = new TabPage(pItem, this);
                pItem->DrawOpen(*this);
                ui->tabWidget->setCurrentIndex(ui->tabWidget->addTab(pNewTab, pItem->GetIcon(), pItem->GetName(false)));
            }
            break;

        case QUEUEDITEM_Show:
            // Hide everything
            if(m_pProjOwner->IsOverrideDraw())
            {
                m_pProjOwner->SetOverrideDrawState(PROJDRAWSTATE_Nothing);
                
                while(m_pProjOwner->IsOverrideDraw())
                    m_pProjOwner->DrawUpdate(*this);
            }

            for(int i = 0; i < ui->tabWidget->count(); ++i)
                static_cast<TabPage *>(ui->tabWidget->widget(i))->GetItem()->DrawHide(*this);

            // Then show
            MainWindow::SetCurrentItem(pItem);
            pItem->DrawShow(*this);
            break;

        case QUEUEDITEM_Close:
            for(int i = 0; i < ui->tabWidget->count(); ++i)
            {
                TabPage *pTabPage = static_cast<TabPage *>(ui->tabWidget->widget(i));
                if(pTabPage->GetItem() == pItem)
                {
                    pItem->DrawClose(*this);
                    ui->tabWidget->removeTab(i);
                    break;
                }
            }
            break;
        }
    }

    if(m_pProjOwner->IsOverrideDraw())
        m_pProjOwner->DrawUpdate(*this);
    else if(ui->tabWidget->currentWidget())
        static_cast<TabPage *>(ui->tabWidget->currentWidget())->GetItem()->DrawUpdate(*this);

    return true;
}

// IHyApplication override
/*virtual*/ bool WidgetTabsManager::Shutdown()
{
    return true;
}

void WidgetTabsManager::OpenItem(Item *pItem)
{
    m_ActionQueue.enqueue(std::pair<Item *, eQueuedAction>(pItem, QUEUEDITEM_Open));
}

void WidgetTabsManager::CloseItem(Item *pItem)
{
    m_ActionQueue.enqueue(std::pair<Item *, eQueuedAction>(pItem, QUEUEDITEM_Close));
}

void WidgetTabsManager::on_tabWidget_currentChanged(int index)
{
    if(index < 0)
        return;
    
    m_ActionQueue.enqueue(std::pair<Item *, eQueuedAction>(static_cast<TabPage *>(ui->tabWidget->widget(index))->GetItem(), QUEUEDITEM_Show));
}
