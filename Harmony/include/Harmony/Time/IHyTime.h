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
using std::vector;

// Forward declarations
class IHyTimeInst;
class HyStopwatch;

class IHyTime
{
	static const uint32			sm_kuiUpdateStep;
	static const double			sm_kdUpdateStep;

	vector<IHyTimeInst *>		m_vTimeInsts;

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
	IHyTime();
	~IHyTime(void);

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

	void AddTimeInst(IHyTimeInst *pTimeInst);
	void RemoveTimeInst(IHyTimeInst *pTimeInst);
};

#endif /* __IHyTime_h__ */
