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
#include "Diagnostics/Console/IHyConsole.h"
#include "Time/HyTime.h"
#include "Utilities/HyMath.h"

HyTimer::HyTimer(void) :
	IHyTimeInst(),
	m_dDuration(0.0),
	m_fpCallbackFunc(nullptr),
	m_pCallbackData(nullptr)
{
}

HyTimer::HyTimer(double dDuration) :
	IHyTimeInst(),
	m_dDuration(0.0),
	m_fpCallbackFunc(nullptr),
	m_pCallbackData(nullptr)
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
	m_dElapsedTime = 0.0f;
	m_dDuration = dDuration;
	m_bIsRunning = false;
}

void HyTimer::InitStart(double dDuration)
{
	Init(dDuration);
	Start();
}

void HyTimer::Reset()
{
	Init(m_dDuration);
}

void HyTimer::Start()
{
	m_bIsRunning = true;
}

void HyTimer::Resume()
{
	Start();
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
	
	return HyMath::Max(m_dDuration - m_dElapsedTime, 0.0);
}

void HyTimer::SetExpiredCallback(std::function<void(void *)> fpFunc, void *pData)
{
	m_fpCallbackFunc = fpFunc;
	m_pCallbackData = pData;
}

/*virtual*/ void HyTimer::OnUpdate() /*override*/
{
	if(m_fpCallbackFunc && IsExpired() && m_dDuration > 0.0)
		m_fpCallbackFunc(m_pCallbackData);
}
