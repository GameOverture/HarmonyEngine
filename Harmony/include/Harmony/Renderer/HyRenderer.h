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
#include "Afx/HyInteropAfx.h"

#include "Renderer/HyGfxComms.h"

class HyViewport;

class HyRenderer
{
	HyGfxComms				m_GfxComms;
	HY_GFX_API *			m_pGfxApi;

public:
	HyRenderer(vector<HyViewport> &vViewportsRef, HY_GFX_API *pSuppliedGfx = NULL);
	~HyRenderer();

	HyGfxComms &			GetGfxComms()	{ return m_GfxComms; }

	bool Update();
};

#endif
