/**************************************************************************
*	HyConsole_Gui.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __HyConsole_Gui_h__
#define __HyConsole_Gui_h__

#include "Diagnostics/Console/HyConsole.h"

class HyConsole_Gui : public HyConsole
{
public:
	HyConsole_Gui(bool bCreateConsole, HyWindowInfo &consoleInfo);
	virtual ~HyConsole_Gui();

	virtual void Log(std::ostream &os, const char *szMsg, LogType eType);
};

#endif /* __HyConsole_Gui_h__ */
