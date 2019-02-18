/**************************************************************************
 *	IHyTimeInst.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IHyTimeInst_h__
#define IHyTimeInst_h__

class HyTime;

class IHyTimeInst
{
	friend class HyTime;

protected:
	static HyTime *sm_pTime;

public:
	IHyTimeInst(void);
	virtual ~IHyTimeInst(void);

protected:
	virtual void Update(double dDelta) = 0;
};

#endif /* IHyTimeInst_h__ */
