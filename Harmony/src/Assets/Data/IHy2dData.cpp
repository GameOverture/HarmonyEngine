/**************************************************************************
*	IHy2dData.cpp
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Assets/Data/IHy2dData.h"

#include "Renderer/IHyRenderer.h"
#include "Renderer/IHyShader.h"

HyTextures *IHy2dData::sm_pTextures = NULL;

IHy2dData::IHy2dData(HyInstanceType eInstType, const std::string &sPath, int32 iShaderId) : IHyData(HYDATA_2d, eInstType, sPath),
																							m_iSHADER_ID(iShaderId)
{
}

IHy2dData::~IHy2dData()
{
}

int32 IHy2dData::GetShaderId()
{
	return m_iSHADER_ID;
}

/*virtual*/ void IHy2dData::SetLoadState(HyLoadState eState)
{
	IHyData::SetLoadState(eState);

	if(GetLoadState() == HYLOADSTATE_Queued)
	{
		for(std::set<HyAtlasGroup *>::iterator iter = m_AssociatedAtlases.begin(); iter != m_AssociatedAtlases.end(); ++iter)
			(*iter)->Assign(this);
	}
	if(GetLoadState() == HYLOADSTATE_Discarded)
	{
		for(std::set<HyAtlasGroup *>::iterator iter = m_AssociatedAtlases.begin(); iter != m_AssociatedAtlases.end(); ++iter)
			(*iter)->Relinquish(this); fix this
	}
}

HyAtlasGroup *IHy2dData::RequestTexture(uint32 uiAtlasGroupId)
{
	HyAtlasGroup *pAtlasGrp = sm_pTextures->RequestTexture(uiAtlasGroupId);
	m_AssociatedAtlases.insert(pAtlasGrp);

	return pAtlasGrp;
}

const std::set<HyAtlasGroup *> &IHy2dData::GetAssociatedAtlases()
{
	return m_AssociatedAtlases;
}

const std::set<IHyShader *> &IHy2dData::GetAssociatedShaders()
{
	return m_AssociatedShaders;
}


/*virtual*/ void IHy2dData::OnLoadThread()
{
	IHyShader *pShader = IHyRenderer::FindShader(m_iSHADER_ID);
	HyAssert(pShader, "IHy2dData::OnLoadThread could not find a valid shader");

	pShader->OnLoadThread();
	HyAssert(pShader->IsFinalized(), "IHy2dData::OnLoadThread processed an non-finalized shader");

	if(m_iSHADER_ID < 0 || m_iSHADER_ID >= HYSHADERPROG_CustomStartIndex)
		m_AssociatedShaders.insert(pShader);

	IHyData::OnLoadThread();
}
