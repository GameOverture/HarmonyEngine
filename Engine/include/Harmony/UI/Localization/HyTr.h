/**************************************************************************
*	HyTr.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyTr_h__
#define HyTr_h__

#include "Afx/HyStdAfx.h"

class HyTr
{
	enum TrState
	{
		TR_English = 0,
		TR_EnglishBritish,
		TR_Spanish,
	};

	static TrState sm_code;

public:
	static const std::string Get(const std::string id);

	static void SetCode(TrState code);

private:
};

#endif /* HyTr_h__ */
