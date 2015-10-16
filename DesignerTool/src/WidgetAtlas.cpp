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

#include "HyGuiAtlasGroup.h"
#include "HyGuiTexture.h"

WidgetAtlasManager::WidgetAtlasManager(QWidget *parent /*= 0*/) :   QWidget(parent),
                                                                    ui(new Ui::WidgetAtlasManager),
                                                                    m_pProjOwner(NULL)
{
    ui->setupUi(this);
    
    // NOTE: THIS CONSTRUCTOR IS INVALID TO USE. IT EXISTS FOR QT TO ALLOW Q_OBJECT TO WORK
}

WidgetAtlasManager::WidgetAtlasManager(ItemProject *pProjOwner, QWidget *parent /*= 0*/) :   QWidget(parent),
                                                                                             ui(new Ui::WidgetAtlasManager),
                                                                                             m_pProjOwner(pProjOwner)
{
    ui->setupUi(this);
    
    m_MetaDataFile.setFile(m_pProjOwner->GetPath() % HYGUIPATH_RelMetaDataAtlasFile);
    
    ui->frameList->setSelectionMode(QAbstractItemView::MultiSelection);
    
    LoadData();
}

WidgetAtlasManager::~WidgetAtlasManager()
{
    while(m_AtlasGroups.empty() == false)
    {
        HyGuiAtlasGroup *pAtlasGrp = m_AtlasGroups.front();
        delete pAtlasGrp;
        
        m_AtlasGroups.pop_front();
    }
    
    delete ui;
}

void WidgetAtlasManager::SetPackerSettings(ImagePacker *pPacker)
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

int WidgetAtlasManager::GetTexWidth()
{
    return ui->sbTextureWidth->value();
}
int WidgetAtlasManager::GetTexHeight()
{
    return ui->sbTextureHeight->value();
}
int WidgetAtlasManager::GetHeuristicIndex()
{
    return ui->cmbHeuristic->currentIndex();
}

int WidgetAtlasManager::GetNextTextureId()
{
    return ui->frameList->topLevelItemCount();
}

void WidgetAtlasManager::on_btnTexSize256_clicked()
{
    ui->sbTextureWidth->setValue(256);
    ui->sbTextureHeight->setValue(256);
    
    m_bSettingsDirty = true;
}

void WidgetAtlasManager::on_btnTexSize512_clicked()
{
    ui->sbTextureWidth->setValue(512);
    ui->sbTextureHeight->setValue(512);
    
    m_bSettingsDirty = true;
}

void WidgetAtlasManager::on_btnTexSize1024_clicked()
{
    ui->sbTextureWidth->setValue(1024);
    ui->sbTextureHeight->setValue(1024);
    
    m_bSettingsDirty = true;
}

void WidgetAtlasManager::on_btnTexSize2048_clicked()
{
    ui->sbTextureWidth->setValue(2048);
    ui->sbTextureHeight->setValue(2048);
    
    m_bSettingsDirty = true;
}

void WidgetAtlasManager::on_btnAddFiles_clicked()
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

void WidgetAtlasManager::on_btnAddDir_clicked()
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

void WidgetAtlasManager::on_tabWidget_currentChanged(int index)
{
    if(m_bSettingsDirty)
    {
        ui->frameList->topLevelItemCount();
    }
}

QTreeWidgetItem *WidgetAtlasManager::CreateTreeItem(QTreeWidgetItem *pParent, QString sName, eAtlasNodeType eType)
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

void WidgetAtlasManager::LoadData()
{
    QDir metaAtlasDir(m_pProjOwner->GetPath() % HYGUIPATH_RelMetaDataAtlasDir);

    QFileInfoList atlasGroupsList = metaAtlasDir.entryInfoList(NoFilter, QDir::Name);
    if(atlasGroupsList.empty())
    {
        metaAtlasDir.mkdir("00001");
        m_AtlasGroups.push_back(new HyGuiAtlasGroup());

        return;
    }

    for(unsigned int i = 0; i < atlasGroupsList.size(); ++i)
    {
        if(atlasGroupsList[i].isDir() == false)
            continue;

        QFileInfo atlasGrpSettings(atlasGroupsList[i].absolutePath() % HYGUIPATH_DataAtlasFileName);
        QFile dataFile(atlasGrpSettings.absoluteFilePath());
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
            HYLOG("Atlas group settings file not found. Generating new one.", LOGTYPE_Info);

        QFileInfoList atlasGrp = QDir(atlasGroupsList[i].absolutePath())..entryInfoList(NoFilter, QDir::Name);
    }
}

void WidgetAtlasManager::SaveData()
{
    //QJsonArray atlasGroupArray;
    //atlasGroupArray.append(
    
//    // Write global atlas information
//    QJsonObject atlasInfo;
//    atlasInfo.insert("numTextures", QJsonValue(m_Textures.size()));
//    atlasInfo.insert("width", ui->sbTextureWidth->value());
//    atlasInfo.insert("height", ui->sbTextureHeight->value());
    
//    QJsonArray textureArray;
//    for(int i = 0; i < m_Textures.size(); ++i)
//    {
//        // Save any changed textures to disk
//        if(m_Textures[i]->IsDirty())
//            m_Textures[i]->GenerateImg();

//        textureInfo.insert("id", QJsonValue(i));
//        textureInfo.insert("loadGroup", QJsonValue(m_Textures[i]->GetLoadGroup()));

//        QJsonObject textureInfo;
//        QJsonArray frameArray = m_Textures[i]->GetFrameArray();
//        textureInfo.insert("srcFrames", frameArray);
        
//        textureArray.append(textureInfo);
//    }
//    atlasInfo.insert("textures", textureArray);
    
//    QFile file(m_DataFile.absoluteFilePath());
//    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
//    {
//        HYLOG("Couldn't open atlas data file for writing", LOGTYPE_Error);
//        return;
//    }

//    QJsonDocument doc(atlasInfo);
//    if(0 == file.write(doc.toJson()))
//    {
//        HYLOG("Could not write to atlas data file", LOGTYPE_Error);
//        return;
//    }
}

HyGuiTexture *WidgetAtlasManager::GetActiveTexture()
{
    if(m_Textures.empty())
        m_Textures.append(new HyGuiTexture(this));
    
    return m_Textures.last();
}

void WidgetAtlasManager::ImportFrames(QStringList sImportImgList)
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

void WidgetAtlasManager::RepackFrames()
{
    for(int i = 0; i < m_Textures.size(); ++i)
    {
        QList<QStringList> missingFrames = m_Textures[i]->PackFrames();
        // TODO: handle missing frames
    }
}

void WidgetAtlasManager::RenderAtlas()
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

void WidgetAtlasManager::on_frameList_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    //item->ic
}

void WidgetAtlasManager::on_frameList_itemClicked(QTreeWidgetItem *item, int column)
{
    RenderAtlas();
}

void WidgetAtlasManager::on_btnSettings_clicked()
{

}
