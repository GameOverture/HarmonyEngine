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

DlgInputName::DlgInputName(const QString sDlgTitle, Item *pItem, QWidget *parent /*= 0*/) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
                                                                                            ui(new Ui::DlgInputName),
                                                                                            m_pItem(pItem)
{
    if(m_pItem == NULL)
    {
        HyGuiLog("DlgInHyGuiLogme recieved a NULL Item pointer", LOGTYPE_Error);
        return;
    }
    
    ui->setupUi(this);
    
    QFileInfo itemInfo;
    itemInfo.setFile(m_pItem->GetPath());
    m_sPathMinusName = itemInfo.path();
    m_sFileExt = HyGlobal::ItemExt(m_pItem->GetType());
    
    setWindowTitle(sDlgTitle);
    setWindowIcon(m_pItem->GetIcon());
    
    ui->txtName->setValidator(HyGlobal::FileNameValidator());
    ui->txtName->setText(m_pItem->GetName(false));
    ui->txtName->selectAll();
    
    ui->lblName->setText(HyGlobal::ItemName(m_pItem->GetType()) % " Name:");
    
    ErrorCheck();
}

DlgInputName::DlgInputName(const QString sDlgTitle, eItemType eType, const QString sPathMinusName, QWidget *parent /*= 0*/) :   QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
                                                                                                                                ui(new Ui::DlgInputName),
                                                                                                                                m_pItem(NULL)
{
    ui->setupUi(this);
    
    m_sPathMinusName = sPathMinusName;
    m_sFileExt = HyGlobal::ItemExt(eType);
    
    setWindowTitle(sDlgTitle);
    setWindowIcon(HyGlobal::ItemIcon(eType));
    
    ui->txtName->setValidator(HyGlobal::FileNameValidator());
    ui->txtName->setText("New" % HyGlobal::ItemName(eType) % "Name");
    ui->txtName->selectAll();
    
    ui->lblName->setText(HyGlobal::ItemName(eType) % " Name:");
    
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
        
        QDir itemDir(GetFullPathNameMinusExt() % m_sFileExt);
        if(itemDir.exists())
        {
            ui->lblError->setText("Error: An item with this name at this location already exists.");
            bIsError = true;
            break;
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

