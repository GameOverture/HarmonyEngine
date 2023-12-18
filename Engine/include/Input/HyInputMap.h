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

class HyGamePad;
class HyJoystick;

class HyInputMap
{
	struct ActionInfo
	{
		const int32		iID;

		int32			iBtn;
		int32			iBtnAlternative;
		
		HyGamePadBtn	ePadBtn;

		HyJoystick *	pJoystick;
		int32			iJoyBtn;

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
		uint32			uiFlags;

		ActionInfo(int32 iId) :	iID(iId),
								iBtn(HYKEY_Unassigned),
								iBtnAlternative(HYKEY_Unassigned),
								ePadBtn(HYPAD_Unassigned),
								pJoystick(nullptr),
								iJoyBtn(-1),
								uiFlags(0)
		{ }
	};
	std::map<int32, uint32>			m_ActionIndexMap;
	std::vector<ActionInfo>			m_ActionList;

	HyGamePad *						m_pGamePad;		// The GamePad assigned to this input map (a GamePad is a Joystick, but not all Joysticks are GamePads)
	std::vector<HyJoystick *>		m_JoystickList; // Joysticks assigned to this input map (may have multiple joysticks assigned to a single input map)

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

	HyGamePad *GetGamePad() const;
	void AssignGamePad(HyGamePad *pGamePad);
	void RemoveGamePad();

	const std::vector<HyJoystick *> &GetJoystickList() const;
	void AssignJoystick(HyJoystick *pJoystick);
	void RemoveJoystick(HyJoystick *pJoystick);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Check input

	bool IsActionDown(int32 iUserId) const;
	bool IsActionReleased(int32 iUserId) const;	// Only true for a single frame upon button release

	void Update();
	void ApplyKeyBoardInput(int32 iKey, HyBtnPressState ePressState);
	void ApplyGamePadButton(HyGamePadBtn eGamePadBtn, HyBtnPressState ePressState);
};

#endif /* HyInputMap_h__ */
