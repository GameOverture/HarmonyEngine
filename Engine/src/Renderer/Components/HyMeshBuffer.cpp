/**************************************************************************
 *	HyMeshBuffer.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Renderer/Components/HyMeshBuffer.h"
#include "Renderer/IHyRenderer.h"

HyMeshBuffer::HyMeshBuffer(IHyRenderer &rendererRef) :
	m_RendererRef(rendererRef)
{
}

HyMeshBuffer::~HyMeshBuffer()
{
}
