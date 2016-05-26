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

WidgetRenderer::WidgetRenderer(QWidget *parent) :   QWidget(parent),
                                                    ui(new Ui::WidgetRenderer)
{
    ui->setupUi(this);
    
    while(ui->tabManagers->count())
        ui->tabManagers->removeWidget(ui->tabManagers->currentWidget());
}

WidgetRenderer::~WidgetRenderer()
{
    delete ui;
}

void WidgetRenderer::LoadItemProject(ItemProject *pProj)
{
    if(pProj)
    {
        bool bTabsFound = false;
        for(int i = 0; i < ui->tabManagers->count(); ++i)
        {
            if(ui->tabManagers->widget(i) == pProj->GetTabsManager())
            {
                ui->tabManagers->setCurrentIndex(i);
                bTabsFound = true;
            }
        }

        if(bTabsFound == false)
        {
            ui->tabManagers->addWidget(pProj->GetTabsManager());
            ui->tabManagers->setCurrentWidget(pProj->GetTabsManager());
        }
    }
    
    ui->openGLWidget->LoadItemProject(pProj);
}

void WidgetRenderer::OpenItem(Item *pItem)
{
    if(pItem == NULL)
    {
        HyGuiLog("WidgetRenderer::OpenItem tried to render NULL item", LOGTYPE_Warning);
        return;
    }
    
    WidgetTabsManager *pTabsManager = static_cast<WidgetTabsManager *>(ui->tabManagers->currentWidget());
    if(pTabsManager == NULL)
    {
        HyGuiLog("WidgetRenderer::OpenItem tried to open an item with a 'm_pBlankTabs'", LOGTYPE_Warning);
        return;
    }
    
    pTabsManager->OpenItem(pItem);
}

void WidgetRenderer::CloseItem(Item *pItem)
{
    if(pItem == NULL)
    {
        HyGuiLog("WidgetRenderer::CloseItem tried to hide NULL item", LOGTYPE_Warning);
        return;
    }
    
    WidgetTabsManager *pTabsManager = static_cast<WidgetTabsManager *>(ui->tabManagers->currentWidget());
    if(pTabsManager == NULL)
    {
        HyGuiLog("WidgetRenderer::CloseItem tried to close an item with a 'm_pBlankTabs'", LOGTYPE_Warning);
        return;
    }
    
    pTabsManager->CloseItem(pItem);
}
