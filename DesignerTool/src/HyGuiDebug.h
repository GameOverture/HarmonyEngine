#ifndef HYGUIDEBUG_H
#define HYGUIDEBUG_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>

#include <QList>

class HyGuiDebug : public QTcpSocket
{
    Q_OBJECT

    bool        m_bConnected;

public:
    explicit HyGuiDebug(QObject *parent = 0);
    
    void Initialize();
    
signals:
    
public slots:
    void hostFound();
    
};

#endif // HYGUIDEBUG_H
