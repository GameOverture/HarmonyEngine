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

public:
	HyInput(uint32 uiNumInputMappings, std::vector<HyWindow *> &windowListRef);
	~HyInput();

	glm::vec2 GetMousePos();
	glm::vec2 GetWorldMousePos();
	bool IsMouseBtnDown(HyMouseBtn eBtn);

	void StartRecording();
	void StopRecording();
	void SerializeRecording();

	void StartPlayback();
	void StopPlayback();

private:
	void Update();
	HyInputMap *GetInputMapArray();

	void ApplyKeyToInputMaps(

#ifdef HY_PLATFORM_DESKTOP
	friend void glfw_MouseButtonCallback(GLFWwindow *pWindow, int32 iButton, int32 iAction, int32 iMods);
	friend void glfw_CursorPosCallback(GLFWwindow *pWindow, double dX, double dY);
	friend void glfw_ScrollCallback(GLFWwindow *pWindow, double dX, double dY);
	friend void glfw_KeyCallback(GLFWwindow *pWindow, int32 iKey, int32 iScancode, int32 iAction, int32 iMods);
	friend void glfw_CharCallback(GLFWwindow *pWindow, uint32 uiCodepoint);
	friend void glfw_CharModsCallback(GLFWwindow *pWindow, uint32 uiCodepoint, int32 iMods);
#endif
};

#endif /* HyInput_h__ */
