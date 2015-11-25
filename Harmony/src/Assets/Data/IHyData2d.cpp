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

HyTextures *IHyData2d::sm_pTextures = NULL;

IHyData2d::IHyData2d(HyInstanceType eInstType, const std::string &sPath) : IHyData(HYDATA_2d, eInstType, sPath)
{
}

IHyData2d::~IHyData2d()
{
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

HyAtlasGroup *IHyData2d::RequestTexture(uint32 uiAtlasGroupId, uint32 uiTextureIndex)
{
	HyAtlasGroup *pAtlasGrp = sm_pTextures->RequestTexture(uiAtlasGroupId, uiTextureIndex);
	m_AssociatedAtlases.insert(pAtlasGrp);

	return pAtlasGrp;
}

const std::set<HyAtlasGroup *> &IHyData2d::GetAssociatedAtlases()
{
	return m_AssociatedAtlases;
}
