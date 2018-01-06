/**************************************************************************
 *	HyTimer.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyTimer_h__
#define HyTimer_h__

#include "IHyTimeInst.h"

class HyTimer : public IHyTimeInst
{
public:
	HyTimer(void);
	virtual ~HyTimer(void);
};

#endif /* HyTimer_h__ */
