#include "WidgetRenderer.h"
#include "ui_WidgetRenderer.h"

WidgetRenderer::WidgetRenderer(QWidget *parent) :   QWidget(parent),
                                                    ui(new Ui::WidgetRenderer),
                                                    m_bInitialized(false)
{
    ui->setupUi(this);
    ui->tabWidget->clear();
    m_bInitialized = true;    

    HarmonyInit initStruct;

    initStruct.szGameName = "Harmony Designer Tool";
#ifdef QT_DEBUG
    initStruct.szDataDir = "../data";    // This is used for production, default is fine for release
#endif
    initStruct.vStartResolution = vec2(1024.0f, 512.0f);
    initStruct.eWindowType = HYWINDOW_WindowedSizeable;
    initStruct.fPixelsPerMeter = 180.0f;
    initStruct.uiNumInputMappings = 1;
    initStruct.pSuppliedGfx = ui->renderer;

    m_pHyApp = new HyApp(initStruct);
    m_pHyEngine = new HyEngine(*m_pHyApp);
    m_pHyEngine->Initialize();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(Render()));
    timer->start(17);
}

WidgetRenderer::~WidgetRenderer()
{
    delete ui;
}

void WidgetRenderer::ClearItems()
{
    ui->tabWidget->clear();
}

void WidgetRenderer::OpenItem(Item *pItem)
{
    for(int i = 0; i < ui->tabWidget->count(); ++i)
    {
        if(reinterpret_cast<TabPage *>(ui->tabWidget->widget(i))->GetItem() == pItem)
        {
            // Already opened
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

void WidgetRenderer::Render()
{
    // HyGfx::update() belongs to QGLWidget. This will invoke QGLWidget's
    // paintGL, which inturn will invoke Harmony's IGfxApi::Update()
    ui->renderer->update();

    // This updates the Harmony engine as per usual
    m_pHyEngine->Update();
}

void WidgetRenderer::on_tabWidget_currentChanged(int index)
{
    if(m_bInitialized == false)
        return;
    
    TabPage *pTabPage = reinterpret_cast<TabPage *>(ui->tabWidget->widget(index));
    m_pHyApp->SetItem(pTabPage->GetItem());    
}
