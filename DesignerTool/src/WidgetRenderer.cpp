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
//    vector<uint32>	vGamePadIds;
//    m_pInputArray->GetGamePadIds(vGamePadIds);

//    if(vGamePadIds.empty() == false)
//    {
//        m_pInputArray->BindBtnMap(GP360_ButtonA, HyInputKey(vGamePadIds[0], GP360_ButtonA));
//        m_pInputArray->BindBtnMap(GP360_ButtonB, HyInputKey(vGamePadIds[0], GP360_ButtonB));

//        m_pInputArray->BindAxisMap(GP_RStickX, HyInputKey(vGamePadIds[0], GP_RStickX));
//        m_pInputArray->BindAxisMap(GP_RStickY, HyInputKey(vGamePadIds[0], GP_RStickY));
//        m_pInputArray->BindAxisMap(GP_Triggers, HyInputKey(vGamePadIds[0], GP_Triggers));

//        m_pInputArray->BindAxisMap(GP_LStickX, HyInputKey(vGamePadIds[0], GP_LStickX));
//    }
//    m_pInputArray->BindAxisMapPos(GP_LStickX, HyInputKey('D'));
//    m_pInputArray->BindAxisMapNeg(GP_LStickX, HyInputKey('A'));

//    m_pInputArray->BindBtnMap(GP360_ButtonA, HyInputKey(' '));

    m_pCam = Viewport().CreateCamera2d();

    //HyGfxWindow::tResolution tRes;
    //m_Window.GetResolution(tRes);
    //m_Camera.SetOrthographic(static_cast<float>(tRes.iWidth), static_cast<float>(tRes.iHeight));

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
    if(iIndex < 0)
        return static_cast<TabPage *>(ui->tabWidget->currentWidget())->GetItem();
    else
        return static_cast<TabPage *>(ui->tabWidget->widget(iIndex))->GetItem();
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

void WidgetRenderer::on_tabWidget_currentChanged(int index)
{
    if(m_bInitialized == false)
        return;

    for(int i = 0; i < ui->tabWidget->count(); ++i)
    {
        if(i != index)
            GetItem(i)->Hide();
        else
            GetItem(i)->Show();
    }
}
