/**************************************************************************
 *	HyTimer.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Time/Watches/HyTimer.h"
#include "Diagnostics/Console/HyConsole.h"
#include "Time/HyTime.h"
#include "Utilities/HyMath.h"

HyTimer::HyTimer(void) :
	IHyTimeInst(),
	m_dDuration(0.0)
{
}

HyTimer::HyTimer(double dDuration) :
	IHyTimeInst(),
	m_dDuration(0.0)
{
	Init(dDuration);
}

HyTimer::~HyTimer(void)
{
}

double HyTimer::GetDuration() const
{
	return m_dDuration;
}

void HyTimer::Init(double dDuration)
{
	if(dDuration <= 0.0)
	{
		HyLogWarning("HyTimer::Init() was passed a value that was <= 0.0. Ignoring call");
		return;
	}

	m_dElapsedTime = 0.0f;
	m_dDuration = dDuration;
	m_bIsRunning = false;
}

void HyTimer::Reset()
{
	Init(m_dDuration);
}

void HyTimer::Start()
{
	if(m_dDuration == 0.0)
	{
		HyLogWarning("HyTimer::Start() was invoked with uninitialized data. Should call Init() prior");
		return;
	}
	
	m_bIsRunning = true;
}

void HyTimer::Pause()
{
	m_bIsRunning = false;
}

bool HyTimer::IsExpired() const
{
	return m_dElapsedTime >= m_dDuration;
}

double HyTimer::TimeLeft() const
{
	if(IsExpired())
		return 0.0f;
	
	return HyMax(m_dDuration - m_dElapsedTime, 0.0);
}
