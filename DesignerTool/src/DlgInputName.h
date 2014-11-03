#ifndef DLGINPUTNAME_H
#define DLGINPUTNAME_H

#include <QDialog>
#include "Item.h"

namespace Ui {
class DlgInputName;
}

class DlgInputName : public QDialog
{
    Q_OBJECT
    
    QString         m_sPathMinusName;
    QString         m_sFileExt;
    Item *          m_pItem;
    
public:
    explicit DlgInputName(const QString sDlgTitle, Item *pItem, QWidget *parent = 0);
    explicit DlgInputName(const QString sDlgTitle, eItemType eType, const QString sPathMinusName, QWidget *parent = 0);
    ~DlgInputName();
    
    QString GetFullPathNameMinusExt();
    
private slots:
    void on_txtName_textChanged(const QString &arg1);
    
private:
    Ui::DlgInputName *ui;
    
    void ErrorCheck();
};

#endif // DLGINPUTNAME_H
