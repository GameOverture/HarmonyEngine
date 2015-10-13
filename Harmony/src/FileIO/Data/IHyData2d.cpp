/**************************************************************************
*	IHyData2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "FileIO/Data/IHyData2d.h"

HyAtlasManager *sm_pAtlasManager = NULL;

IHyData2d::IHyData2d(HyInstanceType eDataType, const std::string &sPath) : IHyData(eDataType, sPath)
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
	else if(GetLoadState() == HYLOADSTATE_ReloadGfx)
	{

	}
}

HyAtlasGroup *IHyData2d::RequestTexture(uint32 uiTextureId)
{
	HyAtlasGroup *pAtlasGrp = sm_pAtlasManager->RequestTexture(uiTextureId);
	m_AssociatedAtlases.insert(pAtlasGrp);

	return pAtlasGrp;
}

const std::set<HyAtlasGroup *> &IHyData2d::GetAssociatedAtlases()
{
	return m_AssociatedAtlases;
}
