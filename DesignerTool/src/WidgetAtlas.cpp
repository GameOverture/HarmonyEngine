#include "WidgetAtlas.h"
#include "ui_WidgetAtlas.h"

#include "MainWindow.h"

#include <QTreeWidget>
#include <QFileDialog>
#include <QStack>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>

#include "HyGuiTexture.h"

WidgetAtlas::WidgetAtlas(QWidget *parent /*= 0*/) : QWidget(parent),
                                                    ui(new Ui::WidgetAtlas),
                                                    m_pProjOwner(NULL)
{
    ui->setupUi(this);
    
    // NOTE: THIS CONSTRUCTOR IS INVALID TO USE. IT EXISTS FOR QT TO ALLOW Q_OBJECT TO WORK
}

WidgetAtlas::WidgetAtlas(ItemProject *pProjOwner, QWidget *parent /*= 0*/) :    QWidget(parent),
                                                                                ui(new Ui::WidgetAtlas),
                                                                                m_bSettingsDirty(true),
                                                                                m_pProjOwner(pProjOwner)
{
    ui->setupUi(this);
    
    m_DataFile.setFile(m_pProjOwner->GetPath() % HYGUIPATH_RelDataAtlasFile);
    m_MetaDataFile.setFile(m_pProjOwner->GetPath() % HYGUIPATH_RelMetaDataAtlasFile);
    
    // Search for packer settings file. If none exist create one with defaults and show settings page, else load setting from it and show frames page.
    if(m_MetaDataFile.exists() == false)
    {
        SaveSettings();
        ui->stackedWidget->setCurrentIndex(PAGE_Settings);
    }
    else
    {
        LoadSettings();
        ui->stackedWidget->setCurrentIndex(PAGE_Frames);
    }
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
    //LoadSettings();
    
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

int WidgetAtlas::GetNextTextureId()
{
    return ui->frameList->topLevelItemCount();
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
    QFileDialog dlg(this);
    dlg.setFileMode(QFileDialog::ExistingFile);
    dlg.setViewMode(QFileDialog::Detail);
    dlg.setWindowModality( Qt::ApplicationModal );
    dlg.setModal(true);

    QString sSelectedFilter(tr("PNG (*.png)"));
    QStringList sImageList = QFileDialog::getOpenFileNames(this,
                                                           "Import image(s) into atlases",
                                                           QString(),
                                                           tr("All files (*.*);;PNG (*.png)"),
                                                           &sSelectedFilter);
    
    ImportFrames(sImageList);
    GenTextureSheets();
    
    // Display texture
    MainWindow::OpenItem(this->m_pProjOwner);
}

void WidgetAtlas::on_btnAddDir_clicked()
{
    QFileDialog dlg(this);
    dlg.setFileMode(QFileDialog::Directory);
    dlg.setOption(QFileDialog::ShowDirsOnly, true);
    dlg.setViewMode(QFileDialog::Detail);
    dlg.setWindowModality(Qt::ApplicationModal);
    dlg.setModal(true);

    if(dlg.exec() == QDialog::Rejected)
        return;
    
    QStringList sDirs = dlg.selectedFiles();
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
    
    ImportFrames(sImportImgList);
    GenTextureSheets();
    
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

void WidgetAtlas::on_btnSaveSettings_clicked()
{
    if(m_Textures.size() == 0)
        SaveSettings();
    else if(m_bSettingsDirty)
    {
        QMessageBox dlg(QMessageBox::Question, "Harmony Designer Tool", "Atlas texture settings have changed. Would you like to save settings and regenerate all textures?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        switch(dlg.exec())
        {
        case QMessageBox::Yes:
            // Save was clicked. Reload every texture with new settings, then show 'frames'
            SaveSettings();
            RepackFrames();
            break;
        case QMessageBox::No:
            // Don't Save was clicked. Restore the cached settings and show the 'frames'
            LoadSettings();
            break;
        case QMessageBox::Cancel:
            // Cancel was clicked. Don't do anything and stay on the 'settings'
            return;
        default:
            // should never be reached
            break;
        }
    }
    
    ui->stackedWidget->setCurrentIndex(PAGE_Frames);
    m_bSettingsDirty = false;
}

void WidgetAtlas::on_btnChangeSettings_clicked()
{
    ui->stackedWidget->setCurrentIndex(PAGE_Settings);
}

void WidgetAtlas::LoadSettings()
{
    QFile file(m_MetaDataFile.absoluteFilePath());
    if(file.open(QIODevice::ReadOnly) == false)
    {
        HYLOG("Could not open atlas settings file for reading", LOGTYPE_Error);
        return;
    }
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromBinaryData(data);
    file.close();
    
    QJsonObject settings = doc.object();
    ui->cmbSortOrder->setCurrentIndex(settings["cmbSortOrder"].toInt());
    ui->sbFrameMarginTop->setValue(settings["sbFrameMarginTop"].toInt());
    ui->sbFrameMarginLeft->setValue(settings["sbFrameMarginLeft"].toInt());
    ui->sbFrameMarginRight->setValue(settings["sbFrameMarginRight"].toInt());
    ui->sbFrameMarginBottom->setValue(settings["sbFrameMarginBottom"].toInt());
    ui->extrude->setValue(settings["extrude"].toInt());
    ui->chkMerge->setChecked(settings["chkMerge"].toBool());
    ui->chkSquare->setChecked(settings["chkSquare"].toBool());
    ui->chkAutosize->setChecked(settings["chkAutosize"].toBool());
    ui->minFillRate->setValue(settings["minFillRate"].toInt());
    ui->cmbRotationStrategy->setCurrentIndex(settings["cmbRotationStrategy"].toInt());
    
    ui->sbTextureWidth->setValue(settings["sbTextureWidth"].toInt());
    ui->sbTextureHeight->setValue(settings["sbTextureHeight"].toInt());
    ui->cmbHeuristic->setCurrentIndex(settings["cmbHeuristic"].toInt());
}

void WidgetAtlas::SaveSettings()
{
    QJsonObject settings;
    settings.insert("cmbSortOrder", QJsonValue(ui->cmbSortOrder->currentIndex()));
    settings.insert("sbFrameMarginTop", QJsonValue(ui->sbFrameMarginTop->value()));
    settings.insert("sbFrameMarginLeft", QJsonValue(ui->sbFrameMarginLeft->value()));
    settings.insert("sbFrameMarginRight", QJsonValue(ui->sbFrameMarginRight->value()));
    settings.insert("sbFrameMarginBottom", QJsonValue(ui->sbFrameMarginBottom->value()));
    settings.insert("extrude", QJsonValue(ui->extrude->value()));
    settings.insert("chkMerge", QJsonValue(ui->chkMerge->isChecked()));
    settings.insert("chkSquare", QJsonValue(ui->chkSquare->isChecked()));
    settings.insert("chkAutosize", QJsonValue(ui->chkAutosize->isChecked()));
    settings.insert("minFillRate", QJsonValue(ui->minFillRate->value()));
    settings.insert("cmbRotationStrategy", QJsonValue(ui->cmbRotationStrategy->currentIndex()));
    
    settings.insert("sbTextureWidth", QJsonValue(ui->sbTextureWidth->value()));
    settings.insert("sbTextureHeight", QJsonValue(ui->sbTextureHeight->value()));
    settings.insert("cmbHeuristic", QJsonValue(ui->cmbHeuristic->currentIndex()));
    
            
    QFile file(m_MetaDataFile.absoluteFilePath());
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        HYLOG("Couldn't open atlas settings file for writing", LOGTYPE_Error);
        return;
    }

    QJsonDocument doc(settings);
    qint64 iBytesWritten = file.write(doc.toBinaryData());
    if(0 == iBytesWritten || -1 == iBytesWritten)
    {
        HYLOG("Could not write to atlas settings file: " % file.errorString(), LOGTYPE_Error);
        return;
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
    // Write global atlas information
    QJsonObject atlasInfo;
    atlasInfo.insert("NumTextures", QJsonValue(m_Textures.size()));
    
    for(int i = 0; i < m_Textures.size(); ++i)
    {
        // Save any changed textures to disk
        if(m_Textures[i]->IsDirty())
            m_Textures[i]->GenerateImg();
        
        m_Textures[i]->GetFrameList(atlasInfo);
    }
    
    QFile file(m_DataFile.absoluteFilePath());
    if(file.exists())
    {
        if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            HYLOG("Couldn't open atlas data file for writing", LOGTYPE_Error);
            return;
        }
    }
    QJsonDocument doc(atlasInfo);
    if(0 == file.write(doc.toJson()))
    {
        HYLOG("Could not write to atlas data file", LOGTYPE_Error);
        return;
    }
}

void WidgetAtlas::ImportFrames(QStringList sImportImgList)
{
    QList<QStringList> sMissingImgPaths = GetActiveTexture()->ImportImgs(sImportImgList);
    bool bFailedToPack = false;
    for(int i = 0; i < sMissingImgPaths.size(); ++i)
    {
        HyGuiTexture *pNewTexture = new HyGuiTexture(this);
    
        QList<QStringList> sFailedImgPaths = pNewTexture->ImportImgs(sMissingImgPaths[i]);
        if(sFailedImgPaths.size() > 0)
        {
            bFailedToPack = true;
            for(int j = 0; j < sFailedImgPaths.size(); ++j)
            {
                foreach(const QString sPath, sFailedImgPaths[j])
                    HYLOG("Could not import: " % sPath, LOGTYPE_Info);
            }
        }
        else
            m_Textures.append(pNewTexture); // Becomes active texture
    }
    
    if(bFailedToPack)
        HYLOG("Imported image(s) failed to pack into current texture settings. Check log for info.", LOGTYPE_Warning);
}

void WidgetAtlas::RepackFrames()
{
    for(int i = 0; i < m_Textures.size(); ++i)
    {
        QList<QStringList> missingFrames = m_Textures[i]->RepackImgs();
        // TODO: handle missing frames
    }
}

