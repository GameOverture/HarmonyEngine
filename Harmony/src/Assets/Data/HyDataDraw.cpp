/**************************************************************************
*	HyDataDraw.cpp
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Assets/Data/HyDataDraw.h"

#include "Renderer/IHyRenderer.h"
#include "Renderer/Components/IHyShader.h"

HyDataDraw::HyDataDraw() :	m_eLoadState(HYLOADSTATE_Inactive)
{
}

HyDataDraw::~HyDataDraw()
{
}

HyLoadState HyDataDraw::GetLoadState()
{
	return m_eLoadState;
}

/*virtual*/ void HyDataDraw::SetLoadState(HyLoadState eState)
{
	m_eLoadState = eState;
}


int32 HyDataDraw::GetShaderId()
{
	return m_iShaderId;
}

bool HyDataDraw::IsIncrementRenderRefs()
{
	return m_bIncRenderRef;
}

HyAtlasGroup *HyDataDraw::RequestTexture(uint32 uiAtlasGroupId)
{
	HyAtlasGroup *pAtlasGrp = sm_pTextures->RequestTexture(uiAtlasGroupId);
	m_AssociatedAtlases.insert(pAtlasGrp);

	return pAtlasGrp;
}

const std::set<HyAtlasGroup *> &HyDataDraw::GetAssociatedAtlases()
{
	return m_AssociatedAtlases;
}

const std::set<IHyShader *> &HyDataDraw::GetAssociatedShaders()
{
	return m_AssociatedShaders;
}

/*virtual*/ void HyDataDraw::OnLoadThread()
{
	if(m_iSHADER_ID != -1)
	{
		IHyShader *pShader = IHyRenderer::FindShader(m_iSHADER_ID);
		HyAssert(pShader, "HyDataDraw::OnLoadThread could not find a valid shader");

		pShader->OnLoadThread();
		HyAssert(pShader->IsFinalized(), "HyDataDraw::OnLoadThread processed an non-finalized shader");

		if(m_iSHADER_ID < 0 || m_iSHADER_ID >= HYSHADERPROG_CustomStartIndex)
			m_AssociatedShaders.insert(pShader);
	}

	IHyData::OnLoadThread();
}
