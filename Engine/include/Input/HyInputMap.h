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
	struct ActionInfo
	{
		const int32	iID;

		int32			iBtn;
		int32			iBtnAlternative;
		HyGamePadBtn	ePadBtn;

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
								ePadBtn(HYPAD_Unassigned),
								uiFlags(0)
		{ }
	};
	std::map<int32, uint32>			m_ActionIndexMap;
	std::vector<ActionInfo>			m_ActionList;

	int32							m_iGamePadIndex;
	float							m_AxisValues[HYNUM_HYPADAXIS];

public:
	HyInputMap();
	~HyInputMap(void);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Setup input

	// Categorize actions (game, UI, etc.) so the same physical button can be used for multiple mappings.
	void SetActionCategory(int32 iActionId, uint8 uiCategory);

	// Returns -1, or the mapping 'eBtn' used to be assigned to
	int32 MapBtn(int32 iActionId, HyKeyboardBtn eBtn);
	int32 MapBtn(int32 iActionId, HyMouseBtn eBtn);

	// Returns -1, or the mapping 'iBtnAlternative' used to be assigned to
	int32 MapAlternativeBtn(int32 iActionId, HyKeyboardBtn eBtn);
	int32 MapAlternativeBtn(int32 iActionId, HyMouseBtn eBtn);

	int32 MapPadBtn(int32 iActionId, HyGamePadBtn eBtn);

	bool Unmap(int32 iActionId);
	bool IsMapped(int32 iActionId) const;

	int GetGamePadIndex() const;
	void SetGamePadIndex(int32 iGamePadIndex);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Check input

	bool IsActionDown(int32 iUserId) const;
	bool IsActionReleased(int32 iUserId) const;	// Only true for a single frame upon button release
	float GetAxis(HyGamePadAxis eAxis) const;
	float GetAxisDelta(HyGamePadAxis eAxis) const;

	void Update();
	void ApplyInput(int32 iKey, HyBtnPressState ePressState);
	void ApplyPadInput(int32 iPadBtn, HyBtnPressState ePressState);
	void ApplyPadAxis(int32 iAxis, float fValue);
};

#endif /* HyInputMap_h__ */
