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
	double							m_dDuration;

	std::function<void(void *)>		m_fpCallbackFunc;
	void *							m_pCallbackData;

public:
	HyTimer(void);
	HyTimer(double dDuration);
	virtual ~HyTimer(void);

	double GetDuration() const;

	void Init(double dDuration);
	void Reset();

	void Start();
	void Pause();

	bool IsExpired() const;
	double TimeLeft() const;

	void SetExpiredCallback(std::function<void(void *)> fpFunc, void *pData);

private:
	virtual void OnUpdate() override;
};

#endif /* HyTimer_h__ */
