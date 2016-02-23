/**************************************************************************
 *	DlgSetEngineLocation.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DLGOPENPROJECT_H
#define DLGOPENPROJECT_H

#include <QDialog>
#include <QFileSystemModel>
#include <QShowEvent>

namespace Ui {
class DlgSetEngineLocation;
}

class DlgSetEngineLocation : public QDialog
{
    Q_OBJECT
    
    QFileSystemModel *m_pFileModel;

public:
    explicit DlgSetEngineLocation(QWidget *parent = 0);
    ~DlgSetEngineLocation();
    
    QString SelectedDir();
    
private slots:
    void on_listView_doubleClicked(const QModelIndex &index);
    
    void on_txtCurDirectory_editingFinished();
    
    void on_listView_clicked(const QModelIndex &index);
    
private:
    Ui::DlgSetEngineLocation *ui;

    void ErrorCheck();
};

#endif // DLGOPENPROJECT_H
