#include "WidgetAtlas.h"
#include "ui_WidgetAtlas.h"

#include "MainWindow.h"

#include <QTreeWidget>
#include <QFileDialog>
#include <QStack>

WidgetAtlas::WidgetAtlas(ItemProject *pProjOwner, QWidget *parent) :    QWidget(parent),
                                                                        ui(new Ui::WidgetAtlas),
                                                                        m_pProjOwner(pProjOwner)
{
    ui->setupUi(this);
}

WidgetAtlas::~WidgetAtlas()
{
    while(m_Textures.empty() == false)
    {
        tTexture *pTex = m_Textures.front();
        delete pTex;
        
        m_Textures.pop_front();
    }
    
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
        
        bool bErrorOccurred = false;
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
                    if(ImportImage(info.filePath()) == false)
                        bErrorOccurred = true;
                }
            }
        }
        
        if(bErrorOccurred)
            HYLOG("Could not import image(s). Check log.", LOGTYPE_Warning);
    }
    
    // Display texture
    MainWindow::OpenItem(this->m_pProjOwner);
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

bool WidgetAtlas::ImportImage(QString sImagePath)
{
    bool bIsPacked = false;
    tTexture *pTexture = NULL;
    
    for(int i = 0; i < m_Textures.size(); ++i)
    {
        if(TryPackNew(sImagePath, m_Textures[i]))
        {
            pTexture = m_Textures[i];
            bIsPacked = true;
            break;
        }
    }
    
    // Could not fit this image on any available atlas texture, try one more
    // time with a new, empty texture
    if(bIsPacked == false)
    {
        pTexture = new tTexture;
        m_Textures.append(pTexture);
        
        if(TryPackNew(sImagePath, pTexture))
        {
            QString sName;
            sName.sprintf("%05i", m_Textures.size() - 1);
            CreateTreeItem(NULL, sName, ATLAS_Texture);
            
            bIsPacked = true;
        }
        else
        {
            HYLOG("Cannot pack image: " % sImagePath, LOGTYPE_Info);
            m_Textures.removeOne(pTexture);
            
            delete pTexture;
            pTexture = NULL;
        }
    }
    
    if(bIsPacked)
        pTexture->bDirty = true;
    
    return bIsPacked;
}

bool WidgetAtlas::TryPackNew(QString sImagePath, tTexture *pTex)
{
    QImage *pImg = new QImage(sImagePath);
    
    int iNeededSpace = pImg->width() * pImg->height();
    int iRemainingSpace = ((ui->sbTextureWidth->value() * ui->sbTextureHeight->value()) - pTex->packer.area);
    if(iNeededSpace <= iRemainingSpace)
    {
        pTex->packer.addItem(sImagePath, pImg);
        pTex->packer.pack(ui->cmbHeuristic->currentIndex(), ui->sbTextureWidth->value(), ui->sbTextureHeight->value());
        
        if(pTex->packer.missingImages)
        {
            pTex->packer.removeId(pImg);
            
            delete pImg;
            return false;
        }
        
        QFileInfo imagePathInfo(sImagePath);
        
        // Add tree item
        CreateTreeItem(&pTex->treeItem, imagePathInfo.baseName(), ATLAS_Frame);
       
        // Save the source image in the _metaData, which is needed for whenever the texture needs to be rebuilt.
        QString sPath;
        // TODO: Check to see when loading that 'textureId' matches in both the 'packer' and the filename.
        sPath.sprintf("atlases/%05i-%05i-%s", m_Textures.size() - 1, pTex->packer.images.last().textureId, imagePathInfo.fileName());
        pImg->save(m_pProjOwner->GetMetaDataPath() % sPath);
        
        return true;
    }
    
    delete pImg;
    return false;
}

QTreeWidgetItem *WidgetAtlas::CreateTreeItem(QTreeWidgetItem *pParent, QString sName, eAtlasNodeType eType)
{
    QTreeWidgetItem *pNewTreeItem;
    if(pParent == NULL)
        pNewTreeItem = new QTreeWidgetItem(ui->frameList);
    else
        pNewTreeItem = new QTreeWidgetItem();
    
    pNewTreeItem->setText(0, sName);
    pNewTreeItem->setIcon(0, HyGlobal::AtlasIcon(eType));
    
//    QVariant v; v.setValue(pItem);
//    v.in
//    pNewTreeItem->setData(0, Qt::UserRole, v);

    if(pParent)
        pParent->addChild(pNewTreeItem);

    return pNewTreeItem;

    return NULL;
}

void WidgetAtlas::RebuildDirtyTextures()
{
    for(int i = 0; i < m_Textures.size(); ++i)
    {
        if(m_Textures[i]->bDirty)
        {
            QList<inputImage> imgs = m_Textures[i]->packer.images;
            for(int j = 0; j < imgs.size(); ++j)
            {
                //imgs[j].
            }
        }
    }
}

