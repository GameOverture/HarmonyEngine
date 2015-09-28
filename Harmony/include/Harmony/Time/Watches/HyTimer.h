/**************************************************************************
 *	HyTimer.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyTimer_h__
#define __HyTimer_h__

#include "IHyTimeInst.h"

class HyTimer : public IHyTimeInst
{
public:
	HyTimer(void);
	virtual ~HyTimer(void);
};

#endif /* __HyTimer_h__ */
