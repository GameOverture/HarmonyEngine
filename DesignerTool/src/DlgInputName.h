/**************************************************************************
 *	DlgInputName.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DLGINPUTNAME_H
#define DLGINPUTNAME_H

#include <QDialog>
#include "DataExplorerItem.h"

namespace Ui {
class DlgInputName;
}

class DlgInputName : public QDialog
{
    Q_OBJECT
    
    void CtorInit(QString sDlgTitle, QString sCurName);

public:
    explicit DlgInputName(const QString sDlgTitle, QString sCurName, QWidget *pParent = 0);
    explicit DlgInputName(const QString sDlgTitle, DataExplorerItem *pItem, QWidget *parent = 0);
    ~DlgInputName();
    
    QString GetName();
    
private Q_SLOTS:
    void on_txtName_textChanged(const QString &arg1);
    
private:
    Ui::DlgInputName *ui;

    
    void ErrorCheck();
};

#endif // DLGINPUTNAME_H
