/**************************************************************************
 *	HyRenderer_OpenGL.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/HyRenderer.h"

#if defined(HY_PLATFORM_WINDOWS) && !defined(HY_PLATFORM_GUI)
	#include "Renderer/GfxApi/OpenGL/Interop/HyOpenGL_Win.h"
#elif defined(HY_PLATFORM_OSX) && !defined(HY_PLATFORM_GUI)
	#include "Renderer/GfxApi/OpenGL/Interop/HyOpenGL_OSX.h"
#elif defined(HY_PLATFORM_LINUX) && !defined(HY_PLATFORM_GUI)
	#include "Renderer/GfxApi/OpenGL/Interop/HyOpenGL_Linux.h"
#elif defined(HY_PLATFORM_GUI)
	#include "Renderer/GfxApi/OpenGL/HyOpenGL.h"
#endif

//#include "Renderer/DrawData/HyDrawSprite2d.h"
//#include "Renderer/DrawData/HyDrawText2d.h"
//#include "Renderer/DrawData/HyDrawSpine2d.h"
//#include "Renderer/DrawData/HyDrawPrimitive2d.h"

//uint32 HyRenderer::sm_uiLargest2dDrawSize = 0;

HyRenderer::HyRenderer(HyViewport &gameViewportRef, HY_GFX_API *pSuppliedGfx /*= NULL*/)
{
	//sm_uiLargest2dDrawSize = 0;
	//if(sizeof(HyDrawQuadBatch2d) > sm_uiLargest2dDrawSize)
	//	sm_uiLargest2dDrawSize = sizeof(HyDrawQuadBatch2d);
	//if(sizeof(HyDrawSprite2d) > sm_uiLargest2dDrawSize)
	//	sm_uiLargest2dDrawSize = sizeof(HyDrawSprite2d);
	//if(sizeof(HyDrawPrimitive2d) > sm_uiLargest2dDrawSize)
	//	sm_uiLargest2dDrawSize = sizeof(HyDrawPrimitive2d);
	//if(sizeof(HyDrawText2d) > sm_uiLargest2dDrawSize)
	//	sm_uiLargest2dDrawSize = sizeof(HyDrawText2d);

	m_GfxComms.SetGfxInit(gameViewportRef);

	if(pSuppliedGfx)
		m_pGfxApi = pSuppliedGfx;
	else
		m_pGfxApi = new HY_GFX_API();

	m_pGfxApi->SetGfxComms(&m_GfxComms);

#ifdef HY_MULTITHREADING
	// Setup and run/start render thread
	m_pRenderThread = ThreadManager::Get()->BeginThread(_T("Render Thread"), THREAD_START_PROCEDURE(RenderThread), m_pGfxApi);
#endif
}

HyRenderer::~HyRenderer()
{
}

#ifdef HY_MULTITHREADING
//-------------------------------------------------------------------------
// Render
//
// Will initialize the gfx API and start a looping thread update which 
// draws a render description buffer.
//-------------------------------------------------------------------------
/*static*/ void HyRenderer::RenderThread(void *pParam)
{
	HY_GFX_API *pGfxApi = reinterpret_cast<HY_GFX_API *>(pParam);

	if(pGfxApi->Initialize() == false)
	{
		HyError("Graphics API's Initialize() failed");
	}

	HyAssert(pGfxApi->GetGfxInfo(), "Graphics API must m_GfxComms.SetGfxInfo() within its Initialize()");

	while (pGfxApi->PollApi())
		pGfxApi->Update();

	pGfxApi->Shutdown();
}
#endif

