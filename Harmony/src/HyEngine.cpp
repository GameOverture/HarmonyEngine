/**************************************************************************
 *	HyEngine.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "HyEngine.h"

#include <stdio.h>

HyEngine *		HyEngine::sm_pInstance = nullptr;

#ifdef HY_PLATFORM_GUI
	#define HyThrottleUpdate

	/*static*/ HyEngine *HyEngine::GuiCreate(IHyApplication &projectRef)
	{
		if(sm_pInstance != nullptr)
			return nullptr;

		sm_pInstance = new HyEngine(projectRef);
		return sm_pInstance;
	}

	/*static*/ void HyEngine::GuiDelete()
	{
		if(sm_pInstance)
		{
			sm_pInstance->m_AppRef.Shutdown();
			sm_pInstance->Shutdown();
		}

		delete sm_pInstance;
		sm_pInstance = nullptr;
	}

#else
	#define HyThrottleUpdate while(m_Time.ThrottleUpdate())
#endif

// Private ctor() invoked from RunGame()
HyEngine::HyEngine(IHyApplication &appRef) :	m_AppRef(appRef),
												m_Scene(m_AppRef.m_WindowList),
												m_Assets(m_AppRef.m_Init.sDataDir, m_Scene),
												m_GuiComms(m_AppRef.m_Init.uiDebugPort, m_Assets),
												m_Time(m_AppRef.m_Init.uiUpdateTickMs),
												m_Diagnostics(m_AppRef.m_Init, m_Time, m_Assets, m_Scene),
												m_Input(m_AppRef.m_Init.uiNumInputMappings, m_AppRef.m_WindowList),
												m_Renderer(m_Diagnostics, m_AppRef.m_WindowList),
												m_Audio(m_AppRef.m_WindowList)
{
	HyAssert(sm_pInstance == nullptr, "HyEngine::RunGame() must instanciate the engine once per HyEngine::Shutdown(). HyEngine ptr already created");
}

HyEngine::~HyEngine()
{
}

/*static*/ void HyEngine::RunGame(IHyApplication *pGame)
{
	HyAssert(pGame, "HyEngine::RunGame was passed a nullptr");

	sm_pInstance = HY_NEW HyEngine(*pGame);

	pGame->SetInputMapPtr(sm_pInstance->m_Input.GetInputMapArray());
	sm_pInstance->m_Input.InitCallbacks();

	while(sm_pInstance->IsInitialized() == false)
	{ }

	sm_pInstance->m_Diagnostics.BootMessage();

	if(pGame->Initialize() == false)
		HyError("IApplication Initialize() failed");

	sm_pInstance->m_Time.ResetDelta();

	HyLogTitle("Starting Update Loop");
	while(sm_pInstance->Update())
	{ }

	pGame->Shutdown();
	sm_pInstance->Shutdown();

	delete sm_pInstance;
	sm_pInstance = nullptr;

	delete pGame;

	// Below prints all the memory leaks to stdout once the program exits (if in debug and MSVC compiler)
#if defined(HY_DEBUG) && defined(_MSC_VER)
	HY_SET_CRT_DEBUG_FIELD(_CRTDBG_LEAK_CHECK_DF);
#endif
}

bool HyEngine::IsInitialized()
{
	if(m_Assets.IsLoaded() == false)
		return false;

	return true;
}

bool HyEngine::Update()
{
	m_Time.CalcTimeDelta();
	m_Diagnostics.ApplyTimeDelta();

	//HyThrottleUpdate
	{
		m_Scene.UpdatePhysics();
		m_Scene.UpdateNodes();

		HY_PROFILE_BEGIN(HYPROFILERSECTION_Update)
		if(PollPlatformApi() == false || m_AppRef.Update() == false)
			return false;
		HY_PROFILE_END

		m_Assets.Update(m_Renderer);
		m_Renderer.ProcessMsgs();

		m_GuiComms.Update();
	}

	m_Scene.PrepareRender(m_Renderer);
	m_Renderer.Render();

	return true;
}

void HyEngine::Shutdown()
{
	// Unload any load-pending assets
	m_Assets.Shutdown();
	while(m_Assets.IsShutdown() == false)
	{
		m_Assets.Update(m_Renderer);
		m_Renderer.ProcessMsgs();
	}
}

bool HyEngine::PollPlatformApi()
{
#ifdef HY_PLATFORM_DESKTOP
	for(uint32 i = 0; i < m_AppRef.GetNumWindows(); ++i)
	{
		if(glfwWindowShouldClose(m_AppRef.Window(i).GetHandle()))
			return false;
	}

	glfwPollEvents();
#endif

	m_Input.Update();
	return true;
}

HyRendererInterop &HyEngine::GetRenderer()
{
	return m_Renderer;
}

/*friend*/ float Hy_UpdateStep()
{
	HyAssert(HyEngine::sm_pInstance != nullptr, "Hy_UpdateStep() was invoked before engine has been initialized.");
	return HyEngine::sm_pInstance->m_Time.GetUpdateStepSeconds();
}

/*friend*/ void Hy_PauseGame(bool bPause)
{
	HyAssert(HyEngine::sm_pInstance != nullptr, "Hy_PauseGame() was invoked before engine has been initialized.");
	HyEngine::sm_pInstance->m_Scene.SetPause(bPause);
}

/*friend*/ HyInput &Hy_Input()
{
	HyAssert(HyEngine::sm_pInstance != nullptr, "Hy_Input() was invoked before engine has been initialized.");
	return HyEngine::sm_pInstance->m_Input;
}

/*friend*/ b2World &Hy_Physics2d()
{
	HyAssert(HyEngine::sm_pInstance != nullptr, "Hy_Physics2d() was invoked before engine has been initialized.");
	return HyEngine::sm_pInstance->m_Scene.GetPhysics2d();
}

/*friend*/ HyDiagnostics &Hy_Diagnostics()
{
	HyAssert(HyEngine::sm_pInstance != nullptr, "Hy_Diagnostics() was invoked before engine has been initialized.");
	return HyEngine::sm_pInstance->m_Diagnostics;
}

/*friend*/ HyShaderHandle Hy_DefaultShaderHandle(HyType eType)
{
	HyAssert(HyEngine::sm_pInstance != nullptr, "Hy_DefaultShaderHandle() was invoked before engine has been initialized.");
	return HyEngine::sm_pInstance->m_Renderer.GetDefaultShaderHandle(eType);
}

/*friend*/ bool Hy_IsDrawInst(HyType eType)
{
	switch(eType)
	{
	case HYTYPE_Particles2d:
	case HYTYPE_Sprite2d:
	case HYTYPE_Spine2d:
	case HYTYPE_TexturedQuad2d:
	case HYTYPE_Primitive2d:
	case HYTYPE_Text2d:
	case HYTYPE_Mesh3d:
		return true;
	}

	return false;
}

/*friend*/ std::string Hy_DateTime()
{
	HyAssert(HyEngine::sm_pInstance != nullptr, "Hy_DateTime() was invoked before engine has been initialized.");
	return HyEngine::sm_pInstance->m_Time.GetDateTime();
}
