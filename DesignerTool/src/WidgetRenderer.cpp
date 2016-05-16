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
    
    m_pBlankTabs = ui->tabManager;
}

WidgetRenderer::~WidgetRenderer()
{
    delete ui;
}

void WidgetRenderer::LoadItemProject(ItemProject *pProj)
{
    ui->tabManager->hide();
    
    if(pProj == NULL)
        ui->tabManager = m_pBlankTabs;
    else
        ui->tabManager = pProj->GetTabsManager();
    
    ui->tabManager->show();
    
    ui->openGLWidget->LoadItemProject(pProj);
}

void WidgetRenderer::OpenItem(Item *pItem)
{
    if(pItem == NULL)
    {
        HyGuiLog("WidgetRenderer::OpenItem tried to render NULL item", LOGTYPE_Warning);
        return;
    }
    
    if(ui->tabManager == m_pBlankTabs)
    {
        HyGuiLog("WidgetRenderer::OpenItem tried to open an item with a 'm_pBlankTabs'", LOGTYPE_Warning);
        return;
    }
    
    ui->tabManager->OpenItem(pItem);
}

void WidgetRenderer::CloseItem(Item *pItem)
{
    if(pItem == NULL)
    {
        HyGuiLog("WidgetRenderer::CloseItem tried to hide NULL item", LOGTYPE_Warning);
        return;
    }
    
    if(ui->tabManager == m_pBlankTabs)
    {
        HyGuiLog("WidgetRenderer::CloseItem tried to close an item with a 'm_pBlankTabs'", LOGTYPE_Warning);
        return;
    }
    
    ui->tabManager->CloseItem(pItem);
}
