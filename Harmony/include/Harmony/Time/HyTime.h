/**************************************************************************
 *	HyTime.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyTime_h__
#define __HyTime_h__

#include "Afx/HyStdAfx.h"

#include "Watches/HyTimerWatch.h"
#include "Watches/HyStopwatch.h"

#include <vector>
using std::vector;

// Forward declarations
class HyWatch;
class HyStopwatch;

class HyTime
{
	static const uint32			sm_kuiUpdateStep;
	static const double			sm_kdUpdateStep;

	vector<HyWatch *>			m_TimeInstList;

	double						m_dTotalElapsedTime;
	double						m_dThrottledTime;
	double						m_dTimeManipulation;

	// FPS members
	int							m_iFPSUpdateFrames;
	int							m_iCurFPSUpdate;
	double						m_dFPSLogCounter;
	bool						m_bDumpFPSToConsole;

protected:
	double						m_dCurDeltaTime;

public:
	HyTime();
	~HyTime(void);

	static uint32 GetUpdateMilliseconds()	{ return sm_kuiUpdateStep; }
	static float GetUpdateStepSeconds()		{ return static_cast<float>(sm_kdUpdateStep); }

	// Sets member variable 'm_dCurDeltaTime' to the delta seconds from its previous call (or from its initialization)
	// Delta time is in seconds.
	virtual void SetCurDeltaTime() = 0;

	// Only update game (returns true) in specified time intervals
	// Also saves input if recording replay, or applies input if showing replay.
	// Returns 'true' if the engine should update the game
	bool ThrottleTime();

	//int GetUpdateFPS()		{ return m_iCurFPSUpdate; }
	//void ToggleFPSOutput()	{ m_bDumpFPSToConsole = !m_bDumpFPSToConsole; }

	void AddTimeInst(HyWatch *pTimeInst);
	void RemoveTimeInst(HyWatch *pTimeInst);
};

#endif /* __HyTime_h__ */
