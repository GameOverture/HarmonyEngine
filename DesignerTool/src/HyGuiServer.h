#ifndef HYGUISERVER_H
#define HYGUISERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>

#include <QList>

class HyGuiServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit HyGuiServer(QObject *parent = 0);
    
    void Initialize();
    
signals:
    
public slots:
    
    
private:
    QList<QTcpSocket *> m_TcpClients;
    
};

#endif // HYGUISERVER_H
