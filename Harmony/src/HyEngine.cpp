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
											m_Renderer(m_GfxBuffer, m_AppRef.m_vViewports, m_AppRef.m_Init.pSuppliedGfx),
											m_Creator(m_GfxBuffer, m_AppRef.m_vViewports[0], m_AppRef.m_Init.eDefaultCoordinateType, m_AppRef.m_Init.fPixelsPerMeter)
{
	HyAssert(sm_pInstance == NULL, "HyEngine::RunGame() must instanciate the engine once per HyEngine::Shutdown(). HyEngine ptr already created");

	HyFileIO::InitAtlasData();
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

	gameRef.Shutdown();
	
	delete sm_pInstance;
}

void HyEngine::Initialize()
{
	if(pSuppliedGfx)
		m_pGfxApi = pSuppliedGfx;
	else
		m_pGfxApi = new HY_GFX_API();

	m_Renderer.SetGfxComms(&m_GfxComms);
	m_Renderer.SetViewportRef(&vViewportsRef);

	if(m_Renderer.CreateWindows() == false)
		HyError("Graphics API's CreateWindows() failed");

	if(m_Renderer.Initialize() == false)
		HyError("Graphics API's Initialize() failed");

	HyAssert(m_Renderer.GetGfxInfo(), "Graphics API must m_GfxComms.SetGfxInfo() within its Initialize()");

	m_AppRef.Initialize();
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

