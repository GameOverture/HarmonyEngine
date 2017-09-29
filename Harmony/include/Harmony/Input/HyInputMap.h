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

	struct ActionInfo
	{
		const int32	iID;

		int32	iBtn;
		int32	iBtnAlternative;

		enum Flag
		{
			FLAG_IsJoystick		= 1 << 0,
			FLAG_Pressed		= 1 << 1,
			FLAG_WasReleased	= 1 << 2,
			FLAG_IsReleased		= 1 << 3
		};
		uint32	uiFlags;

		ActionInfo(int32 iId) :	iID(iId),
								iBtn(HYKEY_Unknown),
								iBtnAlternative(HYKEY_Unknown),
								uiFlags(0)
		{ }
	};
	std::map<int32, uint32>			m_ActionIndexMap;
	std::vector<ActionInfo>			m_ActionList;

public:
	HyInputMap();
	~HyInputMap(void);

	// Setup input
	int32 MapBtn(int32 iActionId, HyKeyboardBtn eBtn);
	int32 MapBtn(int32 iActionId, HyMouseBtn eBtn);
	int32 MapAlternativeBtn(int32 iActionId, HyKeyboardBtn eBtn);
	int32 MapAlternativeBtn(int32 iActionId, HyMouseBtn eBtn);
	bool MapJoystickBtn(int32 iActionId, HyGamePadBtn eBtn, uint32 uiJoystickIndex);

	bool MapAxis_GP(int32 iUserId, HyGamePadBtn eAxis, float fMin = 0.0f, float fMax = 1.0f);

	bool Unmap(int32 iActionId);
	bool IsMapped(int32 iActionId) const;

	// Check input
	bool IsActionDown(int32 iUserId) const;
	bool IsActionReleased(int32 iUserId) const;	// Only true for a single frame upon button release
	float GetAxis(int32 iUserId) const;
	float GetAxisDelta(int32 iUserId) const;

private:
	void Update();

	void ApplyInput(int32 iKey, HyBtnPressState ePressState);
};

#endif /* HyInputMap_h__ */
