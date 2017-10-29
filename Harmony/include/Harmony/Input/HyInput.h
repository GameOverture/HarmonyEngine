/**************************************************************************
 *	HyInput.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyInput_h__
#define HyInput_h__

#include "Afx/HyStdAfx.h"
#include "Input/HyInputMap.h"
#include "Threading/Threading.h"

class HyWindow;

class HyInput
{
	friend class HyEngine;

	const uint32				m_uiNUM_INPUT_MAPS;
	HyInputMap *				m_pInputMaps;

	std::vector<HyWindow *> &	m_WindowListRef;
	HyWindow *					m_pMouseWindow;
	glm::vec2					m_ptMousePos;
	uint32						m_uiMouseBtnFlags;
	uint32						m_uiMouseBtnFlags_NewlyPressed;
	uint32						m_uiMouseBtnFlags_Buffered;

	bool						m_bTouchScreenHack;	// Some touch screens do not send a 'MOUSE DOWN' message on initial touch, until you "drag" the cursor at least 1px or release the touch

	int32						m_JoystickList[HYNUM_JOYSTICK];
	uint32						m_uiJoystickCount;

public:
	HyInput(uint32 uiNumInputMappings, std::vector<HyWindow *> &windowListRef);
	~HyInput();

	glm::vec2 GetMousePos();
	glm::vec2 GetWorldMousePos();
	
	// All clicks are guaranteed to be down for at least '1' frame, even if it is released before the next update.
	// Clicks released and then pressed down again within one frame will not be detected.
	bool IsMouseBtnDown(HyMouseBtn eBtn);

	void StartRecording();
	void StopRecording();
	void SerializeRecording();

	void StartPlayback();
	void StopPlayback();

	// Some touch screens do not send a 'MOUSE DOWN' message on initial touch, until you "drag" the cursor at least 1px or release the touch
	// Enabling this hack will artificially send a 'MOUSE DOWN' message whenever the cursor position changes as touching/clicking is typically the only way to move the cursor
	void EnableTouchScreenHack(bool bEnable);

private:
	void Update();
	HyInputMap *GetInputMapArray();
	void InitCallbacks();

#ifdef HY_PLATFORM_DESKTOP
	friend void glfw_MouseButtonCallback(GLFWwindow *pWindow, int32 iButton, int32 iAction, int32 iMods);
	friend void glfw_CursorPosCallback(GLFWwindow *pWindow, double dX, double dY);
	friend void glfw_ScrollCallback(GLFWwindow *pWindow, double dX, double dY);
	friend void glfw_KeyCallback(GLFWwindow *pWindow, int32 iKey, int32 iScancode, int32 iAction, int32 iMods);
	friend void glfw_CharCallback(GLFWwindow *pWindow, uint32 uiCodepoint);
	friend void glfw_CharModsCallback(GLFWwindow *pWindow, uint32 uiCodepoint, int32 iMods);
	friend void glfw_JoystickCallback(int32 iJoyId, int32 iEvent);

	void OnGlfwKey(int32 iKey, int32 iAction);
#endif
};

#endif /* HyInput_h__ */
