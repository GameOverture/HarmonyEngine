/**************************************************************************
 *	HyRenderer.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyRenderer_h__
#define __HyRenderer_h__

#include "Afx/HyStdAfx.h"

#include "Threading/Threading.h"
#include "Renderer/HyGfxComms.h"

class HyViewport;
class HY_GFX_API;

class HyRenderer
{
	static uint32			sm_uiLargest2dDrawSize;

#ifdef HY_MULTITHREADING
	ThreadInfoPtr			m_pRenderThread;
#endif
	
	HyGfxComms				m_GfxComms;
	HY_GFX_API *			m_pGfxApi;

public:
	HyRenderer(HyViewport &gameWindowRef, HY_GFX_API *pSuppliedGfx = NULL);
	~HyRenderer();

	static uint32 GetLargest2dDrawSize()	{ return sm_uiLargest2dDrawSize; }
	HyGfxComms &			GetGfxComms()	{ return m_GfxComms; }

#ifdef HY_MULTITHREADING
	bool IsRenderThreadActive()				{ return m_pRenderThread->IsAlive(); }

private:
	static void RenderThread(void *pParam);
#endif
};

#endif
