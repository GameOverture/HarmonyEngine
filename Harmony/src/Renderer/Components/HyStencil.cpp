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

uint32 HyStencil::sm_iIdCount = 0;

HyStencil::HyStencil() :	m_iID(++sm_iIdCount)
{
	IHyRenderer::AddStencil(this);
}

HyStencil::~HyStencil()
{
	IHyRenderer::RemoveStencil(this);
}

int32 HyStencil::GetId()
{
	return m_iID;
}
