/**************************************************************************
 *	HarmonyConnection.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "HarmonyConnection.h"

#include <QDateTime>

HarmonyConnection::HarmonyConnection(QObject *parent) :
	QObject(parent),
	m_Socket(this),
	m_Address(QHostAddress::LocalHost),
	m_uiPort(1313),
	m_uiPacketSize(0)
{
	//m_Socket.connect(
	//connect(m_pTcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));

	connect(&m_Socket, SIGNAL(hostFound()), this, SLOT(OnHostFound()));
	connect(&m_Socket, SIGNAL(readyRead()), this, SLOT(ReadData()));
	connect(&m_Socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnError(QAbstractSocket::SocketError)));
}

void HarmonyConnection::Connect()
{
	if(m_Socket.isOpen())
	{
		HyGuiLog("Debugger is alreadyHyGuiLogan open connection", LOGTYPE_Warning);
		return;
	}

	HyGuiLog("Attempting to connect HyGuiDebugger " % m_Address.toString() % " [" % QString::number(m_uiPort) % "]", LOGTYPE_Normal);
	m_Socket.abort();
	m_Socket.connectToHost(m_Address, m_uiPort);
}

void HarmonyConnection::Write(HyPacketType eType, quint32 uiSize, void *pData)
{
	QByteArray packetData;
	
	quint32 n = eType;
	packetData.append(reinterpret_cast<const char *>(&n), 4);

	n = uiSize;
	packetData.append(reinterpret_cast<const char *>(&n), 4);

	packetData.append(reinterpret_cast<char *>(pData), uiSize);
	
	m_Socket.write(packetData);
}

void HarmonyConnection::WriteReloadPacket(QStringList &sPaths)
{
	if(m_Socket.isOpen() == false)
		return;
	
	quint32 id = QTime::currentTime().msecsSinceStartOfDay();
	Write(HYPACKET_ReloadStart, sizeof(quint32), &id);
	
	for(int i = 0; i < sPaths.size(); ++i)
	{
		QByteArray testBuffer;
		testBuffer.append(reinterpret_cast<const char *>(&id), sizeof(quint32));
		testBuffer.append(sPaths[i]);
		quint32 uiSize = sPaths[i].length() + sizeof(quint32) + 1;  // +1 is for null terminator
		
		Write(HYPACKET_ReloadItem, uiSize, testBuffer.data());
	}
	
	Write(HYPACKET_ReloadEnd, 4, &id);
}

void HarmonyConnection::OnHostFound()
{
	HyGuiLog("Host lookup has succeeded", LOGTYPE_Normal);
}

void HarmonyConnection::ReadData()
{
	QDataStream in(&m_Socket);
	in.setVersion(QDataStream::Qt_4_0);

	// Read the packet header
	if (m_uiPacketSize == 0)
	{
		if (m_Socket.bytesAvailable() < HyGuiMessage::HeaderSize)
			return;

		in >> m_uiPacketType;
		in >> m_uiPacketSize;
	}

	if (m_Socket.bytesAvailable() < m_uiPacketSize)
		return;

	// Process the full packet below
	QString sMessage;
	in >> sMessage;

	switch(m_uiPacketType)
	{
	case HYPACKET_LogNormal:
		HyGuiLog(sMessage, LOGTYPE_Normal);
		break;
	case HYPACKET_LogWarning:
		HyGuiLog(sMessage, LOGTYPE_Warning);
		break;
	case HYPACKET_LogError:
		HyGuiLog(sMessage, LOGTYPE_Error);
		break;
	case HYPACKET_LogInfo:
		HyGuiLog(sMessage, LOGTYPE_Info);
		break;
	case HYPACKET_LogTitle:
		HyGuiLog(sMessage, LOGTYPE_Title);
		break;

	case HYPACKET_Int:
		break;
	case HYPACKET_Float:
		break;
	}

	// reset
	m_uiPacketSize = 0;

//    if (nextFortune == currentFortune) {
//        QTimer::singleShot(0, this, SLOT(requestNewFortune()));
//        return;
//    }

//    currentFortune = nextFortune;
//    statusLabel->setText(currentFortune);
//    getFortuneButton->setEnabled(true);
}

void HarmonyConnection::OnError(QAbstractSocket::SocketError socketError)
{
	switch (socketError)
	{
	case QAbstractSocket::RemoteHostClosedError:
		HyGuiLog("The game debugger connection has been lost", LOGTYPE_Info);
		break;
		
	case QAbstractSocket::HostNotFoundError:
		HyGuiLog("The game hosted at [" % m_Address.toString() % "] port: " % QString::number(m_uiPort) % " was not found", LOGTYPE_Error);
		break;
		
	case QAbstractSocket::ConnectionRefusedError:
		HyGuiLog("The game hosted at [" % m_Address.toString() % "] port: " % QString::number(m_uiPort) % " has refused the connection", LOGTYPE_Error);
		break;
		
	default:
		HyGuiLog("Debugger Socket Error: " % m_Socket.errorString(), LOGTYPE_Error);
		break;
	}
}
