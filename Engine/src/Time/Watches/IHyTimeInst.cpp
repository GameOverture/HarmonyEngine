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

IHyTimeInst::IHyTimeInst(void) :
	m_bIsRunning(false),
	m_dElapsedTime(0.0)
{
	HyTime::AddTimeInst(this);
}

IHyTimeInst::~IHyTimeInst(void)
{
	HyTime::RemoveTimeInst(this);
}

bool IHyTimeInst::IsRunning() const
{
	return m_bIsRunning;
}

double IHyTimeInst::TimeElapsed() const
{
	return m_dElapsedTime;
}

void IHyTimeInst::Update(double dDelta)
{
	m_dElapsedTime += dDelta;
}
