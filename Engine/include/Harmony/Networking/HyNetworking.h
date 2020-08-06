/**************************************************************************
*	HyNetworking.h
*
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyNetworking_h__
#define HyNetworking_h__

#include "Afx/HyInteropAfx.h"

class HyNetworking
{
public:
	HyNetworking();
	~HyNetworking();

	bool CreateClient(std::string sHost, uint16 uiPort, TCPsocket &socketOut);

	// Returns the number of bytes received. If the number returned is less than or equal to zero, then an error occured, or the remote host has closed the connection.
	int32 TcpRecv(TCPsocket hSocket, void *pData, int iMaxLength);
	int32 TcpSend(TCPsocket hSocket, const void *pData, uint32 uiNumBytes);

	void CloseConnection(TCPsocket hSocket);
};

#endif /* HyNetworking_h__ */
