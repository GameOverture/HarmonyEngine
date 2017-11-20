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
#include "Renderer/Components/HyRenderState.h"

HyStencilHandle HyStencil::sm_hHandleCount = 0;

HyStencil::HyStencil() :	m_hHANDLE(++sm_hHandleCount),
							m_pRenderStateBuffer(nullptr),
							m_bInstanceListDirty(false),
							m_pRenderStatePtr(nullptr)
{
	IHyRenderer::AddStencil(this);
}

HyStencil::~HyStencil()
{
	IHyRenderer::RemoveStencil(this);

	delete m_pRenderStateBuffer;
}

HyStencilHandle HyStencil::GetHandle()
{
	return m_hHANDLE;
}

void HyStencil::AddInstance(IHyLeafDraw2d *pInstance)
{
	m_InstanceList.push_back(pInstance);
	m_bInstanceListDirty = true;
}

bool HyStencil::RemoveInstance(IHyLeafDraw2d *pInstance)
{
	for(auto it = m_InstanceList.begin(); it != m_InstanceList.end(); ++it)
	{
		if((*it) == pInstance)
		{
			m_InstanceList.erase(it);
			m_bInstanceListDirty = true;
			return true;
		}
	}

	return false;
}

void HyStencil::SetAsCullMask()
{
}

void HyStencil::SetAsInvertedCullMask()
{
}

const std::vector<IHyLeafDraw2d *> &HyStencil::GetInstanceList()
{
	return m_InstanceList;
}

void HyStencil::SetRenderStatePtr(HyRenderState *pPtr)
{
	m_pRenderStatePtr = pPtr;
}
