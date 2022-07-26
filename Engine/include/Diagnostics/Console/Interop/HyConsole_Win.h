/**************************************************************************
*	HyConsole_Win.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyConsole_Win_h__
#define HyConsole_Win_h__

#include "Diagnostics/Console/IHyConsole.h"

class HyConsole_Win : public IHyConsole
{
public:
	HyConsole_Win(bool bCreateConsole, const HyWindowInfo &consoleInfo);
	virtual ~HyConsole_Win();

	virtual void OnLog(std::ostream &os, const char *szMsg, LogType eType) override;
	virtual void Clear() override;
};

#endif /* HyConsole_Win_h__ */
