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

	void Start(); void Resume(); // Synonymous functions
	void ResetStart();
	void Pause();

	void Reset();

	virtual std::string ToString() const override;
};

#endif /* HyStopwatch_h__ */
