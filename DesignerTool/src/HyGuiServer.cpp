#include "HyGuiServer.h"

#include "Harmony/HyEngine.h"
#include "HyGlobal.h"

HyGuiServer::HyGuiServer(QObject *parent) : QTcpServer(parent)
{
    //m_TcpServer.listen(QHostAddress::LocalHost, 1313);
}

void HyGuiServer::Initialize()
{
    
}

