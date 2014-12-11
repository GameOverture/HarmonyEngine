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
        QDir dirEntry(sDirs[iDirIndex]);
        
        
        QFileInfoList list = dirEntry.entryInfoList();
        QStack<QFileInfoList> dirStack;
        dirStack.push(list);
        
        while(dirStack.isEmpty() == false)
        {
            list = dirStack.pop();
            for(int i = 0; i < list.count(); i++)
            {
                QFileInfo info = list[i];

                if(info.isDir() && info.fileName() != ".." && info.fileName() != ".")
                {
                    QDir subDir(info.filePath());
                    QFileInfoList subList = subDir.entryInfoList();
                    
                    dirStack.push(subList);
                }
                else if(info.suffix().toLower() == "png") // Only supporting PNG for now
                {
                    ImportImage(info.filePath());
//                    AddFrame
//                    if(!QFile::exists(name + info.completeBaseName() + QString(".atlas")))
//                    {
//                        ui-> tilesList->addItem(filePath.replace(topImageDir, ""));
                        
//                        packerData *data = new packerData;
//                        data->listItem = ui->tilesList->item(ui->tilesList->count() - 1);
//                        data->path = info.absoluteFilePath();
//                        m_Atlases.addItem(data->path, data);
//                    }
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

void WidgetAtlas::ImportImage(QString sImagePath)
{
    QImage img(sImagePath);
    
    m_Textures[0].packer.area
    img.save(
    int iLastIndex = m_Textures.size() - 1;
    
    //                        packerData *data = new packerData;
    //                        data->listItem = ui->tilesList->item(ui->tilesList->count() - 1);
    //                        data->path = info.absoluteFilePath();
    //                        m_Atlases.addItem(data->path, data);
    
    m_Textures[iLastIndex].addItem(sImagePath, 
    sImagePath
}

QTreeWidgetItem *WidgetAtlas::CreateTreeItem(QTreeWidgetItem *pParent)
{
    QTreeWidgetItem *pNewTreeItem;
    if(pParent == NULL)
        pNewTreeItem = new QTreeWidgetItem(ui->frameList);
    else
        pNewTreeItem = new QTreeWidgetItem();
    
    pNewTreeItem->setText(0, pItem->GetName());
    pNewTreeItem->setIcon(0, pItem->GetIcon());
//    pNewTreeItem->setFlags(pNewItem->flags() | Qt::ItemIsEditable);
    
    QVariant v; v.setValue(pItem);
    pNewTreeItem->setData(0, Qt::UserRole, v);
    
    if(pParent)
        pParent->addChild(pNewTreeItem);
    
    return pNewTreeItem;
}

