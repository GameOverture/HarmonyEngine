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

int32 HyFilesManifest::sm_iIndexFlagsArraySize[HYNUM_FILETYPES] = { -1, -1, -1, -1 };	// -1 indicates that this has not been initialized

HyFilesManifest::HyFilesManifest(HyFileType eFileType) :
	m_eFILE_TYPE(eFileType),
	m_pIndexFlags(nullptr)
{
	HyAssert(sm_iIndexFlagsArraySize[m_eFILE_TYPE] >= 0, "HyFilesManifest was constructed before HyFilesManifest::sm_iIndexFlagsArraySize has been set");

	m_pIndexFlags = HY_NEW uint32[sm_iIndexFlagsArraySize[m_eFILE_TYPE]];
	memset(m_pIndexFlags, 0, sizeof(uint32) * sm_iIndexFlagsArraySize[m_eFILE_TYPE]);
}

HyFilesManifest::~HyFilesManifest()
{
	delete[] m_pIndexFlags;
}

bool HyFilesManifest::IsSet(uint32 uiManifestIndex) const
{
	uint32 uiIndex = uiManifestIndex / 32;
	uint32 uiBitPos = uiManifestIndex % 32;

	return 0 != (m_pIndexFlags[uiIndex] & (1 << uiBitPos));
}

bool HyFilesManifest::IsSet(const HyFilesManifest &otherRef) const
{
	for(int32 i = 0; i < sm_iIndexFlagsArraySize[m_eFILE_TYPE]; ++i)
	{
		if(otherRef.m_pIndexFlags[i] != (m_pIndexFlags[i] & otherRef.m_pIndexFlags[i]))
			return false;
	}

	return true;
}

void HyFilesManifest::Set(uint32 uiManifestIndex)
{
	uint32 uiIndex = uiManifestIndex / 32;
	uint32 uiBitPos = uiManifestIndex % 32;

	m_pIndexFlags[uiIndex] |= (1 << uiBitPos);
}

void HyFilesManifest::Clear(uint32 uiManifestIndex)
{
	uint32 uiIndex = uiManifestIndex / 32;
	uint32 uiBitPos = uiManifestIndex % 32;

	m_pIndexFlags[uiIndex] &= ~(1 << uiBitPos);
}

bool HyFilesManifest::IsEmpty() const
{
	for(int32 i = 0; i < sm_iIndexFlagsArraySize[m_eFILE_TYPE]; ++i)
	{
		if(m_pIndexFlags[i] != 0)
			return false;
	}

	return true;
}
