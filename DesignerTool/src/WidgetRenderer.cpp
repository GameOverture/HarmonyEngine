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
                                                    ui(new Ui::WidgetRenderer)
{
    ui->setupUi(this);
}

WidgetRenderer::~WidgetRenderer()
{
    delete ui;
}

HyGuiRenderer *WidgetRenderer::GetRenderer()
{
    return ui->openGLWidget;
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
