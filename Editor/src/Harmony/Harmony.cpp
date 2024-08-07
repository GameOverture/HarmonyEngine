/**************************************************************************
*	Harmony.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "Harmony.h"
#include "MainWindow.h"

Harmony *Harmony::sm_pInstance = nullptr;

Harmony::Harmony(MainWindow &mainWindowRef) :
	QObject(&mainWindowRef),
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
	sm_pInstance = nullptr;
	delete m_pWidget;
}

/*static*/ Project *Harmony::GetProject()
{
	if(sm_pInstance == nullptr) // NOTE: This may happen if this static function is invoked between the dtor and ctor
		return nullptr;

	return sm_pInstance->m_pWidget->GetProject();
}

/*static*/ void Harmony::SetProject(Project *pProject)
{
	if(sm_pInstance == nullptr) // NOTE: This may happen if this static function is invoked between the dtor and ctor
	{
		HyGuiLog("Harmony::SetProject() invoked when 'sm_pInstance' was nullptr", LOGTYPE_Error);
		return;
	}

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
		connect(sm_pInstance->m_pWidget->GetWgtHarmony(), &WgtHarmony::HarmonyWidgetReady, sm_pInstance, &Harmony::OnHarmonyWidgetReady);
		sm_pInstance->m_MainWindowRef.SetStatus("Loading new Harmony instance", 0);
	}
	else
		sm_pInstance->m_MainWindowRef.SetCurrentProject(nullptr);
}

/*static*/ void Harmony::Reload(Project *pProject)
{
	//if(sm_pInstance->m_pWidget->IsProject(pProject) == false)
	//{
	//	//MainWindow::ClearLoading();
	//	return;
	//}

	SetProject(nullptr);
	SetProject(pProject);
}

/*static*/ void Harmony::OnProjectDestructor(Project *pProject)
{
	if(sm_pInstance->m_pWidget->IsProject(pProject))
		sm_pInstance->m_pWidget->CloseProject();
}

/*static*/ HarmonyWidget *Harmony::GetHarmonyWidget(Project *pProject)
{
	if(sm_pInstance->m_pWidget->IsProject(pProject) == false)
	{
		HyGuiLog("Harmony::GetWidget passed a project that is not the currently set project", LOGTYPE_Error);
		return nullptr;
	}

	return sm_pInstance->m_pWidget;
}

/*slot*/ void Harmony::OnHarmonyWidgetReady(WgtHarmony *pWidget)
{
	m_pWidget->GetProject()->HarmonyInitialize();

	m_MainWindowRef.SetCurrentProject(m_pWidget->GetProject());
	m_MainWindowRef.ClearLoading(LOADINGTYPE_ReloadHarmony);
}
