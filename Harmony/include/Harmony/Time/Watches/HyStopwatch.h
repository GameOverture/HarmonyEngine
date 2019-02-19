/**************************************************************************
 *	HyStopwatch.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyStopwatch_h__
#define HyStopwatch_h__

#include "IHyTimeInst.h"

class HyStopwatch : public IHyTimeInst
{
public:
	HyStopwatch(void);
	virtual ~HyStopwatch(void);

	double TimeElapsed() const;

	void Start();
	void Pause();

	void Reset();
};

#endif /* HyStopwatch_h__ */
