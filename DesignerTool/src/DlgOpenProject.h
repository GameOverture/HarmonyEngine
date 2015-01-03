#ifndef DLGOPENPROJECT_H
#define DLGOPENPROJECT_H

#include <QFileDialog>

class DlgOpenProject : public QObject
{
    Q_OBJECT
    
    QFileDialog         m_Dlg;
    
public:
    explicit DlgOpenProject(QObject *parent = 0);
    
    int Exec();
    
    QString SelectedDir();
    
signals:
    
public slots:
    void dirEntered(const QString &sDir);
    
};

#endif // DLGOPENPROJECT_H
