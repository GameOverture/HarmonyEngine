/**************************************************************************
 *	IHyTimeInst.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __IHyTimeInst_h__
#define __IHyTimeInst_h__

class IHyTime;

class IHyTimeInst
{
	static IHyTime *sm_pTime;

public:
	IHyTimeInst(void);
	virtual ~IHyTimeInst(void);

	void Update(double dDelta);
};

#endif /* __IHyTimeInst_h__ */
