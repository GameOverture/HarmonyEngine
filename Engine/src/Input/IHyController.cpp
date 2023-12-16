/**************************************************************************
 *	IHyController.cpp
 *
 *	Harmony Engine
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Input/IHyController.h"

IHyController::IHyController() :
	m_iRefCount(0),
	m_iId(-1),
	m_sName("")
{
}

/*virtual*/ IHyController::~IHyController(void)
{
}

int32 IHyController::GetId() const
{
	return m_iId;
}

std::string IHyController::GetName() const
{
	return m_sName;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HyGamePad::HyGamePad(int32 iIndex) :
	IHyController()
{
#ifdef HY_USE_GLFW
	m_iId = iIndex;
	m_sName = glfwGetGamepadName(m_iId);
	m_CachedGamePadState = {};
#elif defined(HY_USE_SDL2)
	m_iId = SDL_JoystickGetDeviceInstanceID(iIndex);
	m_sName = SDL_GameControllerNameForIndex(iIndex);
	m_pSdlGameController = nullptr;
	for(uint32 i = 0; i < HYNUM_GAMEPADAXES; ++i)
		m_AxisValueList[i] = 0.0f;
#endif
}

HyGamePad::~HyGamePad(void)
{
}

#ifdef HY_USE_GLFW
unsigned char HyGamePad::GetButtonValue(HyGamePadBtn eBtn) const
{
	return m_CachedGamePadState.buttons[eBtn];
}
void HyGamePad::UpdateGamePadState(GLFWgamepadstate &gamePadStateRef)
{
	m_CachedGamePadState = gamePadStateRef;
}
#endif

float HyGamePad::GetAxisValue(HyGamePadAxis eAxis) const
{
#ifdef HY_USE_GLFW
	return m_CachedGamePadState.axes[eAxis];
#elif defined(HY_USE_SDL2)
	return m_AxisValueList[eAxis];
#endif
}

void HyGamePad::SetAxisValue(HyGamePadAxis eAxis, float fValue)
{
#if defined(HY_USE_GLFW)
	m_CachedGamePadState.axes[eAxis] = fValue; // NOTE: UpdateGamePadState() should be used instead of this function when compiling with GLFW
#elif defined(HY_USE_SDL2)
	m_AxisValueList[eAxis] = fValue;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HyJoystick::HyJoystick(int iIndex) :
	IHyController()
{
#ifdef HY_USE_GLFW
	m_iId = iIndex;
	m_sName = glfwGetJoystickName(m_iId);
#elif defined(HY_USE_SDL2)
	m_iId = SDL_JoystickGetDeviceInstanceID(iIndex);
	m_sName = SDL_JoystickNameForIndex(iIndex);
	m_pSdlJoystick = nullptr;
#endif
}

HyJoystick::~HyJoystick(void)
{
}
