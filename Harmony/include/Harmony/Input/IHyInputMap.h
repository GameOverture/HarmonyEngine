/**************************************************************************
 *	IHyInputMap.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __IHyInputMap_h__
#define __IHyInputMap_h__

#include "Afx/HyStdAfx.h"

enum HyKeyboardBtn
{
	HYKEY_Escape,
	HYKEY_F1,
	HYKEY_F2,
	HYKEY_F3,
	HYKEY_F4,
	HYKEY_F5,
	HYKEY_F6,
	HYKEY_F7,
	HYKEY_F8,
	HYKEY_F9,
	HYKEY_F10,
	HYKEY_F11,
	HYKEY_F12,
	HYKEY_F13,
	HYKEY_F14,
	HYKEY_F15,
	HYKEY_F16,
	HYKEY_F17,
	HYKEY_F18,
	HYKEY_F19,
	HYKEY_Print,
	HYKEY_ScrollLock,
	HYKEY_Break,

	HYKEY_Space = 0x0020,

	HYKEY_Apostrophe = 0x0027,
	HYKEY_Comma = 0x002c,
	HYKEY_Minus = 0x002d,
	HYKEY_Period = 0x002e,
	HYKEY_Slash = 0x002f,

	HYKEY_0 = 0x0030,
	HYKEY_1 = 0x0031,
	HYKEY_2 = 0x0032,
	HYKEY_3 = 0x0033,
	HYKEY_4 = 0x0034,
	HYKEY_5 = 0x0035,
	HYKEY_6 = 0x0036,
	HYKEY_7 = 0x0037,
	HYKEY_8 = 0x0038,
	HYKEY_9 = 0x0039,

	HYKEY_Semicolon = 0x003b,
	HYKEY_Less = 0x003c,
	HYKEY_Equal = 0x003d,

	HYKEY_A = 0x0041,
	HYKEY_B = 0x0042,
	HYKEY_C = 0x0043,
	HYKEY_D = 0x0044,
	HYKEY_E = 0x0045,
	HYKEY_F = 0x0046,
	HYKEY_G = 0x0047,
	HYKEY_H = 0x0048,
	HYKEY_I = 0x0049,
	HYKEY_J = 0x004a,
	HYKEY_K = 0x004b,
	HYKEY_L = 0x004c,
	HYKEY_M = 0x004d,
	HYKEY_N = 0x004e,
	HYKEY_O = 0x004f,
	HYKEY_P = 0x0050,
	HYKEY_Q = 0x0051,
	HYKEY_R = 0x0052,
	HYKEY_S = 0x0053,
	HYKEY_T = 0x0054,
	HYKEY_U = 0x0055,
	HYKEY_V = 0x0056,
	HYKEY_W = 0x0057,
	HYKEY_X = 0x0058,
	HYKEY_Y = 0x0059,
	HYKEY_Z = 0x005a,

	HYKEY_BracketLeft = 0x005b,
	HYKEY_Backslash = 0x005c,
	HYKEY_BracketRight = 0x005d,

	HYKEY_Grave = 0x0060,

	HYKEY_Left,
	HYKEY_Right,
	HYKEY_Up,
	HYKEY_Down,
	HYKEY_Insert,
	HYKEY_Home,
	HYKEY_Delete,
	HYKEY_End,
	HYKEY_PageUp,
	HYKEY_PageDown,

	HYKEY_NumLock,
	HYKEY_KpEqual,
	HYKEY_KpDivide,
	HYKEY_KpMultiply,
	HYKEY_KpSubtract,
	HYKEY_KpAdd,
	HYKEY_KpEnter,
	HYKEY_KpInsert, // 0
	HYKEY_KpEnd, // 1
	HYKEY_KpDown, // 2
	HYKEY_KpPageDown, // 3
	HYKEY_KpLeft, // 4
	HYKEY_KpBegin, // 5
	HYKEY_KpRight, // 6
	HYKEY_KpHome, // 7
	HYKEY_KpUp, // 8
	HYKEY_KpPageUp, // 9
	HYKEY_KpDelete, // ,

	HYKEY_BackSpace,
	HYKEY_Tab,
	HYKEY_Return,
	HYKEY_CapsLock,
	HYKEY_ShiftL,
	HYKEY_CtrlL,
	HYKEY_SuperL,
	HYKEY_AltL,
	HYKEY_AltR,
	HYKEY_SuperR,
	HYKEY_Menu,
	HYKEY_CtrlR,
	HYKEY_ShiftR,

	HYKEY_Back,
	HYKEY_SoftLeft,
	HYKEY_SoftRight,
	HYKEY_Call,
	HYKEY_Endcall,
	HYKEY_Star,
	HYKEY_Pound,
	HYKEY_DpadCenter,
	HYKEY_VolumeUp,
	HYKEY_VolumeDown,
	HYKEY_Power,
	HYKEY_Camera,
	HYKEY_Clear,
	HYKEY_Symbol,
	HYKEY_Explorer,
	HYKEY_Envelope,
	HYKEY_Equals,
	HYKEY_At,
	HYKEY_Headsethook,
	HYKEY_Focus,
	HYKEY_Plus,
	HYKEY_Notification,
	HYKEY_Search,
	HYKEY_MediaPlayPause,
	HYKEY_MediaStop,
	HYKEY_MediaNext,
	HYKEY_MediaPrevious,
	HYKEY_MediaRewind,
	HYKEY_MediaFastForward,
	HYKEY_Mute,
	HYKEY_Pictsymbols,
	HYKEY_SwitchCharset,

	HYKEY_Forward,
	HYKEY_Extra1,
	HYKEY_Extra2,
	HYKEY_Extra3,
	HYKEY_Extra4,
	HYKEY_Extra5,
	HYKEY_Extra6,
	HYKEY_Fn,

	HYKEY_Circumflex,
	HYKEY_Ssharp,
	HYKEY_Acute,
	HYKEY_AltGr,
	HYKEY_Numbersign,
	HYKEY_Udiaeresis,
	HYKEY_Adiaeresis,
	HYKEY_Odiaeresis,
	HYKEY_Section,
	HYKEY_Aring,
	HYKEY_Diaeresis,
	HYKEY_Twosuperior,
	HYKEY_RightParenthesis,
	HYKEY_Dollar,
	HYKEY_Ugrave,
	HYKEY_Asterisk,
	HYKEY_Colon,
	HYKEY_Exclam,

	HYKEY_BraceLeft,
	HYKEY_BraceRight,
	HYKEY_SysRq,

	HYKEY_Count_
};

enum HyMouseBtn
{
	HYMOUSE_Button0 = 0,
	HYMOUSE_ButtonLeft = HYMOUSE_Button0,
	HYMOUSE_Button1,
	HYMOUSE_ButtonMiddle = HYMOUSE_Button1,
	HYMOUSE_Button2,
	HYMOUSE_ButtonRight = HYMOUSE_Button2,
	HYMOUSE_Button3,
	HYMOUSE_ButtonWheelUp = HYMOUSE_Button3,
	HYMOUSE_Button4,
	HYMOUSE_ButtonWheelDown = HYMOUSE_Button4,
	HYMOUSE_Button5,
	HYMOUSE_Button6,
	HYMOUSE_Button7,
	HYMOUSE_Button8,
	HYMOUSE_Button9,
	HYMOUSE_Button10,
	HYMOUSE_Button11,
	HYMOUSE_Button12,
	HYMOUSE_Button13,
	HYMOUSE_Button14,
	HYMOUSE_Button15,
	HYMOUSE_Button16,
	HYMOUSE_Button17,
	HYMOUSE_Button18,
	HYMOUSE_Button19,
	HYMOUSE_Button20,

	HYMOUSE_ButtonCount,
};

enum HyMouseAxis
{
	HYMOUSE_AxisX = HYMOUSE_ButtonCount,
	HYMOUSE_AxisY,

	HYMOUSE_AxisCount = 2
};

enum HyGamePadBtn
{
	HYPAD_ButtonLeftStickX,
	HYPAD_ButtonLeftStickY,
	HYPAD_ButtonRightStickX,
	HYPAD_ButtonRightStickY,
	HYPAD_ButtonAxis4, // L2/Left trigger
	HYPAD_ButtonAxis5, // R2/Right trigger
	HYPAD_ButtonAxis6,
	HYPAD_ButtonAxis7,
	HYPAD_ButtonAxis8,
	HYPAD_ButtonAxis9,
	HYPAD_ButtonAxis10,
	HYPAD_ButtonAxis11,
	HYPAD_ButtonAxis12,
	HYPAD_ButtonAxis13,
	HYPAD_ButtonAxis14,
	HYPAD_ButtonAxis15,
	HYPAD_ButtonAxis16,
	HYPAD_ButtonAxis17,
	HYPAD_ButtonAxis18,
	HYPAD_ButtonAxis19,
	HYPAD_ButtonAxis20,
	HYPAD_ButtonAxis21,
	HYPAD_ButtonAxis22,
	HYPAD_ButtonAxis23,
	HYPAD_ButtonAxis24,
	HYPAD_ButtonAxis25,
	HYPAD_ButtonAxis26,
	HYPAD_ButtonAxis27,
	HYPAD_ButtonAxis28,
	HYPAD_ButtonAxis29,
	HYPAD_ButtonAxis30,
	HYPAD_ButtonAxis31,
	HYPAD_ButtonAccelerationX,
	HYPAD_ButtonAccelerationY,
	HYPAD_ButtonAccelerationZ,
	HYPAD_ButtonGravityX,
	HYPAD_ButtonGravityY,
	HYPAD_ButtonGravityZ,
	HYPAD_ButtonGyroscopeX,
	HYPAD_ButtonGyroscopeY,
	HYPAD_ButtonGyroscopeZ,
	HYPAD_ButtonMagneticFieldX,
	HYPAD_ButtonMagneticFieldY,
	HYPAD_ButtonMagneticFieldZ,
	HYPAD_ButtonStart,
	HYPAD_ButtonAxisCount_ = HYPAD_ButtonStart,
	HYPAD_ButtonSelect,
	HYPAD_ButtonLeft,
	HYPAD_ButtonRight,
	HYPAD_ButtonUp,
	HYPAD_ButtonDown,
	HYPAD_ButtonA, // Cross
	HYPAD_ButtonB, // Circle
	HYPAD_ButtonX, // Square
	HYPAD_ButtonY, // Triangle
	HYPAD_ButtonL1,
	HYPAD_ButtonR1,
	HYPAD_ButtonL2,
	HYPAD_ButtonR2,
	HYPAD_ButtonL3, // Left thumb
	HYPAD_ButtonR3, // Right thumb
	HYPAD_ButtonHome, // PS button
	HYPAD_Button17,
	HYPAD_Button18,
	HYPAD_Button19,
	HYPAD_Button20,
	HYPAD_Button21,
	HYPAD_Button22,
	HYPAD_Button23,
	HYPAD_Button24,
	HYPAD_Button25,
	HYPAD_Button26,
	HYPAD_Button27,
	HYPAD_Button28,
	HYPAD_Button29,
	HYPAD_Button30,
	HYPAD_Button31,
	HYPAD_ButtonMax_,
	HYPAD_ButtonCount_ = HYPAD_ButtonMax_ - HYPAD_ButtonAxisCount_
};

class IHyInputMap
{
	friend class IHyInput;

protected:
	IHyInput *			m_pInputManager;

	static glm::vec2	sm_ptWorldMousePos;

public:
	IHyInputMap(IHyInput *pInputManager);
	virtual ~IHyInputMap(void);

	static glm::vec2 GetWorldMousePos();

	virtual bool MapBtn_KB(uint32 iUserId, HyKeyboardBtn eBtn) = 0;
	virtual bool MapBtn_MO(uint32 iUserId, HyMouseBtn eBtn) = 0;
	virtual bool MapBtn_GP(uint32 iUserId, HyGamePadBtn eBtn, uint32 uiGamePadIndex) = 0;
	virtual bool MapAxis_MO(uint32 iUserId, HyMouseAxis eAxis, float fMin = 0.0f, float fMax = 1.0f) = 0;
	virtual bool MapAxis_GP(uint32 iUserId, HyGamePadBtn eAxis, float fMin = 0.0f, float fMax = 1.0f) = 0;

	virtual void Unmap(uint32 iUserId) = 0;
	virtual bool IsMapped(uint32 iUserId) const = 0;

	virtual bool IsBtnDown(uint32 iUserId) const = 0;
	virtual bool IsBtnDownBuffered(uint32 iUserId) const = 0;		// If the button was just pressed
	virtual bool IsBtnReleased(uint32 iUserId) const = 0;
	virtual float GetAxis(uint32 iUserId) const = 0;
	virtual float GetAxisDelta(uint32 iUserId) const = 0;

	void StartRecording();
	void StopRecording();
	void SerializeRecording();

	void StartPlayback();
	void StopPlayback();
};

#endif /* __IHyInputMap_h__ */
