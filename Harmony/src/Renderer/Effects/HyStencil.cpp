/**************************************************************************
*	HyStencil.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Renderer/Effects/HyStencil.h"
#include "Renderer/IHyRenderer.h"
#include "Renderer/Components/HyRenderState.h"
#include "Scene/Nodes/Draws/Instances/IHyDrawInst2d.h"

HyStencilHandle HyStencil::sm_hHandleCount = 0;

HyStencil::HyStencil() :	m_hHANDLE(++sm_hHandleCount),
							m_pRenderStatePtr(nullptr),
							m_eBehavior(HYSTENCILBEHAVIOR_Mask)
{
	IHyRenderer::AddStencil(this);
}

HyStencil::~HyStencil()
{
}

void HyStencil::Destroy()
{
	IHyRenderer::RemoveStencil(this);
	delete this;
}

HyStencilHandle HyStencil::GetHandle() const
{
	return m_hHANDLE;
}

void HyStencil::AddInstance(IHyDrawInst2d *pInstance)
{
	pInstance->Load();
	m_InstanceList.push_back(pInstance);
}

bool HyStencil::RemoveInstance(IHyDrawInst2d *pInstance)
{
	for(auto it = m_InstanceList.begin(); it != m_InstanceList.end(); ++it)
	{
		if((*it) == pInstance)
		{
			m_InstanceList.erase(it);
			return true;
		}
	}

	return false;
}

HyStencilBehavior HyStencil::GetBehavior() const
{
	return m_eBehavior;
}

void HyStencil::SetAsMask()
{
	m_eBehavior = HYSTENCILBEHAVIOR_Mask;
}

void HyStencil::SetAsInvertedMask()
{
	m_eBehavior = HYSTENCILBEHAVIOR_InvertedMask;
}

const std::vector<IHyDrawInst2d *> &HyStencil::GetInstanceList() const
{
	return m_InstanceList;
}

HyRenderState *HyStencil::GetRenderStatePtr() const
{
	return m_pRenderStatePtr;
}

void HyStencil::SetRenderStatePtr(HyRenderState *pPtr)
{
	m_pRenderStatePtr = pPtr;
}
