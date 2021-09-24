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

#ifdef HY_PLATFORM_BROWSER
	EM_BOOL EmscriptenResizeCallback(int eventType, const EmscriptenUiEvent *e, void *userData)
	{
		glm::ivec2 vNewWindowSize(e->windowInnerWidth, e->windowInnerHeight);

		HyLog("Em Resize Callback: " << vNewWindowSize.x << ", " << vNewWindowSize.y);
		HyEngine::Window().SetWindowSize(vNewWindowSize);

		return 0;
	}
#endif

HyEngine *HyEngine::sm_pInstance = nullptr;

HyEngine::HyEngine(const HarmonyInit &initStruct) :
	m_Init(initStruct),
	m_Console(m_Init.bUseConsole, m_Init.consoleInfo),
	m_WindowManager(m_Init.uiNumWindows, m_Init.bShowCursor, m_Init.windowInfo),
	m_Audio(),
	m_Scene(m_Audio.GetCore(), m_WindowManager.GetWindowList()),
	m_Assets(m_Audio.GetCore(), m_Scene, m_Init.sDataDir),
	m_GuiComms(m_Init.uiDebugPort, m_Assets),
	m_Time(m_Init.uiUpdateTickMs),
	m_Diagnostics(m_Init, m_Time, m_Assets, m_Scene),
	m_Input(m_Init.uiNumInputMappings, m_WindowManager.GetWindowList()),
	m_Renderer(m_Diagnostics, m_WindowManager.GetWindowList())
{
	HyAssert(sm_pInstance == nullptr, "Only one instance of IHyEngine may exist. Delete existing instance before constructing again.");

#ifndef HY_CONFIG_SINGLETHREAD
	// TODO Cleanup: decide whether to block 
	while(m_Assets.IsInitialized() == false)
	{
	}
#endif

#ifdef HY_PLATFORM_BROWSER
	emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, EmscriptenResizeCallback);
#endif

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

#ifdef HY_USE_GLFW
	glfwTerminate(); // TODO: this is leaking 304 bytes of memory on my machine
#endif

#ifdef HY_USE_SDL2
	SDL_Quit();
#endif

	// Below prints all the memory leaks to stdout once the program exits (if in debug and MSVC compiler on Windows)
	// _CrtSetBreakAlloc(18); // set memory-allocation breakpoints in code
#if defined(HY_DEBUG) && defined(HY_COMPILER_MSVC) && defined(HY_PLATFORM_WINDOWS) && !defined(HY_USE_SPINE)
	HY_SET_CRT_DEBUG_FIELD(_CRTDBG_LEAK_CHECK_DF);
#endif

	sm_pInstance = nullptr;
}

HyRendererInterop &HyEngine::GetRenderer()
{
	return m_Renderer;
}

#ifdef HY_PLATFORM_BROWSER
	void HyEngine_BrowserUpdate(void *pUserData)
	{
		reinterpret_cast<HyEngine *>(pUserData)->Update();
	}
#endif

int32 HyEngine::RunGame()
{
	m_Diagnostics.BootMessage();
	m_Time.ResetDelta();

#ifndef HY_PLATFORM_BROWSER
	HyLogTitle("Starting Update Loop");
	while(Update())
	{ }
#else
	HyLogTitle("Setting Emscripten Loop");
	emscripten_set_main_loop_arg(HyEngine_BrowserUpdate, this, 0, false);
#endif

	return 0;
}

bool HyEngine::Update()
{
	m_Time.CalcTimeDelta();
	m_Diagnostics.ApplyTimeDelta();

#ifdef HY_CONFIG_SINGLETHREAD
	IHyThreadClass::SingleThreadUpdate();
#endif

	//HyThrottleUpdate
	{
		m_Scene.UpdateNodes();
		m_Scene.UpdatePhysics();
		m_Audio.GetCore().OnUpdate();
		
		HY_PROFILE_BEGIN(HYPROFILERSECTION_Update)
		if(PollPlatformApi() == false || OnUpdate() == false)
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

bool HyEngine::PollPlatformApi()
{
#ifdef HY_USE_GLFW
	for(uint32 i = 0; i < m_Init.uiNumWindows; ++i)
	{
		if(glfwWindowShouldClose(HyEngine::Window(i).GetInterop()))
			return false;
	}

	// TODO: invoke OnWindowResized() and OnWindowMoved() where appropriate

	glfwPollEvents();
#elif defined(HY_USE_SDL2)
	SDL_Event sdlEvent;
	while(SDL_PollEvent(&sdlEvent))
	{
		switch(sdlEvent.type)
		{
		case SDL_QUIT:
			return false;
		case SDL_WINDOWEVENT: {
			HyWindow *pWindow = m_WindowManager.DoEvent(sdlEvent, m_Input);
			if(pWindow && sdlEvent.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				OnWindowResized(*pWindow);
			else if(pWindow && sdlEvent.window.event == SDL_WINDOWEVENT_MOVED)
				OnWindowMoved(*pWindow);
			break; }
		case SDL_KEYDOWN:
			m_Input.DoKeyDownEvent(sdlEvent);
			break;
		case SDL_KEYUP:
			m_Input.DoKeyUpEvent(sdlEvent);
			break;
		case SDL_MOUSEMOTION:
			m_Input.DoMouseMoveEvent(sdlEvent);
			break;
		case SDL_MOUSEBUTTONDOWN:
			m_Input.DoMouseDownEvent(sdlEvent);
			break;
		case SDL_MOUSEBUTTONUP:
			m_Input.DoMouseUpEvent(sdlEvent);
			break;
		case SDL_MOUSEWHEEL:
			m_Input.DoMouseWheelEvent(sdlEvent);
			break;
		}
	}
#endif
	m_Input.Update();
	return true;
}

/*static*/ bool HyEngine::IsInitialized()
{
	return sm_pInstance != nullptr;
}

/*static*/ const HarmonyInit &HyEngine::InitValues()
{
	HyAssert(sm_pInstance != nullptr, "HyEngine::InitValues() was invoked before engine has been initialized.");
	return sm_pInstance->m_Init;
}

/*static*/ float HyEngine::DeltaTime()
{
	HyAssert(sm_pInstance != nullptr, "HyEngine::DeltaTime() was invoked before engine has been initialized.");
	return sm_pInstance->m_Time.GetUpdateStepSeconds();
}

/*static*/ double HyEngine::DeltaTimeD()
{
	HyAssert(sm_pInstance != nullptr, "HyEngine::DeltaTimeD() was invoked before engine has been initialized.");
	return sm_pInstance->m_Time.GetUpdateStepSecondsDbl();
}

/*static*/ void HyEngine::PauseGame(bool bPause)
{
	HyAssert(sm_pInstance != nullptr, "HyEngine::PauseGame() was invoked before engine has been initialized.");
	sm_pInstance->m_Scene.SetPause(bPause);
}

/*static*/ uint32 HyEngine::NumWindows()
{
	HyAssert(sm_pInstance != nullptr, "HyEngine::NumWindows() was invoked before engine has been initialized.");
	return static_cast<uint32>(sm_pInstance->m_WindowManager.GetWindowList().size());
}

/*static*/ HyWindow &HyEngine::Window(uint32 uiWindowIndex /*= 0*/)
{
	HyAssert(sm_pInstance != nullptr, "HyEngine::Window() was invoked before engine has been initialized.");
	return sm_pInstance->m_WindowManager.GetWindow(uiWindowIndex);
}

/*static*/ HyInput &HyEngine::Input()
{
	HyAssert(sm_pInstance != nullptr, "HyEngine::Input() was invoked before engine has been initialized.");
	return sm_pInstance->m_Input;
}

/*static*/ HyAudioHarness &HyEngine::Audio()
{
	HyAssert(sm_pInstance != nullptr, "HyEngine::Audio() was invoked before engine has been initialized.");
	return sm_pInstance->m_Audio;
}

/*static*/ void HyEngine::LoadingStatus(uint32 &uiNumQueuedOut, uint32 &uiTotalOut)
{
	HyAssert(sm_pInstance != nullptr, "HyEngine::LoadingStatus() was invoked before engine has been initialized.");
	return sm_pInstance->m_Assets.GetNodeLoadingStatus(uiNumQueuedOut, uiTotalOut);
}

/*static*/ HyDiagnostics &HyEngine::Diagnostics()
{
	HyAssert(sm_pInstance != nullptr, "HyEngine::Diagnostics() was invoked before engine has been initialized.");
	return sm_pInstance->m_Diagnostics;
}

/*static*/ HyShaderHandle HyEngine::DefaultShaderHandle(HyType eType)
{
	HyAssert(sm_pInstance != nullptr, "HyEngine::DefaultShaderHandle() was invoked before engine has been initialized.");
	return sm_pInstance->m_Renderer.GetDefaultShaderHandle(eType);
}

/*static*/ std::string HyEngine::DateTime()
{
	HyAssert(sm_pInstance != nullptr, "HyEngine::DateTime() was invoked before engine has been initialized.");
	return sm_pInstance->m_Time.GetDateTime();
}

/*static*/ std::string HyEngine::DataDir()
{
	HyAssert(sm_pInstance != nullptr, "HyEngine::DataDir() was invoked before engine has been initialized.");

	char *pBuffer = nullptr;

	// Get the current working directory:
#if defined(HY_PLATFORM_WINDOWS)
	pBuffer = _getcwd(nullptr, 0);
	HyAssert(pBuffer, "_getcwd error");
#elif defined(HY_PLATFORM_LINUX)
	pBuffer = getcwd(nullptr, 0);
	HyAssert(pBuffer, "getcwd error");
#endif

	std::string sAbsDataDir;
	if(pBuffer)
	{
		sAbsDataDir = pBuffer;
		sAbsDataDir += "/";
	}
	free(pBuffer);

	sAbsDataDir += sm_pInstance->m_Assets.GetDataDir();
	sAbsDataDir = HyIO::CleanPath(sAbsDataDir.c_str(), "/", false);
	
	return sAbsDataDir;
}



