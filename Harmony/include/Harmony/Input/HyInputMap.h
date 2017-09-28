/**************************************************************************
 *	HyInputMap.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyInputMap_h__
#define HyInputMap_h__

#include "Afx/HyStdAfx.h"
#include "Input/HyInputKeys.h"

class HyInputMap
{
	friend class HyInput;

	struct ButtonInfo
	{
		int32	iBtn;
		int32	iBtnAlternative;

		bool	bPrevious;
		bool	bCurrent;

		ButtonInfo() :	iBtn(HYKEY_Unknown),
						bPrevious(false),
						bCurrent(false)
		{ }

		ButtonInfo(int32 iBtn_) :	iBtn(iBtn_),
									bPrevious(false),
									bCurrent(false)
		{ }
	};
	std::map<int32, ButtonInfo>		m_ButtonMap_KB;
	std::map<int32, ButtonInfo>		m_ButtonMap_MO;
	std::map<int32, ButtonInfo>		m_ButtonMap_JOY;

public:
	HyInputMap();
	~HyInputMap(void);

	// Setup input
	bool MapBtn(int32 iUserId, HyKeyboardBtn eBtn);
	bool MapBtn(int32 iUserId, HyMouseBtn eBtn);
	bool MapBtn(int32 iUserId, HyGamePadBtn eBtn);

	bool MapAxis_GP(int32 iUserId, HyGamePadBtn eAxis, float fMin = 0.0f, float fMax = 1.0f);

	bool Unmap(int32 iUserId);
	bool IsMapped(int32 iUserId) const;

	// Check input
	bool IsBtnDown(int32 iUserId) const;
	bool IsBtnReleased(int32 iUserId) const;	// Only true for a single frame upon button release
	float GetAxis(int32 iUserId) const;
	float GetAxisDelta(int32 iUserId) const;

private:
	void Update();
};

#endif /* HyInputMap_h__ */
