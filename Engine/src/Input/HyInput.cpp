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
#include "Input/IHyController.h"
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

		if(inputRef.m_bTouchActive == false)
		{
			inputRef.m_pMouseWindow = reinterpret_cast<HyWindow *>(glfwGetWindowUserPointer(pWindow));
			inputRef.m_ptMousePos.x = static_cast<float>(dX);
			inputRef.m_ptMousePos.y = static_cast<float>(dY);
		}

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

		//if(iEvent == GLFW_CONNECTED)
		//{
		//	inputRef.AllocateController(iJoyId, glfwJoystickIsGamepad(iJoyId));
		//}
		//else // GLFW_DISCONNECTED
		//{
		//	for(uint32 i = 0; i < static_cast<uint32>(inputRef.m_GamePadList.size()); ++i)
		//	{
		//		if(inputRef.m_GamePadList[i]->m_iId == iJoyId)
		//		{
		//			HyLog("Lost GamePad controller named \"" << inputRef.m_GamePadList[i]->GetName() << "\"");

		//			while(inputRef.m_GamePadList[i]->m_iRefCount > 0)
		//			{
		//				asdf;
		//			}

		//			delete inputRef.m_GamePadList[i];
		//			inputRef.m_GamePadList.erase(inputRef.m_GamePadList.begin() + i);
		//			break;
		//		}
		//	}
		//	for(uint32 i = 0; i < static_cast<uint32>(inputRef.m_JoystickList.size()); ++i)
		//	{
		//		if(inputRef.m_JoystickList[i]->m_iId == iJoyId)
		//		{
		//			HyLog("Lost Joystick controller named \"" << inputRef.m_JoystickList[i]->GetName() << "\"");

		//			while(inputRef.m_JoystickList[i]->m_iRefCount > 0)
		//			{
		//				asdf;
		//			}

		//			delete inputRef.m_JoystickList[i];
		//			inputRef.m_JoystickList.erase(inputRef.m_JoystickList.begin() + i);
		//			break;
		//		}
		//	}
		//}
	}
#endif

HyInput::HyInput(uint32 uiNumInputMappings, std::vector<HyWindow *> &windowListRef) :
	m_uiNUM_INPUT_MAPS(uiNumInputMappings),
	m_uiUserInputOccured(0),
	m_WindowListRef(windowListRef),
	m_uiMouseBtnFlags(0),
	m_uiMouseBtnFlags_NewlyPressed(0),
	m_uiMouseBtnFlags_Buffered(0),
	m_bTextInputActive(false),
	m_bTouchScreen(false),
	m_bTouchActive(false),
	m_iTouchId(0),
	m_bControllerBackgroundInputEnabled(false)
{
	m_pInputMaps = reinterpret_cast<HyInputMap *>(HY_NEW unsigned char[sizeof(HyInputMap) * m_uiNUM_INPUT_MAPS]);

	HyInputMap *pWriteLoc = static_cast<HyInputMap *>(m_pInputMaps);
	for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i, ++pWriteLoc)
		new (pWriteLoc)HyInputMap();

	HyAssert(m_WindowListRef.empty() == false, "HyInput::HyInput has a window list that is empty");
	m_pMouseWindow = m_WindowListRef[0];

#if defined(HY_USE_GLFW)
	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
	{
		glfwSetMouseButtonCallback(m_WindowListRef[i]->GetInterop(), HyGlfw_MouseButtonCallback);
		glfwSetCursorPosCallback(m_WindowListRef[i]->GetInterop(), HyGlfw_CursorPosCallback);
		glfwSetScrollCallback(m_WindowListRef[i]->GetInterop(), HyGlfw_ScrollCallback);
		glfwSetKeyCallback(m_WindowListRef[i]->GetInterop(), HyGlfw_KeyCallback);
		glfwSetCharCallback(m_WindowListRef[i]->GetInterop(), HyGlfw_CharCallback);
	}

	glfwSetJoystickCallback(HyGlfw_JoystickCallback);
	for(int i = 0; i < GLFW_JOYSTICK_LAST; ++i)
	{
		if(glfwJoystickPresent(i))
			AllocateController(i, glfwJoystickIsGamepad(i) == GLFW_TRUE);
	}
#elif defined(HY_USE_SDL2)
	SDL_JoystickEventState(SDL_ENABLE);
	int iNumConnectedJoysticks = SDL_NumJoysticks();
	for(int i = 0; i < iNumConnectedJoysticks; ++i)
		AllocateController(i, SDL_IsGameController(i) == SDL_TRUE);
#endif

	// Try assigning controllers to input maps
	for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i)
	{
		if(m_GamePadList.size() > i)
			AssignGamePad(m_GamePadList[i], i);
		else
			break;
	}

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

bool HyInput::UserInputOccured() const
{
	return m_uiUserInputOccured != 0;
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

bool HyInput::IsControllerBackgroundInputEnabled() const
{
	return m_bControllerBackgroundInputEnabled;
}

void HyInput::SetControllerBackgroundInputEnabled(bool bEnable)
{
	m_bControllerBackgroundInputEnabled = bEnable;
#ifdef HY_USE_SDL2
	//SDL_JoystickEventState(m_bControllerBackgroundInputEnabled ? SDL_IGNORE : SDL_ENABLE);
	SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, m_bControllerBackgroundInputEnabled ? "1" : "0");
#endif
}

uint32 HyInput::GetNumGamePads() const
{
	return static_cast<uint32>(m_GamePadList.size());
}

HyGamePad *HyInput::GetGamePad(uint32 uiIndex) const
{
	if(uiIndex >= m_GamePadList.size())
	{
		HyLogWarning("HyInput::GetGamePad() - uiIndex out of range");
		return nullptr;
	}

	return m_GamePadList[uiIndex];
}

uint32 HyInput::GetNumJoysticks() const
{
	return static_cast<uint32>(m_JoystickList.size());
}

HyJoystick *HyInput::GetJoystick(uint32 uiIndex) const
{
	if(uiIndex >= m_JoystickList.size())
	{
		HyLogWarning("HyInput::GetJoystick() - uiIndex out of range");
		return nullptr;
	}

	return m_JoystickList[uiIndex];
}

void HyInput::AssignGamePad(HyGamePad *pGamePad, uint32 uiMappingIndex /*= 0*/)
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified while max is: " << m_uiNUM_INPUT_MAPS);
	
	if(pGamePad == nullptr)
	{
		RemoveGamePad(uiMappingIndex);
		return;
	}

	if(m_pInputMaps[uiMappingIndex].GetGamePad() == pGamePad) // Already assigned
		return;

#if defined(HY_USE_SDL2)
	if(pGamePad->m_iRefCount == 0)
	{
		// This GamePad hasn't been assigned yet, so we need to open it
		// Reacquire the device index using the instance id
		int iDeviceIndex = -1;
		int iNumConnectedJoysticks = SDL_NumJoysticks();
		for(int i = 0; i < iNumConnectedJoysticks; ++i)
		{
			if(SDL_JoystickGetDeviceInstanceID(i) == pGamePad->m_iId)
			{
				iDeviceIndex = i;
				break;
			}
		}
		if(iDeviceIndex == -1)
		{
			HyLogWarning("HyInput::AssignGamePad() - Could not find gamepad with instance id: " << pGamePad->m_iId);
			return;
		}
		
		HyAssert(SDL_IsGameController(iDeviceIndex), "HyInput::AssignGamePad() - Device at index " << iDeviceIndex << " is not a gamepad");
		pGamePad->m_pSdlGameController = SDL_GameControllerOpen(iDeviceIndex);
	}
#endif
	pGamePad->m_iRefCount++;

	m_pInputMaps[uiMappingIndex].AssignGamePad(pGamePad);
}

void HyInput::RemoveGamePad(uint32 uiMappingIndex /*= 0*/)
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified while max is: " << m_uiNUM_INPUT_MAPS);
	
	HyGamePad *pGamePad = m_pInputMaps[uiMappingIndex].GetGamePad();
	if(pGamePad == nullptr)
		return;

	pGamePad->m_iRefCount--;

#if defined(HY_USE_SDL2)
	// After decrementing the reference count, if it's zero then close the gamepad
	if(pGamePad->m_iRefCount == 0)
	{
		SDL_GameControllerClose(pGamePad->m_pSdlGameController);
		pGamePad->m_pSdlGameController = nullptr;
	}
#endif

	m_pInputMaps[uiMappingIndex].RemoveGamePad();
}

void HyInput::AssignJoystick(HyJoystick *pJoystick, uint32 uiMappingIndex /*= 0*/)
{
	if(pJoystick == nullptr)
		return;

	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified while max is: " << m_uiNUM_INPUT_MAPS);
	const std::vector<HyJoystick *> &joystickListRef = m_pInputMaps[uiMappingIndex].GetJoystickList();
	for(uint32 i = 0; i < joystickListRef.size(); ++i)
	{
		if(joystickListRef[i] == pJoystick) // Already assigned
			return;
	}

#if defined(HY_USE_SDL2)
	if(pJoystick->m_iRefCount == 0)
	{
		// This Joystick hasn't been assigned yet, so we need to open it
		// Reacquire the device index using the instance id
		int iDeviceIndex = -1;
		int iNumConnectedJoysticks = SDL_NumJoysticks();
		for(int i = 0; i < iNumConnectedJoysticks; ++i)
		{
			if(SDL_JoystickGetDeviceInstanceID(i) == pJoystick->m_iId)
			{
				iDeviceIndex = i;
				break;
			}
		}
		if(iDeviceIndex == -1)
		{
			HyLogWarning("HyInput::AssignJoystick() - Could not find joystick with instance id: " << pJoystick->m_iId);
			return;
		}

		HyAssert(SDL_IsGameController(iDeviceIndex) == false, "HyInput::AssignJoystick() - Device at index " << iDeviceIndex << " is a gamepad, not a joystick");
		pJoystick->m_pSdlJoystick = SDL_JoystickOpen(iDeviceIndex);
	}
#endif
	pJoystick->m_iRefCount++;

	m_pInputMaps[uiMappingIndex].AssignJoystick(pJoystick);
}

void HyInput::RemoveJoystick(HyJoystick *pJoystick, uint32 uiMappingIndex /*= 0*/)
{
	HyAssert(uiMappingIndex < m_uiNUM_INPUT_MAPS, "HyInput - Improper uiMappingIndex '" << uiMappingIndex << "' specified while max is: " << m_uiNUM_INPUT_MAPS);

	if(pJoystick == nullptr)
		return;

	// Make sure the joystick is assigned to this mapping
	const std::vector<HyJoystick *> &joystickListRef = m_pInputMaps[uiMappingIndex].GetJoystickList();
	bool bFound = false;
	for(uint32 i = 0; i < joystickListRef.size(); ++i)
	{
		if(joystickListRef[i] == pJoystick) // Already assigned
		{
			bFound = true;
			break;
		}
	}
	if(bFound == false)
		return;

	pJoystick->m_iRefCount--;

#if defined(HY_USE_SDL2)
	// After decrementing the reference count, if it's zero then close the gamepad
	if(pJoystick->m_iRefCount == 0)
	{
		SDL_JoystickClose(pJoystick->m_pSdlJoystick);
		pJoystick->m_pSdlJoystick = nullptr;
	}
#endif

	m_pInputMaps[uiMappingIndex].RemoveJoystick(pJoystick);
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
	if(m_pInputMaps[uiMappingIndex].GetGamePad() == nullptr)
		return 0.0f;

	return m_pInputMaps[uiMappingIndex].GetGamePad()->GetAxisValue(eAxis);
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

void HyInput::DoTouchStart(int32 iId, int32 iX, int32 iY)
{
	m_bTouchScreen = true;
	//if(/*m_bTouchScreen == false ||*/ m_bTouchActive)
	//	return;

	m_bTouchActive = true;
	m_iTouchId = iId;
	
	m_ptMousePos.x = static_cast<float>(iX);
	m_ptMousePos.y = static_cast<float>(iY);

	m_uiMouseBtnFlags |= (1 << HYMOUSE_BtnLeft);
	m_uiMouseBtnFlags_NewlyPressed |= (1 << HYMOUSE_BtnLeft);
}

void HyInput::DoTouchMove(int32 iId, int32 iX, int32 iY)
{
	if(m_iTouchId != iId)
		return;

	m_ptMousePos.x = static_cast<float>(iX);
	m_ptMousePos.y = static_cast<float>(iY);
}

void HyInput::DoTouchEnd(int32 iId, int32 iX, int32 iY)
{
	if(m_iTouchId != iId)
		return;

	m_bTouchActive = false;
	m_uiMouseBtnFlags &= ~(1 << HYMOUSE_BtnLeft);
}

void HyInput::DoTouchCancel(int32 iId)
{
	if(m_iTouchId != iId)
		return;

	m_bTouchActive = false;
	m_uiMouseBtnFlags &= ~(1 << HYMOUSE_BtnLeft);
}

void HyInput::AllocateController(int32 iDeviceIndex, bool bIsGamePad)
{
	if(bIsGamePad)
	{
		HyGamePad *pNewGamePad = HY_NEW HyGamePad(iDeviceIndex);
		HyLog("Allocated controller[" << iDeviceIndex << "] (Game Pad) named \"" << pNewGamePad->GetName() << "\"");
		m_GamePadList.push_back(pNewGamePad);
	}
	else
	{
		HyJoystick *pNewJoystick = HY_NEW HyJoystick(iDeviceIndex);
		HyLog("Allocated controller[" << iDeviceIndex << "] (Joystick) named \"" << pNewJoystick->GetName() << "\"");
		m_JoystickList.push_back(pNewJoystick);
	}
}

#ifdef HY_USE_GLFW
	void HyInput::OnGlfwKey(int32 iKey, int32 iAction)
	{
		DistrubuteKeyboardInput(static_cast<HyKeyboardBtn>(iKey));

		for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i)
			m_pInputMaps[i].ApplyKeyBoardInput(iKey, static_cast<HyBtnPressState>(iAction));
	}

	void HyInput::UpdateGlfwControllers()
	{
		GLFWgamepadstate gamePadState;
		for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i)
		{
			HyGamePad *pGamePad = m_pInputMaps[i].GetGamePad();
			if(pGamePad)
			{
				glfwGetGamepadState(pGamePad->m_iId, &gamePadState);

				for(int iBtn = 0; iBtn < HYNUM_GAMEPADBUTTONS; ++iBtn)
				{
					HyGamePadBtn eBtn = static_cast<HyGamePadBtn>(iBtn);
					if(pGamePad->GetButtonValue(eBtn) != gamePadState.buttons[eBtn])
						OnEventGamePadButton(pGamePad->m_iId, eBtn, gamePadState.buttons[eBtn] == GLFW_PRESS ? HYBTN_Press : HYBTN_Release);
				}

				pGamePad->UpdateGamePadState(gamePadState);
			}

			const std::vector<HyJoystick *> joystickListRef = m_pInputMaps[i].GetJoystickList();
			for(uint32 j = 0; j < joystickListRef.size(); ++j)
			{
				HyJoystick *pJoystick = joystickListRef[j];

				// TODO: Implement this!

				// Axes
				int iNumAxes = 0;
				const float *pAxesValues = glfwGetJoystickAxes(pJoystick->m_iId, &iNumAxes);
				
				// Buttons
				int iNumButtons = 0;
				const unsigned char *pButtonValues = glfwGetJoystickButtons(pJoystick->m_iId, &iNumButtons);

				// Hats
				int iNumHats = 0;
				const unsigned char *pHatValues = glfwGetJoystickHats(pJoystick->m_iId, &iNumHats);
			}
		}
	}
#elif defined(HY_USE_SDL2)
	void HyInput::DoKeyDownEvent(const SDL_Event &eventRef)
	{
		DistrubuteKeyboardInput(static_cast<HyKeyboardBtn>(eventRef.key.keysym.sym));

		for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i)
			m_pInputMaps[i].ApplyKeyBoardInput(eventRef.key.keysym.sym, HYBTN_Press);
	}

	void HyInput::DoKeyUpEvent(const SDL_Event &eventRef)
	{
		for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i)
			m_pInputMaps[i].ApplyKeyBoardInput(eventRef.key.keysym.sym, HYBTN_Release);
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
		if(m_bTouchActive == false)
		{
			m_ptMousePos.x = static_cast<float>(eventRef.motion.x);
			m_ptMousePos.y = static_cast<float>(eventRef.motion.y);
		}
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

void HyInput::OnEventGamePadAxis(int32 iGamePadId, HyGamePadAxis eAxis, float fAxisValue)
{
	for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i)
	{
		if(m_pInputMaps[i].GetGamePad() && iGamePadId == m_pInputMaps[i].GetGamePad()->m_iId)
			m_pInputMaps[i].GetGamePad()->SetAxisValue(eAxis, fAxisValue);
	}
}

void HyInput::OnEventGamePadButton(int32 iGamePadId, HyGamePadBtn eButtonType, HyBtnPressState ePressState)
{
	for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i)
	{
		if(m_pInputMaps[i].GetGamePad() && iGamePadId == m_pInputMaps[i].GetGamePad()->m_iId)
			m_pInputMaps[i].ApplyGamePadButton(eButtonType, ePressState);
	}
}

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

	m_uiUserInputOccured |= m_uiMouseBtnFlags_Buffered;
	
	for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i)
		m_pInputMaps[i].Update();

	m_vMouseScroll_ThisFrame = m_vMouseScroll_LiveCount;
	m_vMouseScroll_LiveCount.x = m_vMouseScroll_LiveCount.y = 0;
}
