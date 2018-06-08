/**************************************************************************
 *	HyInputMap.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
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
			FLAG_Pressed			= 1 << 0,
			FLAG_WasReleased		= 1 << 1,
			FLAG_IsReleased			= 1 << 2,

			// NOTE: Must use 8 or less flags above, as the upper 24bits are used for action category and joystick ids mapped
			FLAG_CategoryBitMask	= 0x0000FF00,
			FLAG_CategoryShiftAmt	= 8,
			FLAG_JoystickBitMask	= 0xFFFF0000
		};
		uint32	uiFlags;

		ActionInfo(int32 iId) :	iID(iId),
								iBtn(HYKEY_Unassigned),
								iBtnAlternative(HYKEY_Unassigned),
								uiFlags(0)
		{ }
	};
	std::map<int32, uint32>			m_ActionIndexMap;
	std::vector<ActionInfo>			m_ActionList;

public:
	HyInputMap();
	~HyInputMap(void);

	// Setup input
	/////////////////////////

	// Categorize actions (game, UI, etc.) so the same button can be used for multiple mappings.
	void SetActionCategory(int32 iActionId, uint8 uiCategory);

	// Returns -1, or the mapping 'eBtn' used to be assigned to
	int32 MapBtn(int32 iActionId, HyKeyboardBtn eBtn);
	int32 MapBtn(int32 iActionId, HyMouseBtn eBtn);

	// Returns -1, or the mapping 'iBtnAlternative' used to be assigned to
	int32 MapAlternativeBtn(int32 iActionId, HyKeyboardBtn eBtn);
	int32 MapAlternativeBtn(int32 iActionId, HyMouseBtn eBtn);


	bool MapJoystickBtn(int32 iActionId, HyGamePadBtn eBtn, uint32 uiJoystickIndex);
	bool MapJoystickAxis(int32 iUserId, HyGamePadBtn eAxis, float fMin = 0.0f, float fMax = 1.0f);

	bool Unmap(int32 iActionId);
	bool IsMapped(int32 iActionId) const;

	// Check input
	/////////////////////////

	bool IsActionDown(int32 iUserId) const;
	bool IsActionReleased(int32 iUserId) const;	// Only true for a single frame upon button release
	float GetAxis(int32 iUserId) const;
	float GetAxisDelta(int32 iUserId) const;

private:
	void Update();

	void ApplyInput(int32 iKey, HyBtnPressState ePressState);
};

#endif /* HyInputMap_h__ */
