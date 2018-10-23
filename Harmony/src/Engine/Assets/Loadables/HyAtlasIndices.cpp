/**************************************************************************
*	HyAtlasIndices.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Loadables/HyAtlasIndices.h"

int32 HyAtlasIndices::sm_iIndexFlagsArraySize = -1;	// -1 indicates that this has not been initialized

HyAtlasIndices::HyAtlasIndices() : m_pIndexFlags(nullptr)
{
	HyAssert(sm_iIndexFlagsArraySize >= 0, "HyAtlasIndices was constructed before HyAtlasIndices::sm_iIndexFlagsArraySize has been set");
	
	m_pIndexFlags = HY_NEW uint32[sm_iIndexFlagsArraySize];
	memset(m_pIndexFlags, 0, sizeof(uint32) * sm_iIndexFlagsArraySize);
}

HyAtlasIndices::~HyAtlasIndices()
{
	delete[] m_pIndexFlags;
}

bool HyAtlasIndices::IsSet(uint32 uiAtlasIndex) const
{
	uint32 uiIndex = uiAtlasIndex / 32;
	uint32 uiBitPos = uiAtlasIndex % 32;

	return 0 != (m_pIndexFlags[uiIndex] & (1 << uiBitPos));
}

bool HyAtlasIndices::IsSet(const HyAtlasIndices &otherRef) const
{
	for(int32 i = 0; i < sm_iIndexFlagsArraySize; ++i)
	{
		if(otherRef.m_pIndexFlags[i] != (m_pIndexFlags[i] & otherRef.m_pIndexFlags[i]))
			return false;
	}

	return true;
}

void HyAtlasIndices::Set(uint32 uiAtlasIndex)
{
	uint32 uiIndex = uiAtlasIndex / 32;
	uint32 uiBitPos = uiAtlasIndex % 32;

	m_pIndexFlags[uiIndex] |= (1 << uiBitPos);
}

void HyAtlasIndices::Clear(uint32 uiAtlasIndex)
{
	uint32 uiIndex = uiAtlasIndex / 32;
	uint32 uiBitPos = uiAtlasIndex % 32;

	m_pIndexFlags[uiIndex] &= ~(1 << uiBitPos);
}

bool HyAtlasIndices::IsEmpty() const
{
	for(int32 i = 0; i < sm_iIndexFlagsArraySize; ++i)
	{
		if(m_pIndexFlags[i] != 0)
			return false;
	}

	return true;
}
