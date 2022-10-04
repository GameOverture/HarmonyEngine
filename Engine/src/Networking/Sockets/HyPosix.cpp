/**************************************************************************
*	HyPosix.cpp
*
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyInteropAfx.h"
#include "Networking/Sockets/HyPosix.h"
#include "Diagnostics/Console/IHyConsole.h"

#if !defined(HY_USE_SDL2_NET) && defined(HY_PLATFORM_LINUX)

HyPosix::HyPosix()
{
}

/*virtual*/ HyPosix::~HyPosix()
{
}

/*virtual*/ bool HyPosix::CreateClient(std::string sHost, uint16 uiPort, HyTcpSocket &socketOut) /*override*/
{
	socketOut = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(socketOut == -1)
	{
		HyLogError("HyNetworking::CreateClient failed to create socket");
		return false;
	}
	fcntl(socketOut, F_SETFL, O_NONBLOCK);

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(uiPort);
	if(inet_pton(AF_INET, sHost.c_str(), &addr.sin_addr) != 1)
	{
		HyLogError("HyNetworking::CreateClient inet_pton failed");
		return false;
	}

	const int res = ::connect(socketOut, (struct sockaddr *)&addr, sizeof(addr));
	if(res == -1)
	{
		if(errno == EINPROGRESS)
		{
			HyLog("HyNetworking::CreateClient Connection in progress for fd:" << socketOut);

			// Wait for connection to complete
			fd_set sockets;
			FD_ZERO(&sockets);
			FD_SET(socketOut, &sockets);

			// TODO: Set a timeout instead of blocking indefinately
			while(select(socketOut + 1, nullptr, &sockets, nullptr, nullptr) <= 0) {}
		}
		else
		{
			HyLogWarning("HyNetworking::CreateClient connection failed");
			return false;
		}
	}

	return true;
}

/*virtual*/ int32 HyPosix::TcpRecv(const HyTcpSocket &hSocket, void *pData, int iMaxLength) /*override*/
{
	/* Wait timeout milliseconds to receive data */
	int iTimeout = -1;
	fd_set sockets;
	FD_ZERO(&sockets);
	FD_SET(hSocket, &sockets);
	timeval t{ 0, iTimeout * 1000 };
	int ret = select(hSocket + 1, &sockets, nullptr, nullptr, (iTimeout == -1) ? nullptr : &t);
	if(ret == 0)
		return 0; // Timeout occured
	else if(ret < 0)
	{
		HyLogError("HyNetworking::TcpRecv select failed");
		return 0;
	}

	ret = recv(hSocket, pData, iMaxLength, 0);
	if(ret < 0)
	{
		HyLogError("HyNetworking::TcpRecv recv failed");
		return 0;
	}

	return ret;
}

/*virtual*/ int32 HyPosix::TcpSend(const HyTcpSocket &hSocket, const void *pData, uint32 uiNumBytes) /*override*/
{
	const int ret = ::send(hSocket, pData, uiNumBytes, 0);
	if(ret == -1)
	{
		HyLogError("HyNetworking::TcpSend send failed");
		CloseConnection(hSocket);
		return -1;
	}
	return ret;
}

/*virtual*/ void HyPosix::CloseConnection(const HyTcpSocket &hSocket) /*override*/
{
	if(hSocket != -1)
		::close(hSocket);
}

#endif // #if !defined(HY_USE_SDL2_NET) && defined(HY_PLATFORM_LINUX)
