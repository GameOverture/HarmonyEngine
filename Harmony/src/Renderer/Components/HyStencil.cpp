/**************************************************************************
*	HyStencil.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Renderer/Components/HyStencil.h"
#include "Renderer/IHyRenderer.h"

HyStencilHandle HyStencil::sm_hHandleCount = 0;

HyStencil::HyStencil() :	m_hHANDLE(++sm_hHandleCount)
{
	IHyRenderer::AddStencil(this);
}

HyStencil::~HyStencil()
{
	IHyRenderer::RemoveStencil(this);
}

HyStencilHandle HyStencil::GetHandle()
{
	return m_hHANDLE;
}

HyShape2d &HyStencil::GetShape()
{
	return m_Shape.GetShape();
}
