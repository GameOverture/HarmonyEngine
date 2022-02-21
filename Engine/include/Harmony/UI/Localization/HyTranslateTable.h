/**************************************************************************
*	HyTranslateTable.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyTranslateTable_h__
#define HyTranslateTable_h__

#include "Afx/HyStdAfx.h"
#include <unordered_map>

#define HyTr(x) HyTranslateTable::Get(x)

class HyTranslateTable
{
	static std::unordered_map <std::string, std::string>table;

public:

	static void Load();

	static const std::string Get(const std::string id);

private:
};

#endif /* HyTranslateTable_h__ */
