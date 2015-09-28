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
	KeyEscape,
	KeyF1,
	KeyF2,
	KeyF3,
	KeyF4,
	KeyF5,
	KeyF6,
	KeyF7,
	KeyF8,
	KeyF9,
	KeyF10,
	KeyF11,
	KeyF12,
	KeyF13,
	KeyF14,
	KeyF15,
	KeyF16,
	KeyF17,
	KeyF18,
	KeyF19,
	KeyPrint,
	KeyScrollLock,
	KeyBreak,

	KeySpace = 0x0020,

	KeyApostrophe = 0x0027,
	KeyComma = 0x002c,
	KeyMinus = 0x002d,
	KeyPeriod = 0x002e,
	KeySlash = 0x002f,

	Key0 = 0x0030,
	Key1 = 0x0031,
	Key2 = 0x0032,
	Key3 = 0x0033,
	Key4 = 0x0034,
	Key5 = 0x0035,
	Key6 = 0x0036,
	Key7 = 0x0037,
	Key8 = 0x0038,
	Key9 = 0x0039,

	KeySemicolon = 0x003b,
	KeyLess = 0x003c,
	KeyEqual = 0x003d,

	KeyA = 0x0041,
	KeyB = 0x0042,
	KeyC = 0x0043,
	KeyD = 0x0044,
	KeyE = 0x0045,
	KeyF = 0x0046,
	KeyG = 0x0047,
	KeyH = 0x0048,
	KeyI = 0x0049,
	KeyJ = 0x004a,
	KeyK = 0x004b,
	KeyL = 0x004c,
	KeyM = 0x004d,
	KeyN = 0x004e,
	KeyO = 0x004f,
	KeyP = 0x0050,
	KeyQ = 0x0051,
	KeyR = 0x0052,
	KeyS = 0x0053,
	KeyT = 0x0054,
	KeyU = 0x0055,
	KeyV = 0x0056,
	KeyW = 0x0057,
	KeyX = 0x0058,
	KeyY = 0x0059,
	KeyZ = 0x005a,

	KeyBracketLeft = 0x005b,
	KeyBackslash = 0x005c,
	KeyBracketRight = 0x005d,

	KeyGrave = 0x0060,

	KeyLeft,
	KeyRight,
	KeyUp,
	KeyDown,
	KeyInsert,
	KeyHome,
	KeyDelete,
	KeyEnd,
	KeyPageUp,
	KeyPageDown,

	KeyNumLock,
	KeyKpEqual,
	KeyKpDivide,
	KeyKpMultiply,
	KeyKpSubtract,
	KeyKpAdd,
	KeyKpEnter,
	KeyKpInsert, // 0
	KeyKpEnd, // 1
	KeyKpDown, // 2
	KeyKpPageDown, // 3
	KeyKpLeft, // 4
	KeyKpBegin, // 5
	KeyKpRight, // 6
	KeyKpHome, // 7
	KeyKpUp, // 8
	KeyKpPageUp, // 9
	KeyKpDelete, // ,

	KeyBackSpace,
	KeyTab,
	KeyReturn,
	KeyCapsLock,
	KeyShiftL,
	KeyCtrlL,
	KeySuperL,
	KeyAltL,
	KeyAltR,
	KeySuperR,
	KeyMenu,
	KeyCtrlR,
	KeyShiftR,

	KeyBack,
	KeySoftLeft,
	KeySoftRight,
	KeyCall,
	KeyEndcall,
	KeyStar,
	KeyPound,
	KeyDpadCenter,
	KeyVolumeUp,
	KeyVolumeDown,
	KeyPower,
	KeyCamera,
	KeyClear,
	KeySymbol,
	KeyExplorer,
	KeyEnvelope,
	KeyEquals,
	KeyAt,
	KeyHeadsethook,
	KeyFocus,
	KeyPlus,
	KeyNotification,
	KeySearch,
	KeyMediaPlayPause,
	KeyMediaStop,
	KeyMediaNext,
	KeyMediaPrevious,
	KeyMediaRewind,
	KeyMediaFastForward,
	KeyMute,
	KeyPictsymbols,
	KeySwitchCharset,

	KeyForward,
	KeyExtra1,
	KeyExtra2,
	KeyExtra3,
	KeyExtra4,
	KeyExtra5,
	KeyExtra6,
	KeyFn,

	KeyCircumflex,
	KeySsharp,
	KeyAcute,
	KeyAltGr,
	KeyNumbersign,
	KeyUdiaeresis,
	KeyAdiaeresis,
	KeyOdiaeresis,
	KeySection,
	KeyAring,
	KeyDiaeresis,
	KeyTwosuperior,
	KeyRightParenthesis,
	KeyDollar,
	KeyUgrave,
	KeyAsterisk,
	KeyColon,
	KeyExclam,

	KeyBraceLeft,
	KeyBraceRight,
	KeySysRq,

	KeyCount_
};

enum HyMouseBtn
{
	MouseButton0 = 0,
	MouseButtonLeft = MouseButton0,
	MouseButton1,
	MouseButtonMiddle = MouseButton1,
	MouseButton2,
	MouseButtonRight = MouseButton2,
	MouseButton3,
	MouseButtonWheelUp = MouseButton3,
	MouseButton4,
	MouseButtonWheelDown = MouseButton4,
	MouseButton5,
	MouseButton6,
	MouseButton7,
	MouseButton8,
	MouseButton9,
	MouseButton10,
	MouseButton11,
	MouseButton12,
	MouseButton13,
	MouseButton14,
	MouseButton15,
	MouseButton16,
	MouseButton17,
	MouseButton18,
	MouseButton19,
	MouseButton20,

	MouseButtonMax = MouseButton20,
	MouseButtonCount,

	MouseAxisX = MouseButtonCount,
	MouseAxisY,
	MouseButtonCount_,

	MouseAxisCount = MouseButtonCount_ - MouseAxisX
};

enum HyGamePadBtn
{
	PadButtonLeftStickX,
	PadButtonLeftStickY,
	PadButtonRightStickX,
	PadButtonRightStickY,
	PadButtonAxis4, // L2/Left trigger
	PadButtonAxis5, // R2/Right trigger
	PadButtonAxis6,
	PadButtonAxis7,
	PadButtonAxis8,
	PadButtonAxis9,
	PadButtonAxis10,
	PadButtonAxis11,
	PadButtonAxis12,
	PadButtonAxis13,
	PadButtonAxis14,
	PadButtonAxis15,
	PadButtonAxis16,
	PadButtonAxis17,
	PadButtonAxis18,
	PadButtonAxis19,
	PadButtonAxis20,
	PadButtonAxis21,
	PadButtonAxis22,
	PadButtonAxis23,
	PadButtonAxis24,
	PadButtonAxis25,
	PadButtonAxis26,
	PadButtonAxis27,
	PadButtonAxis28,
	PadButtonAxis29,
	PadButtonAxis30,
	PadButtonAxis31,
	PadButtonAccelerationX,
	PadButtonAccelerationY,
	PadButtonAccelerationZ,
	PadButtonGravityX,
	PadButtonGravityY,
	PadButtonGravityZ,
	PadButtonGyroscopeX,
	PadButtonGyroscopeY,
	PadButtonGyroscopeZ,
	PadButtonMagneticFieldX,
	PadButtonMagneticFieldY,
	PadButtonMagneticFieldZ,
	PadButtonStart,
	PadButtonAxisCount_ = PadButtonStart,
	PadButtonSelect,
	PadButtonLeft,
	PadButtonRight,
	PadButtonUp,
	PadButtonDown,
	PadButtonA, // Cross
	PadButtonB, // Circle
	PadButtonX, // Square
	PadButtonY, // Triangle
	PadButtonL1,
	PadButtonR1,
	PadButtonL2,
	PadButtonR2,
	PadButtonL3, // Left thumb
	PadButtonR3, // Right thumb
	PadButtonHome, // PS button
	PadButton17,
	PadButton18,
	PadButton19,
	PadButton20,
	PadButton21,
	PadButton22,
	PadButton23,
	PadButton24,
	PadButton25,
	PadButton26,
	PadButton27,
	PadButton28,
	PadButton29,
	PadButton30,
	PadButton31,
	PadButtonMax_,
	PadButtonCount_ = PadButtonMax_ - PadButtonAxisCount_
};

#define HYINPUT_KEYBOARD_CODE_STD 32
#define HYINPUT_KEYBOARD_CODE_EXT 256
#define HYINPUT_KEYBOARD_CODE_NUMPAD 320
#define HYINPUT_KEYBOARD_CODE_NUMFLAGS 3

enum eActionButton // binary [on..off]
{
	Action_Btn0 = 0,
	Action_Btn1 = 1,
	Action_Btn2 = 2,
	Action_Btn3 = 3,
	Action_Btn4 = 4,
	Action_Btn5 = 5,
	Action_Btn6 = 6,
	Action_Btn7 = 7,
	Action_Btn8 = 8,
	Action_Btn9 = 9,
	Action_Btn10 = 10,
	Action_Btn11 = 11,
	Action_Btn12 = 12,
	Action_Btn13 = 13,
	Action_Btn14 = 14,
	Action_Btn15 = 15,
	Action_MaxBtns = 16,

	GP360_ButtonA = Action_Btn0,
	GP360_ButtonB = Action_Btn1,
	GP360_ButtonX = Action_Btn2,
	GP360_ButtonY = Action_Btn3,
	GP360_ButtonLB = Action_Btn4,
	GP360_ButtonRB = Action_Btn5,
	GP360_ButtonSelect = Action_Btn6,
	GP360_ButtonStart = Action_Btn7,
	GP360_ButtonLS = Action_Btn8,
	GP360_ButtonRS = Action_Btn9,
};

enum eActionAxis // float range [-1..1]
{
	Action_Axis0 = 0,
	Action_Axis1 = 1,
	Action_Axis2 = 2,
	Action_Axis3 = 3,
	Action_Axis4 = 4,
	Action_Axis5 = 5,
	Action_Axis6 = 6,
	Action_Axis7 = 7,
	Action_Axis8 = 8,
	Action_Axis9 = 9,
	Action_MaxAxes = 10,

	GP_LStickX = Action_Axis0,
	GP_LStickY = Action_Axis1,
	GP_RStickX = Action_Axis4,
	GP_RStickY = Action_Axis3,
	GP_Triggers = Action_Axis2,
	GP_DPadX = Action_Axis5,
	GP_DPadY = Action_Axis6,
};

class IHyInputMap
{
	friend class IHyInput;

public:
	IHyInputMap();
	~IHyInputMap(void);

	virtual bool MapBtn_KB(uint32 iUserId, HyKeyboardBtn eBtn) = 0;
	virtual bool MapBtn_MO(uint32 iUserId, HyMouseBtn eBtn) = 0;
	virtual bool MapBtn_GP(uint32 iUserId, HyGamePadBtn eBtn) = 0;

	virtual bool MapAxis_MO(uint32 iUserId, HyMouseBtn eAxis, float fMin = 0.0f, float fMax = 1.0f) = 0;
	virtual bool MapAxis_GP(uint32 iUserId, HyGamePadBtn eAxis, float fMin = 0.0f, float fMax = 1.0f) = 0;

	virtual void Unmap(uint32 iUserId) = 0;
	virtual bool IsMapped(uint32 iUserId) const = 0;

	virtual bool IsBtnDown(uint32 iUserId) const = 0;
	virtual bool IsBtnDownBuffered(uint32 iUserId) const = 0;		// If the button was just pressed
	virtual bool IsBtnReleased(uint32 iUserId) const = 0;
	
	virtual float GetAxis(uint32 iUserId) const = 0;
	virtual float GetAxisDelta(uint32 iUserId) const = 0;
};

#endif /* __IHyInputMap_h__ */
