/**************************************************************************
*	HySdlNet.h
*
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HySdlNet_h__
#define HySdlNet_h__

#include "Afx/HyStdAfx.h"
#include "Networking/IHyNetworking.h"

#if defined(HY_USE_SDL2) && defined(HY_USE_SDL2_NET)
class HySdlNet : public IHyNetworking
{
public:
	HySdlNet();
	virtual ~HySdlNet();

	virtual bool CreateClient(std::string sHost, uint16 uiPort, HyTcpSocket &socketOut) override;

	// Returns the number of bytes received. If the number returned is less than or equal to zero, then an error occurred, or the remote host has closed the connection.
	virtual int32 TcpRecv(const HyTcpSocket &hSocket, void *pData, int iMaxLength) override;
	virtual int32 TcpSend(const HyTcpSocket &hSocket, const void *pData, uint32 uiNumBytes) override;

	virtual void CloseConnection(const HyTcpSocket &hSocket) override;
};
#endif

#endif /* HySdlNet_h__ */
