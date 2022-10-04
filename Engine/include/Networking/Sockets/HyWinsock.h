/**************************************************************************
*	HyWinsock.h
*
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyWinsock_h__
#define HyWinsock_h__

#include "Afx/HyStdAfx.h"
#include "Networking/IHyNetworking.h"

#if !defined(HY_USE_SDL2_NET) && defined(HY_PLATFORM_WINDOWS)
class HyWinsock : public IHyNetworking
{
public:
	HyWinsock();
	virtual ~HyWinsock();

	virtual bool CreateClient(std::string sHost, uint16 uiPort, HyTcpSocket &socketOut) override;

	// Returns the number of bytes received. If the number returned is less than or equal to zero, then an error occurred, or the remote host has closed the connection.
	virtual int32 TcpRecv(const HyTcpSocket &hSocket, void *pData, int iMaxLength) override;
	virtual int32 TcpSend(const HyTcpSocket &hSocket, const void *pData, uint32 uiNumBytes) override;

	virtual void CloseConnection(const HyTcpSocket &hSocket) override;
};
#endif

#endif /* HyWinsock_h__ */
