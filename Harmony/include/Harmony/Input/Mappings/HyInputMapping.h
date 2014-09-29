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

#include "HyInputAfx.h"

#include <vector>
#include <map>
using std::vector;
using std::map;

// Forward declaration
class HyInput;

class HyInputMapping
{
	friend class HyInput;

	HyInput *										m_pHyInput;

	vector<std::pair<eActionButton, HyInputKey> >	m_vBtnMappings;
	vector<std::pair<eActionAxis, HyInputKey> >		m_vAxisMappings;

	tInputState										m_tCurConvertedInput;
	uint32											m_uiPrevConvertedBtnFlags;

public:
	HyInputMapping();
	~HyInputMapping(void);

	void BindBtnMap(eActionButton eActionBtn, HyInputKey &tKey);
	void BindAxisMap(eActionAxis eActionBtn, HyInputKey &tKey);
	void BindAxisMapPos(eActionAxis eAxis, HyInputKey &tKey);
	void BindAxisMapNeg(eActionAxis eAxis, HyInputKey &tKey);

	void GetGamePadIds(vector<uint32> &vGamePadIdsOut);
	HyString GetGamePadName(uint32 uiGamePadId);

	bool GpBtnDown(eActionButton eBtn);
	bool GpBtnDownBuff(eActionButton eBtn);
	bool GpBtnUp(eActionButton eBtn);
	float GpAxis(eActionAxis eAxis);

	static void SetReadKeyCallback(void (*fpCallback)(HyInputKey &tKey, void *pParam), void *pParam = NULL);

private:
	void SetHyInputPtr(HyInput *pHyInput)	{ m_pHyInput = pHyInput; }
};

#endif /* __HyInputProfile_h__ */
