/**************************************************************************
*	IHyNetworking.h
*
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyNetworking_h__
#define IHyNetworking_h__

#include "Afx/HyStdAfx.h"

class IHyNetworking
{
public:
	IHyNetworking();
	virtual ~IHyNetworking();

	virtual bool CreateClient(std::string sHost, uint16 uiPort, HyTcpSocket &socketOut) = 0;

	// Returns the number of bytes received. If the number returned is less than or equal to zero, then an error occurred, or the remote host has closed the connection.
	virtual int32 TcpRecv(const HyTcpSocket &hSocket, void *pData, int iMaxLength) = 0;
	virtual int32 TcpSend(const HyTcpSocket &hSocket, const void *pData, uint32 uiNumBytes) = 0;

	virtual void CloseConnection(const HyTcpSocket &hSocket) = 0;
};

#endif /* IHyNetworking_h__ */
