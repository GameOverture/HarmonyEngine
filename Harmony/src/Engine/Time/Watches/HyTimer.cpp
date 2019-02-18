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
	m_bEnabled(false),
	m_dCurDuration(0.0),
	m_dTotalDuration(0.0)
{
}

HyTimer::HyTimer(double dDuration) :
	m_bEnabled(false),
	m_dCurDuration(0.0),
	m_dTotalDuration(0.0)
{
	Init(dDuration);
}

HyTimer::~HyTimer(void)
{
}

void HyTimer::Init(double dDuration)
{
	if(dDuration <= 0.0)
	{
		HyLogWarning("HyTimer::Init() was passed a value that was <= 0.0. Ignoring call");
		return;
	}

	m_dCurDuration = 0.0f;
	m_dTotalDuration = dDuration;
	m_bEnabled = false;
}

void HyTimer::Reset()
{
	Init(m_dTotalDuration);
}

void HyTimer::Start()
{
	if(m_dTotalDuration == 0.0)
	{
		HyLogWarning("HyTimer::Start() was invoked with uninitialized data. Should call Init() prior");
		return;
	}
	
	m_bEnabled = true;
}

void HyTimer::Pause()
{
	m_bEnabled = false;
}

bool HyTimer::IsExpired() const
{
	return m_bEnabled && m_dCurDuration >= m_dTotalDuration;
}

double HyTimer::TimeLeft() const
{
	if(IsExpired())
		return 0.0f;
	
	return HyMax(m_dTotalDuration - m_dCurDuration, 0.0);
}

/*virtual*/ void HyTimer::Update(double dDelta) /*override*/
{
	if(m_bEnabled)
		m_dCurDuration += dDelta;
}
