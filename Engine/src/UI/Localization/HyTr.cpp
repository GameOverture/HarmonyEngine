/**************************************************************************
*	HyTr.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Localization/HyTr.h"

/*static*/ HyTr::TrState HyTr::sm_code(HyTr::TR_English);

/*static*/ const std::string HyTr::Get(const std::string id)
{
	return id;
}

/*static*/ void HyTr::SetCode(TrState code)
{
	HyTr::sm_code = code;
}