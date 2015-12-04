#include "DlgOpenProject.h"
#include "ui_DlgOpenProject.h"

#include <QFileDialog>

#include "HyGlobal.h"


DlgOpenProject::DlgOpenProject(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgOpenProject)
{
    ui->setupUi(this);
    
    QString sTempDir = "C:/soft";


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

DlgOpenProject::~DlgOpenProject()
{
    delete ui;
}

QString DlgOpenProject::SelectedDir()
{
    return ui->txtCurDirectory->text();
}

void DlgOpenProject::on_listView_doubleClicked(const QModelIndex &index)
{
    ui->listView->setRootIndex(index);
    
    QString sPath = m_pFileModel->fileInfo(index).absoluteFilePath();
    ui->txtCurDirectory->setText(sPath);

    ErrorCheck();
}

void DlgOpenProject::on_txtCurDirectory_editingFinished()
{
    QString sPath = ui->txtCurDirectory->text();// m_pDirModel->fileInfo(index).absoluteFilePath();
    ui->listView->setRootIndex(m_pFileModel->setRootPath(sPath));
    
    //ui->txtCurDirectory->setText(sPath);

    ErrorCheck();
}

void DlgOpenProject::ErrorCheck()
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

        if(HyGlobal::IsWorkspaceValid(projDir) == false)
        {
            ui->lblError->setText("Error: This is not a valid Harmony workspace directory");
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
