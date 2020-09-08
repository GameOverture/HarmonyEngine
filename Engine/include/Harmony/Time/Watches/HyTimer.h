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
	double	m_dDuration;

public:
	HyTimer(void);
	HyTimer(double dDuration);
	virtual ~HyTimer(void);

	void Init(double dDuration);
	void Reset();

	void Start();
	void Pause();

	bool IsExpired() const;
	double TimeLeft() const;
};

#endif /* HyTimer_h__ */
