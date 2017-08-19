/**************************************************************************
 *	DlgSetEngineLocation.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "DlgSetEngineLocation.h"
#include "ui_DlgSetEngineLocation.h"

#include <QFileDialog>

#include "HyGuiGlobal.h"


DlgSetEngineLocation::DlgSetEngineLocation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgSetEngineLocation)
{
    ui->setupUi(this);
    
    //QString sTempDir = "C:/soft";


    //QFileDialog *fd = new QFileDialog;

//    fd->
//    QTreeView *tree = fd->findChild <QTreeView*>();
//    tree->setRootIsDecorated(true);
//    tree->setItemsExpandable(true);
//    fd->setFileMode(QFileDialog::Directory);
//    fd->setOption(QFileDialog::ShowDirsOnly);
//    fd->setViewMode(QFileDialog::Detail);
//    int result = fd->exec();
//    QString directory;
//    if (result)
//    {
//        directory = fd->selectedFiles()[0];
//        qDebug()<<directory;
//    }
    
    m_pFileModel = new QFileSystemModel(this);
    m_pFileModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    m_pFileModel->setRootPath(QDir::currentPath());
    
    ui->listView->setModel(m_pFileModel);

    ErrorCheck();
}

DlgSetEngineLocation::~DlgSetEngineLocation()
{
    delete ui;
}

QString DlgSetEngineLocation::SelectedDir()
{
    return ui->txtCurDirectory->text();
}

void DlgSetEngineLocation::on_listView_doubleClicked(const QModelIndex &index)
{
    if(ui->buttonBox->button(QDialogButtonBox::Ok)->isEnabled() == false)
        ui->listView->setRootIndex(index);
    
    QString sPath = m_pFileModel->fileInfo(index).absoluteFilePath();
    ui->txtCurDirectory->setText(sPath);

    ErrorCheck();
}

void DlgSetEngineLocation::on_txtCurDirectory_editingFinished()
{
    QString sPath = ui->txtCurDirectory->text();// m_pDirModel->fileInfo(index).absoluteFilePath();
    ui->listView->setRootIndex(m_pFileModel->setRootPath(sPath));
    
    //ui->txtCurDirectory->setText(sPath);

    ErrorCheck();
}

void DlgSetEngineLocation::ErrorCheck()
{
    QString sProjDir = ui->txtCurDirectory->text();

    bool bIsError = false;
    do
    {
        if(sProjDir.isEmpty())
        {
            ui->lblError->setText("Error: The directory path cannot be blank");
            bIsError = true;
            break;
        }

        QDir projDir(sProjDir);
        if(projDir.exists() == false)
        {
            ui->lblError->setText("Error: This directory does not exist");
            bIsError = true;
            break;
        }

        if(HyGlobal::IsEngineDirValid(projDir) == false)
        {
            ui->lblError->setText("Error: This is not the Harmony Engine project directory");
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
    //ui->buttonBox->button(QDialogButtonBox::Ok);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!bIsError);
}

void DlgSetEngineLocation::on_listView_clicked(const QModelIndex &index)
{
    QString sPath = m_pFileModel->fileInfo(index).absoluteFilePath();
    ui->txtCurDirectory->setText(sPath);

    ErrorCheck();
}
