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

HyRenderer::HyRenderer(HyViewport &gameViewportRef, HY_GFX_API *pSuppliedGfx /*= NULL*/)
{
	m_GfxComms.SetGfxInit(gameViewportRef);

	if(pSuppliedGfx)
		m_pGfxApi = pSuppliedGfx;
	else
		m_pGfxApi = new HY_GFX_API();

	m_pGfxApi->SetGfxComms(&m_GfxComms);

	if(m_pGfxApi->CreateWindows() == false)
		HyError("Graphics API's CreateWindows() failed");

	if(m_pGfxApi->Initialize() == false)
		HyError("Graphics API's Initialize() failed");

	HyAssert(m_pGfxApi->GetGfxInfo(), "Graphics API must m_GfxComms.SetGfxInfo() within its Initialize()");
}

HyRenderer::~HyRenderer()
{
}

bool HyRenderer::Update()
{
	return m_pGfxApi->Update();
}

