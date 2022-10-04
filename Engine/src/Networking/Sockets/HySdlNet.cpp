/**************************************************************************
*	HySdlNet.cpp
*
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyInteropAfx.h"
#include "Networking/Sockets/HySdlNet.h"
#include "Diagnostics/Console/IHyConsole.h"

#if defined(HY_USE_SDL2) && defined(HY_USE_SDL2_NET)

HySdlNet::HySdlNet()
{
	if(SDLNet_Init() != 0)
		HyError("HyNetworking::HyNetworking - SDLNet_Init failed: " << SDLNet_GetError());
	// Print out the runtime version of SDL2_net vs. what this exe was compiled with
	SDL_version compiledVersion; SDL_NET_VERSION(&compiledVersion);
	HyLog("Compiled SDL_net version: " << compiledVersion.major << "." << compiledVersion.minor << "." << compiledVersion.patch);
	const SDL_version *pLinkedVersion = SDLNet_Linked_Version();
	HyLog("Running SDL_net version: " << pLinkedVersion->major << "." << pLinkedVersion->minor << "." << pLinkedVersion->patch);
}

/*virtual*/ HySdlNet::~HySdlNet()
{
	SDLNet_Quit();
}

/*virtual*/ bool HySdlNet::CreateClient(std::string sHost, uint16 uiPort, HyTcpSocket &socketOut) /*override*/
{
	socketOut = nullptr;
	// connect using TCP (client)
	IPaddress ip;
	if(SDLNet_ResolveHost(&ip, sHost.c_str(), uiPort) == -1)
	{
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
}

/*virtual*/ int32 HySdlNet::TcpRecv(const HyTcpSocket &hSocket, void *pData, int iMaxLength) /*override*/
{
	return SDLNet_TCP_Recv(hSocket, pData, iMaxLength);
}

/*virtual*/ int32 HySdlNet::TcpSend(const HyTcpSocket &hSocket, const void *pData, uint32 uiNumBytes) /*override*/
{
	return SDLNet_TCP_Send(hSocket, pData, static_cast<int>(uiNumBytes));
}

/*virtual*/ void HySdlNet::CloseConnection(const HyTcpSocket &hSocket) /*override*/
{
	SDLNet_TCP_Close(hSocket);
}

#endif // #if defined(HY_USE_SDL2) && defined(HY_USE_SDL2_NET)
