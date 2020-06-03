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

HyWindowManager::HyWindowManager(uint32 uiNumWindows, bool bShowCursor, const HyWindowInfo windowInfos[HY_MAXWINDOWS])
{
#ifdef HY_USE_SDL2
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
		HyLogError(SDL_GetError());

	SDL_ShowCursor(bShowCursor ? SDL_ENABLE : SDL_DISABLE);
#endif

	HyAssert(uiNumWindows >= 1, "HyWindowManager was constructed with 0 windows");
	for(uint32 i = 0; i < uiNumWindows; ++i)
		m_WindowList.push_back(HY_NEW HyWindow(i, windowInfos[i]));
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

#ifdef HY_USE_SDL2
void HyWindowManager::DoEvent(const SDL_Event& eventRef, HyInput &inputRef)
{
	for(auto window : m_WindowList)
	{
		if(eventRef.window.windowID == window->GetId())
		{
			window->DoEvent(eventRef, inputRef);
			break;
		}
	}
}
#endif
