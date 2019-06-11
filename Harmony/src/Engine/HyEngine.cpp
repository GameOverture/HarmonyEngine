/**************************************************************************
 *	HyEngine.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "HyEngine.h"

#include <stdio.h>

#ifdef HY_PLATFORM_GUI
	#define HyThrottleUpdate
#else
	#define HyThrottleUpdate while(m_Time.ThrottleUpdate())
#endif

HyEngine *HyEngine::sm_pInstance = nullptr;

HyEngine::HyEngine(HarmonyInit &initStruct) :
	m_Init(initStruct),
	m_WindowManager(m_Init.uiNumWindows, m_Init.bShowCursor, m_Init.windowInfo),
	m_Console(initStruct.bUseConsole, initStruct.consoleInfo),
	m_Audio(m_Init.sDataDir),
	m_Scene(m_WindowManager.GetWindowList()),
	m_Assets(m_Audio, m_Scene, m_Init.sDataDir),
	m_GuiComms(m_Init.uiDebugPort, m_Assets),
	m_Time(m_Init.uiUpdateTickMs),
	m_Diagnostics(m_Init, m_Time, m_Assets, m_Scene),
	m_Input(m_Init.uiNumInputMappings, m_WindowManager.GetWindowList()),
	m_Renderer(m_Diagnostics, m_WindowManager.GetWindowList())
{
	HyAssert(sm_pInstance == nullptr, "Only one instance of IHyEngine may exist. Delete existing instance before constructing again.");
	HyAssert(m_Init.fPixelsPerMeter > 0.0f, "HarmonyInit's 'fPixelsPerMeter' cannot be <= 0.0f");

	// TODO Cleanup: decide whether to block 
	while(m_Assets.IsInitialized() == false)
	{
	}

	sm_pInstance = this;
}

HyEngine::~HyEngine()
{
	// Unload any load-pending assets
	m_Assets.Shutdown();
	while(m_Assets.IsShutdown() == false)
	{
		m_Assets.Update(m_Renderer);
		m_Renderer.ProcessMsgs();
	}

#ifdef HY_PLATFORM_DESKTOP
	glfwTerminate();
#endif

	// Below prints all the memory leaks to stdout once the program exits (if in debug and MSVC compiler on Windows)
#if defined(HY_DEBUG) && defined(_MSC_VER) && defined(HY_PLATFORM_WINDOWS)
	HY_SET_CRT_DEBUG_FIELD(_CRTDBG_LEAK_CHECK_DF);
#endif

	sm_pInstance = nullptr;
}

HyRendererInterop &HyEngine::GetRenderer()
{
	return m_Renderer;
}

void HyEngine::RunGame()
{
	m_Diagnostics.BootMessage();
	m_Time.ResetDelta();

	HyLogTitle("Starting Update Loop");
	while(Update())
	{ }
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
		if(PollPlatformApi() == false || OnUpdate() == false)
			return false;
		HY_PROFILE_END

		m_Audio.Update();

		m_Assets.Update(m_Renderer);
		m_Renderer.ProcessMsgs();

		m_GuiComms.Update();
	}

	m_Scene.PrepareRender(m_Renderer);
	m_Renderer.Render();

	return true;
}

bool HyEngine::PollPlatformApi()
{
#ifdef HY_PLATFORM_DESKTOP
	for(uint32 i = 0; i < m_Init.uiNumWindows; ++i)
	{
		if(glfwWindowShouldClose(Hy_Window(i).GetHandle()))
			return false;
	}

	glfwPollEvents();
#endif

	m_Input.Update();
	return true;
}

/*friend*/ bool Hy_IsInitialized()
{
	return HyEngine::sm_pInstance != nullptr;
}

/*friend*/ const HarmonyInit &Hy_Init()
{
	HyAssert(HyEngine::sm_pInstance != nullptr, "Hy_Init() was invoked before engine has been initialized.");
	return HyEngine::sm_pInstance->m_Init;
}

/*friend*/ float Hy_UpdateStep()
{
	HyAssert(HyEngine::sm_pInstance != nullptr, "Hy_UpdateStep() was invoked before engine has been initialized.");
	return HyEngine::sm_pInstance->m_Time.GetUpdateStepSeconds();
}

/*friend*/ double Hy_UpdateStepDbl()
{
	HyAssert(HyEngine::sm_pInstance != nullptr, "Hy_UpdateStepDbl() was invoked before engine has been initialized.");
	return HyEngine::sm_pInstance->m_Time.GetUpdateStepSecondsDbl();
}

/*friend*/ void Hy_PauseGame(bool bPause)
{
	HyAssert(HyEngine::sm_pInstance != nullptr, "Hy_PauseGame() was invoked before engine has been initialized.");
	HyEngine::sm_pInstance->m_Scene.SetPause(bPause);
}

/*friend*/ HyWindow &Hy_Window(uint32 uiWindowIndex /*= 0*/)
{
	HyAssert(HyEngine::sm_pInstance != nullptr, "Hy_Window() was invoked before engine has been initialized.");
	return HyEngine::sm_pInstance->m_WindowManager.GetWindow(uiWindowIndex);
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
	case HYTYPE_Particles:
	case HYTYPE_Sprite:
	case HYTYPE_Spine:
	case HYTYPE_TexturedQuad:
	case HYTYPE_Primitive:
	case HYTYPE_Text:
	case HYTYPE_Prefab:
		return true;
	}

	return false;
}

/*friend*/ std::string Hy_DateTime()
{
	HyAssert(HyEngine::sm_pInstance != nullptr, "Hy_DateTime() was invoked before engine has been initialized.");
	return HyEngine::sm_pInstance->m_Time.GetDateTime();
}

/*friend*/ std::string Hy_DataDir()
{
	HyAssert(HyEngine::sm_pInstance != nullptr, "Hy_DataDir() was invoked before engine has been initialized.");

	char szBuffer[FILENAME_MAX];

#if defined(HY_PLATFORM_WINDOWS)
	_getcwd(szBuffer, FILENAME_MAX);
#elif defined(HY_PLATFORM_LINUX)
	getcwd(szBuffer, FILENAME_MAX);
#endif

	std::string sAbsDataDir(szBuffer);
	sAbsDataDir += "/";
	sAbsDataDir += HyEngine::sm_pInstance->m_Assets.GetDataDir();
	sAbsDataDir = HyStr::MakeStringProperPath(sAbsDataDir.c_str(), "/", false);
	
	return sAbsDataDir;
}
