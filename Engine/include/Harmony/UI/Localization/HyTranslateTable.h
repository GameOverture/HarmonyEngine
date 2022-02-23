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

#define HYTRANSLATETABLE_DataFile "Translations.data"

#define HyTr(x) HyTranslateTable::Get(x)

class HyTranslateTable
{
	static std::unordered_map<std::string, std::string> sm_table;
	static std::string sm_language;
	static bool sm_testMode;

public:

	static void Load(std::string sIso639Code, std::string sIso3166Code);

	static const std::string Get(const std::string id);

	static void SetTestMode(bool state);

private:
};

#endif /* HyTranslateTable_h__ */
