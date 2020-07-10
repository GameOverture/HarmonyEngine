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
	#include "SDL_net.h"
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

TCPsocket HyNetworking::CreateClient(std::string sHost, uint16 uiPort)
{
	TCPsocket sock = nullptr;

#ifdef HY_USE_SDL2_NET
	// connect to localhost at port 9999 using TCP (client)
	IPaddress ip;
	if(SDLNet_ResolveHost(&ip, sHost.c_str(), uiPort) == -1) {
		HyLogError("SDLNet_ResolveHost: " << SDLNet_GetError());
		return false;
	}

	sock = SDLNet_TCP_Open(&ip);
	if(!sock) {
		HyLogError("SDLNet_TCP_Open: " << SDLNet_GetError());
	}
#endif

	return sock;
}

int32 HyNetworking::TcpRecv(TCPsocket hSocket, void *pData, int iMaxLength)
{
#ifdef HY_USE_SDL2_NET
	return SDLNet_TCP_Recv(hSocket, pData, iMaxLength);
#else
	return 0;
#endif
}

int32 HyNetworking::TcpSend(TCPsocket hSocket, const void *pData, uint32 uiNumBytes)
{
#ifdef HY_USE_SDL2_NET
	return SDLNet_TCP_Send(hSocket, pData, static_cast<int>(uiNumBytes));
#else
	return 0;
#endif
}

void HyNetworking::CloseConnection(TCPsocket hSocket)
{
#ifdef HY_USE_SDL2_NET
	SDLNet_TCP_Close(hSocket);
#endif
}
