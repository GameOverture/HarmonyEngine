/**************************************************************************
 *	DlgNewItem.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "DlgNewItem.h"
#include "ui_DlgNewItem.h"

#include "HyGlobal.h"

#include <QDirIterator>
#include <QStringBuilder>
#include <QPushButton>

DlgNewItem::DlgNewItem(const QString &sSubDirPath, eItemType eItem, QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
    ui(new Ui::DlgNewItem)
{
    
    ui->setupUi(this);

    setWindowTitle("Add a new " % HyGlobal::ItemName(eItem) % " item");
    setWindowIcon(HyGlobal::ItemIcon(eItem));
    
    m_sSubDirPath = sSubDirPath;
    m_sItemExt = HyGlobal::ItemExt(eItem);
    
    QDir subDir(m_sSubDirPath);
    Q_ASSERT(0 == QString::compare(subDir.dirName(), HyGlobal::ItemName(HyGlobal::GetCorrespondingDirItem(eItem)), Qt::CaseInsensitive));

    ui->txtName->setValidator(HyGlobal::FileNameValidator());
    ui->txtPrefix->setValidator(HyGlobal::FilePathValidator());
    on_chkNewPrefix_stateChanged(ui->chkNewPrefix->isChecked() ? Qt::Checked : Qt::Unchecked);

    QStringList sListOfDirPrefixs;
    sListOfDirPrefixs.append(QString("<no prefix>"));
    
    QDirIterator dirIter(subDir, QDirIterator::Subdirectories);
    while(dirIter.hasNext())
    {
        QString sCurPrefixPath = dirIter.next();
        
        if(sCurPrefixPath.endsWith(QChar('.')) || dirIter.fileInfo().isDir() == false)
            continue;
        
        QString sRelativePath = subDir.relativeFilePath(sCurPrefixPath);
        sListOfDirPrefixs.append(sRelativePath);
    }
    ui->cmbPrefixList->clear();
    ui->cmbPrefixList->addItems(sListOfDirPrefixs);
    
    
    ui->lblName->setText(HyGlobal::ItemName(eItem) % " Name:");
    ui->txtName->setText("New" % HyGlobal::ItemName(eItem));
    
    
    ui->txtName->selectAll();
}

DlgNewItem::~DlgNewItem()
{
    delete ui;
}

QString DlgNewItem::GetName()
{
    return ui->txtName->text();
}

QString DlgNewItem::GetPrefix()
{
    if(ui->chkNewPrefix->isChecked())
        return QDir::cleanPath(ui->txtPrefix->text());
    else
        return ui->cmbPrefixList->currentIndex() == 0 ? QString() : ui->cmbPrefixList->currentText();
}

void DlgNewItem::on_chkNewPrefix_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked)
    {
        ui->txtPrefix->setVisible(true);
        ui->txtPrefix->setFocus();
        ui->txtPrefix->selectAll();
        ui->lblPrefixInstructions->setText(QString("e.g. \"<b>Level2</b>\" or \"<b>Level3/Intro</b>\""));

        ui->cmbPrefixList->setVisible(false);
    }
    else
    {
        ui->txtPrefix->setVisible(false);
        ui->lblPrefixInstructions->setText("");

        ui->cmbPrefixList->setVisible(true);
    }

    ErrorCheck();
}

void DlgNewItem::on_txtName_textChanged(const QString &arg1)
{
    ErrorCheck();
}

void DlgNewItem::on_txtPrefix_textChanged(const QString &arg1)
{
    ErrorCheck();
}

void DlgNewItem::on_cmbPrefixList_currentIndexChanged(const QString &arg1)
{
    ErrorCheck();
}

void DlgNewItem::ErrorCheck()
{
    QString sPrefix;
    
    if(ui->chkNewPrefix->isChecked())
        sPrefix = ui->txtPrefix->text();
    else if(ui->cmbPrefixList->currentIndex() != 0)
        sPrefix = ui->cmbPrefixList->currentText();
    
    bool bIsError = false;
    do
    {
        QStringList sPrefixDirList = sPrefix.split('/');
        for(int i = 0; i < sPrefixDirList.size() && bIsError == false; ++i)
        {
            QStringList sSubDirList = HyGlobal::SubDirNameList();
            for(int j = 0; j < sSubDirList.size() && bIsError == false; ++j)
            {
                if(sPrefixDirList[i].compare(sSubDirList[j], Qt::CaseInsensitive) == 0 &&
                   ui->txtName->text().compare(sSubDirList[j], Qt::CaseInsensitive) == 0)
                {
                    ui->lblError->setText("Error: The prefix and/or name is using a reserved name.");
                    bIsError = true;
                    break;
                }
            }
        }
        
        if(ui->txtName->text().isEmpty())
        {
            ui->lblError->setText("Error: An item's name cannot be blank");
            bIsError = true;
            break;
        }
        
        QString sNewItemPath = m_sSubDirPath % '/' % sPrefix % '/' % ui->txtName->text() % m_sItemExt;
        QDir newItemDir(sNewItemPath);
        if(newItemDir.exists())
        {
            ui->lblError->setText("Error: An item with this name at this location already exists.");
            bIsError = true;
            break;
        }

        if(ui->chkNewPrefix->isChecked())
        {
            QString sPrefixPath = m_sSubDirPath % '/' % ui->txtPrefix->text();
            QDir prefixDir(sPrefixPath);
            if(prefixDir.exists())
            {
                ui->lblError->setText("Error: This prefix already exists.");
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

void DlgNewItem::on_buttonBox_accepted()
{
    if(ui->chkNewPrefix->isChecked())
    {
        QDir itemDir(m_sSubDirPath);
        itemDir.mkpath(ui->txtPrefix->text());
    }
}
