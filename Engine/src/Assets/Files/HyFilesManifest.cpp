/**************************************************************************
 *	HyFilesManifest.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Files/HyFilesManifest.h"

int32 HyFileIndices::sm_iIndexFlagsArraySize[HYNUM_FILETYPES] = { -1 };	// -1 indicates that this has not been initialized

HyFileIndices::HyFileIndices(HyFileType eFileType) :
	m_eFILE_TYPE(eFileType),
	m_pIndexFlags(nullptr)
{
	HyAssert(sm_iIndexFlagsArraySize >= 0, "HyFileIndices was constructed before HyFileIndices::sm_iIndexFlagsArraySize has been set");

	m_pIndexFlags = HY_NEW uint32[sm_iIndexFlagsArraySize[m_eFILE_TYPE]];
	memset(m_pIndexFlags, 0, sizeof(uint32) * sm_iIndexFlagsArraySize[m_eFILE_TYPE]);
}

HyFileIndices::~HyFileIndices()
{
	delete[] m_pIndexFlags;
}

bool HyFileIndices::IsSet(uint32 uiAtlasIndex) const
{
	uint32 uiIndex = uiAtlasIndex / 32;
	uint32 uiBitPos = uiAtlasIndex % 32;

	return 0 != (m_pIndexFlags[uiIndex] & (1 << uiBitPos));
}

bool HyFileIndices::IsSet(const HyFileIndices &otherRef) const
{
	for(int32 i = 0; i < sm_iIndexFlagsArraySize[m_eFILE_TYPE]; ++i)
	{
		if(otherRef.m_pIndexFlags[i] != (m_pIndexFlags[i] & otherRef.m_pIndexFlags[i]))
			return false;
	}

	return true;
}

void HyFileIndices::Set(uint32 uiAtlasIndex)
{
	uint32 uiIndex = uiAtlasIndex / 32;
	uint32 uiBitPos = uiAtlasIndex % 32;

	m_pIndexFlags[uiIndex] |= (1 << uiBitPos);
}

void HyFileIndices::Clear(uint32 uiAtlasIndex)
{
	uint32 uiIndex = uiAtlasIndex / 32;
	uint32 uiBitPos = uiAtlasIndex % 32;

	m_pIndexFlags[uiIndex] &= ~(1 << uiBitPos);
}

bool HyFileIndices::IsEmpty() const
{
	for(int32 i = 0; i < sm_iIndexFlagsArraySize[m_eFILE_TYPE]; ++i)
	{
		if(m_pIndexFlags[i] != 0)
			return false;
	}

	return true;
}
