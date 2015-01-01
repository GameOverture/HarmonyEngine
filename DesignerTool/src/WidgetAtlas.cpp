#include "WidgetAtlas.h"
#include "ui_WidgetAtlas.h"

#include "MainWindow.h"

#include <QTreeWidget>
#include <QFileDialog>
#include <QStack>

#include "HyGuiTexture.h"
#include "scriptum/imagepacker.h"

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
        HyGuiTexture *pTex = m_Textures.front();
        delete pTex;
        
        m_Textures.pop_front();
    }
    
    delete ui;
}

void WidgetAtlas::SetPackerSettings(ImagePacker *pPacker)
{
    pPacker->sortOrder = ui->cmbSortOrder->currentIndex();
    pPacker->border.t = ui->sbFrameMarginTop->value();
    pPacker->border.l = ui->sbFrameMarginLeft->value();
    pPacker->border.r = ui->sbFrameMarginRight->value();
    pPacker->border.b = ui->sbFrameMarginBottom->value();
    pPacker->extrude = ui->extrude->value();
    pPacker->merge = ui->chkMerge->isChecked();
    pPacker->square = ui->chkSquare->isChecked();
    pPacker->autosize = ui->chkAutosize->isChecked();
    pPacker->minFillRate = ui->minFillRate->value();
    pPacker->mergeBF = false;
    pPacker->rotate = ui->cmbRotationStrategy->currentIndex();
}

int WidgetAtlas::GetTexWidth()
{
    return ui->sbTextureWidth->value();
}
int WidgetAtlas::GetTexHeight()
{
    return ui->sbTextureHeight->value();
}
int WidgetAtlas::GetHeuristicIndex()
{
    return ui->cmbHeuristic->currentIndex();
}

void WidgetAtlas::on_btnTexSize256_clicked()
{
    ui->sbTextureWidth->setValue(256);
    ui->sbTextureHeight->setValue(256);
    
    m_bSettingsDirty = true;
}

void WidgetAtlas::on_btnTexSize512_clicked()
{
    ui->sbTextureWidth->setValue(512);
    ui->sbTextureHeight->setValue(512);
    
    m_bSettingsDirty = true;
}

void WidgetAtlas::on_btnTexSize1024_clicked()
{
    ui->sbTextureWidth->setValue(1024);
    ui->sbTextureHeight->setValue(1024);
    
    m_bSettingsDirty = true;
}

void WidgetAtlas::on_btnTexSize2048_clicked()
{
    ui->sbTextureWidth->setValue(2048);
    ui->sbTextureHeight->setValue(2048);
    
    m_bSettingsDirty = true;
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
    QStringList sImportImgList;
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
                    sImportImgList.push_back(info.filePath());
                }
            }
        }
    }
    
    QList<QStringList> sMissingImgPaths = GetActiveTexture()->ImportImgs(sImportImgList);
    bool bFailedToPack = false;
    for(int i = 0; i < sMissingImgPaths.size(); ++i)
    {
        HyGuiTexture *pNewTexture = new HyGuiTexture(this);
        m_Textures.append(pNewTexture);
    
        QList<QStringList> tmp = pNewTexture->ImportImgs(sMissingImgPaths[i]);
        if(tmp.size() > 0)
        {
            bFailedToPack = true;
            for(int j = 0; j < tmp.size(); ++j)
            {
                foreach(const QString sPath, tmp[j])
                    HYLOG("Could not import: " % sPath, LOGTYPE_Info);
            }
        }
    }
    
    GenTextureSheets();
    
    if(bFailedToPack)
        HYLOG("Imported image(s) failed to pack into current texture settings. Check log for info.", LOGTYPE_Warning);
    
    // Display texture
    MainWindow::OpenItem(this->m_pProjOwner);
}

void WidgetAtlas::on_cmbHeuristic_currentIndexChanged(const QString &arg1)
{
    m_bSettingsDirty = true;
}


void WidgetAtlas::on_cmbSortOrder_currentIndexChanged(const QString &arg1)
{
    m_bSettingsDirty = true;
}

void WidgetAtlas::on_cmbRotationStrategy_currentIndexChanged(const QString &arg1)
{
    m_bSettingsDirty = true;
}


void WidgetAtlas::on_sbFrameMarginTop_valueChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void WidgetAtlas::on_sbFrameMarginRight_valueChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void WidgetAtlas::on_sbFrameMarginBottom_valueChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void WidgetAtlas::on_sbFrameMarginLeft_valueChanged(int arg1)
{
    m_bSettingsDirty = true;
}

void WidgetAtlas::on_tabWidget_currentChanged(int index)
{
    if(m_bSettingsDirty)
    {
        ui->frameList->topLevelItemCount();
    }
}

HyGuiTexture *WidgetAtlas::GetActiveTexture()
{
    if(m_Textures.empty())
        m_Textures.append(new HyGuiTexture(this));
    
    return m_Textures.last();
}

void WidgetAtlas::GenTextureSheets()
{
    for(int i = 0; i < m_Textures.size(); ++i)
    {
        
    }
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

