/**************************************************************************
 *	IHyTimeInst.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
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

