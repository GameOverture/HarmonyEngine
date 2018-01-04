/**************************************************************************
*	HyConsole_Gui.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyConsole_Gui_h__
#define HyConsole_Gui_h__

#include "Diagnostics/Console/HyConsole.h"

class HyConsole_Gui : public HyConsole
{
public:
	HyConsole_Gui(bool bCreateConsole, HyWindowInfo &consoleInfo);
	virtual ~HyConsole_Gui();

	virtual void Log(std::ostream &os, const char *szMsg, LogType eType);
};

#endif /* HyConsole_Gui_h__ */
