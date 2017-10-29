/**************************************************************************
 *	HyGuiDebugger.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HARMONYCONNECTION_H
#define HARMONYCONNECTION_H

#include <QObject>
#include <QAction>
#include <QTcpSocket>
#include <QTcpServer>

#include <QList>

#include "Harmony/HyEngine.h"

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
