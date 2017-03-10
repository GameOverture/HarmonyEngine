/**************************************************************************
*	HyConsole_Win.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __HyConsole_Win_h__
#define __HyConsole_Win_h__

#include "Diagnostics\HyConsole.h"

class HyConsole_Win : public HyConsole
{
public:
	HyConsole_Win(bool bCreateConsole, HyWindowInfo &consoleInfo);
	virtual ~HyConsole_Win();

	virtual void Log(std::ostream &os, const char *szMsg, LogType eType);
};

#endif /* __HyConsole_Win_h__ */
