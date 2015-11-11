/**************************************************************************
 *	HyGuiComms.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Diagnostics/HyGuiComms.h"

#ifndef HY_PLATFORM_GUI

#define HY_SERVER_PORT 1313

#include "Time/IHyTime.h"

HyGuiComms *HyGuiComms::sm_pInstance = NULL;

HyGuiComms::HyGuiComms(void)
{
	HyAssert(sm_pInstance == NULL, "HyGuiComms was instantiated twice");
	sm_pInstance = this;

	try
	{
		if(argc < 2)
		{
			std::cerr << "Usage: chat_server <port> [<port> ...]\n";
			return 1;
		}

		asio::io_service io_service;

		std::list<chat_server> servers;
		for(int i = 1; i < argc; ++i)
		{
			tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
			servers.emplace_back(io_service, endpoint);
		}

		io_service.run();
	}
	catch(std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
}

HyGuiComms::~HyGuiComms(void)
{
}

void HyGuiComms::Update()
{
	// Send any dirty live params

	// Send all queued up log messages

	// Send diagnostics
}

void HyGuiComms::start_accept()
{
	tcp_connection::pointer new_connection =
		tcp_connection::create(acceptor_.get_io_service());

	acceptor_.async_accept(new_connection->socket(),
		boost::bind(&tcp_server::handle_accept, this, new_connection,
		asio::placeholders::error));
}

void HyGuiComms::handle_accept(tcp_connection::pointer new_connection, const asio::error_code& error)
{
	if(!error)
	{
		new_connection->start();
	}

	start_accept();
}

void HyGuiComms::SendToGui(ePacketType eType, uint32 uiDataSize, const void *pDataToCopy)
{
}

void HyGuiComms::Log(const char *szMessage, uint32 uiLevel)
{
	sm_pInstance->SendToGui(static_cast<ePacketType>(uiLevel), static_cast<uint32>(strlen(szMessage)), szMessage);
}

#endif
