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

	static HyTime *	sm_pTime;

protected:
	bool			m_bIsRunning;
	double			m_dElapsedTime;

public:
	IHyTimeInst(void);
	virtual ~IHyTimeInst(void);

	bool IsRunning() const;

private:
	void Update(double dDelta);
};

#endif /* IHyTimeInst_h__ */
