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
#include "Window/HyWindowManager.h"

#ifdef HY_PLATFORM_DESKTOP
void glfw_ErrorCallback(int iError, const char *szDescription)
{
	HyLogError("GLFW Error " << iError << ": " << szDescription);
}
#endif

HyWindowManager::HyWindowManager(uint32 uiNumWindows, bool bShowCursor, const HyWindowInfo windowInfos[HY_MAXWINDOWS])
{
#ifdef HY_PLATFORM_DESKTOP
	// Setup error callback before glfwInit to catch anything that might go wrong with glfwInit
	glfwSetErrorCallback(glfw_ErrorCallback);

	if(glfwInit() == GLFW_FALSE)
		HyLogError("glfwInit failed");
#endif

	HyAssert(uiNumWindows >= 1, "HyWindowManager was constructed with 0 windows");
	for(uint32 i = 0; i < uiNumWindows; ++i)
		m_WindowList.push_back(HY_NEW HyWindow(i, windowInfos[i], bShowCursor, i != 0 ? m_WindowList[0]->GetHandle() : nullptr));
}

HyWindowManager::~HyWindowManager()
{
}

std::vector<HyWindow *> &HyWindowManager::GetWindowList()
{
	return m_WindowList;
}

HyWindow &HyWindowManager::GetWindow(uint32 uiWindowIndex)
{
	HyAssert(uiWindowIndex < m_WindowList.size(), "HyWindowManager::GetWindow was passed an invalid index");
	return *m_WindowList[uiWindowIndex];
}
