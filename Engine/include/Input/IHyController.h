/**************************************************************************
 *	IHyController.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IHyController_h__
#define IHyController_h__

#include "Afx/HyStdAfx.h"
#include "Input/HyInputKeys.h"

class IHyController
{
protected:
	const bool			m_bIS_GAMEPAD;	// True if this controller is a gamepad, false if it is a joystick
	int32				m_iRefCount;	// HyInputMap reference count. Zero means this controller is not in use

	int32				m_iId;			// -1 means invalid
	std::string			m_sName;
	std::string			m_sGuid;

public:
	IHyController(int32 iIndex, bool bIsGamePad);
	virtual ~IHyController(void);

	bool IsGamePad() const;
	std::string GetName() const;
	std::string GetGuid() const;

protected:
	void IncRefCount();
	void DecRefCount();

#if defined(HY_USE_SDL2)
	virtual void OnOpenController(int32 iIndex) = 0;
	virtual void OnCloseController() = 0;
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HyGamePad : public IHyController
{
	friend class HyInput;

#ifdef HY_USE_GLFW
	GLFWgamepadstate		m_CachedGamePadState;	// Used to determine when new changes have been made within HyInput::UpdateGlfwControllers()
#elif defined(HY_USE_SDL2)
	SDL_GameController *	m_pSdlGameController; // When m_iRefCount == 0, this is nullptr. It should be reopened when m_iRefCount > 0
	float					m_AxisValueList[HYNUM_GAMEPADAXES];
#endif

private:
	HyGamePad(int32 iIndex);
	virtual ~HyGamePad(void);

#ifdef HY_USE_GLFW
	unsigned char GetButtonValue(HyGamePadBtn eBtn) const;
	void UpdateGamePadState(GLFWgamepadstate &gamePadStateRef);
#elif defined(HY_USE_SDL2)
	virtual void OnOpenController(int32 iIndex) override;
	virtual void OnCloseController() override;
#endif
	
	float GetAxisValue(HyGamePadAxis eAxis) const;
	void SetAxisValue(HyGamePadAxis eAxis, float fValue);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HyJoystick : public IHyController
{
	friend class HyInput;

#if defined(HY_USE_SDL2)
	SDL_Joystick *			m_pSdlJoystick;
#endif

	std::vector<int>		m_ButtonValueList;
	std::vector<float>		m_AxisValueList;

private:
	HyJoystick(int32 iIndex);
	virtual ~HyJoystick(void);

#if defined(HY_USE_SDL2)
	virtual void OnOpenController(int32 iIndex) override;
	virtual void OnCloseController() override;
#endif
};

#endif /* IHyController_h__ */
