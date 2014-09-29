/**************************************************************************
 *	HyInputAfx.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyInputAfx_h__
#define __HyInputAfx_h__

#define HYINPUT_KEYBOARD_CODE_STD 32
#define HYINPUT_KEYBOARD_CODE_EXT 256
#define HYINPUT_KEYBOARD_CODE_NUMPAD 320
#define HYINPUT_KEYBOARD_CODE_NUMFLAGS 3

enum eActionButton // binary [on..off]
{
	Action_Btn0		= 0,
	Action_Btn1		= 1,
	Action_Btn2		= 2,
	Action_Btn3		= 3,
	Action_Btn4		= 4,
	Action_Btn5		= 5,
	Action_Btn6		= 6,
	Action_Btn7		= 7,
	Action_Btn8		= 8,
	Action_Btn9		= 9,
	Action_Btn10	= 10,
	Action_Btn11	= 11,
	Action_Btn12	= 12,
	Action_Btn13	= 13,
	Action_Btn14	= 14,
	Action_Btn15	= 15,
	Action_MaxBtns	= 16,

	GP360_ButtonA	= Action_Btn0,
	GP360_ButtonB	= Action_Btn1,
	GP360_ButtonX	= Action_Btn2,
	GP360_ButtonY	= Action_Btn3,
	GP360_ButtonLB	= Action_Btn4,
	GP360_ButtonRB	= Action_Btn5,
	GP360_ButtonSelect	= Action_Btn6,
	GP360_ButtonStart	= Action_Btn7,
	GP360_ButtonLS	= Action_Btn8,
	GP360_ButtonRS	= Action_Btn9,
};

enum eActionAxis // float range [-1..1]
{
	Action_Axis0	= 0,
	Action_Axis1	= 1,
	Action_Axis2	= 2,
	Action_Axis3	= 3,
	Action_Axis4	= 4,
	Action_Axis5	= 5,
	Action_Axis6	= 6,
	Action_Axis7	= 7,
	Action_Axis8	= 8,
	Action_Axis9	= 9,
	Action_MaxAxes	= 10,

	GP_LStickX		= Action_Axis0,
	GP_LStickY		= Action_Axis1,
	GP_RStickX		= Action_Axis4,
	GP_RStickY		= Action_Axis3,
	GP_Triggers		= Action_Axis2,
	GP_DPadX		= Action_Axis5,
	GP_DPadY		= Action_Axis6,
};

struct tInputState
{
	float	pAxisStates[Action_MaxAxes];
	uint16	uiButtonFlags;

	// TODO?: Account for struct padding when saving replays

	tInputState()
	{
		uiButtonFlags = 0;
		for(int32 i = 0; i < Action_MaxAxes; ++i)
			pAxisStates[i] = 0.0f;
	}
};

class HyInputKey
{
	friend class HyInput;

public:
	enum eInputType
	{
		InputType_Unknown = -1,
		InputType_GP,
		InputType_AXIS,
		InputType_KB,
		InputType_MO
	};
private:

	eInputType				m_eType;

	int32					m_iAux;	// Additional data about the input mapping, such as which gamepad Id
	bool					m_bPositiveAxis;
	uint32					m_uiKey;

public:
	HyInputKey() :	m_eType(InputType_Unknown),
					m_iAux(0),
					m_uiKey(0)
	{ }

	HyInputKey(uint32 uiGamePadId, eActionButton eBtn) :	m_eType(InputType_GP),
															m_iAux(static_cast<int32>(uiGamePadId)),
															m_uiKey(eBtn)
	{ }

	HyInputKey(uint32 uiGamePadId, eActionAxis eAxis) :		m_eType(InputType_AXIS),
															m_iAux(static_cast<int32>(uiGamePadId)),
															m_uiKey(eAxis)
	{ }

	HyInputKey(uint32 uiKey)
	{
		HyAssert(uiKey >= HYINPUT_KEYBOARD_CODE_STD, "HyInputKey(uint32 uiKey) cannot accept key value of " << uiKey << ".");

		m_eType = InputType_KB;
		if(uiKey < HYINPUT_KEYBOARD_CODE_EXT)
		{
			m_iAux = 0;		// Offset of '32'
			m_uiKey = uiKey - HYINPUT_KEYBOARD_CODE_STD;
		}
		else if(uiKey < HYINPUT_KEYBOARD_CODE_NUMPAD)
		{
			m_iAux = 1;		// Offset of '256'
			m_uiKey = uiKey;
		}
		else
		{
			m_iAux = 2;		// Offset of '320'
			m_uiKey = uiKey;
		}
	}

	// This function doesn't need to be used by the game code.
	eInputType GetInputType()			{ return m_eType; }

	// This function doesn't need to be used by the game code.
	void SetAsPositiveAxis(bool bPos)	{ m_bPositiveAxis = bPos; }

	void SetAsGamePadBtn(uint32 uiGamePadId, eActionButton eBtn)
	{
		m_eType = InputType_GP;
		m_iAux = static_cast<int32>(uiGamePadId);
		m_uiKey = eBtn;
	}

	void SetAsGamePadAxis(uint32 uiGamePadId, eActionAxis eAxis)
	{
		m_eType = InputType_AXIS;
		m_iAux = static_cast<int32>(uiGamePadId);
		m_uiKey = eAxis;
	}

	void SetAsKeyBoardKey(uint32 uiKey)
	{
		HyAssert(uiKey < HYINPUT_KEYBOARD_CODE_STD, "tInputKey::SetAsKeyBoardKey() cannot accept key value of " << uiKey << ".");
		
		m_eType = InputType_KB;
		if(uiKey < HYINPUT_KEYBOARD_CODE_EXT)
			m_iAux = 0;		// Offset of '32'
		else if(uiKey < HYINPUT_KEYBOARD_CODE_NUMPAD)
			m_iAux = 1;		// Offset of '256'
		else
			m_iAux = 2;		// Offset of '320'

		m_uiKey = uiKey;
	}
};

#endif /* __HyInputAfx_h__ */
