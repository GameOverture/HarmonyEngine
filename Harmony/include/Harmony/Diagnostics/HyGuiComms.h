/**************************************************************************
 *	HyGuiComms.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyGuiComms_h__
#define __HyGuiComms_h__

#include "Afx/HyStdAfx.h"

#ifndef HY_PLATFORM_GUI

#include "asio/asio.hpp"
using asio::ip::tcp;

class HyGuiComms
{
	static HyGuiComms *		sm_pInstance;

	asio::ip::tcp::socket	m_Socket;
	asio::io_service		m_IOService;

	tcp::acceptor			m_Acceptor;

	enum ePacketType
	{
		PACKET_LogNormal = 0,
		PACKET_LogWarning,
		PACKET_LogError,
		PACKET_LogInfo,
		PACKET_LogTitle,
		
		PACKET_Int,
		PACKET_Float,

		PACKET_ReloadStart,
		PACKET_ReloadItem,
		PACKET_ReloadEnd
	};


public:
	HyGuiComms(void);
	~HyGuiComms(void);

	static void Log(const char *szMessage, uint32 uiType);
	
	void SendToGui(ePacketType eType, uint32 uiDataSize, const void *pDataToCopy);

	void Update();

private:
	void start_accept();
	void handle_accept(tcp_connection::pointer new_connection, const asio::error_code& error);
};

#define HyLog(msg) {\
	std::stringstream ss; \
	ss << msg << std::endl \
	HyGuiComms::Log(ss.str().c_str(), 0); }
#define HyLogWarning(msg) {\
	std::stringstream ss; \
	ss << msg << std::endl; \
	HyGuiComms::Log(ss.str().c_str(), 1); }
#define HyLogError(msg) {\
	std::stringstream ss; \
	ss << msg << std::endl; \
	HyGuiComms::Log(ss.str().c_str(), 2); }
#define HyLogInfo(msg) {\
	std::stringstream ss; \
	ss << msg << std::endl; \
	HyGuiComms::Log(ss.str().c_str(), 3); }
#define HyLogTitle(msg) {\
	std::stringstream ss; \
	ss << msg << std::endl; \
	HyGuiComms::Log(ss.str().c_str(), 4); }

#else
#define HyLog(msg) do { } while (false)
#define HyLogWarning(msg) do { } while (false)
#define HyLogError(msg) do { } while (false)
#define HyLogInfo(msg) do { } while (false)
#define HyLogTitle(msg) do { } while (false)
#endif

#endif /* __HyGuiComms_h__ */
