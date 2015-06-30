#include "WidgetAtlas.h"
#include "ui_WidgetAtlas.h"

#include "MainWindow.h"

#include <QTreeWidget>
#include <QFileDialog>
#include <QStack>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
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
    
    ui->frameList->setSelectionMode(QAbstractItemView::MultiSelection);
    
    // Search for packer settings file. If none exist create one with defaults and show settings page, else load setting from it and show frames page.
    LoadSettings();
    LoadData();
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

int WidgetAtlas::GetNextTextureId()
{
    return ui->frameList->topLevelItemCount();
}

int WidgetAtlas::FindTextureId(HyGuiTexture *pTex)
{
    for(int i = 0; i < m_Textures.size(); ++i)
    {
        if(pTex == m_Textures[i])
            return i;
    }
    
    return -1;
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
    SaveData();
    
    // Display texture
    RenderAtlas();
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
    SaveData();
    
    // Display texture
    RenderAtlas();
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
    if(m_MetaDataFile.exists() == false)
    {
        HYLOG("Atlas settings file not found. Generating new one.", LOGTYPE_Info);
        SaveSettings();
        ui->stackedWidget->setCurrentIndex(PAGE_Settings);
        
        return;
    }
    
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
    ui->cmbSortOrder->setCurrentIndex(JSONOBJ_TOINT(settings, "cmbSortOrder"));
    ui->sbFrameMarginTop->setValue(JSONOBJ_TOINT(settings, "sbFrameMarginTop"));
    ui->sbFrameMarginLeft->setValue(JSONOBJ_TOINT(settings, "sbFrameMarginLeft"));
    ui->sbFrameMarginRight->setValue(JSONOBJ_TOINT(settings, "sbFrameMarginRight"));
    ui->sbFrameMarginBottom->setValue(JSONOBJ_TOINT(settings, "sbFrameMarginBottom"));
    ui->extrude->setValue(JSONOBJ_TOINT(settings, "extrude"));
    ui->chkMerge->setChecked(settings["chkMerge"].toBool());
    ui->chkSquare->setChecked(settings["chkSquare"].toBool());
    ui->chkAutosize->setChecked(settings["chkAutosize"].toBool());
    ui->minFillRate->setValue(JSONOBJ_TOINT(settings, "minFillRate"));
    ui->cmbRotationStrategy->setCurrentIndex(JSONOBJ_TOINT(settings, "cmbRotationStrategy"));
    ui->sbTextureWidth->setValue(JSONOBJ_TOINT(settings, "sbTextureWidth"));
    ui->sbTextureHeight->setValue(JSONOBJ_TOINT(settings, "sbTextureHeight"));
    ui->cmbHeuristic->setCurrentIndex(JSONOBJ_TOINT(settings, "cmbHeuristic"));
    
    //
    ui->stackedWidget->setCurrentIndex(PAGE_Frames);
}

void WidgetAtlas::LoadData()
{
    if(m_DataFile.exists() == false)
    {
        HYLOG("Atlas data file not found. Generating new one.", LOGTYPE_Info);
        SaveData();
        
        return;
    }
    
    QFileInfoList srcFrameImgList = QDir(m_pProjOwner->GetPath() % HYGUIPATH_RelMetaDataAtlasDir).entryInfoList();
    
    QFile dataFile(m_DataFile.absoluteFilePath());
    if(dataFile.open(QIODevice::ReadOnly))
    {
        QJsonDocument dataJsonDoc = QJsonDocument::fromJson(dataFile.readAll());
        QJsonObject contents = dataJsonDoc.object();
        
        QJsonArray textureArray = contents["textures"].toArray();
        foreach (const QJsonValue &textureInfo, textureArray)
        {
            HyGuiTexture *pNewTexture = new HyGuiTexture(this);
            
            QJsonArray srcFramesArray = textureInfo.toObject()["srcFrames"].toArray();
            foreach(const QJsonValue &frameInfo, srcFramesArray)
            {
                quint32 uiHash = JSONOBJ_TOINT(frameInfo.toObject(), "hash");
                foreach(const QFileInfo imgInfo, srcFrameImgList)
                {
                    QString sImgName = imgInfo.baseName();
                    quint32 uiTestHash = static_cast<quint32>(sImgName.left(sImgName.indexOf(QChar('-'))).toLongLong());
                    if(uiHash == uiTestHash)
                    {
                        int iSplitIndex = sImgName.indexOf(QChar('-'));
                        sImgName = sImgName.right(sImgName.length() - iSplitIndex - 1); // -1 so we don't include the '-'
                        pNewTexture->LoadFrame(QImage(imgInfo.absoluteFilePath()), uiHash, sImgName, imgInfo.absoluteFilePath());
                    }
                }
                
            }
            
            QList<QStringList> unPackedList = pNewTexture->PackFrames();
            if(unPackedList.empty() == false)
                HYLOG("Loading an atlas failed to pack properly", LOGTYPE_Error);
            
            m_Textures.append(pNewTexture);
        }
    }
    else
        HYLOG("Could not open: " % m_DataFile.absoluteFilePath(), LOGTYPE_Warning);
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

void WidgetAtlas::SaveData()
{
    // Write global atlas information
    QJsonObject atlasInfo;
    atlasInfo.insert("numTextures", QJsonValue(m_Textures.size()));
    
    QJsonArray textureArray;
    for(int i = 0; i < m_Textures.size(); ++i)
    {
        // Save any changed textures to disk
        if(m_Textures[i]->IsDirty())
            m_Textures[i]->GenerateImg();
        
        QJsonObject textureInfo;
        textureInfo.insert("id", QJsonValue(i));
        
        QJsonArray frameArray = m_Textures[i]->GetFrameArray();
        textureInfo.insert("srcFrames", frameArray);
        
        textureArray.append(textureInfo);
    }
    atlasInfo.insert("textures", textureArray);
    
    QFile file(m_DataFile.absoluteFilePath());
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        HYLOG("Couldn't open atlas data file for writing", LOGTYPE_Error);
        return;
    }

    QJsonDocument doc(atlasInfo);
    if(0 == file.write(doc.toJson()))
    {
        HYLOG("Could not write to atlas data file", LOGTYPE_Error);
        return;
    }
}

HyGuiTexture *WidgetAtlas::GetActiveTexture()
{
    if(m_Textures.empty())
        m_Textures.append(new HyGuiTexture(this));
    
    return m_Textures.last();
}

void WidgetAtlas::ImportFrames(QStringList sImportImgList)
{
    QList<QStringList> sMissingImgPaths = GetActiveTexture()->ImportFrames(sImportImgList);
    bool bFailedToPack = false;
    for(int i = 0; i < sMissingImgPaths.size(); ++i)
    {
        HyGuiTexture *pNewTexture = new HyGuiTexture(this);
    
        QList<QStringList> sFailedImgPaths = pNewTexture->ImportFrames(sMissingImgPaths[i]);
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
        QList<QStringList> missingFrames = m_Textures[i]->PackFrames();
        // TODO: handle missing frames
    }
}

void WidgetAtlas::RenderAtlas()
{
    if(ui->frameList->selectedItems().empty())
        return;
            
    // Determine what's the last frame/texture selected, and display that texture in the renderer
    QTreeWidgetItem *pTreeItem = ui->frameList->selectedItems()[ui->frameList->selectedItems().count() - 1];
    while(pTreeItem->parent() != NULL)
        pTreeItem = pTreeItem->parent();
    
    int iTextureId = 0;
    for(int i = 0; i < ui->frameList->topLevelItemCount(); ++i)
    {
        if(ui->frameList->topLevelItem(i) == pTreeItem)
        {
            iTextureId = i;
            break;
        }
    }
            
    m_pProjOwner->SetDrawState(ItemProject::DRAWSTATE_AtlasManager, iTextureId);
    MainWindow::OpenItem(m_pProjOwner);
}

void WidgetAtlas::on_frameList_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    //item->ic
}

void WidgetAtlas::on_frameList_itemClicked(QTreeWidgetItem *item, int column)
{
    RenderAtlas();
}
