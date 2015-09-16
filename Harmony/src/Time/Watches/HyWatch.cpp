/**************************************************************************
 *	HyWatch.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Time/Watches/HyWatch.h"

#include "Time/HyTime.h"

HyTime *HyWatch::sm_pTime = NULL;

HyWatch::HyWatch(void)
{
}


HyWatch::~HyWatch(void)
{
	sm_pTime->RemoveTimeInst(this);
}

void HyWatch::Update(double dDelta)
{
}

