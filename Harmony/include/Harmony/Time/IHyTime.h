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
	static uint32				sm_uiUPDATESTEP_MILLISECONDS;
	static double				sm_dUPDATESTEP_SECONDS;

	std::vector<IHyTimeInst *>	m_TimeInstList;

	double						m_dTotalElapsedTime;
	double						m_dThrottledTime;

	// FPS members
	uint32						m_uiFpsFrameCount;
	double						m_dFpsElapsedTime;
	bool						m_bDumpFPSToConsole;

protected:
	static double				sm_dCurDeltaTime;

public:
	IHyTime();
	~IHyTime();

	//static uint32 GetUpdateStepMilliseconds();
	static float GetUpdateStepSeconds();

	// Sets member variable 'sm_dCurDeltaTime' to the delta seconds from its previous call (or from its initialization)
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
	void ShowFps(bool bShow);

	virtual std::string GetDateTime() = 0;

	void AddTimeInst(IHyTimeInst *pTimeInst);
	void RemoveTimeInst(IHyTimeInst *pTimeInst);
};

#endif /* __IHyTime_h__ */
