#include "HyGuiServer.h"

#include "Harmony/HyEngine.h"
#include "HyGlobal.h"

HyGuiServer::HyGuiServer(QObject *parent) : QObject(parent)
{
    m_TcpServer.listen(QHostAddress::LocalHost, 1313);
}

void HyGuiServer::Initialize()
{
    
}

void HyGuiServer::newConnection()
{
    QTcpSocket *pNewSocket = nextPendingConnection();

    if(QAbstractSocket::ConnectedState == pNewSocket->state())
    {
        m_TcpClients.push_back(pNewSocket);
    }
}
