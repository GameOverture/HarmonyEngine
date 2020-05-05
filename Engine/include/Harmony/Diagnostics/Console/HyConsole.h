/**************************************************************************
 *	HyConsole.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyConsole_h__
#define HyConsole_h__

#include <mutex>
#include "Afx/HyStdAfx.h"

class HyConsole
{
	static HyConsole *			sm_pInstance;

protected:
	HyConsole();

public:
	~HyConsole();

	static std::stringstream	sm_ss;
	static std::mutex			sm_csLog;

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

#ifdef HY_PLATFORM_GUI
	// TODO: Pipe messages to the Editor Tool
	#define HyLog(msg) { }
	#define HyLogWarning(msg) { }
	#define HyLogError(msg) { }
	#define HyLogInfo(msg) { }
	#define HyLogTitle(msg) { }
	#define HyLogSection(msg) { }
#else
	#define HyLog(msg) {\
		HyConsole::sm_csLog.lock(); \
		HyConsole::sm_ss.str(std::string()); HyConsole::sm_ss << msg; \
		HyConsole::Get()->Log(std::cout, HyConsole::sm_ss.str().c_str(), HyConsole::LOG_Regular); \
		HyConsole::sm_csLog.unlock(); }
	#define HyLogWarning(msg) {\
		HyConsole::sm_csLog.lock(); \
		HyConsole::sm_ss.str(std::string()); HyConsole::sm_ss << msg; \
		HyConsole::Get()->Log(std::cout, HyConsole::sm_ss.str().c_str(), HyConsole::LOG_Warning); \
		HyConsole::sm_csLog.unlock(); }
	#define HyLogError(msg) {\
		HyConsole::sm_csLog.lock(); \
		HyConsole::sm_ss.str(std::string()); HyConsole::sm_ss << msg; \
		HyConsole::Get()->Log(std::cout, HyConsole::sm_ss.str().c_str(), HyConsole::LOG_Error); \
		HyConsole::sm_csLog.unlock(); }
	#define HyLogInfo(msg) {\
		HyConsole::sm_csLog.lock(); \
		HyConsole::sm_ss.str(std::string()); HyConsole::sm_ss << msg; \
		HyConsole::Get()->Log(std::cout, HyConsole::sm_ss.str().c_str(), HyConsole::LOG_Info); \
		HyConsole::sm_csLog.unlock(); }
	#define HyLogTitle(msg) {\
		HyConsole::sm_csLog.lock(); \
		HyConsole::sm_ss.str(std::string()); HyConsole::sm_ss << msg; \
		HyConsole::Get()->Log(std::cout, HyConsole::sm_ss.str().c_str(), HyConsole::LOG_Title); \
		HyConsole::sm_csLog.unlock(); }
	#define HyLogSection(msg) {\
		HyConsole::sm_csLog.lock(); \
		HyConsole::sm_ss.str(std::string()); HyConsole::sm_ss << msg; \
		HyConsole::Get()->Log(std::cout, HyConsole::sm_ss.str().c_str(), HyConsole::LOG_Section); \
		HyConsole::sm_csLog.unlock(); }
#endif

#endif /*__HyConsole_h__*/
