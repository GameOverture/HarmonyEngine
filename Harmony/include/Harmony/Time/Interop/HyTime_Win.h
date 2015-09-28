/**************************************************************************
 *	HyTime_Win.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyTime_Win_h__
#define __HyTime_Win_h__

#include "Time/IHyTime.h"

class HyTime_Win : public IHyTime
{
	bool						m_bUsingPerformanceTimer;	// Timer Selection Flag

	double						m_dTimeFactor;				// Time Scaling Factor
	int64						m_i64LastTime;				// Previous timer value
	int64						m_i64CurTime;				// Current timer value

public:
	HyTime_Win();
	virtual ~HyTime_Win(void);

	// Sets member variable 'm_dCurDeltaTime' to the delta seconds from its previous call (or from its initialization)
	// Delta time is in seconds.
	virtual void SetCurDeltaTime();
};

#endif /* __HyTime_Win_h__ */
