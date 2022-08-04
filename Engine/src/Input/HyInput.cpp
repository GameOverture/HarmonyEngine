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
#include "UI/HyUiContainer.h"

#include <codecvt>

#ifdef HY_USE_GLFW
/*friend*/ void HyGlfw_MouseButtonCallback(GLFWwindow *pWindow, int32 iButton, int32 iAction, int32 iMods)
	{
		HyInput &inputRef = HyEngine::Input();
		
		inputRef.m_pMouseWindow = reinterpret_cast<HyWindow *>(glfwGetWindowUserPointer(pWindow));

		if(iAction == GLFW_PRESS)
		{
			inputRef.m_uiMouseBtnFlags |= (1 << iButton);
			inputRef.m_uiMouseBtnFlags_NewlyPressed |= (1 << iButton);
		}
		else // GLFW_RELEASE
			inputRef.m_uiMouseBtnFlags &= ~(1 << iButton);
	}

	/*friend*/ void HyGlfw_CursorPosCallback(GLFWwindow *pWindow, double dX, double dY)
	{
		HyInput &inputRef = HyEngine::Input();

		inputRef.m_pMouseWindow = reinterpret_cast<HyWindow *>(glfwGetWindowUserPointer(pWindow));
		inputRef.m_ptMousePos.x = static_cast<float>(dX);
		inputRef.m_ptMousePos.y = static_cast<float>(dY);

		//if(inputRef.m_bTouchScreenHack)
		//	HyGlfw_MouseButtonCallback(pWindow, HYMOUSE_BtnLeft, GLFW_PRESS, 0);
	}

	/*friend*/ void HyGlfw_ScrollCallback(GLFWwindow *pWindow, double dX, double dY)
	{
		HyInput &inputRef = HyEngine::Input();

		inputRef.m_vMouseScroll_LiveCount.x += static_cast<int32>(dX);
		inputRef.m_vMouseScroll_LiveCount.y += static_cast<int32>(dY);
	}

	/*friend*/ void HyGlfw_KeyCallback(GLFWwindow *pWindow, int32 iKey, int32 iScancode, int32 iAction, int32 iMods)
	{
		HyEngine::Input().OnGlfwKey(iKey, iAction);
	}

	/*friend*/ void HyGlfw_CharCallback(GLFWwindow *pWindow, uint32 uiCodepoint)
	{
		//// the UTF-8 - UTF-32 standard conversion facet
		//std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;

		//// UTF-32 to UTF-8
		//std::u32string sUtf32;//(uiCodepoint); // TODO: fix this
		//std::string sUtf8 = cvt.to_bytes(sUtf32);

		if(uiCodepoint >= 128)
			HyLogWarning("HyGlfw_CharCallback needs proper conversion!");

		std::string sUtf8;
		sUtf8 += static_cast<char>(uiCodepoint);
		
		HyEngine::Input().DistrubuteTextInput(sUtf8);
	}

	/*friend*/ void HyGlfw_JoystickCallback(int32 iJoyId, int32 iEvent)
	{
		HyInput &inputRef = HyEngine::Input();

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

HyInput::HyInput(uint32 uiNumInputMappings, std::vector<HyWindow *> &windowListRef) :
	m_uiNUM_INPUT_MAPS(uiNumInputMappings),
	m_WindowListRef(windowListRef),
	m_uiMouseBtnFlags(0),
	m_uiMouseBtnFlags_NewlyPressed(0),
	m_uiMouseBtnFlags_Buffered(0),
	m_bTextInputActive(false),
	m_bTouchScreen(false),
	m_uiJoystickCount(0)
{
	memset(m_JoystickList, 0, sizeof(int32) * HYNUM_JOYSTICK);

	m_pInputMaps = reinterpret_cast<HyInputMap *>(HY_NEW unsigned char[sizeof(HyInputMap) * m_uiNUM_INPUT_MAPS]);

	HyInputMap *pWriteLoc = static_cast<HyInputMap *>(m_pInputMaps);
	for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i, ++pWriteLoc)
		new (pWriteLoc)HyInputMap();

	HyAssert(m_WindowListRef.empty() == false, "HyInput::HyInput has a window list that is empty");
	m_pMouseWindow = m_WindowListRef[0];

#ifdef HY_USE_GLFW
	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
	{
		glfwSetMouseButtonCallback(m_WindowListRef[i]->GetInterop(), HyGlfw_MouseButtonCallback);
		glfwSetCursorPosCallback(m_WindowListRef[i]->GetInterop(), HyGlfw_CursorPosCallback);
		glfwSetScrollCallback(m_WindowListRef[i]->GetInterop(), HyGlfw_ScrollCallback);
		glfwSetKeyCallback(m_WindowListRef[i]->GetInterop(), HyGlfw_KeyCallback);
		glfwSetCharCallback(m_WindowListRef[i]->GetInterop(), HyGlfw_CharCallback);
	}
	glfwSetJoystickCallback(HyGlfw_JoystickCallback);
#endif

	StopTextInput();
}

/*virtual*/ HyInput::~HyInput()
{
	for(auto iter = m_LoadedCursorsMap.begin(); iter != m_LoadedCursorsMap.end(); ++iter)
	{
#if defined(HY_USE_GLFW)
		glfwDestroyCursor(iter->second);
#elif defined(HY_USE_SDL2)
		SDL_FreeCursor(iter->second);
#endif
	}

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
	return glm::vec2(m_ptMousePos.x, m_pMouseWindow->GetWindowSize().y - m_ptMousePos.y); // Y-axis goes up in Harmony, so inverse it
}

bool HyInput::GetWorldMousePos(glm::vec2 &ptWorldPosOut) const
{
	return m_pMouseWindow->ProjectToWorldPos2d(GetMousePos(), ptWorldPosOut);
}

glm::ivec2 HyInput::GetMouseScroll() const
{
	return m_vMouseScroll_ThisFrame;
}

void HyInput::SetMouseCursor(HyMouseCursor eCursor)
{
	if(m_LoadedCursorsMap.find(eCursor) == m_LoadedCursorsMap.end())
	{
#if defined(HY_USE_GLFW)
		m_LoadedCursorsMap[eCursor] = glfwCreateStandardCursor(static_cast<int>(eCursor));
#elif defined(HY_USE_SDL2)
		m_LoadedCursorsMap[eCursor] = SDL_CreateSystemCursor(static_cast<SDL_SystemCursor>(eCursor));
#endif
	}

#if defined(HY_USE_GLFW)
	glfwSetCursor(m_WindowListRef[0]->GetInterop(), m_LoadedCursorsMap[eCursor]);
#elif defined(HY_USE_SDL2)
	SDL_SetCursor(m_LoadedCursorsMap[eCursor]);
#endif

	m_bCursorWasSet = true;
}

void HyInput::ResetMouseCursor()
{
	if(m_bCursorWasSet)
		return;

#if defined(HY_USE_GLFW)
	glfwSetCursor(m_WindowListRef[0]->GetInterop(), nullptr);
#elif defined(HY_USE_SDL2)
	SetMouseCursor(HYMOUSECURSOR_Arrow);
#endif
}

void HyInput::SetActionCategory(int32 iActionId, uint8 uiCategory, uint32 uiMappingIndex /*= 0*/)
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified while max is: " << m_uiNUM_INPUT_MAPS);
	m_pInputMaps[uiMappingIndex].SetActionCategory(iActionId, uiCategory);
}

int32 HyInput::MapBtn(int32 iActionId, HyKeyboardBtn eBtn, uint32 uiMappingIndex /*= 0*/)
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified while max is: " << m_uiNUM_INPUT_MAPS);
	return m_pInputMaps[uiMappingIndex].MapBtn(iActionId, eBtn);
}

int32 HyInput::MapBtn(int32 iActionId, HyMouseBtn eBtn, uint32 uiMappingIndex /*= 0*/)
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified while max is: " << m_uiNUM_INPUT_MAPS);
	return m_pInputMaps[uiMappingIndex].MapBtn(iActionId, eBtn);
}

int32 HyInput::MapAlternativeBtn(int32 iActionId, HyKeyboardBtn eBtn, uint32 uiMappingIndex /*= 0*/)
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified while max is: " << m_uiNUM_INPUT_MAPS);
	return m_pInputMaps[uiMappingIndex].MapAlternativeBtn(iActionId, eBtn);
}

int32 HyInput::MapAlternativeBtn(int32 iActionId, HyMouseBtn eBtn, uint32 uiMappingIndex /*= 0*/)
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified while max is: " << m_uiNUM_INPUT_MAPS);
	return m_pInputMaps[uiMappingIndex].MapAlternativeBtn(iActionId, eBtn);
}

bool HyInput::MapGamePadBtn(int32 iActionId, HyGamePadBtn eBtn, uint32 uiMappingIndex /*= 0*/)
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified while max is: " << m_uiNUM_INPUT_MAPS);

#ifdef HY_USE_GLFW
#elif defined(HY_USE_SDL2)
	SDL_GameControllerOpen(uiMappingIndex);
#endif

	return m_pInputMaps[uiMappingIndex].MapPadBtn(iActionId, eBtn);
}

bool HyInput::Unmap(int32 iActionId, uint32 uiMappingIndex /*= 0*/)
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified while max is: " << m_uiNUM_INPUT_MAPS);
	return m_pInputMaps[uiMappingIndex].Unmap(iActionId);
}

bool HyInput::IsMapped(int32 iActionId, uint32 uiMappingIndex /*= 0*/) const
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified while max is: " << m_uiNUM_INPUT_MAPS);
	return m_pInputMaps[uiMappingIndex].IsMapped(iActionId);
}

bool HyInput::IsActionDown(int32 iUserId, uint32 uiMappingIndex /*= 0*/) const
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified while max is: " << m_uiNUM_INPUT_MAPS);
	return m_pInputMaps[uiMappingIndex].IsActionDown(iUserId);
}

bool HyInput::IsActionReleased(int32 iUserId, uint32 uiMappingIndex /*= 0*/) const
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified while max is: " << m_uiNUM_INPUT_MAPS);
	return m_pInputMaps[uiMappingIndex].IsActionReleased(iUserId);
}

float HyInput::GetGamePadAxis(HyGamePadAxis eAxis, uint32 uiMappingIndex /*= 0*/) const
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified while max is: " << m_uiNUM_INPUT_MAPS);
	return m_pInputMaps[uiMappingIndex].GetAxis(eAxis);
}

float HyInput::GetGamePadAxisDelta(HyGamePadAxis eAxis, uint32 uiMappingIndex /*= 0*/) const
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified while max is: " << m_uiNUM_INPUT_MAPS);
	return m_pInputMaps[uiMappingIndex].GetAxisDelta(eAxis);
}

bool HyInput::IsTextInputActive()
{
#if defined(HY_USE_SDL2)
	return SDL_IsTextInputActive();
#else
	return m_bTextInputActive;
#endif
}

void HyInput::StartTextInput()
{
	m_bTextInputActive = true;
#if defined(HY_USE_SDL2)
	SDL_StartTextInput();
#endif
}

void HyInput::StopTextInput()
{
	m_bTextInputActive = false;
#if defined(HY_USE_SDL2)
	SDL_StopTextInput();
#endif
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

void HyInput::UsingTouchScreen(bool bEnable)
{
	m_bTouchScreen = bEnable;
}

bool HyInput::IsUsingTouchScreen() const
{
	return m_bTouchScreen;
}

#ifdef HY_USE_GLFW
	void HyInput::OnGlfwKey(int32 iKey, int32 iAction)
	{
		DistrubuteKeyboardInput(static_cast<HyKeyboardBtn>(iKey));

		for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i)
			m_pInputMaps[i].ApplyInput(iKey, static_cast<HyBtnPressState>(iAction));
	}
#elif defined(HY_USE_SDL2)
	void HyInput::DoKeyDownEvent(const SDL_Event &eventRef)
	{
		DistrubuteKeyboardInput(static_cast<HyKeyboardBtn>(eventRef.key.keysym.sym));

		for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i)
			m_pInputMaps[i].ApplyInput(eventRef.key.keysym.sym, HYBTN_Press);
	}

	void HyInput::DoKeyUpEvent(const SDL_Event &eventRef)
	{
		for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i)
			m_pInputMaps[i].ApplyInput(eventRef.key.keysym.sym, HYBTN_Release);
	}

	void HyInput::DoTextInputEvent(const SDL_Event &eventRef)
	{
		size_t uiTextLen = SDL_strlen(eventRef.text.text);
		if(uiTextLen == 0)// || eventRef.text.text[0] == '\n')// || markedRect.w < 0)
			return;

		DistrubuteTextInput(std::string(eventRef.text.text, uiTextLen));

		// After text inputed, we can clear up markedText because it is committed
		m_sTextComposition.clear();
	}

	void HyInput::DoTextEditEvent(const SDL_Event &eventRef)
	{
		m_sTextComposition = eventRef.edit.text;
		m_iTextCursorIndex = eventRef.edit.start;
		m_iTextSelectLength = eventRef.edit.length;

		// When does this return something interesting?
		HyLog("HyInput::DoTextEditEvent:" << m_sTextComposition << "[" << m_iTextCursorIndex << ", " << m_iTextSelectLength << "]");
	}

	void HyInput::DoMouseMoveEvent(const SDL_Event &eventRef)
	{
		m_ptMousePos.x = static_cast<float>(eventRef.motion.x);
		m_ptMousePos.y = static_cast<float>(eventRef.motion.y);
	}

	void HyInput::DoMouseDownEvent(const SDL_Event &eventRef)
	{
		m_uiMouseBtnFlags |= (1 << eventRef.button.button);
		m_uiMouseBtnFlags_NewlyPressed |= (1 << eventRef.button.button);
	}

	void HyInput::DoMouseUpEvent(const SDL_Event &eventRef)
	{
		m_uiMouseBtnFlags &= ~(1 << eventRef.button.button);
	}

	void HyInput::DoMouseWheelEvent(const SDL_Event &eventRef)
	{
		m_vMouseScroll_LiveCount.x += eventRef.wheel.x;
		m_vMouseScroll_LiveCount.y += eventRef.wheel.y;
	}

	void HyInput::SetMouseWindow(HyWindow *pWindow)
	{
		m_pMouseWindow = pWindow;
	}

	void HyInput::DoPadAxis(const SDL_Event &eventRef)
	{
		for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i)
			m_pInputMaps[i].ApplyPadAxis(eventRef.caxis.axis, static_cast<float>(eventRef.caxis.value) / static_cast<float>(SDL_JOYSTICK_AXIS_MAX));
	}

	void HyInput::DoPadBtnDown(const SDL_Event &eventRef)
	{
		for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i)
			m_pInputMaps[i].ApplyPadInput(static_cast<HyGamePadBtn>(eventRef.cbutton.button), HYBTN_Press);
	}

	void HyInput::DoPadBtnUp(const SDL_Event &eventRef)
	{
		for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i)
			m_pInputMaps[i].ApplyPadInput(static_cast<HyGamePadBtn>(eventRef.cbutton.button), HYBTN_Release);
	}

	void HyInput::DoTouchDownEvent(const SDL_Event &eventRef)
	{
		m_ptMousePos.x = eventRef.tfinger.x * m_pMouseWindow->GetWidthF();
		m_ptMousePos.y = eventRef.tfinger.y * m_pMouseWindow->GetHeightF();

		m_uiMouseBtnFlags |= (1 << SDL_BUTTON_LEFT);
		m_uiMouseBtnFlags_NewlyPressed |= (1 << SDL_BUTTON_LEFT);
	}

	void HyInput::DoTouchMoveEvent(const SDL_Event &eventRef)
	{
		m_ptMousePos.x = eventRef.tfinger.x * m_pMouseWindow->GetWidthF();
		m_ptMousePos.y = eventRef.tfinger.y * m_pMouseWindow->GetHeightF();
	}

	void HyInput::DoTouchUpEvent(const SDL_Event &eventRef)
	{
		m_ptMousePos.x = eventRef.tfinger.x * m_pMouseWindow->GetWidthF();
		m_ptMousePos.y = eventRef.tfinger.y * m_pMouseWindow->GetHeightF();

		m_uiMouseBtnFlags &= ~(1 << SDL_BUTTON_LEFT);
	}
#elif defined(HY_PLATFORM_GUI)
	void HyInput::SetWidgetMousePos(glm::vec2 ptMousePos)
	{
		m_ptMousePos = ptMousePos;
	}
#endif

void HyInput::DistrubuteTextInput(std::string sNewText)
{
	HyUiContainer::DistrubuteTextInput(sNewText);
}

void HyInput::DistrubuteKeyboardInput(HyKeyboardBtn eBtn)
{
	HyUiContainer::DistrubuteKeyboardInput(eBtn);
}

void HyInput::Update()
{
	m_bCursorWasSet = false;

	m_uiMouseBtnFlags_Buffered = (m_uiMouseBtnFlags ^ m_uiMouseBtnFlags_NewlyPressed);
	m_uiMouseBtnFlags_NewlyPressed = 0;
	
	for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i)
		m_pInputMaps[i].Update();

	m_vMouseScroll_ThisFrame = m_vMouseScroll_LiveCount;
	m_vMouseScroll_LiveCount.x = m_vMouseScroll_LiveCount.y = 0;
}
