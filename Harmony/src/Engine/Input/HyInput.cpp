///**************************************************************************
// *	HyInput.cpp
// *	
// *	Harmony Engine
// *	Copyright (c) 2013 Jason Knobler
// *
// *	Harmony License:
// *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
// *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Afx/HyInteropAfx.h"
#include "HyEngine.h"
#include "Input/HyInput.h"
#include "Input/HyInputMap.h"
#include "Window/HyWindow.h"

#ifdef HY_PLATFORM_DESKTOP
	void glfw_MouseButtonCallback(GLFWwindow *pWindow, int32 iButton, int32 iAction, int32 iMods)
	{
		HyInput &inputRef = Hy_Input();
		
		inputRef.m_pMouseWindow = reinterpret_cast<HyWindow *>(glfwGetWindowUserPointer(pWindow));

		if(iAction == GLFW_PRESS)
		{
			inputRef.m_uiMouseBtnFlags |= (1 << iButton);
			inputRef.m_uiMouseBtnFlags_NewlyPressed |= (1 << iButton);
		}
		else // GLFW_RELEASE
			inputRef.m_uiMouseBtnFlags &= ~(1 << iButton);
	}

	void glfw_CursorPosCallback(GLFWwindow *pWindow, double dX, double dY)
	{
		HyInput &inputRef = Hy_Input();

		inputRef.m_pMouseWindow = reinterpret_cast<HyWindow *>(glfwGetWindowUserPointer(pWindow));
		inputRef.m_ptMousePos.x = static_cast<float>(dX);
		inputRef.m_ptMousePos.y = static_cast<float>(dY);

		if(inputRef.m_bTouchScreenHack)
			glfw_MouseButtonCallback(pWindow, HYMOUSE_BtnLeft, GLFW_PRESS, 0);
	}

	void glfw_ScrollCallback(GLFWwindow *pWindow, double dX, double dY)
	{
	}

	void glfw_KeyCallback(GLFWwindow *pWindow, int32 iKey, int32 iScancode, int32 iAction, int32 iMods)
	{
		Hy_Input().OnGlfwKey(iKey, iAction);
	}

	void glfw_CharCallback(GLFWwindow *pWindow, uint32 uiCodepoint)
	{
	}

	void glfw_CharModsCallback(GLFWwindow *pWindow, uint32 uiCodepoint, int32 iMods)
	{
	}

	void glfw_JoystickCallback(int32 iJoyId, int32 iEvent)
	{
		HyInput &inputRef = Hy_Input();

		if(iEvent == GLFW_CONNECTED)
		{
			int32 iAxisCount, iButtonCount;
			glfwGetJoystickAxes(iJoyId, &iAxisCount);
			glfwGetJoystickButtons(iJoyId, &iButtonCount);

			HyLog("Found joystick " << iJoyId + 1 << " named \"" << glfwGetJoystickName(iJoyId) << "\" with " << iAxisCount << " axes, " << iButtonCount <<" buttons");
			inputRef.m_JoystickList[inputRef.m_uiJoystickCount++] = iJoyId;
		}
		else // GLFW_DISCONNECTED
		{
			uint32 i = 0;
			for(; i < inputRef.m_uiJoystickCount; ++i)
			{
				if(inputRef.m_JoystickList[i] == iJoyId)
					break;
			}

			for (i = i + 1; i < inputRef.m_uiJoystickCount; ++i)
				inputRef.m_JoystickList[i - 1] = inputRef.m_JoystickList[i];

			HyLog("Lost joystick " << iJoyId + 1);
			inputRef.m_uiJoystickCount--;
		}
	}
#endif

HyInput::HyInput(uint32 uiNumInputMappings, std::vector<HyWindow *> &windowListRef) :	m_uiNUM_INPUT_MAPS(uiNumInputMappings),
																						m_WindowListRef(windowListRef),
																						m_uiMouseBtnFlags(0),
																						m_uiMouseBtnFlags_NewlyPressed(0),
																						m_uiMouseBtnFlags_Buffered(0),
																						m_bTouchScreenHack(false),
																						m_uiJoystickCount(0)
{
	memset(m_JoystickList, 0, sizeof(int32) * HYNUM_JOYSTICK);

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

	//for(int32 i = HYJOYSTICK_0; i < HYNUM_JOYSTICK; ++i)
	//{
	//	if(glfwJoystickPresent(i))
	//		glfw_JoystickCallback(i, GLFW_CONNECTED);
	//}
	glfwSetJoystickCallback(glfw_JoystickCallback);
#endif
}

/*virtual*/ HyInput::~HyInput()
{
	for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i)
		static_cast<HyInputMap *>(m_pInputMaps)[i].~HyInputMap();

	unsigned char *pMemBuffer = reinterpret_cast<unsigned char *>(m_pInputMaps);
	delete[] pMemBuffer;
}

bool HyInput::IsMouseBtnDown(HyMouseBtn eBtn) const
{
	return 0 != ((m_uiMouseBtnFlags | m_uiMouseBtnFlags_Buffered) & (1 << eBtn));
}

uint32 HyInput::GetMouseWindowIndex() const
{
	return m_pMouseWindow->GetIndex();
}

glm::vec2 HyInput::GetMousePos() const
{
	return glm::vec2(m_ptMousePos.x, m_pMouseWindow->GetWindowSize().y - m_ptMousePos.y); // Y-axis goes up in harmony, so flip it
}

glm::vec2 HyInput::GetWorldMousePos() const
{
	return m_pMouseWindow->ConvertViewportCoordinateToWorldPos(m_ptMousePos);
}

void HyInput::SetActionCategory(int32 iActionId, uint8 uiCategory, uint32 uiMappingIndex /*= 0*/)
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified");
	m_pInputMaps[uiMappingIndex].SetActionCategory(iActionId, uiCategory);
}

int32 HyInput::MapBtn(int32 iActionId, HyKeyboardBtn eBtn, uint32 uiMappingIndex /*= 0*/)
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified");
	return m_pInputMaps[uiMappingIndex].MapBtn(iActionId, eBtn);
}

int32 HyInput::MapBtn(int32 iActionId, HyMouseBtn eBtn, uint32 uiMappingIndex /*= 0*/)
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified");
	return m_pInputMaps[uiMappingIndex].MapBtn(iActionId, eBtn);
}

int32 HyInput::MapAlternativeBtn(int32 iActionId, HyKeyboardBtn eBtn, uint32 uiMappingIndex /*= 0*/)
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified");
	return m_pInputMaps[uiMappingIndex].MapAlternativeBtn(iActionId, eBtn);
}

int32 HyInput::MapAlternativeBtn(int32 iActionId, HyMouseBtn eBtn, uint32 uiMappingIndex /*= 0*/)
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified");
	return m_pInputMaps[uiMappingIndex].MapAlternativeBtn(iActionId, eBtn);
}

bool HyInput::MapJoystickBtn(int32 iActionId, HyGamePadBtn eBtn, uint32 uiJoystickIndex, uint32 uiMappingIndex /*= 0*/)
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified");
	return m_pInputMaps[uiMappingIndex].MapJoystickBtn(iActionId, eBtn, uiJoystickIndex);
}

bool HyInput::MapJoystickAxis(int32 iUserId, HyGamePadBtn eAxis, float fMin /*= 0.0f*/, float fMax /*= 1.0f*/, uint32 uiMappingIndex /*= 0*/)
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified");
	return m_pInputMaps[uiMappingIndex].MapJoystickAxis(iUserId, eAxis, fMin, fMax);
}

bool HyInput::Unmap(int32 iActionId, uint32 uiMappingIndex /*= 0*/)
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified");
	return m_pInputMaps[uiMappingIndex].Unmap(iActionId);
}

bool HyInput::IsMapped(int32 iActionId, uint32 uiMappingIndex /*= 0*/) const
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified");
	return m_pInputMaps[uiMappingIndex].IsMapped(iActionId);
}

bool HyInput::IsActionDown(int32 iUserId, uint32 uiMappingIndex /*= 0*/) const
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified");
	return m_pInputMaps[uiMappingIndex].IsActionDown(iUserId);
}

bool HyInput::IsActionReleased(int32 iUserId, uint32 uiMappingIndex /*= 0*/) const
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified");
	return m_pInputMaps[uiMappingIndex].IsActionReleased(iUserId);
}

float HyInput::GetAxis(int32 iUserId, uint32 uiMappingIndex /*= 0*/) const
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified");
	return m_pInputMaps[uiMappingIndex].GetAxis(iUserId);
}

float HyInput::GetAxisDelta(int32 iUserId, uint32 uiMappingIndex /*= 0*/) const
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified");
	return m_pInputMaps[uiMappingIndex].GetAxisDelta(iUserId);
}

void HyInput::RecordingStart()
{
}

void HyInput::RecordingStop()
{
}

void HyInput::PlaybackStart()
{
}

void HyInput::PlaybackStop()
{
}

void HyInput::EnableTouchScreenHack(bool bEnable)
{
	m_bTouchScreenHack = bEnable;
}

void HyInput::Update()
{
	m_uiMouseBtnFlags_Buffered = (m_uiMouseBtnFlags ^ m_uiMouseBtnFlags_NewlyPressed);
	m_uiMouseBtnFlags_NewlyPressed = 0;
	
	for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i)
		m_pInputMaps[i].Update();
}

#ifdef HY_PLATFORM_DESKTOP
void HyInput::OnGlfwKey(int32 iKey, int32 iAction)
{
	for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i)
		m_pInputMaps[i].ApplyInput(iKey, static_cast<HyBtnPressState>(iAction));
}
#endif
