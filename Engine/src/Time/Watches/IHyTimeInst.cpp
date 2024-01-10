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
	OnUpdate();
}

std::string IHyTimeInst::FormatString(double dSeconds) const
{
	int iHours = static_cast<int>(dSeconds / 3600);
	int iMinutes = static_cast<int>((dSeconds - iHours * 3600) / 60);
	int iSeconds = static_cast<int>(dSeconds - iHours * 3600 - iMinutes * 60);
	int iMilliseconds = static_cast<int>((dSeconds - static_cast<double>(iHours) * 3600 - static_cast<double>(iMinutes) * 60 - iSeconds) * 1000);

	std::ostringstream oss;

	if(iHours > 0)
		oss << std::setfill('0') << std::setw(2) << iHours << ":";
	oss << std::setfill('0') << std::setw(2) << iMinutes << ":";
	oss << std::setfill('0') << std::setw(2) << iSeconds << "." << std::setfill('0') << std::setw(3) << iMilliseconds;

	return oss.str();
}
