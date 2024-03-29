/**************************************************************************
*	HyPosix.h
*
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyPosix_h__
#define HyPosix_h__

#include "Afx/HyStdAfx.h"
#include "Networking/IHyNetworking.h"

#if !(defined(HY_USE_SDL2) && defined(HY_USE_SDL2_NET)) && defined(HY_PLATFORM_LINUX)
class HyPosix : public IHyNetworking
{
public:
	HyPosix();
	virtual ~HyPosix();

	virtual bool CreateClient(std::string sHost, uint16 uiPort, HyTcpSocket &socketOut) override;

	// Returns the number of bytes received. If the number returned is less than or equal to zero, then an error occurred, or the remote host has closed the connection.
	virtual int32 TcpRecv(const HyTcpSocket &hSocket, void *pData, int iMaxLength) override;
	virtual int32 TcpSend(const HyTcpSocket &hSocket, const void *pData, uint32 uiNumBytes) override;

	virtual void CloseConnection(const HyTcpSocket &hSocket) override;
};
#endif

#endif /* HyPosix_h__ */
