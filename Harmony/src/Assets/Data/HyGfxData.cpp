/**************************************************************************
*	HyGfxData.cpp
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Assets/Data/HyGfxData.h"

HyGfxData::HyGfxData() :	m_eLoadState(HYLOADSTATE_Inactive)
{
}

HyGfxData::~HyGfxData()
{
}

HyLoadState HyGfxData::GetLoadState()
{
	return m_eLoadState;
}

void HyGfxData::Clear()
{
	m_RequiredAtlasIds.clear();
	m_RequiredCustomShaders.clear();
}

void HyGfxData::SetRequiredAtlasId(uint32 uiAtlasId)
{
	m_RequiredAtlasIds.insert(uiAtlasId);
}

void HyGfxData::SetRequiredCustomShaderId(int32 iShaderId)
{
	HyAssert(iShaderId >= HYSHADERPROG_CustomStartIndex, "HyGfxData::SetRequiredCustomShaderId was passed an invalid custom shader Id");
	m_RequiredCustomShaders.insert(iShaderId);
}
