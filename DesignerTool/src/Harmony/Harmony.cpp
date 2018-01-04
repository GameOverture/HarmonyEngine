#include "Harmony.h"
#include "MainWindow.h"
#include "HarmonyWidget.h"

Harmony *Harmony::sm_pInstance = nullptr;

Harmony::Harmony(MainWindow &mainWindowRef) :  QObject(&mainWindowRef),
											   m_MainWindowRef(mainWindowRef),
											   m_pWidget(new HarmonyWidget(nullptr)),
											   m_Connection(this)
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
	return sm_pInstance->m_pWidget->GetProject();
}

/*static*/ void Harmony::SetProject(Project *pProject)
{
	Project *pCurrentProject = GetProject();
	if(pCurrentProject == pProject)
		return;

	// If valid project, unload all the open tabs so Draws will reload properly
	if(pCurrentProject)
		pCurrentProject->UnloadAllTabs();

	// Delete the current reference and replace it with a newly allocated widget
	delete sm_pInstance->m_pWidget;
	sm_pInstance->m_pWidget = new HarmonyWidget(pProject);
	sm_pInstance->m_MainWindowRef.SetHarmonyWidget(sm_pInstance->m_pWidget);

	if(pProject != nullptr)
	{
		connect(sm_pInstance->m_pWidget, &HarmonyWidget::HarmonyWidgetReady, sm_pInstance, &Harmony::HarmonyWidgetReady);
		sm_pInstance->m_MainWindowRef.SetLoading("Loading new Harmony instance");
	}
	else
		sm_pInstance->m_MainWindowRef.SetCurrentProject(nullptr);
}

/*static*/ void Harmony::Reload(Project *pProject)
{
	if(sm_pInstance->m_pWidget->IsProject(pProject) == false)
	{
		HyGuiLog("Harmony::Reload passed a project that is not the currently set project", LOGTYPE_Error);
		return;
	}

	SetProject(nullptr);
	SetProject(pProject);
}

/*static*/ HarmonyWidget *Harmony::GetWidget(Project *pProject)
{
	if(sm_pInstance->m_pWidget->IsProject(pProject) == false)
	{
		HyGuiLog("Harmony::GetWidget passed a project that is not the currently set project", LOGTYPE_Error);
		return nullptr;
	}

	return sm_pInstance->m_pWidget;
}

/*slot*/ void Harmony::HarmonyWidgetReady(HarmonyWidget *pWidget)
{
	m_pWidget->GetProject()->Initialize();

	m_MainWindowRef.SetCurrentProject(m_pWidget->GetProject());
	m_MainWindowRef.ClearLoading();
}
