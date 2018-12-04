/**************************************************************************
 *	HyWindowManager.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyWindowManager_h__
#define HyWindowManager_h__

#include "Window/HyWindow.h"

class HyWindowManager
{
	std::vector<HyWindow *>				m_WindowList;

public:
	HyWindowManager(uint32 uiNumWindows, bool bShowCursor, const HyWindowInfo windowInfos[HY_MAXWINDOWS]);
	~HyWindowManager();

	std::vector<HyWindow *> &GetWindowList();
	HyWindow &GetWindow(uint32 uiWindowIndex);
};

#endif /* HyWindowManager_h__ */
