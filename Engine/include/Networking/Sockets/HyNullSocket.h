/**************************************************************************
*	HyNullSocket.h
*
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyNullSocket_h__
#define HyNullSocket_h__

#include "Afx/HyStdAfx.h"
#include "Networking/IHyNetworking.h"

class HyNullSocket : public IHyNetworking
{
	virtual bool CreateClient(std::string sHost, uint16 uiPort, HyTcpSocket &socketOut) override;
	virtual int32 TcpRecv(const HyTcpSocket &hSocket, void *pData, int iMaxLength) override;
	virtual int32 TcpSend(const HyTcpSocket &hSocket, const void *pData, uint32 uiNumBytes) override;
	virtual void CloseConnection(const HyTcpSocket &hSocket) override;
};

#endif /* HyNullSocket_h__ */
