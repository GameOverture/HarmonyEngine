/**************************************************************************
 *	IHyData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "FileIO/Data/IHyData.h"

HyAtlasManager *	IHyData::sm_pAtlasesRef = NULL;

/*static*/ void IHyData::SetAtlasesRef(HyFactory<HyAtlasGroupData> *pAtlasesRef)
{
	sm_pAtlasesRef = pAtlasesRef;
}
