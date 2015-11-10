#ifndef HYGUISERVER_H
#define HYGUISERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>

#include <QList>

class HyGuiServer : public QTcpSocket
{
    Q_OBJECT

public:
    explicit HyGuiServer(QObject *parent = 0);
    
    void Initialize();
    
signals:
    
public slots:
    
    
};

#endif // HYGUISERVER_H
