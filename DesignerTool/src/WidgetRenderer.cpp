#include "WidgetRenderer.h"
#include "ui_WidgetRenderer.h"

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

/*virtual*/ bool WidgetRenderer::Initialize()
{
    return true;
}

/*virtual*/ bool WidgetRenderer::Update()
{
    if(GetItem())
        GetItem()->Draw(*this);

    return true;
}

/*virtual*/ bool WidgetRenderer::Shutdown()
{
    return true;
}

void WidgetRenderer::ClearItems()
{
    ui->tabWidget->clear();
}

void WidgetRenderer::OpenItem(Item *pItem)
{
    if(pItem->GetType() == ITEM_Project)
    {
        ShowItem(pItem);
        return;
    }
    
    for(int i = 0; i < ui->tabWidget->count(); ++i)
    {
        // Determine if already opened
        if(reinterpret_cast<TabPage *>(ui->tabWidget->widget(i))->GetItem() == pItem)
        {
            ui->tabWidget->setCurrentIndex(i);
            return;
        }
    }
    
    TabPage *pNewTab = new TabPage(pItem, this);
    ui->tabWidget->setCurrentIndex(ui->tabWidget->addTab(pNewTab, pItem->GetIcon(), pItem->GetName()));
}

void WidgetRenderer::CloseItem(Item *pItem)
{
    if(m_pActiveItemProj == pItem)
    {
        m_pActiveItemProj->Hide();
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
                ui->tabWidget->removeTab(i);
                break;
            }
        }
    }
}

void WidgetRenderer::ReloadItems(QStringList &sPaths)
{
    //ui->openGLWidget->
}

Item *WidgetRenderer::GetItem(int iIndex /*= -1*/)
{
    if(m_pActiveItemProj)
        return m_pActiveItemProj;
    
    if(ui->tabWidget->currentWidget() == NULL)
        return NULL;

    if(iIndex < 0)
        return static_cast<TabPage *>(ui->tabWidget->currentWidget())->GetItem();
    else
        return static_cast<TabPage *>(ui->tabWidget->widget(iIndex))->GetItem();
}

void WidgetRenderer::ShowItem(Item *pItem)
{
    if(pItem == NULL)
        return;
    
    if(m_pActiveItemProj)
        m_pActiveItemProj->Hide();
                
    for(int i = 0; i < ui->tabWidget->count(); ++i)
        GetItem(i)->Hide();

    if(pItem->GetType() == ITEM_Project)
        m_pActiveItemProj = static_cast<ItemProject *>(pItem);
        
    pItem->Show();
}

void WidgetRenderer::on_tabWidget_currentChanged(int iIndex)
{
    if(m_bInitialized == false)
        return;

    ShowItem(GetItem(iIndex));
}
