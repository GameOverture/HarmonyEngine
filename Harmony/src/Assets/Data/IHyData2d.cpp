/**************************************************************************
*	IHyData2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Assets/Data/IHyData2d.h"

#include "Renderer/IHyRenderer.h"
#include "Renderer/IHyShader.h"

HyTextures *IHyData2d::sm_pTextures = NULL;

IHyData2d::IHyData2d(HyInstanceType eInstType, const std::string &sPath, int32 iShaderId) : IHyData(HYDATA_2d, eInstType, sPath),
																							m_iSHADER_ID(iShaderId)
{
}

IHyData2d::~IHyData2d()
{
}

int32 IHyData2d::GetShaderId()
{
	return m_iSHADER_ID;
}

/*virtual*/ void IHyData2d::SetLoadState(HyLoadState eState)
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
			(*iter)->Relinquish(this);
	}
}

HyAtlasGroup *IHyData2d::RequestTexture(uint32 uiAtlasGroupId)
{
	HyAtlasGroup *pAtlasGrp = sm_pTextures->RequestTexture(uiAtlasGroupId);
	m_AssociatedAtlases.insert(pAtlasGrp);

	return pAtlasGrp;
}

const std::set<HyAtlasGroup *> &IHyData2d::GetAssociatedAtlases()
{
	return m_AssociatedAtlases;
}

/*virtual*/ void IHyData2d::OnLoadThread()
{
	IHyShader *pShader = IHyRenderer::FindShader(m_iSHADER_ID);
	HyAssert(pShader, "IHyData2d::OnLoadThread could not find a valid shader");

	pShader->OnLoadThread();
	HyAssert(pShader->IsFinalized(), "IHyData2d::OnLoadThread processed an non-finalized shader");

	IHyData::OnLoadThread();
}
