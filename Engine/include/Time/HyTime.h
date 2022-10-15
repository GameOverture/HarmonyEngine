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

#include "Afx/HyInteropAfx.h"
#include "Watches/HyTimer.h"
#include "Watches/HyStopwatch.h"

// Forward declarations
class IHyTimeInst;
class HyStopwatch;
class HyDiagnostics;

class HyTime
{
	HyTimeStamp												m_CurrTime;
	HyTimeStamp												m_PrevTime;

	double													m_dFrameDelta;
	double													m_dFixedUpdateDelta;	// Non-zero value when a 'ThrottledUpdate' is desired

	std::function<bool(HyTime*)>							m_fpIsUpdateNeeded;
	double													m_dAccumulatedUpdateTime;
	std::function<double()>									m_fpGetUpdateDelta;


	static std::vector<IHyTimeInst *>						sm_TimeInstList;

public:
	HyTime(uint32 uiUpdatesPerSec);
	~HyTime();

	void BeginFrame();
	double GetFrameDelta() const;

	void SetUpdatesPerSec(uint32 uiUpdatesPerSec);
	bool IsUpdateNeeded();
	float GetExtrapolatePercent() const;

	float GetUpdateDelta() const;
	double GetUpdateDeltaDbl() const;

	double GetTotalElapsedTime() const;
	//double GetFrameElapsedTime() const;

	void ResetDelta();

	std::string GetDateTime() const;

	static void AddTimeInst(IHyTimeInst *pTimeInst);
	static void RemoveTimeInst(IHyTimeInst *pTimeInst);

protected:
	bool ThrottledUpdate();
	bool VariableUpdate();
};

#endif /* HyTime_h__ */
