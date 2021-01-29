/**************************************************************************
*	HyNetworking.cpp
*
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Networking/HyNetworking.h"
#include "Networking/IHyNetworkClass.h"
#include "Diagnostics/Console/HyConsole.h"

#ifdef HY_USE_SDL2_NET
	#ifdef HY_PLATFORM_BROWSER
		#include <SDL2/SDL_net.h>
	#else
		#include "SDL_net.h"
	#endif

//#elif defined(HY_PLATFORM_BROWSER)
//	#include <errno.h> /* EINPROGRESS, errno */
//	#include <sys/types.h> /* timeval */
//	#include <sys/socket.h>
//	#include <arpa/inet.h>
//	#include <fcntl.h>
//	#include <unistd.h>
#endif

HyNetworking::HyNetworking()
{
#ifdef HY_USE_SDL2_NET
	if(SDLNet_Init() != 0) {
		HyError("HyNetworking::HyNetworking - SDLNet_Init failed: " << SDLNet_GetError());
	}

	// Print out the runtime version of SDL2_net vs. what this exe was compiled with
	SDL_version compiledVersion; SDL_NET_VERSION(&compiledVersion);
	HyLog("Compiled SDL_net version: " << compiledVersion.major << "." << compiledVersion.minor << "." << compiledVersion.patch);

	const SDL_version *pLinkedVersion = SDLNet_Linked_Version();
	HyLog("Running SDL_net version: " << pLinkedVersion->major << "." << pLinkedVersion->minor << "." << pLinkedVersion->patch);
#endif

	IHyNetworkClass::sm_pNetworking = this;
}

HyNetworking::~HyNetworking()
{
#ifdef HY_USE_SDL2_NET
	SDLNet_Quit();
#endif

	IHyNetworkClass::sm_pNetworking = nullptr;
}

bool HyNetworking::CreateClient(std::string sHost, uint16 uiPort, HyTcpSocket &socketOut)
{
#ifdef HY_USE_SDL2_NET
	socketOut = nullptr;

	// connect using TCP (client)
	IPaddress ip;
	if(SDLNet_ResolveHost(&ip, sHost.c_str(), uiPort) == -1) {
		HyLogError("SDLNet_ResolveHost: " << SDLNet_GetError());
		return false;
	}

	socketOut = SDLNet_TCP_Open(&ip);
	if(!socketOut)
	{
		HyLogError("SDLNet_TCP_Open: " << SDLNet_GetError());
		return false;
	}

	return true;
#elif defined(HY_PLATFORM_BROWSER)
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
#else
	return false;
#endif
}

int32 HyNetworking::TcpRecv(HyTcpSocket hSocket, void *pData, int iMaxLength)
{
#ifdef HY_USE_SDL2_NET
	return SDLNet_TCP_Recv(hSocket, pData, iMaxLength);
#elif defined(HY_PLATFORM_BROWSER)
	/* Wait timeout milliseconds to receive data */
	int iTimeout = -1;
	fd_set sockets;
	FD_ZERO(&sockets);
	FD_SET(hSocket, &sockets);
	timeval t{0, iTimeout*1000};
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
#else
	return 0;
#endif
}

int32 HyNetworking::TcpSend(HyTcpSocket hSocket, const void *pData, uint32 uiNumBytes)
{
#ifdef HY_USE_SDL2_NET
	return SDLNet_TCP_Send(hSocket, pData, static_cast<int>(uiNumBytes));
#elif defined(HY_PLATFORM_BROWSER)
	const int ret = ::send(hSocket, pData, uiNumBytes, 0);
	if(ret == -1)
	{
		HyLogError("HyNetworking::TcpSend send failed");
		CloseConnection(hSocket);
		return -1;
	}
	return ret;
#else
	return -1;
#endif
}

void HyNetworking::CloseConnection(HyTcpSocket hSocket)
{
#ifdef HY_USE_SDL2_NET
	SDLNet_TCP_Close(hSocket);
#elif defined(HY_PLATFORM_BROWSER)
	if(hSocket != -1)
		::close(hSocket);
#endif
}
