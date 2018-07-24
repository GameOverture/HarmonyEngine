/**************************************************************************
 *	HarmonyConnection.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HARMONYCONNECTION_H
#define HARMONYCONNECTION_H

#include "Global.h"

#include <QObject>
#include <QAction>
#include <QTcpSocket>
#include <QTcpServer>
#include <QList>

class HarmonyConnection : public QObject
{
	Q_OBJECT

	QTcpSocket      m_Socket;

	QHostAddress    m_Address;
	quint16         m_uiPort;

	quint32         m_uiPacketType;
	quint32         m_uiPacketSize;

public:
	explicit HarmonyConnection(QObject *parent = 0);

	void Connect();

	void Write(eHyPacketType eType, quint32 uiSize, void *pData);
	
	void WriteReloadPacket(QStringList &sPaths);
	
Q_SIGNALS:
	
public Q_SLOTS:
	void OnHostFound();

private Q_SLOTS:
	void ReadData();
	void OnError(QAbstractSocket::SocketError socketError);
	
};

#endif // HARMONYCONNECTION_H
