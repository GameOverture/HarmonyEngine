/**************************************************************************
*	HyWinsock.cpp
*
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyInteropAfx.h"
#include "Networking/Sockets/HyWinsock.h"
#include "Diagnostics/Console/IHyConsole.h"

#if !defined(HY_USE_SDL2_NET) && defined(HY_PLATFORM_WINDOWS)

HyWinsock::HyWinsock()
{
}

/*virtual*/ HyWinsock::~HyWinsock()
{
}

/*virtual*/ bool HyWinsock::CreateClient(std::string sHost, uint16 uiPort, HyTcpSocket &socketOut) /*override*/
{
	int iResult;

	// Initialize Winsock
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(iResult != 0)
	{
		HyLogError("WSAStartup failed: " << iResult);
		return false;
	}

	struct addrinfo *result = nullptr, *ptr = nullptr, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;// AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(sHost.c_str(), std::to_string(uiPort).c_str(), &hints, &result);
	if(iResult != 0)
	{
		HyLogError("getaddrinfo failed: " << iResult);
		WSACleanup();
		return false;
	}
	socketOut = INVALID_SOCKET;
	// Attempt to connect to the first address returned by the call to getaddrinfo
	ptr = result;

	// Create a SOCKET for connecting to server
	socketOut = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if(socketOut == INVALID_SOCKET)
	{
		HyLogError("Error at socket(): " << WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return false;
	}

	iResult = connect(socketOut, ptr->ai_addr, (int)ptr->ai_addrlen);
	if(iResult == SOCKET_ERROR)
	{
		closesocket(socketOut);
		socketOut = INVALID_SOCKET;
	}

	// Should really try the next address returned by getaddrinfo if the connect call failed
	freeaddrinfo(result);
	if(socketOut == INVALID_SOCKET)
	{
		HyLogError("Unable to connect to server!");
		WSACleanup();
		return false;
	}

	return true;
}

/*virtual*/ int32 HyWinsock::TcpRecv(const HyTcpSocket &hSocket, void *pData, int iMaxLength) /*override*/
{
	int32 iResult = recv(hSocket, static_cast<char *>(pData), iMaxLength, 0);
	if(iResult == 0)
		HyLogError("HyWinsock::TcpRecv - Connection closed");
	else if(iResult < 0)
		HyLogError("HyWinsock::TcpRecv failed: " << WSAGetLastError());
	
	return iResult;
}

/*virtual*/ int32 HyWinsock::TcpSend(const HyTcpSocket &hSocket, const void *pData, uint32 uiNumBytes) /*override*/
{
	int32 iResult = send(hSocket, static_cast<const char *>(pData), uiNumBytes, 0);
	if(iResult == SOCKET_ERROR)
	{
		HyLogError("send failed: %d\n", WSAGetLastError());
		closesocket(hSocket);
		WSACleanup();
		return -1;
	}

	return iResult;
}

/*virtual*/ void HyWinsock::CloseConnection(const HyTcpSocket &hSocket) /*override*/
{
	int32 iResult = shutdown(hSocket, SD_SEND);
	if(iResult == SOCKET_ERROR)
		HyLogError("shutdown failed: " << WSAGetLastError());

	closesocket(hSocket);
	WSACleanup();
}

#endif // #if !defined(HY_USE_SDL2_NET) && defined(HY_PLATFORM_WINDOWS)
