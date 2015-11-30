#include "DlgOpenProject.h"
#include "ui_DlgOpenProject.h"

#include <QFileDialog>


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

    
    m_pDirModel = new QFileSystemModel(this);
    m_pDirModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    m_pDirModel->setRootPath(QDir::currentPath());
    
    ui->treeView->setModel(m_pDirModel);
    
    m_pFileModel = new QFileSystemModel(this);
    m_pFileModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    m_pFileModel->setRootPath(QDir::currentPath());
    
    ui->listView->setModel(m_pFileModel);
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

void DlgOpenProject::on_treeView_clicked(const QModelIndex &index)
{
    QString sPath = m_pDirModel->fileInfo(index).absoluteFilePath();
    ui->listView->setRootIndex(m_pFileModel->setRootPath(sPath));
    
    ui->txtCurDirectory->setText(sPath);
}

void DlgOpenProject::on_listView_doubleClicked(const QModelIndex &index)
{
    QString sPath = m_pFileModel->fileInfo(index).absoluteFilePath();
    
    ui->listView->setRootIndex(index);
    ui->treeView->setSelectionModel(ui->listView->selectionModel());// setRootIndex(m_pDirModel->setRootPath(sPath));
    
    ui->txtCurDirectory->setText(sPath);
}

void DlgOpenProject::on_txtCurDirectory_editingFinished()
{
    QString sPath = ui->txtCurDirectory->text();// m_pDirModel->fileInfo(index).absoluteFilePath();
    ui->treeView->setRootIndex(m_pFileModel->setRootPath(sPath));
    
    //ui->txtCurDirectory->setText(sPath);
}
