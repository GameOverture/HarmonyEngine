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


HyMemoryHeap &	HyEngine::sm_Mem = IApplication::GetMemoryHeap();

// Private ctor() invoked from RunGame()
HyEngine::HyEngine(IApplication &appRef) :	m_AppRef(appRef),
											m_Input(m_AppRef.m_Init.uiNumInputMappings, m_AppRef.m_pInputArray),
											m_Time(m_Input),
											m_Renderer(m_AppRef.m_Viewport, m_AppRef.m_Init.pSuppliedGfx),
											m_Creator(m_Renderer.GetGfxComms(), m_AppRef.m_Viewport, m_AppRef.m_Init.eDefaultCoordinateType, m_AppRef.m_Init.fPixelsPerMeter)
{
	HyAssert(sm_pInstance == NULL, "HyEngine::RunGame() must instanciate the engine once per HyEngine::Shutdown(). HyEngine ptr already created");
}

HyEngine::~HyEngine()
{
}

void * HyEngine::operator new(tMEMSIZE size)
{
	return sm_Mem.Alloc(size);
}

void HyEngine::operator delete(void *ptr)
{
	sm_Mem.Free(ptr);
}

/*static*/ void HyEngine::RunGame(IApplication &gameRef)
{
	sm_pInstance = new HyEngine(gameRef);
	
	sm_pInstance->Initialize();

	while(sm_pInstance->Update())
	{ }

	sm_pInstance->Shutdown();
	delete sm_pInstance;
}

void HyEngine::Initialize()
{
#ifdef HY_MULTITHREADING
	// Block here until renderThread initializes
	while(sm_pInstance->m_Creator.m_GfxCommsRef.IsRendererInitialized() == false)
		InteropSleep(30);
#endif

	m_AppRef.Initialize();
}

bool HyEngine::Update()
{
	while(m_Time.ThrottleTime())
	{
		// Update all game and engine by one 'step'
		m_Creator.PreUpdate();
		if(m_AppRef.Update() == false)
			return false;
		m_Creator.PostUpdate();

#ifdef HY_MULTITHREADING
		if(m_Renderer.IsRenderThreadActive() == false)
			return false;
#endif		
	}

	return true;
}

void HyEngine::Shutdown()
{
	m_AppRef.Shutdown();
}
