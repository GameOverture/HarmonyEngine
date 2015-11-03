#include "WidgetRenderer.h"
#include "ui_WidgetRenderer.h"

WidgetRenderer::WidgetRenderer(QWidget *parent) :   QWidget(parent),
                                                    IHyApplication(HarmonyInit()),
                                                    ui(new Ui::WidgetRenderer),
                                                    m_bInitialized(false)
{
    ui->setupUi(this);
    ui->tabWidget->clear();
    //ui->tabWidget->setTabBarAutoHide(true);
    m_bInitialized = true;
}

WidgetRenderer::~WidgetRenderer()
{
    delete ui;
}

/*virtual*/ bool WidgetRenderer::Initialize()
{
    m_pCam = Window().CreateCamera2d();

    m_pCam->Pos().Set(0.0f, 0.0f);
    m_pCam->SetZoom(0.8f);

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

Item *WidgetRenderer::GetItem(int iIndex /*= -1*/)
{
    if(ui->tabWidget->currentWidget() == NULL)
        return NULL;

    if(iIndex < 0)
        return static_cast<TabPage *>(ui->tabWidget->currentWidget())->GetItem();
    else
        return static_cast<TabPage *>(ui->tabWidget->widget(iIndex))->GetItem();
}

void WidgetRenderer::ShowItem(Item *pItem)
{
    for(int i = 0; i < ui->tabWidget->count(); ++i)
        GetItem(i)->Hide();

    pItem->Show();
}

void WidgetRenderer::ClearItems()
{
    ui->tabWidget->clear();
}

void WidgetRenderer::OpenItem(Item *pItem)
{
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
    ui->tabWidget->addTab(pNewTab, pItem->GetIcon(), pItem->GetName());
}

void WidgetRenderer::CloseItem(Item *pItem)
{
    for(int i = 0; i < ui->tabWidget->count(); ++i)
    {
        if(reinterpret_cast<TabPage *>(ui->tabWidget->widget(i))->GetItem() == pItem)
        {
            ui->tabWidget->removeTab(i);
            break;
        }
    }
}

void WidgetRenderer::on_tabWidget_currentChanged(int iIndex)
{
    if(m_bInitialized == false)
        return;

    ShowItem(GetItem(iIndex));
}
