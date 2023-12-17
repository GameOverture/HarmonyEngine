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
#include "Diagnostics/Console/IHyConsole.h"

IHyController::IHyController(int32 iIndex, bool bIsGamePad) :
	m_bIS_GAMEPAD(bIsGamePad),
	m_iRefCount(0),
	m_iId(-1) // -1 is invalid, should be set in ctor
{
#ifdef HY_USE_GLFW
	m_iId = iIndex;
	if(m_bIS_GAMEPAD)
		m_sName = glfwGetGamepadName(m_iId);
	else
		m_sName = glfwGetJoystickName(m_iId);
	m_sGuid = glfwGetJoystickGUID(m_iId);
#elif defined(HY_USE_SDL2)
	m_iId = SDL_JoystickGetDeviceInstanceID(iIndex);
	if(m_bIS_GAMEPAD)
		m_sName = SDL_GameControllerNameForIndex(iIndex);
	else
		m_sName = SDL_JoystickNameForIndex(iIndex);
	char szGuid[40];
	SDL_JoystickGetGUIDString(SDL_JoystickGetDeviceGUID(iIndex), szGuid, 40);
	m_sGuid = szGuid;
#endif
}

/*virtual*/ IHyController::~IHyController(void)
{
}

bool IHyController::IsGamePad() const
{
	return m_bIS_GAMEPAD;
}

std::string IHyController::GetName() const
{
	return m_sName;
}

std::string IHyController::GetGuid() const
{
	return m_sGuid;
}

void IHyController::IncRefCount()
{
#if defined(HY_USE_SDL2)
	if(m_iRefCount == 0)
	{
		// This GamePad hasn't been assigned yet, so we need to open it
		// Reacquire the device index using the instance id
		int iDeviceIndex = -1;
		int iNumConnectedJoysticks = SDL_NumJoysticks();
		for(int i = 0; i < iNumConnectedJoysticks; ++i)
		{
			if(SDL_JoystickGetDeviceInstanceID(i) == m_iId)
			{
				iDeviceIndex = i;
				break;
			}
		}
		if(iDeviceIndex == -1)
		{
			HyLogWarning("HyInput::AssignGamePad() - Could not find gamepad with instance id: " << m_iId);
			return;
		}

		OnOpenController(iDeviceIndex);
	}
#endif
	m_iRefCount++;
}

void IHyController::DecRefCount()
{
	m_iRefCount--;

#if defined(HY_USE_SDL2)
	// After decrementing the reference count, if it's zero then close the gamepad
	if(m_iRefCount == 0)
		OnCloseController();
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HyGamePad::HyGamePad(int32 iIndex) :
	IHyController(iIndex, true)
{
#ifdef HY_USE_GLFW
	m_CachedGamePadState = {};
#elif defined(HY_USE_SDL2)
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
#elif defined(HY_USE_SDL2)
/*virtual*/ void HyGamePad::OnOpenController(int32 iIndex) /*override*/
{
	HyAssert(SDL_IsGameController(iIndex), "HyInput::AssignGamePad() - Device at index " << iIndex << " is not a gamepad");
	m_pSdlGameController = SDL_GameControllerOpen(iIndex);
}
/*virtual*/ void HyGamePad::OnCloseController() /*override*/
{
	SDL_GameControllerClose(m_pSdlGameController);
	m_pSdlGameController = nullptr;
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
	IHyController(iIndex, false)
{
#if defined(HY_USE_SDL2)
	m_pSdlJoystick = nullptr;
#endif
}

HyJoystick::~HyJoystick(void)
{
}

#if defined(HY_USE_SDL2)
/*virtual*/ void HyJoystick::OnOpenController(int32 iIndex) /*override*/
{
	HyAssert(SDL_IsGameController(iIndex) == false, "HyInput::AssignJoystick() - Device at index " << iIndex << " is a gamepad, not a joystick");
	m_pSdlJoystick = SDL_JoystickOpen(iIndex);
}

/*virtual*/ void HyJoystick::OnCloseController() /*override*/
{
	SDL_JoystickClose(m_pSdlJoystick);
	m_pSdlJoystick = nullptr;
}
#endif
