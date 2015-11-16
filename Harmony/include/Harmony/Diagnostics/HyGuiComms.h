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
#include "Afx/HyInteropAfx.h"

#ifndef HY_PLATFORM_GUI

// TODO: These below defines might only be necessary for MSVC 2013 (non C++11 compilers)
#define ASIO_STANDALONE 
#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS

#define ASIO_STANDALONE
#include "asio/asio.hpp"
using asio::ip::tcp;

class HyGuiComms
{
	static HyGuiComms *		sm_pInstance;
	
	HyFileIOInterop &		m_FileIORef;

	class session : public std::enable_shared_from_this<session>
	{
		enum { max_length = 1024 };

		tcp::socket			m_Socket;
		char				m_Data[max_length];

	public:
		session(tcp::socket socket) : m_Socket(std::move(socket))
		{
		}

		void start()
		{
			do_read();
		}

	private:
		void do_read()
		{
			auto self(shared_from_this());
			m_Socket.async_read_some(asio::buffer(m_Data, max_length),	[this, self](std::error_code ec, std::size_t length)
																		{
																			if(!ec)
																			{
																				ProcessData();
																				
																				do_write(length);
																			}
																		});
		}

		void do_write(std::size_t length)
		{
			auto self(shared_from_this());
			asio::async_write(m_Socket, asio::buffer(m_Data, length),	[this, self](std::error_code ec, std::size_t /*length*/)
																		{
																			if(!ec)
																			{
																				do_read();
																			}
																		});
		}

		struct CurrentPacket
		{

		};

		void ProcessData()
		{

		}
	};

	asio::io_service		m_IOService;
	tcp::acceptor			m_Acceptor;
	tcp::socket				m_Socket;

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
	HyGuiComms(uint16 uiPort, HyFileIOInterop &fileIORef);
	~HyGuiComms(void);

	static void Log(const char *szMessage, uint32 uiType);
	
	void SendToGui(ePacketType eType, uint32 uiDataSize, const void *pDataToCopy);

	void DoAcceptConnection();

	void Update();
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
class HyGuiComms
{
public:
	HyGuiComms(uint16 uiPort)
	{ }

	void Update()
	{ }
};

#define HyLog(msg) do { } while (false)
#define HyLogWarning(msg) do { } while (false)
#define HyLogError(msg) do { } while (false)
#define HyLogInfo(msg) do { } while (false)
#define HyLogTitle(msg) do { } while (false)
#endif

#endif /* __HyGuiComms_h__ */
