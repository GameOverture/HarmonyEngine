/**************************************************************************
*	HyAtlasIndices.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyAtlasIndices_h__
#define HyAtlasIndices_h__

#include "Afx/HyStdAfx.h"

// NOTE: Can't use std::bitset because the number of bits needed is not known at compile time
class HyAtlasIndices
{
	friend class HyAssets;

	uint32 *						m_pIndexFlags;				// Each bit represents the respective texture index
	static int32					sm_iIndexFlagsArraySize;	// How many 'uint32' are needed to account for every texture index in 'm_pIndexFlags'

public:
	HyAtlasIndices();
	~HyAtlasIndices();

	bool IsSet(uint32 uiAtlasIndex) const;
	bool IsSet(const HyAtlasIndices &otherRef) const;
	void Set(uint32 uiAtlasIndex);
	void Clear(uint32 uiAtlasIndex);

	bool IsEmpty() const;
};

#endif /* HyAtlasIndices_h__ */
