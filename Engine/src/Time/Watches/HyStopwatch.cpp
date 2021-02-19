/**************************************************************************
 *	HyStopwatch.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Time/Watches/HyStopwatch.h"

HyStopwatch::HyStopwatch(void) :
	IHyTimeInst()
{
}

HyStopwatch::~HyStopwatch(void)
{
}

double HyStopwatch::TimeElapsed() const
{
	return m_dElapsedTime;
}

void HyStopwatch::Start()
{
	m_bIsRunning = true;
}

void HyStopwatch::Pause()
{
	m_bIsRunning = false;
}

void HyStopwatch::Reset()
{
	m_bIsRunning = false;
	m_dElapsedTime = 0.0;
}
