#include "Harmony.h"

Harmony *Harmony::sm_pInstance = nullptr;

Harmony::Harmony(MainWindow *pMainWindow) : QObject(pMainWindow),
                                            m_pMainWindow(pMainWindow),
                                            m_Connection(this),
                                            m_pLoadedWidget(nullptr)
{
    if(sm_pInstance != nullptr)
        HyGuiLog("Harmony instance created when the static 'sm_pInstance' was not nullptr", LOGTYPE_Error);

    sm_pInstance = this;
}

/*virtual*/ Harmony::~Harmony()
{

}

/*static*/ Project *Harmony::GetProject()
{
    if(sm_pInstance->m_pLoadedWidget)
        return &sm_pInstance->m_pLoadedWidget->GetProject();

    return nullptr;
}

/*static*/ void Harmony::SetProject(Project &projectRef)
{
    if(sm_pInstance->m_pLoadedWidget &&
       &sm_pInstance->m_pLoadedWidget->GetProject() == &projectRef)
    {
        return;
    }

    // TODO: Thread this
    delete sm_pInstance->m_pLoadedWidget;
    sm_pInstance->m_pLoadedWidget = new HarmonyWidget(projectRef);
    connect(sm_pInstance->m_pLoadedWidget, &HarmonyWidget::HarmonyWidgetReady, sm_pInstance, &Harmony::HarmonyWidgetReady);

    sm_pInstance->m_pMainWindow->SetHarmonyWidget(sm_pInstance->m_pLoadedWidget);
    sm_pInstance->m_pMainWindow->SetLoading("Loading new Harmony instance");

//    SwitchRendererThread *pWorkerThread = new SwitchRendererThread(sm_pInstance->m_pCurRenderer, sm_pInstance);
//    connect(pWorkerThread, &SwitchRendererThread::finished, pWorkerThread, &QObject::deleteLater);
//    connect(pWorkerThread, &SwitchRendererThread::SwitchIsReady, sm_pInstance, &MainWindow::OnSwitchRendererReady);
//    pWorkerThread->start();
}

/*static*/ void Harmony::CloseProject()
{
    delete sm_pInstance->m_pLoadedWidget;
    sm_pInstance->m_pLoadedWidget = nullptr;
}

/*static*/ HarmonyWidget *Harmony::GetWidget(Project &projectRef)
{
    if(sm_pInstance->m_pLoadedWidget == nullptr || sm_pInstance->m_pLoadedWidget->IsProject(projectRef) == false)
    {
        HyGuiLog("Harmony::GetWidget passed a project that is not the currently set project", LOGTYPE_Error);
        return nullptr;
    }

    return sm_pInstance->m_pLoadedWidget;
}

/*slot*/ void Harmony::HarmonyWidgetReady(HarmonyWidget *pWidget)
{
    m_pLoadedWidget = pWidget;

    m_pLoadedWidget->GetProject().Initialize();
    m_pMainWindow->SetCurrentProject(m_pLoadedWidget->GetProject());

    m_pMainWindow->ClearLoading();
}
//void MainWindow::OnSwitchRendererReady(HyGuiRenderer *pRenderer)
//{
//    delete pRenderer;

//    // Swap the harmony engine renderers
//    sm_pInstance->m_pCurRenderer = new HyGuiRenderer(sm_pInstance->m_pCurSelectedProj, sm_pInstance);
//    sm_pInstance->ui->centralVerticalLayout->addWidget(sm_pInstance->m_pCurRenderer);
//}
