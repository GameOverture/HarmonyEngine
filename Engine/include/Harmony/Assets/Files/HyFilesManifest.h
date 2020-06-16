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
class HyFileIndices
{
	friend class HyAssets;

	static int32					sm_iIndexFlagsArraySize[HYNUM_FILETYPES];	// How many 'uint32' are needed to account for every texture index in 'm_pIndexFlags'
	
	const HyFileType				m_eFILE_TYPE;
	uint32 *						m_pIndexFlags;								// Each bit represents the respective texture index

public:
	HyFileIndices(HyFileType eFileType);
	~HyFileIndices();

	bool IsSet(uint32 uiAtlasIndex) const;
	bool IsSet(const HyFileIndices &otherRef) const;
	void Set(uint32 uiAtlasIndex);
	void Clear(uint32 uiAtlasIndex);

	bool IsEmpty() const;
};

class FileManifest
{
	IHyFileData *					m_pFiles;
	uint32							m_uiNumFiles;
	HyFileIndices					m_LoadedFiles;

public:
	FileManifest(HyFileType eFileType);
};

#endif /* HyFilesManifest_h__ */
