#include "Harmony.h"

Harmony *Harmony::sm_pInstance = nullptr;

Harmony::Harmony(MainWindow *pMainWindow) : QObject(pMainWindow),
                                            m_pMainWindow(pMainWindow),
                                            m_LoadingSpinner(pMainWindow),
                                            m_Connection(this),
                                            m_pCurrentWidget(nullptr)
{
    if(sm_pInstance != nullptr)
        HyGuiLog("Harmony instance created when the static 'sm_pInstance' was not nullptr", LOGTYPE_Error);

    sm_pInstance = this;
}

/*virtual*/ Harmony::~Harmony()
{

}

/*static*/ void Harmony::SetProject(Project *pProject)
{
    MainWindow::StartLoading(MDILOAD_Renderer);
    SwitchRendererThread *pWorkerThread = new SwitchRendererThread(sm_pInstance->m_pCurRenderer, sm_pInstance);
    connect(pWorkerThread, &SwitchRendererThread::finished, pWorkerThread, &QObject::deleteLater);
    connect(pWorkerThread, &SwitchRendererThread::SwitchIsReady, sm_pInstance, &MainWindow::OnSwitchRendererReady);
    pWorkerThread->start();
}

/*static*/ HarmonyWidget *Harmony::GetWidget(Project *pProject)
{
    if(pProject == nullptr || sm_pInstance->m_pCurrentWidget == nullptr || sm_pInstance->m_pCurrentWidget->IsProject(pProject) == false)
    {
        HyGuiLog("Harmony::GetWidget passed a project that is not the currently set project", LOGTYPE_Error);
        return nullptr;
    }

    return sm_pInstance->m_pCurrentWidget;
}
