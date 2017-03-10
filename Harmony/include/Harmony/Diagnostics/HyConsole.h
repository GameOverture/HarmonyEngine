/**************************************************************************
 *	HyConsole.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyConsole_h__
#define __HyConsole_h__

#include "Afx/HyStdAfx.h"

#include <sstream>

class HyConsole
{
	static HyConsole *			sm_pInstance;

protected:
	HyConsole();

public:
	~HyConsole();

	static std::stringstream	sm_ss;

	enum LogType
	{
		LOG_Regular = 0,
		LOG_Warning,
		LOG_Error,
		LOG_Info,
		LOG_Title,
		LOG_Section
	};
	static HyConsole *Get();

	virtual void Log(std::ostream &os, const char *szMsg, LogType eType) = 0;
};

#define HyLog(msg) {\
	HyConsole::sm_ss.clear(); HyConsole::sm_ss << msg << std::endl; \
	HyConsole::Get()->Log(std::cout, HyConsole::sm_ss.str().c_str(), HyConsole::LOG_Regular); }
#define HyLogWarning(msg) {\
	HyConsole::sm_ss.clear(); HyConsole::sm_ss << msg << std::endl; \
	HyConsole::Get()->Log(std::cout, HyConsole::sm_ss.str().c_str(), HyConsole::LOG_Warning); }
#define HyLogError(msg) {\
	HyConsole::sm_ss.clear(); HyConsole::sm_ss << msg << std::endl; \
	HyConsole::Get()->Log(std::cout, HyConsole::sm_ss.str().c_str(), HyConsole::LOG_Error); }
#define HyLogInfo(msg) {\
	HyConsole::sm_ss.clear(); HyConsole::sm_ss << msg << std::endl; \
	HyConsole::Get()->Log(std::cout, HyConsole::sm_ss.str().c_str(), HyConsole::LOG_Info); }
#define HyLogTitle(msg) {\
	HyConsole::sm_ss.clear(); HyConsole::sm_ss << msg << std::endl; \
	HyConsole::Get()->Log(std::cout, HyConsole::sm_ss.str().c_str(), HyConsole::LOG_Title); }
#define HyLogSection(msg) {\
	HyConsole::sm_ss.clear(); HyConsole::sm_ss << msg << std::endl; \
	HyConsole::Get()->Log(std::cout, HyConsole::sm_ss.str().c_str(), HyConsole::LOG_Section); }

#endif /*__HyConsole_h__*/
