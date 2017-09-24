/**************************************************************************
 *	HyEngine.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "HyEngine.h"

#include <stdio.h>

HyEngine *		HyEngine::sm_pInstance = NULL;

#ifdef HY_PLATFORM_GUI
	#define HyThrottleUpdate
#else
	#define HyThrottleUpdate while(m_Time.ThrottleUpdate())
#endif

// Private ctor() invoked from RunGame()
HyEngine::HyEngine(IHyApplication &appRef) :	m_AppRef(appRef),
												m_Scene(m_AppRef.m_WindowList),
												m_Assets(m_AppRef.m_Init.sDataDir, m_Scene),
												m_GuiComms(m_AppRef.m_Init.uiDebugPort, m_Assets),
												m_Input(m_AppRef.m_Init.uiNumInputMappings, m_AppRef.m_WindowList),
												m_Renderer(m_Diagnostics, m_AppRef.m_Init.bShowCursor, m_AppRef.m_WindowList),
												m_Audio(m_AppRef.m_WindowList),
												m_Diagnostics(m_AppRef.m_Init, m_Assets, m_Scene),
												m_Time(m_Scene, m_AppRef.m_Init.uiUpdateTickMs)
{
	HyAssert(sm_pInstance == NULL, "HyEngine::RunGame() must instanciate the engine once per HyEngine::Shutdown(). HyEngine ptr already created");

	if(m_Renderer.Initialize() == false)
		HyLogError("IHyRenderer::Initialize() failed");

	m_AppRef.SetInputMapPtr(static_cast<HyInputMapInterop *>(m_Input.GetInputMapArray()));
}

HyEngine::~HyEngine()
{
}

/*static*/ void HyEngine::RunGame(IHyApplication *pGame)
{
	HyAssert(pGame, "HyEngine::RunGame was passed a nullptr");

	sm_pInstance = HY_NEW HyEngine(*pGame);

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

		HY_PROFILE_BEGIN("Update")
		if(PollPlatformApi() == false || m_AppRef.Update() == false)
			return false;
		HY_PROFILE_END

		m_Assets.Update(m_Renderer);
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
		m_Scene.PrepareRender(m_Renderer);
		m_Renderer.Render();
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

//#if defined(HY_PLATFORM_WINDOWS) && !defined(HY_PLATFORM_GUI)
//	MSG msg = {0};
//	int32 iWindowIndex = 0;
//	HWND hWnd = m_Renderer.GetHWND(iWindowIndex);
//
//	while(hWnd != nullptr)
//	{
//		while(PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
//		{
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
//
//			// Mouse hovering over a window will count as Input's current window
//			if(msg.message == WM_MOUSEMOVE)
//			{
//				for(uint32 i = 0; i < m_Renderer.GetNumRenderSurfaces(); ++i)
//				{
//					if(m_Renderer.GetHWND(i) == msg.hwnd)
//					{
//						m_Input.SetWindowIndex(i);
//						break;
//					}
//				}
//			}
//
//			m_Input.HandleMsg(&msg);
//		}
//
//		iWindowIndex++;
//		hWnd = m_Renderer.GetHWND(iWindowIndex);
//	}
//#endif

	m_Input.Update();
	return true;
}

HyRendererInterop &HyEngine::GetRenderer()
{
	return m_Renderer;
}

/*friend*/ float HyUpdateDelta()
{
	HyAssert(HyEngine::sm_pInstance != nullptr, "HyUpdateDelta() was invoked before engine has been initialized.");
	return HyEngine::sm_pInstance->m_Time.GetUpdateStepSeconds();
}

/*friend*/ float Hy_TimeDelta()
{
	HyAssert(HyEngine::sm_pInstance != nullptr, "Hy_TimeDelta() was invoked before engine has been initialized.");
	return HyEngine::sm_pInstance->m_Time.GetFrameDelta();
}

/*friend*/ void Hy_SetUpdateTickMs(uint32 uiUpdateTickMs)
{
	HyAssert(HyEngine::sm_pInstance != nullptr, "Hy_SetUpdateTickMs() was invoked before engine has been initialized.");
	HyEngine::sm_pInstance->m_Time.SetUpdateTickMs(uiUpdateTickMs);
}

/*friend*/ void HyPauseGame(bool bPause)
{
	HyAssert(HyEngine::sm_pInstance != nullptr, "HyPauseGame() was invoked before engine has been initialized.");
	HyEngine::sm_pInstance->m_Scene.SetPause(bPause);
}

/*friend*/ b2World &Hy_Physics2d()
{
	HyAssert(HyEngine::sm_pInstance != nullptr, "Hy_Physics2d() was invoked before engine has been initialized.");
	return HyEngine::sm_pInstance->m_Scene.GetPhysics2d();
}

/*friend*/ HyDiagnostics &HyGetDiagnostics()
{
	HyAssert(HyEngine::sm_pInstance != nullptr, "HyGetDiagnostics() was invoked before engine has been initialized.");
	return HyEngine::sm_pInstance->m_Diagnostics;
}

/*friend*/ float HyPixelsPerMeter()
{
	return IHyApplication::PixelsPerMeter();
}

/*friend*/ HyCoordinateUnit HyDefaultCoordinateUnit()
{
	return IHyApplication::DefaultCoordinateUnit();
}

/*friend*/ std::string HyDateTime()
{
	HyAssert(HyEngine::sm_pInstance != nullptr, "HyDateTime() was invoked before engine has been initialized.");
	return HyEngine::sm_pInstance->m_Time.GetDateTime();
}
