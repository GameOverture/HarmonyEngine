/**************************************************************************
 *	IHyTimeInst.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Time/Watches/IHyTimeInst.h"
#include "Time/HyTime.h"

HyTime *IHyTimeInst::sm_pTime = nullptr;

IHyTimeInst::IHyTimeInst(void)
{
	HyAssert(sm_pTime, "IHyTimeInst was invoked before Engine was initialized (sm_pTime == nullptr)");
	HyAddTimeInst(*sm_pTime, this);
}

IHyTimeInst::~IHyTimeInst(void)
{
	HyRemoveTimeInst(*sm_pTime, this);
}

