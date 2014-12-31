/**************************************************************************
 *	HyTimeApi_Win.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyTimeApi_Win_h__
#define __HyTimeApi_Win_h__

#include "Time/HyTime.h"

class HyTimeApi_Win : public HyTime
{
	bool						m_bUsingPerformanceTimer;	// Timer Selection Flag

	//int64						m_i64LastTime;				// Previous timer value
	double						m_dTimeFactor;				// Time Scaling Factor
	int64						m_i64LastTime;				// Current timer value
	int64						m_i64CurTime;				// Current timer value

public:
	HyTimeApi_Win(HyInput &hyInputRef);
	virtual ~HyTimeApi_Win(void);

	// Sets member variable 'm_dCurDeltaTime' to the delta seconds from its previous call (or from its initialization)
	// Delta time is in seconds.
	virtual void SetCurDeltaTime();
};

#endif /* __HyTimeApi_Win_h__ */
