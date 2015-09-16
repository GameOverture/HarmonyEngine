/**************************************************************************
 *	HyWatch.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyWatch_h__
#define __HyWatch_h__

class HyTime;

class HyWatch
{
	static HyTime *sm_pTime;

public:
	HyWatch(void);
	virtual ~HyWatch(void);

	void Update(double dDelta);
};

#endif /* __HyWatch_h__ */
