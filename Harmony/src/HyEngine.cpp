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
#include <stdlib.h>

HyEngine *		HyEngine::sm_pInstance = NULL;

// Private ctor() invoked from RunGame()
HyEngine::HyEngine(IHyApplication &appRef) :	m_AppRef(appRef),
												m_Scene(m_GfxBuffer, m_AppRef.m_WindowList),
												m_Assets(m_AppRef.m_Init.sDataDir, m_GfxBuffer, m_Scene),
												m_GuiComms(m_AppRef.m_Init.uiDebugPort, m_Assets),
												m_Input(m_AppRef.m_Init.uiNumInputMappings, m_AppRef.m_WindowList),
												m_Renderer(m_GfxBuffer, m_Input, m_AppRef.m_Init.bShowCursor, m_AppRef.m_WindowList),
												m_Audio(m_AppRef.m_WindowList)
{
	HyAssert(sm_pInstance == NULL, "HyEngine::RunGame() must instanciate the engine once per HyEngine::Shutdown(). HyEngine ptr already created");

	m_Renderer.StartUp();
	m_AppRef.SetInputMapPtr(static_cast<HyInputMapInterop *>(m_Input.GetInputMapArray()));
}

HyEngine::~HyEngine()
{
}

/*static*/ void HyEngine::RunGame(IHyApplication &gameRef)
{
	sm_pInstance = HY_NEW HyEngine(gameRef);

	while(sm_pInstance->BootUpdate())
	{ }
	
	while(sm_pInstance->Update())
	{ }

	gameRef.Shutdown();
	sm_pInstance->Shutdown();
	
	delete sm_pInstance;

	// Below prints all the memory leaks to stdout once the program exits (if in debug and MSVC compiler)
#if defined(HY_DEBUG) && defined(_MSC_VER)
	HY_SET_CRT_DEBUG_FIELD(_CRTDBG_LEAK_CHECK_DF);
#endif
}

bool HyEngine::BootUpdate()
{
	//while(m_Time.ThrottleTime())
	//{
	//	if(PollPlatformApi() == false)
	//		return false;
	//}

#ifndef HYSETTING_MultithreadedRenderer
	m_Renderer.Update();
#endif

	if(m_Assets.IsLoaded() == false)
		return true;

	// If here, then engine fully loaded
	if(m_AppRef.Initialize() == false)
		HyError("IApplication Initialize() failed");

	return false;
}

bool HyEngine::Update()
{
#ifdef HYSETTING_ThrottleUpdate
	while(m_Time.ThrottleTime())
#else
	m_Time.ThrottleTime();
#endif
	{
		m_Input.Update();
		m_Scene.PreUpdate();

		if(m_AppRef.Update() == false)
			return false;

		m_Assets.Update();
		m_Scene.PostUpdate();
		m_GuiComms.Update();
				
		// GUI renderer paints on a timer which doesn't work well with fixed updates like this. Ensures only single updates per frame.
#if defined(HY_PLATFORM_GUI) && defined(HYSETTING_ThrottleUpdate)
		break;
#endif
	}

#if !defined(HYSETTING_MultithreadedRenderer) || defined(HY_PLATFORM_GUI)
	if(m_Renderer.PollPlatformApi() == false)
		return false;

	m_Renderer.Update();
#endif

	return true;
}

void HyEngine::Shutdown()
{
	// Unload any load-pending assets
	m_Assets.Shutdown();
	while(m_Assets.IsShutdown() == false)
	{
		m_Assets.Update();
		m_Scene.PostUpdate();
#if !defined(HYSETTING_MultithreadedRenderer) || defined(HY_PLATFORM_GUI)
		m_Renderer.Update();
#endif
	}
}

HyRendererInterop &HyEngine::GetRenderer()
{
	return m_Renderer;
}

/*friend*/ void HyPauseGame(bool bPause)
{
	HyEngine::sm_pInstance->m_Scene.SetPause(bPause);
}

/*friend*/ float HyPixelsPerMeter()
{
	return IHyApplication::PixelsPerMeter();
}

/*friend*/ HyCoordinateType HyDefaultCoordinateType()
{
	return IHyApplication::DefaultCoordinateType();
}

/*friend*/ HyCoordinateUnit HyDefaultCoordinateUnit()
{
	return IHyApplication::DefaultCoordinateUnit();
}
