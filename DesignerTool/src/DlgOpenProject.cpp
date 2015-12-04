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

void DlgOpenProject::showEvent(QShowEvent *pEvent)
{
    QDialog::showEvent(pEvent);
    if(pEvent->spontaneous())
        return;

    ui->treeView->setColumnWidth(0, ui->treeView->width());
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


}

void DlgOpenProject::on_txtCurDirectory_editingFinished()
{
    QString sPath = ui->txtCurDirectory->text();// m_pDirModel->fileInfo(index).absoluteFilePath();
    ui->treeView->setRootIndex(m_pFileModel->setRootPath(sPath));
    
    //ui->txtCurDirectory->setText(sPath);
}

void DlgOpenProject::ErrorCheck()
{
    QString sProjDir = ui->txtCurDirectory->text();
    QDir projDir(sProjDir);

    bool bIsError = false;
    do
    {
        if(ui->txtCurDirectory->text().isEmpty())
        {
            ui->lblError->setText("Error: The directory path cannot be blank");
            bIsError = true;
            break;
        }

        if(projDir.exists() == false)
        {
            ui->lblError->setText("Error: This directory does not exist");
            bIsError = true;
            break;
        }

        if(HyGlobal::IsWorkspaceValid(QDir(pItem->GetPath())) == false)
        {
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
    //ui->buttonBox->button(QDialogButtonBox::Ok);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!bIsError);
}
