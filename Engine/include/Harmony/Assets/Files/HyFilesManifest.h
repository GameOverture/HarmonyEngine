/**************************************************************************
*	HyFilesManifest.h
*
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyFilesManifest_h__
#define HyFilesManifest_h__

#include "Afx/HyStdAfx.h"
#include "Assets/Files/IHyFileData.h"

// NOTE: Can't use std::bitset because the number of bits needed is not known at compile time
class HyFilesManifest
{
	friend class HyAssets;

	static int32					sm_iIndexFlagsArraySize[HYNUM_FILETYPES];	// How many 'uint32' are needed to account for every asset index in 'm_pIndexFlags'
	
	const HyFileType				m_eFILE_TYPE;
	uint32 *						m_pIndexFlags;								// Each bit represents the respective texture index

public:
	HyFilesManifest(HyFileType eFileType);
	~HyFilesManifest();

	bool IsSet(uint32 uiAtlasIndex) const;
	bool IsSet(const HyFilesManifest &otherRef) const;
	void Set(uint32 uiAtlasIndex);
	void Clear(uint32 uiAtlasIndex);

	bool IsEmpty() const;
};

#endif /* HyFilesManifest_h__ */
