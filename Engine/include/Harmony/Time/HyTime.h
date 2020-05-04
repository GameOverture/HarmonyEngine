/**************************************************************************
 *	HyTime.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyTime_h__
#define HyTime_h__

#include "Afx/HyStdAfx.h"
#include "Watches/HyTimer.h"
#include "Watches/HyStopwatch.h"

// Forward declarations
class IHyTimeInst;
class HyStopwatch;
class HyDiagnostics;

class HyTime
{
	uint32						m_uiUpdateTickMs;

	std::vector<IHyTimeInst *>	m_TimeInstList;

	double						m_dTotalElapsedTime;
	double						m_dUpdateTick_Seconds;
	double						m_dThrottledTime;

	double						m_dSpiralOfDeathCounter;	// In 'ThrottledUpdate' environments a potential to have updates take longer than the alloted time step will cause an infinite loop
															// This counter keeps track of these scenarios
	double						m_dPrevTime;
	double						m_dCurDeltaTime;

public:
	HyTime(uint32 uiUpdateTickMs);
	~HyTime();

	uint32 GetUpdateTickMs();
	void SetUpdateTickMs(uint32 uiUpdateTickMs);

	float GetUpdateStepSeconds() const;
	double GetUpdateStepSecondsDbl() const;

	double GetTotalElapsedTime() const;

	// Sets member variable 'm_dCurDeltaTime' to the delta seconds from its previous call (or from its initialization)
	// Delta time is in seconds.
	void SetCurDeltaTime();

	void CalcTimeDelta();

	bool ThrottleUpdate();

	// Will eat any delta that exist, and the next invoke to ThrottleUpdate() will 
	// produce a delta from this point.
	void ResetDelta();

	std::string GetDateTime();

	friend void HyAddTimeInst(HyTime &timeRef, IHyTimeInst *pTimeInst);
	friend void HyRemoveTimeInst(HyTime &timeRef, IHyTimeInst *pTimeInst);
};

#endif /* HyTime_h__ */
