/**************************************************************************
 *	HyTime.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Time/HyTime.h"
#include "Scene/HyScene.h"
#include "Diagnostics/HyDiagnostics.h"
#include "Diagnostics/Console/IHyConsole.h"

#define HYTIME_ThresholdWarningsEvery 25.0	// How often to print a warning
#define HYTIME_ThresholdMaxReset 100.0		// Maximum threshold until we hard reset

std::vector<IHyTimeInst *>	HyTime::sm_TimeInstList;

HyTime::HyTime(uint32 uiUpdatesPerSec) :
	m_CurrTime(0),
	m_PrevTime(0),
	m_dFrameDelta(0.0),
	m_dFixedUpdateDelta(0.0),
	m_fpIsUpdateNeeded(nullptr),
	m_dAccumulatedUpdateTime(0.0),
	m_fpGetUpdateDelta(nullptr)
{
	SetUpdatesPerSec(uiUpdatesPerSec);
}

HyTime::~HyTime(void)
{
}

void HyTime::BeginFrame()
{
#ifdef HY_USE_GLFW
	m_CurrTime = glfwGetTime();
	m_dFrameDelta = m_CurrTime - m_PrevTime;
#elif defined(HY_USE_SDL2)
	m_CurrTime = SDL_GetPerformanceCounter();
	m_dFrameDelta = static_cast<double>(m_CurrTime - m_PrevTime) / static_cast<double>(SDL_GetPerformanceFrequency());
#else
	m_CurrTime = std::chrono::high_resolution_clock::now();
	m_dFrameDelta = (m_PrevTime - m_CurrTime).count();
#endif
	m_PrevTime = m_CurrTime;
	m_dFrameDelta = HyMin(m_dFrameDelta, 0.25); // Clamp delta times to 0.25

	// Update all timers/stopwatches
	m_dAccumulatedUpdateTime += m_dFrameDelta;
	for(auto timer : sm_TimeInstList)
	{
		if(timer->IsRunning())
			timer->Update(m_dFrameDelta);
	}
}

double HyTime::GetFrameDelta() const
{
	return m_dFrameDelta;
}

void HyTime::SetUpdatesPerSec(uint32 uiUpdatesPerSec)
{
#ifdef HY_PLATFORM_GUI
	uiUpdatesPerSec = 0; // Disable throttled update on GUI
#endif

	if(uiUpdatesPerSec == 0)
	{
		HyLogInfo("Update rate was passed '0', using non-throttled, variable updating.");
		m_dFixedUpdateDelta = 0.0;
		m_dAccumulatedUpdateTime = 0.0;
		m_fpIsUpdateNeeded = &HyTime::VariableUpdate;
		m_fpGetUpdateDelta = [this]() { return m_dFrameDelta; };
	}
	else
	{
		HyLogInfo("Update rate has been set to '" << uiUpdatesPerSec << "' updates per second");
		m_dFixedUpdateDelta = (1.0 / uiUpdatesPerSec);
		m_dAccumulatedUpdateTime = 0.0;
		m_fpIsUpdateNeeded = &HyTime::ThrottledUpdate;
		m_fpGetUpdateDelta = [this]() { return m_dFixedUpdateDelta; };
	}
}

bool HyTime::IsUpdateNeeded()
{
	return m_fpIsUpdateNeeded(this);
}

float HyTime::GetExtrapolatePercent() const
{
	if(m_dFixedUpdateDelta == 0.0f)
		return 0.0f;

	return static_cast<float>(m_dAccumulatedUpdateTime / m_dFixedUpdateDelta);
}

float HyTime::GetUpdateDelta() const
{
	return static_cast<float>(m_fpGetUpdateDelta());
}

double HyTime::GetUpdateDeltaDbl() const
{
	return m_fpGetUpdateDelta();
}

double HyTime::GetTotalElapsedTime() const
{
#ifdef HY_USE_GLFW
	return glfwGetTime();
#elif defined(HY_USE_SDL2)
	return static_cast<double>(SDL_GetPerformanceCounter()) / static_cast<double>(SDL_GetPerformanceFrequency());
#else
	return std::chrono::high_resolution_clock::now().count();
#endif
}

//double HyTime::GetFrameElapsedTime() const
//{
//#ifdef HY_USE_GLFW
//	return (glfwGetTime() - m_CurrTime);
//#elif defined(HY_USE_SDL2)
//	return static_cast<double>(SDL_GetPerformanceCounter() - m_CurrTime) / static_cast<double>(SDL_GetPerformanceFrequency());
//	m_dFrameDelta = static_cast<double>(m_CurrTime - m_PrevTime
//#else
//	return (std::chrono::high_resolution_clock::now() - m_CurrTime).count();
//#endif
//}

void HyTime::ResetDelta()
{
#ifdef HY_USE_GLFW
	m_CurrTime = glfwGetTime();
#elif defined(HY_USE_SDL2)
	m_CurrTime = SDL_GetPerformanceCounter();
#else
	m_CurrTime = std::chrono::high_resolution_clock::now();
#endif
	m_PrevTime = m_CurrTime;
	m_dFrameDelta = 0.0;
}

std::string HyTime::GetDateTime() const
{
#ifdef HY_PLATFORM_WINDOWS
	char am_pm[] = "AM";

	__time64_t long_time;
	_time64(&long_time);
	// Convert to local time.
	tm newtime;
	errno_t err = _localtime64_s(&newtime, &long_time);
	if(err) {
		HyLogError("Invalid argument to _localtime64_s.");
	}

	if(newtime.tm_hour >= 12)	// Set up extension.
		strcpy_s(am_pm, sizeof(am_pm), "PM");
	if(newtime.tm_hour > 12)	// Convert from 24-hour
		newtime.tm_hour -= 12;	// to 12-hour clock.
	if(newtime.tm_hour == 0)	// Set hour to 12 if midnight.
		newtime.tm_hour = 12;

	// Convert to an ASCII representation.
	char szBuffer[32];
	err = asctime_s(szBuffer, 32, &newtime);
	if(err) {
		HyLogError("Invalid argument to asctime_s.");
	}

	sprintf_s(szBuffer, 32, "%.19s %s", szBuffer, am_pm);
	return std::string(szBuffer);
#else
	return "<GetDateTime not implemented for this platform>";
#endif
}

/*static*/ void HyTime::AddTimeInst(IHyTimeInst *pTimeInst)
{
	if(pTimeInst == nullptr)
		return;

	sm_TimeInstList.push_back(pTimeInst);
}

/*static*/ void HyTime::RemoveTimeInst(IHyTimeInst *pTimeInst)
{
	if(pTimeInst == nullptr)
		return;

	for(std::vector<IHyTimeInst*>::iterator it = sm_TimeInstList.begin(); it != sm_TimeInstList.end(); ++it)
	{
		if((*it) == pTimeInst)
		{
			it = sm_TimeInstList.erase(it);
			break;
		}
	}
}

bool HyTime::ThrottledUpdate()
{
	if(m_dAccumulatedUpdateTime >= m_dFixedUpdateDelta)
	{
		m_dAccumulatedUpdateTime -= m_dFixedUpdateDelta;
		return true;
	}

	return false;
}

bool HyTime::VariableUpdate()
{
	static bool bDoUpdate = false;

	bDoUpdate = !bDoUpdate;
	return bDoUpdate;
}
