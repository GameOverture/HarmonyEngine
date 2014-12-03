#include "WidgetAtlas.h"
#include "ui_WidgetAtlas.h"

#include <QTreeWidget>
#include <QFileDialog>
#include <QStack>

WidgetAtlas::WidgetAtlas(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetAtlas)
{
    ui->setupUi(this);
}

WidgetAtlas::~WidgetAtlas()
{
    delete ui;
}

void WidgetAtlas::on_btnTexSize256_clicked()
{
    ui->sbTextureWidth->setValue(256);
    ui->sbTextureHeight->setValue(256);
    
    m_bDirty = true;
}

void WidgetAtlas::on_btnTexSize512_clicked()
{
    ui->sbTextureWidth->setValue(512);
    ui->sbTextureHeight->setValue(512);
    
    m_bDirty = true;
}

void WidgetAtlas::on_btnTexSize1024_clicked()
{
    ui->sbTextureWidth->setValue(1024);
    ui->sbTextureHeight->setValue(1024);
    
    m_bDirty = true;
}

void WidgetAtlas::on_btnTexSize2048_clicked()
{
    ui->sbTextureWidth->setValue(2048);
    ui->sbTextureHeight->setValue(2048);
    
    m_bDirty = true;
}

void WidgetAtlas::on_btnAddFiles_clicked()
{
    QFileDialog *pDlg = new QFileDialog(this);
    pDlg->setFileMode(QFileDialog::Directory);
    pDlg->setOption(QFileDialog::ShowDirsOnly, true);

    pDlg->setViewMode(QFileDialog::Detail);
    pDlg->setWindowModality( Qt::ApplicationModal );
    pDlg->setModal(true);

//    if(pDlg->exec() == QDialog::Accepted)
//    {
//        m_packer.addItem(
//        pDlg->selectedFiles()
//        ui->explorer->AddItem(ITEM_Project, , true);
//    }
    
    delete pDlg;
}

void WidgetAtlas::on_btnAddDir_clicked()
{
    QFileDialog *pDlg = new QFileDialog(this);
    pDlg->setFileMode(QFileDialog::Directory);
    pDlg->setOption(QFileDialog::ShowDirsOnly, true);

    pDlg->setViewMode(QFileDialog::Detail);
    pDlg->setWindowModality(Qt::ApplicationModal);
    pDlg->setModal(true);

    if(pDlg->exec() == QDialog::Accepted)
    {
        pDlg->selectedFiles();
    }
    
    QStringList sDirs = pDlg->selectedFiles();
    for(int iDirIndex = 0; iDirIndex < sDirs.size(); ++iDirIndex)
    {
        QDir dirEnt(sDirs[iDirIndex]);
        
        
        QFileInfoList list = dirEnt.entryInfoList();
        QStack<QFileInfoList> dirStack;
        dirStack.push(list);
        
        while(dirStack.isEmpty() == false)
        {
            list = dirStack.pop();
            for(int i = 0; i < list.count(); i++)
            {
                QFileInfo info = list[i];
        
                QString filePath = info.filePath();
                QString fileExt = info.suffix().toLower();
                QString name = dir + QDir::separator();
                if(info.isDir() && info.fileName() != ".." && info.fileName() != ".")
                {
                    QDir subDir(filePath);
                    QFileInfoList subList = subDir.entryInfoList();
                    
                    dirStack.push(subList);
                }
                else if(fileExt == "png") // Only supporting PNG for now
                {
                    if(!QFile::exists(name + info.completeBaseName() + QString(".atlas")))
                    {
                        ui-> tilesList->addItem(filePath.replace(topImageDir, ""));
                        
                        packerData *data = new packerData;
                        data->listItem = ui->tilesList->item(ui->tilesList->count() - 1);
                        data->path = info.absoluteFilePath();
                        m_packer.addItem(data->path, data);
                    }
                }
            }
        }
    }
}

void WidgetAtlas::on_cmbHeuristic_currentIndexChanged(const QString &arg1)
{
    m_bDirty = true;
}


void WidgetAtlas::on_cmbSortOrder_currentIndexChanged(const QString &arg1)
{
    m_bDirty = true;
}

void WidgetAtlas::on_cmbRotationStrategy_currentIndexChanged(const QString &arg1)
{
    m_bDirty = true;
}


void WidgetAtlas::on_sbFrameMarginTop_valueChanged(int arg1)
{
    m_bDirty = true;
}

void WidgetAtlas::on_sbFrameMarginRight_valueChanged(int arg1)
{
    m_bDirty = true;
}

void WidgetAtlas::on_sbFrameMarginBottom_valueChanged(int arg1)
{
    m_bDirty = true;
}

void WidgetAtlas::on_sbFrameMarginLeft_valueChanged(int arg1)
{
    m_bDirty = true;
}

void WidgetAtlas::on_tabWidget_currentChanged(int index)
{
    if(m_bDirty)
    {
        ui->frameList->topLevelItemCount();
    }
}
