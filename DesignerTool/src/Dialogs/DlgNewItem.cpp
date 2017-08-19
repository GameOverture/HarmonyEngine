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

#include "HyGuiGlobal.h"

#include <QDirIterator>
#include <QStringBuilder>
#include <QPushButton>

DlgNewItem::DlgNewItem(Project *pItemProject, HyGuiItemType eItem, QWidget *parent) :   QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
                                                                                        ui(new Ui::DlgNewItem),
                                                                                        m_pItemProject(pItemProject),
                                                                                        m_eItemType(eItem)
{
    if(m_pItemProject == nullptr)
        HyGuiLog("DlgNewItem::DlgNewItem was given a Project * that was nullptr", LOGTYPE_Error);

    ui->setupUi(this);

    setWindowTitle("Add a new " % HyGlobal::ItemName(eItem) % " item");
    setWindowIcon(HyGlobal::ItemIcon(eItem, SUBICON_New));
    
    ui->txtName->setValidator(HyGlobal::FileNameValidator());
    ui->txtPrefix->setValidator(HyGlobal::FilePathValidator());
    on_chkNewPrefix_stateChanged(ui->chkNewPrefix->isChecked() ? Qt::Checked : Qt::Unchecked);

    m_PrefixStringList.clear();

    QTreeWidgetItem *pSubDirItem = nullptr;
    for(int i = 0; i < m_pItemProject->GetTreeItem()->childCount(); ++i)
    {
        pSubDirItem = m_pItemProject->GetTreeItem()->child(i);
        DataExplorerItem *pCurItem = pSubDirItem->data(0, Qt::UserRole).value<DataExplorerItem *>();

        if(pCurItem->GetType() == HyGlobal::GetCorrespondingDirItem(eItem))
            break;
    }

    // prefixes in subdir
    QList<QTreeWidgetItem *> itemList = HyGlobal::RecursiveTreeChildren(pSubDirItem);
    for(int i = 0; i < itemList.size(); ++i)
    {
        if(itemList[i] == pSubDirItem)
            continue;

        DataExplorerItem *pItem = itemList[i]->data(0, Qt::UserRole).value<DataExplorerItem *>();
        if(pItem->GetType() == ITEM_Prefix)
            m_PrefixStringList.append(pItem->GetPrefix());
    }

    qSort(m_PrefixStringList.begin(), m_PrefixStringList.end());
    m_PrefixStringList.prepend(QString("<no prefix>"));

    ui->cmbPrefixList->clear();
    ui->cmbPrefixList->addItems(m_PrefixStringList);
    
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
        return QString(MakeStringProperPath(ui->txtPrefix->text().toStdString().c_str(), nullptr, false).c_str());
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
    QString sPrefix = GetPrefix();
    if(sPrefix.endsWith('/', Qt::CaseInsensitive) == false)
        sPrefix += "/";
    
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
        
        bool bFoundDup = false;
        if(ui->chkNewPrefix->isChecked())
        {
            for(uint i = 0; i < m_PrefixStringList.size(); ++i)
            {
                if(0 == sPrefix.compare(m_PrefixStringList[i], Qt::CaseInsensitive))
                {
                    bFoundDup = true;
                    break;
                }
            }
            if(bFoundDup)
            {
                ui->lblError->setText("Error: This prefix already exists.");
                bIsError = true;
                break;
            }
        }

        QString sNewItemPath = sPrefix % '/' % ui->txtName->text();
        QJsonObject subDirObj = m_pItemProject->GetSubDirObj(HyGlobal::GetCorrespondingDirItem(m_eItemType));
        for(auto objsInSubDirIter = subDirObj.begin(); objsInSubDirIter != subDirObj.end(); ++objsInSubDirIter)
        {
            QString sItemPath = objsInSubDirIter.key();
            if(0 == sNewItemPath.compare(sItemPath, Qt::CaseInsensitive))
            {
                bFoundDup = true;
                break;
            }
        }
        if(bFoundDup)
        {
            ui->lblError->setText("Error: An item with this name at this prefix already exists.");
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

void DlgNewItem::on_buttonBox_accepted()
{
}
