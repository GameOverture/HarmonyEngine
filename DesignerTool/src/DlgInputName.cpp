/**************************************************************************
 *	DlgInputName.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "DlgInputName.h"
#include "ui_DlgInputName.h"

#include <QPushButton>
#include <QStringBuilder>

void DlgInputName::CtorInit(QString sDlgTitle, QString sCurName)
{
    ui->setupUi(this);

    setWindowTitle(sDlgTitle);
    setWindowIcon(QIcon(":/icons16x16/generic-rename.png"));

    ui->txtName->setValidator(HyGlobal::FileNameValidator());
    ui->txtName->setText(sCurName);
    ui->txtName->selectAll();

    ui->lblName->setText("Name:");
}

DlgInputName::DlgInputName(const QString sDlgTitle, QString sCurName, QWidget *pParent /*= 0*/) :   QDialog(pParent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
                                                                                                    ui(new Ui::DlgInputName)
{
    CtorInit(sDlgTitle, sCurName);
}

DlgInputName::DlgInputName(const QString sDlgTitle, Item *pItem, QWidget *parent /*= 0*/) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
                                                                                            ui(new Ui::DlgInputName)
{
    CtorInit(sDlgTitle, pItem->GetName(false));

    if(pItem == NULL)
    {
        HyGuiLog("DlgInputName recieved a NULL Item pointer", LOGTYPE_Error);
        return;
    }

    QFileInfo itemInfo;
    itemInfo.setFile(pItem->GetAbsPath());
    m_sPathMinusName = itemInfo.path();
    m_sFileExt = HyGlobal::ItemExt(pItem->GetType());
    
    setWindowIcon(pItem->GetIcon());
    ui->lblName->setText(HyGlobal::ItemName(pItem->GetType()) % " Name:");
    
    ErrorCheck();
}

DlgInputName::~DlgInputName()
{
    delete ui;
}

void DlgInputName::on_txtName_textChanged(const QString &arg1)
{
    ErrorCheck();
}

QString DlgInputName::GetName()
{
    return ui->txtName->text();
}

QString DlgInputName::GetFullPathNameMinusExt()
{
    return QDir::cleanPath(m_sPathMinusName % "/" % ui->txtName->text());
}

void DlgInputName::ErrorCheck()
{
    bool bIsError = false;
    do
    {
        if(ui->txtName->text().isEmpty())
        {
            ui->lblError->setText("Error: An item's name cannot be blank");
            bIsError = true;
            break;
        }
        
        if(m_sPathMinusName.isEmpty() == false)
        {
            QDir itemDir(GetFullPathNameMinusExt() % m_sFileExt);
            if(itemDir.exists())
            {
                ui->lblError->setText("Error: An item with this name at this location already exists.");
                bIsError = true;
                break;
            }
        }
    }while(false);

    if(bIsError)
        ui->lblError->setStyleSheet("QLabel { background-color : red; color : black; }");
    else
    {
        ui->lblError->setStyleSheet("QLabel { color : black; }");
        ui->lblError->setText("");
    }
    ui->lblError->setVisible(bIsError);
    ui->buttonBox->button(QDialogButtonBox::Ok);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!bIsError);
}

