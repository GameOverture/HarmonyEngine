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

#include "netlink/socket.h"
#include "netlink/socket_group.h"
using namespace NL;

#define HYNETWORKING_MAX_PACKET_SIZE 256

class HyGuiComms
{
	static HyGuiComms *		sm_pInstance;

	NL::Socket *			m_pSocket;
	NL::SocketGroup *		m_pSocketGroup;

	unsigned char			m_pPacketBuffer[HYNETWORKING_MAX_PACKET_SIZE];

	bool					m_bConnected;
	float					m_fReconnectWaitInterval;

	void Connect();

	// Because netlink library doesn't use function pointers, these "callback classes" exist.
	class OnReadCallbackClass : public NL::SocketGroupCmd
	{
		void exec(NL::Socket *pSocket, NL::SocketGroup *pGroup, void *pParam);
	};
	class OnDisconnectCallbackClass : public NL::SocketGroupCmd
	{
		void exec(NL::Socket *pSocket, NL::SocketGroup *pGroup, void *pParam);
	};

	OnReadCallbackClass					m_OnRead;
	OnDisconnectCallbackClass			m_OnDisconnect;

	enum ePacketType
	{
		HYPACKET_LogNormal = 0,
		HYPACKET_LogWarning,
		HYPACKET_LogError,
		HYPACKET_LogInfo,
		HYPACKET_LogTitle,
		HYPACKET_Int,
		HYPACKET_Float,
	};


public:
	HyGuiComms(void);
	~HyGuiComms(void);

	static void Log(const char *szMessage, uint32 uiType);
	
	void SendPacket(ePacketType eType, uint32 uiDataSize, const void *pDataToCopy);

	void Update();
	void ProcessPacket(char *&pCurReadPos);
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
