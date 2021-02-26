/**************************************************************************
 *	IHyConsole.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IHyConsole_h__
#define IHyConsole_h__

#include <mutex>
#include "Afx/HyStdAfx.h"

class IHyConsole
{
	static IHyConsole *			sm_pInstance;

protected:
	std::mutex					m_csLog;

	IHyConsole();
public:
	virtual ~IHyConsole();

	enum LogType
	{
		LOG_Regular = 0,
		LOG_Warning,
		LOG_Error,
		LOG_Info,
		LOG_Title,
		LOG_Section
	};
	static IHyConsole *Get();

	void Log(std::ostream &os, const char *szMsg, LogType eType);
	virtual void OnLog(std::ostream &os, const char *szMsg, LogType eType) = 0;
};

#ifdef HY_PLATFORM_GUI
	// TODO: Pipe messages to the Editor Tool
	#define HyLog(msg) do { } while (false)
	#define HyLogWarning(msg) do { } while (false)
	#define HyLogError(msg) do { } while (false)
	#define HyLogInfo(msg) do { } while (false)
	#define HyLogTitle(msg) do { } while (false)
	#define HyLogSection(msg) do { } while (false)
#else
	#define HyLog(msg) IHyConsole::Get()->Log(std::cout, (std::stringstream() << msg).str().c_str(), IHyConsole::LOG_Regular)
	#define HyLogWarning(msg) IHyConsole::Get()->Log(std::cout, (std::stringstream() << msg).str().c_str(), IHyConsole::LOG_Warning)
	#define HyLogError(msg) IHyConsole::Get()->Log(std::cout, (std::stringstream() << msg).str().c_str(), IHyConsole::LOG_Error)
	#define HyLogInfo(msg) IHyConsole::Get()->Log(std::cout, (std::stringstream() << msg).str().c_str(), IHyConsole::LOG_Info)
	#define HyLogTitle(msg) IHyConsole::Get()->Log(std::cout, (std::stringstream() << msg).str().c_str(), IHyConsole::LOG_Title)
	#define HyLogSection(msg) IHyConsole::Get()->Log(std::cout, (std::stringstream() << msg).str().c_str(), IHyConsole::LOG_Section)
#endif

#endif /* IHyConsole_h__*/
