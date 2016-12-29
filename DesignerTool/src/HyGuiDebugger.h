/**************************************************************************
 *	HyGuiDebugger.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HYGUIDEBUGGER_H
#define HYGUIDEBUGGER_H

#include <QObject>
#include <QAction>
#include <QTcpSocket>
#include <QTcpServer>

#include <QList>

#include "Harmony/HyEngine.h"

class HyGuiDebugger : public QObject
{
    Q_OBJECT

    QAction &       m_ActionConnectRef;

    QTcpSocket      m_Socket;

    QHostAddress    m_Address;
    quint16         m_uiPort;

    quint32         m_uiPacketType;
    quint32         m_uiPacketSize;

public:
    explicit HyGuiDebugger(QAction &actionConnectRef, QObject *parent = 0);

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

#endif // HYGUIDEBUGGER_H
