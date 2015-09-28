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
												m_Input(m_AppRef.m_pInputMap, m_AppRef.m_Init.uiNumInputMappings),
												m_Renderer(m_GfxBuffer, m_AppRef.m_vViewports),
												m_Creator(m_GfxBuffer, m_AppRef.m_vViewports[0], m_AppRef.m_Init.eDefaultCoordinateType, m_AppRef.m_Init.fPixelsPerMeter)
{
	HyAssert(sm_pInstance == NULL, "HyEngine::RunGame() must instanciate the engine once per HyEngine::Shutdown(). HyEngine ptr already created");

	HyFileIO::InitAtlasData();

	if(m_Renderer.Initialize() == false)
		HyError("Graphics API's Initialize() failed");

	if(m_AppRef.Initialize() == false)
		HyError("IApplication Initialize() failed");
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
	while(m_Time.ThrottleTime())
	{
		m_Input.Update();
		m_Creator.PreUpdate();
		
		if(m_AppRef.Update() == false)
			return false;

#ifndef HY_PLATFORM_GUI
		m_GuiComms.Update();
#endif

		m_Creator.PostUpdate();
	}

	return m_Renderer.Update();
}

