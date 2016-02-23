/**************************************************************************
 *	DlgNewItem.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DLGNEWSPRITE_H
#define DLGNEWSPRITE_H

#include <QDialog>

#include "HyGlobal.h"

namespace Ui {
class DlgNewItem;
}

class DlgNewItem : public QDialog
{
    Q_OBJECT

    QString         m_sSubDirPath;
    QString         m_sItemExt;

public:
    explicit DlgNewItem(const QString &sSubDirPath, eItemType eItem, QWidget *parent = 0);
    ~DlgNewItem();
    
    QString GetName();
    
    QString GetPrefix();

private slots:
    void on_chkNewPrefix_stateChanged(int arg1);

    void on_txtPrefix_textChanged(const QString &arg1);

    void on_cmbPrefixList_currentIndexChanged(const QString &arg1);

    void on_txtName_textChanged(const QString &arg1);

    void on_buttonBox_accepted();
    
private:
    Ui::DlgNewItem *ui;

    void ErrorCheck();
};

#endif // DLGNEWSPRITE_H
