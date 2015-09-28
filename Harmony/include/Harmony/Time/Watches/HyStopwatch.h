/**************************************************************************
 *	HyStopwatch.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyStopwatch_h__
#define __HyStopwatch_h__

#include "IHyTimeInst.h"

class HyStopwatch : public IHyTimeInst
{
public:
	HyStopwatch(void);
	virtual ~HyStopwatch(void);
};

#endif /* __HyStopwatch_h__ */
