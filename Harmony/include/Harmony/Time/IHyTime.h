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
	HyScene &					m_SceneRef;
	uint32						m_uiUpdateTickMs;

	std::vector<IHyTimeInst *>	m_TimeInstList;

	double						m_dTotalElapsedTime;
	double						m_dUpdateTick_Seconds;
	double						m_dThrottledTime;

	double						m_dSpiralOfDeathCounter;	// In 'ThrottledUpdate' environments a potential to have updates take longer than the alloted time step will cause an infinite loop
															// This counter keeps track of these scenarios
protected:
	double						m_dCurDeltaTime;

public:
	IHyTime(HyScene &sceneRef, uint32 uiUpdateTickMs);
	~IHyTime();

	uint32 GetUpdateTickMs();
	void SetUpdateTickMs(uint32 uiUpdateTickMs);

	float GetUpdateStepSeconds();
	float GetFrameDelta();

	// Sets member variable 'm_dCurDeltaTime' to the delta seconds from its previous call (or from its initialization)
	// Delta time is in seconds.
	virtual void SetCurDeltaTime() = 0;

	void CalcFrameDelta();

	bool ThrottleUpdate();

	// Will eat any delta that exist, and the next invoke to ThrottleUpdate() will 
	// produce a delta from this point.
	void ResetDelta();

	virtual std::string GetDateTime() = 0;

	void AddTimeInst(IHyTimeInst *pTimeInst);
	void RemoveTimeInst(IHyTimeInst *pTimeInst);
};

#endif /* IHyTime_h__ */
