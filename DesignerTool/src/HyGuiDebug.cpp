#include "HyGuiDebug.h"

#include "Harmony/HyEngine.h"
#include "HyGlobal.h"

HyGuiDebug::HyGuiDebug(QObject *parent) :   QTcpSocket(parent),
                                            m_bConnected(false)
{
    //m_Socket.connect(
}

void HyGuiDebug::Initialize()
{
    
}

void HyGuiDebug::hostFound()
{
    m_bConnected = true;
}
