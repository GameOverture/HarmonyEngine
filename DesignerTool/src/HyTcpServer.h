#ifndef HYTCPSERVER_H
#define HYTCPSERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>

#include <QList>

class HyTcpServer : public QObject
{
    Q_OBJECT
public:
    explicit HyTcpServer(QObject *parent = 0);
    
    void Initialize();
    
signals:
    
public slots:
    
    
private:
    QTcpServer *        m_pTcpServer;
    QList<QTcpSocket *> m_TcpClients;
    
};

#endif // HYTCPSERVER_H
