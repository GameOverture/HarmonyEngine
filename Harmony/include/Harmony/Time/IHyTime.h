/**************************************************************************
 *	IHyTime.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __IHyTime_h__
#define __IHyTime_h__

#include "Afx/HyStdAfx.h"

#include "Watches/HyTimer.h"
#include "Watches/HyStopwatch.h"

#include <vector>

// Forward declarations
class IHyTimeInst;
class HyStopwatch;

class IHyTime
{
	static const uint32			sm_uiUPDATESTEP_MILLISECONDS;
	static const double			sm_dUPDATESTEP_SECONDS;

	std::vector<IHyTimeInst *>	m_TimeInstList;

	double						m_dTotalElapsedTime;
	double						m_dThrottledTime;
	double						m_dTimeManipulation;

	// FPS members
	int							m_iFPSUpdateFrames;
	int							m_iCurFPSUpdate;
	double						m_dFPSLogCounter;
	bool						m_bDumpFPSToConsole;

	int							m_iThrottleSafetyCounter;

protected:
	double						m_dCurDeltaTime;

public:
	IHyTime();
	~IHyTime(void);

	static uint32 GetUpdateStepMilliseconds()	{ return sm_uiUPDATESTEP_MILLISECONDS; }
	static float GetUpdateStepSeconds()			{ return static_cast<float>(sm_dUPDATESTEP_SECONDS); }

	// Sets member variable 'm_dCurDeltaTime' to the delta seconds from its previous call (or from its initialization)
	// Delta time is in seconds.
	virtual void SetCurDeltaTime() = 0;

	// Only update game (returns true) in specified time intervals
	// Also saves input if recording replay, or applies input if showing replay.
	// Returns 'true' if the engine should update the game
	bool ThrottleTime();

	// Will eat any delta that exist, and the next invoke to ThrottleTime() will 
	// produce a delta from this point.
	void ResetDelta();

	//int GetUpdateFPS()		{ return m_iCurFPSUpdate; }
	//void ToggleFPSOutput()	{ m_bDumpFPSToConsole = !m_bDumpFPSToConsole; }

	void AddTimeInst(IHyTimeInst *pTimeInst);
	void RemoveTimeInst(IHyTimeInst *pTimeInst);
};

#endif /* __IHyTime_h__ */
