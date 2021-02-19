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
#include "HyEngine.h"

HyTime *IHyTimeInst::sm_pTime = nullptr;

IHyTimeInst::IHyTimeInst(void) :
	m_bIsRunning(false),
	m_dElapsedTime(0.0)
{
	HyAssert(sm_pTime, "IHyTimeInst was invoked before Engine was initialized (sm_pTime == nullptr)");
	HyAddTimeInst(*sm_pTime, this);
}

IHyTimeInst::~IHyTimeInst(void)
{
	if(HyEngine::IsInitialized())
		HyRemoveTimeInst(*sm_pTime, this);
}

bool IHyTimeInst::IsRunning() const
{
	return m_bIsRunning;
}

void IHyTimeInst::Update(double dDelta)
{
	m_dElapsedTime += dDelta;
}
