///**************************************************************************
// *	HyInput.cpp
// *	
// *	Harmony Engine
// *	Copyright (c) 2013 Jason Knobler
// *
// *	The zlib License (zlib)
// *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
// *************************************************************************/
#include "Afx/HyInteropAfx.h"
#include "HyEngine.h"
#include "Input/HyInput.h"
#include "Input/HyInputMap.h"
#include "Renderer/Components/HyWindow.h"

#ifdef HY_PLATFORM_DESKTOP
	void glfw_MouseButtonCallback(GLFWwindow *pWindow, int32 iButton, int32 iAction, int32 iMods)
	{
		HyInput &inputRef = Hy_Input();
		
		inputRef.m_pMouseWindow = reinterpret_cast<HyWindow *>(glfwGetWindowUserPointer(pWindow));

		if(iAction == GLFW_PRESS)
			inputRef.m_uiMouseBtnFlags |= (1 << iButton);
		else // GLFW_RELEASE
			inputRef.m_uiMouseBtnFlags &= ~(1 << iButton);
	}

	void glfw_CursorPosCallback(GLFWwindow *pWindow, double dX, double dY)
	{
		Hy_Input().m_pMouseWindow = reinterpret_cast<HyWindow *>(glfwGetWindowUserPointer(pWindow));
		Hy_Input().m_ptMousePos.x = static_cast<float>(dX);
		Hy_Input().m_ptMousePos.y = static_cast<float>(dY);
	}

	void glfw_ScrollCallback(GLFWwindow *pWindow, double dX, double dY)
	{
	}

	void glfw_KeyCallback(GLFWwindow *pWindow, int32 iKey, int32 iScancode, int32 iAction, int32 iMods)
	{
	}

	void glfw_CharCallback(GLFWwindow *pWindow, uint32 uiCodepoint)
	{
	}

	void glfw_CharModsCallback(GLFWwindow *pWindow, uint32 uiCodepoint, int32 iMods)
	{
	}
#endif

HyInput::HyInput(uint32 uiNumInputMappings, std::vector<HyWindow *> &windowListRef) :	m_uiNUM_INPUT_MAPS(uiNumInputMappings),
																						m_WindowListRef(windowListRef),
																						m_uiMouseBtnFlags(0)
{
	m_pInputMaps = reinterpret_cast<HyInputMap *>(HY_NEW unsigned char[sizeof(HyInputMap) * m_uiNUM_INPUT_MAPS]);

	HyInputMap *pWriteLoc = static_cast<HyInputMap *>(m_pInputMaps);
	for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i, ++pWriteLoc)
		new (pWriteLoc)HyInputMap();

	HyAssert(m_WindowListRef.empty() == false, "HyInput::HyInput has a window list that is empty");
	m_pMouseWindow = m_WindowListRef[0];

#ifdef HY_PLATFORM_DESKTOP
	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
	{
		glfwSetMouseButtonCallback(m_WindowListRef[i]->GetHandle(), glfw_MouseButtonCallback);
		glfwSetCursorPosCallback(m_WindowListRef[i]->GetHandle(), glfw_CursorPosCallback);
		glfwSetScrollCallback(m_WindowListRef[i]->GetHandle(), glfw_ScrollCallback);
		glfwSetKeyCallback(m_WindowListRef[i]->GetHandle(), glfw_KeyCallback);
		glfwSetCharCallback(m_WindowListRef[i]->GetHandle(), glfw_CharCallback);
		glfwSetCharModsCallback(m_WindowListRef[i]->GetHandle(), glfw_CharModsCallback);
	}
#endif
}

/*virtual*/ HyInput::~HyInput()
{
	for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i)
		static_cast<HyInputMap *>(m_pInputMaps)[i].~HyInputMap();

	unsigned char *pMemBuffer = reinterpret_cast<unsigned char *>(m_pInputMaps);
	delete[] pMemBuffer;
}

glm::vec2 HyInput::GetMousePos()
{
	return m_ptMousePos;
}

glm::vec2 HyInput::GetWorldMousePos()
{
	return m_pMouseWindow->ConvertViewportCoordinateToWorldPos(m_ptMousePos);
}

bool HyInput::IsMouseBtnDown(HyMouseBtn eBtn)
{
	return 0 != (m_uiMouseBtnFlags & (1 << eBtn));
}

void HyInput::StartRecording()
{
}

void HyInput::StopRecording()
{
}

void HyInput::SerializeRecording()
{
}

void HyInput::StartPlayback()
{
}

void HyInput::StopPlayback()
{
}

void HyInput::Update()
{
	for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i)
		m_pInputMaps[i].Update();
}

HyInputMap *HyInput::GetInputMapArray()
{
	return m_pInputMaps;
}
