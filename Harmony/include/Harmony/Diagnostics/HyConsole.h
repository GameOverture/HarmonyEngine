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

class HyConsole
{
public:
	HyConsole(bool bCreateConsole, HyWindowInfo &consoleInfo);
	~HyConsole();
};


#define HyLog(msg) {\
	std::stringstream ss; \
	ss << msg << std::endl; \
	printf(ss.str().c_str()); }
	//HyGuiComms::Broadcast(HYPACKET_LogNormal, static_cast<uint32>(strlen(ss.str().c_str())), ss.str().c_str()); }
#define HyLogWarning(msg) {\
	std::stringstream ss; \
	ss << msg << std::endl; \
	std::cout << ss; }
	//HyGuiComms::Broadcast(HYPACKET_LogWarning, static_cast<uint32>(strlen(ss.str().c_str())), ss.str().c_str()); }
#define HyLogError(msg) {\
	std::stringstream ss; \
	ss << msg << std::endl; \
	std::cout << ss; }
	//HyGuiComms::Broadcast(HYPACKET_LogError, static_cast<uint32>(strlen(ss.str().c_str())), ss.str().c_str()); }
#define HyLogInfo(msg) {\
	std::stringstream ss; \
	ss << msg << std::endl; \
	std::cout << ss; }
	//HyGuiComms::Broadcast(HYPACKET_LogInfo, static_cast<uint32>(strlen(ss.str().c_str())), ss.str().c_str()); }
#define HyLogTitle(msg) {\
	std::stringstream ss; \
	ss << msg << std::endl; \
	std::cout << ss; }
	//HyGuiComms::Broadcast(HYPACKET_LogTitle, static_cast<uint32>(strlen(ss.str().c_str())), ss.str().c_str()); }

//#define HyLog(msg) do { } while (false)
//#define HyLogWarning(msg) do { } while (false)
//#define HyLogError(msg) do { } while (false)
//#define HyLogInfo(msg) do { } while (false)
//#define HyLogTitle(msg) do { } while (false)

#endif /*__HyConsole_h__*/
