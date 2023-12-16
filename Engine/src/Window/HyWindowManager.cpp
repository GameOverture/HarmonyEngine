/**************************************************************************
 *	HyWindowManager.cpp
 *
 *	Harmony Engine
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Afx/HyInteropAfx.h"
#include "Window/HyWindowManager.h"
#include "HyEngine.h"

#ifdef HY_USE_GLFW
void HyGlfw_ErrorCallback(int iError, const char *szDescription)
{
	HyLogError("GLFW Error " << iError << ": " << szDescription);
}
#endif

HyWindowManager::HyWindowManager(HyEngine &engineRef, uint32 uiNumWindows, bool bShowCursor, const HyWindowInfo windowInfos[HY_MAXWINDOWS]) :
	m_EngineRef(engineRef)
{
#ifdef HY_USE_GLFW
	// Setup error callback before glfwInit to catch anything that might go wrong with glfwInit
	glfwSetErrorCallback(HyGlfw_ErrorCallback);

	glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE);
	if(glfwInit() == GLFW_FALSE)
		HyLogError("glfwInit failed");
#endif

#ifdef HY_USE_SDL2
	uint32 uiInitFlags = SDL_INIT_GAMECONTROLLER;
	#ifdef HY_USE_SDL2_AUDIO
		uiInitFlags |= SDL_INIT_AUDIO;
	#endif
	#ifdef HY_USE_GLFW
		if(SDL_Init(uiInitFlags) != 0)
			HyLogError(SDL_GetError());
	#else
		uiInitFlags |= SDL_INIT_VIDEO;
		if(SDL_Init(uiInitFlags) != 0)
			HyLogError(SDL_GetError());

		SDL_ShowCursor(bShowCursor ? SDL_ENABLE : SDL_DISABLE);
	#endif
#endif

	HyAssert(uiNumWindows >= 1, "HyWindowManager was constructed with 0 windows");
	HyLog("HyWindowManager creating '" << uiNumWindows << "' window(s)");
	for(uint32 i = 0; i < uiNumWindows; ++i)
		m_WindowList.push_back(HY_NEW HyWindow(i, *this, windowInfos[i], bShowCursor, i != 0 ? m_WindowList[0]->GetInterop() : nullptr));
}

HyWindowManager::~HyWindowManager()
{
	for(uint32 i = 0; i < static_cast<uint32>(m_WindowList.size()); ++i)
		delete m_WindowList[i];
}

std::vector<HyWindow*>& HyWindowManager::GetWindowList()
{
	return m_WindowList;
}

HyWindow& HyWindowManager::GetWindow(uint32 uiWindowIndex)
{
	HyAssert(uiWindowIndex < m_WindowList.size(), "HyWindowManager::GetWindow was passed an invalid index");
	return *m_WindowList[uiWindowIndex];
}

bool HyWindowManager::HasFocus() const
{
	for(auto pWindow : m_WindowList)
	{
		if(pWindow->HasFocus())
			return true;
	}
	return false;
}

void HyWindowManager::DoWindowResized(HyWindow &windowRef)
{
	m_EngineRef.OnWindowResized(windowRef);
}

void HyWindowManager::DoWindowMoved(HyWindow &windowRef)
{
	m_EngineRef.OnWindowMoved(windowRef);
}

#if defined(HY_USE_SDL2) && !defined(HY_USE_GLFW)
HyWindow *HyWindowManager::DoEvent(const SDL_Event& eventRef, HyInput &inputRef)
{
	for(auto pWindow : m_WindowList)
	{
		if(eventRef.window.windowID == pWindow->GetId())
		{
			pWindow->DoEvent(eventRef, inputRef);
			return pWindow;
		}
	}

	return nullptr;
}
#endif
