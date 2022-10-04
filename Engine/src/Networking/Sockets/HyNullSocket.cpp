/**************************************************************************
*	HyNullSocket.cpp
*
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyInteropAfx.h"
#include "Networking/Sockets/HyNullSocket.h"

/*virtual*/ bool HyNullSocket::CreateClient(std::string sHost, uint16 uiPort, HyTcpSocket &socketOut) /*override*/
{
	return false;
}

/*virtual*/ int32 HyNullSocket::TcpRecv(const HyTcpSocket &hSocket, void *pData, int iMaxLength) /*override*/
{
	return 0;
}

/*virtual*/ int32 HyNullSocket::TcpSend(const HyTcpSocket &hSocket, const void *pData, uint32 uiNumBytes) /*override*/
{
	return -1;
}

/*virtual*/ void HyNullSocket::CloseConnection(const HyTcpSocket &hSocket) /*override*/
{
}
