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


HyMemoryHeap &	HyEngine::sm_Mem = IHyApplication::GetMemoryHeap();

// Private ctor() invoked from RunGame()
HyEngine::HyEngine(IHyApplication &appRef) :	m_AppRef(appRef),
												m_Scene(m_GfxBuffer, m_AppRef.m_vWindows, m_AppRef.m_Init.eDefaultCoordinateType, m_AppRef.m_Init.fPixelsPerMeter),
												m_AssetManager(m_AppRef.m_Init.szDataDir, m_GfxBuffer, m_Scene),
												m_GuiComms(m_AppRef.m_Init.uiDebugPort, m_AssetManager),
												m_Input(m_AppRef.m_vInputMaps),
												m_Renderer(m_GfxBuffer, m_AppRef.m_vWindows)
{
	HyAssert(sm_pInstance == NULL, "HyEngine::RunGame() must instanciate the engine once per HyEngine::Shutdown(). HyEngine ptr already created");

	if(m_AppRef.Initialize() == false)
		HyError("IApplication Initialize() failed");
}

HyEngine::~HyEngine()
{
}

void * HyEngine::operator new(size_t size)
{
	return sm_Mem.Alloc(size);
}

void HyEngine::operator delete(void *ptr)
{
	sm_Mem.Free(ptr);
}

/*static*/ void HyEngine::RunGame(IHyApplication &gameRef)
{
	sm_pInstance = new HyEngine(gameRef);
	
	while(sm_pInstance->Update())
	{ }

	gameRef.Shutdown();
	
	delete sm_pInstance;
}

bool HyEngine::Update()
{
	switch(m_AssetManager.IsReloading())
	{
	case HYRELOADCODE_Inactive:
		break;
			
	case HYRELOADCODE_InProgress:
		return true;

	case HYRELOADCODE_ReInit:
	{
		// Re-instantiate the HyAssetManager on top of the same memory. 
		std::string sNewDataDir = m_AssetManager.GetNewDataDirPath();
		m_AssetManager.~HyAssetManager();
		new (&m_AssetManager) HyAssetManager(sNewDataDir.c_str(), m_GfxBuffer, m_Scene);
	}
	// Above should fall through to reset delta
	case HYRELOADCODE_Finished:
		m_Time.ResetDelta();
		break;
	}

	while(m_Time.ThrottleTime())
	{
		m_Input.Update();

		if(PollPlatformApi() == false)
			return false;
		
		m_Scene.PreUpdate();
		if(m_AppRef.Update() == false)
			return false;

		m_AssetManager.Update();
		m_Scene.PostUpdate();

		m_GuiComms.Update();
	}

	m_Renderer.Update();
	return true;
}

bool HyEngine::PollPlatformApi()
{
#if defined(HY_PLATFORM_WINDOWS) && !defined(HY_PLATFORM_GUI)
	// TODO: return false when windows close message comes in or something similar
	MSG msg = { 0 };
	for(uint32 i = 0; i < static_cast<uint32>(m_AppRef.m_vWindows.size()); ++i)
	{
		while(PeekMessage(&msg, m_Renderer.GetHWND(i), 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			
			// Only take input from the main window
			if(i == 0)
				m_Input.HandleMsg(msg);
		}
	}
#endif

	return true;
}

