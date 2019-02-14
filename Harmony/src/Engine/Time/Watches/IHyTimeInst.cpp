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

HyTime *IHyTimeInst::sm_pTime = NULL;

IHyTimeInst::IHyTimeInst(void)
{
}

IHyTimeInst::~IHyTimeInst(void)
{
	sm_pTime->RemoveTimeInst(this);
}

void IHyTimeInst::Update(double dDelta)
{
}
