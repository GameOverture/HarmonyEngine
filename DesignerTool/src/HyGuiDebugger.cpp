#include "HyGuiDebugger.h"

#include "Harmony/HyEngine.h"
#include "HyGlobal.h"

HyGuiDebugger::HyGuiDebugger(QAction &actionConnectRef, QObject *parent) :  QObject(parent),
                                                                            m_ActionConnectRef(actionConnectRef),
                                                                            m_Socket(this),
                                                                            m_Address(QHostAddress::LocalHost),
                                                                            m_uiPort(1313),
                                                                            m_uiPacketSize(0)
{
    //m_Socket.connect(
    //connect(m_pTcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));

    connect(&m_Socket, SIGNAL(readyRead()), this, SLOT(ReadData()));
    connect(&m_Socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnError(QAbstractSocket::SocketError)));
}

void HyGuiDebugger::Connect()
{
    if(m_Socket.isOpen())
    {
        HYLOG("Debugger is already has an open connection", LOGTYPE_Warning);
        return;
    }

    m_Socket.abort();
    m_Socket.connectToHost(m_Address, m_uiPort);
}

void HyGuiDebugger::Write(eHyPacketType eType, quint32 uiSize, void *pData)
{
    QByteArray testBuffer;
    
    quint32 n = eType;
    testBuffer.append(reinterpret_cast<const char *>(&n), 4);

    n = uiSize;
    testBuffer.append(reinterpret_cast<const char *>(&n), 4);

    testBuffer.append(reinterpret_cast<char *>(pData), uiSize);
    
    m_Socket.write(testBuffer);
}

void HyGuiDebugger::hostFound()
{
}

void HyGuiDebugger::ReadData()
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
        HYLOG(sMessage, LOGTYPE_Normal);
        break;
    case HYPACKET_LogWarning:
        HYLOG(sMessage, LOGTYPE_Warning);
        break;
    case HYPACKET_LogError:
        HYLOG(sMessage, LOGTYPE_Error);
        break;
    case HYPACKET_LogInfo:
        HYLOG(sMessage, LOGTYPE_Info);
        break;
    case HYPACKET_LogTitle:
        HYLOG(sMessage, LOGTYPE_Title);
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

void HyGuiDebugger::OnError(QAbstractSocket::SocketError socketError)
{
     switch (socketError)
     {
     case QAbstractSocket::RemoteHostClosedError:
         HYLOG("The game debugger connection has been lost", LOGTYPE_Info);
         break;

     case QAbstractSocket::HostNotFoundError:
         HYLOG("The game hosted at [" % m_Address.toString() % "] port: " % QString(m_uiPort) % " was not found", LOGTYPE_Error);
         break;

     case QAbstractSocket::ConnectionRefusedError:
         HYLOG("The game hosted at [" % m_Address.toString() % "] port: " % QString(m_uiPort) % " has refused the connection", LOGTYPE_Error);
         break;
     default:
         HYLOG("Debugger Socket Error: " % m_Socket.errorString(), LOGTYPE_Error);
         break;
     }

     m_ActionConnectRef.setChecked(false);
 }
