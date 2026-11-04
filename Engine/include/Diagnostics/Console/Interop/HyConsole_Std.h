/**************************************************************************
*	HyConsole_Std.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	MIT License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/main/Engine/LICENSE
*************************************************************************/
#ifndef HyConsole_Gui_h__
#define HyConsole_Gui_h__

#include "Diagnostics/Console/IHyConsole.h"

class HyConsole_Std : public IHyConsole
{
public:
	HyConsole_Std(bool bCreateConsole, const HyWindowInfo &consoleInfo);
	virtual ~HyConsole_Std();

	virtual void OnLog(std::ostream &os, const char *szMsg, LogType eType) override;
	virtual void Clear() override;
};

#endif /* HyConsole_Gui_h__ */
