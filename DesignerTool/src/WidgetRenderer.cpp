/**************************************************************************
 *	WidgetRenderer.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetRenderer.h"
#include "ui_WidgetRenderer.h"

#include "MainWindow.h"

WidgetRenderer::WidgetRenderer(QWidget *parent) :   QWidget(parent),
                                                    IHyApplication(HarmonyInit()),
                                                    ui(new Ui::WidgetRenderer),
                                                    m_bInitialized(false),
                                                    m_pActiveItemProj(NULL)
{
    ui->setupUi(this);
    ui->tabWidget->clear();

    m_bInitialized = true;
}

WidgetRenderer::~WidgetRenderer()
{
    delete ui;
}

HyGuiRenderer *WidgetRenderer::GetRenderer()
{
    return ui->openGLWidget;
}

// IHyApplication override
/*virtual*/ bool WidgetRenderer::Initialize()
{
    return true;
}

// IHyApplication override
/*virtual*/ bool WidgetRenderer::Update()
{
    while(m_ActionQueue.empty() == false)
    {
        std::pair<Item *, eQueuedAction> action = m_ActionQueue.dequeue();
        Item *pItem = action.first;
        eQueuedAction eActionToTake = action.second;
        
        if(eActionToTake == QUEUEDITEM_Open)
        {
            if(pItem->GetType() == ITEM_Project)
            {
                pItem->DrawOpen(*this);
                ShowItem(pItem);
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
        }
        else if(eActionToTake == QUEUEDITEM_Show)
        {
            ShowItem(pItem);
        }
        else if(eActionToTake == QUEUEDITEM_Close)
        {
            if(m_pActiveItemProj == pItem)
            {
                m_pActiveItemProj->DrawHide(*this);
                m_pActiveItemProj = NULL;
                ShowItem(GetItem());
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
        }
    }
    
    if(GetItem())
        GetItem()->DrawUpdate(*this);

    return true;
}

// IHyApplication override
/*virtual*/ bool WidgetRenderer::Shutdown()
{
    return true;
}

void WidgetRenderer::OpenItem(Item *pItem)
{
    if(pItem == NULL)
    {
        HyGuiLog("WidgetRenderer::RenderItem tried to render NULL item", LOGTYPE_Warning);
        return;
    }
    
    m_ActionQueue.enqueue(std::pair<Item *, eQueuedAction>(pItem, QUEUEDITEM_Open));
}

void WidgetRenderer::CloseItem(Item *pItem)
{
    if(pItem == NULL)
    {
        HyGuiLog("WidgetRenderer::HideItem tried to hide NULL item", LOGTYPE_Warning);
        return;
    }
    
    m_ActionQueue.enqueue(std::pair<Item *, eQueuedAction>(pItem, QUEUEDITEM_Close));
}

QStringList WidgetRenderer::GetOpenItemPaths()
{
    QStringList sOpenItemList;
    
    for(int i = 0; i < ui->tabWidget->count(); ++i)
        sOpenItemList.append(reinterpret_cast<TabPage *>(ui->tabWidget->widget(i))->GetItem()->GetPath());
    
    return sOpenItemList;
}

Item *WidgetRenderer::GetItem(int iIndex /*= -1*/)
{
    if(m_pActiveItemProj)   // Overrides any Item in the current open TabPage
        return m_pActiveItemProj;
    
    if(ui->tabWidget->currentWidget() == NULL)
        return NULL;

    if(iIndex < 0)
        return static_cast<TabPage *>(ui->tabWidget->currentWidget())->GetItem();
    else
        return static_cast<TabPage *>(ui->tabWidget->widget(iIndex))->GetItem();
}

// Do not invoke this function outside of Update()
void WidgetRenderer::ShowItem(Item *pItem)
{
    if(pItem == NULL)
        return;
    
    if(m_pActiveItemProj)
        m_pActiveItemProj->DrawHide(*this);
                
    for(int i = 0; i < ui->tabWidget->count(); ++i)
        GetItem(i)->DrawHide(*this);

    if(pItem->GetType() == ITEM_Project)
        m_pActiveItemProj = static_cast<ItemProject *>(pItem);
    
    MainWindow::SetCurrentItem(pItem);
        
    pItem->DrawShow(*this);
}

void WidgetRenderer::on_tabWidget_currentChanged(int iIndex)
{
    if(m_bInitialized == false)
        return;
    
    m_ActionQueue.enqueue(std::pair<Item *, eQueuedAction>(GetItem(iIndex), QUEUEDITEM_Show));
}
