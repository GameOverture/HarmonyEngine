/**************************************************************************
 *	HyInputMapping.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyInputProfile_h__
#define __HyInputProfile_h__

#include "Afx/HyStdAfx.h"
#include "gainput/gainput.h"

// Forward declaration
class HyInput;

class HyInputMapping
{
	friend class HyInput;

	HyInput *										m_pHyInput;
	gainput::InputMap *								m_pInputMap;

public:
	HyInputMapping();
	~HyInputMapping(void);

private:
	void SetHyInputPtr(HyInput *pHyInput);
};

#endif /* __HyInputProfile_h__ */
