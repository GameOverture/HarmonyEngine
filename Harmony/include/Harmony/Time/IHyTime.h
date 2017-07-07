/**************************************************************************
 *	IHyTime.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IHyTime_h__
#define IHyTime_h__

#include "Afx/HyStdAfx.h"
#include "Watches/HyTimer.h"
#include "Watches/HyStopwatch.h"

#include <vector>

// Forward declarations
class IHyTimeInst;
class HyStopwatch;
class HyDiagnostics;
class HyScene;

class IHyTime
{
	static uint32				sm_uiUPDATESTEP_MILLISECONDS;
	static double				sm_dUPDATESTEP_SECONDS;

	HyScene &					m_SceneRef;
	HyDiagnostics &				m_DiagosticsRef;

	std::vector<IHyTimeInst *>	m_TimeInstList;

	double						m_dTotalElapsedTime;
	double						m_dThrottledTime;

	double						m_dSpiralOfDeathCounter;	// In 'ThrottledUpdate' environments a potential to have updates take longer than the alloted time step will cause an infinite loop
															// This counter keeps track of these scenarios

	// FPS members
	uint32						m_uiCurFpsCount;
	uint32						m_uiFps_Update;
	uint32						m_uiFps_Render;
	double						m_dFpsElapsedTime;

protected:
	static double				sm_dCurDeltaTime;

public:
	IHyTime(HyScene &sceneRef, HyDiagnostics &diagRef);
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

	uint32 GetFps_Update();
	uint32 GetFps_Render();

	virtual std::string GetDateTime() = 0;

	void AddTimeInst(IHyTimeInst *pTimeInst);
	void RemoveTimeInst(IHyTimeInst *pTimeInst);
};

#endif /* IHyTime_h__ */
