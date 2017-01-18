/**************************************************************************
 *	WidgetAtlasGroup.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetAtlasGroup.h"
#include "ui_WidgetAtlasGroup.h"

#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStack>
#include <QPainter>
#include <QByteArray>
#include <QTreeWidgetItemIterator>

#include <ctime>

#include "MainWindow.h"
#include "WidgetAtlasManager.h"
#include "DlgInputName.h"

WidgetAtlasGroupTreeWidget::WidgetAtlasGroupTreeWidget(QWidget *parent /*= Q_NULLPTR*/) : QTreeWidget(parent)
{
}

void WidgetAtlasGroupTreeWidget::SetOwner(WidgetAtlasGroup *pOwner)
{
    m_pOwner = pOwner;
}

/*virtual*/ void WidgetAtlasGroupTreeWidget::dropEvent(QDropEvent *e)
{
//    foreach (const QUrl &url, e->mimeData()->urls())
//    {
//        QString fileName = url.toLocalFile();
//        qDebug() << "Dropped file:" << fileName;
//    }

    QTreeWidget::dropEvent(e);

    sortItems(0, Qt::AscendingOrder);
    m_pOwner->WriteMetaSettings();
}


bool WidgetAtlasGroupTreeWidgetItem::operator<(const QTreeWidgetItem &rhs) const
{
    bool bLeftIsFilter = this->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter;
    bool bRightIsFilter = rhs.data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter;

    if(bLeftIsFilter && bRightIsFilter == false)
        return true;
    if(bLeftIsFilter == false && bRightIsFilter)
        return false;

    return this->text(0) < rhs.text(0);
}

WidgetAtlasGroup::WidgetAtlasGroup(QWidget *parent) :   QWidget(parent),
                                                        ui(new Ui::WidgetAtlasGroup)
{
    ui->setupUi(this);

    // Invalid constructor. This exists so Q_OBJECT can work.
    HyGuiLog("WidgetAtlasGroup::WidgetAtlasGroup() invalid constructor used", LOGTYPE_Error);
}

WidgetAtlasGroup::WidgetAtlasGroup(QDir metaDir, QDir dataDir, WidgetAtlasManager *pManager, QWidget *parent) : QWidget(parent),
                                                                                                                m_pManager(pManager),
                                                                                                                m_MetaDir(metaDir),
                                                                                                                m_DataDir(dataDir),
                                                                                                                ui(new Ui::WidgetAtlasGroup)
{    
    ui->setupUi(this);
    
    // Set the default name, which will get overwritten if LoadSettings() is called
    m_dlgSettings.SetName("Group ID: " % QString::number(GetId()));
    
    ui->actionDeleteImages->setEnabled(false);
    ui->actionReplaceImages->setEnabled(false);
    
    ui->btnDeleteImages->setDefaultAction(ui->actionDeleteImages);
    ui->btnReplaceImages->setDefaultAction(ui->actionReplaceImages);
    ui->btnAddFilter->setDefaultAction(ui->actionAddFilter);

    ui->atlasList->SetOwner(this);

    ui->atlasList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->atlasList->setDragEnabled(true);
    //ui->atlasList->viewport()->setAcceptDrops(true);
    ui->atlasList->setDropIndicatorShown(true);
    ui->atlasList->setDragDropMode(QAbstractItemView::InternalMove);

    m_FrameList.clear();
    ui->atlasList->clear();

    QFile settingsFile(m_MetaDir.absoluteFilePath(HYGUIPATH_MetaSettings));
    if(settingsFile.exists())
    {
        if(!settingsFile.open(QIODevice::ReadOnly))
            HyGuiLog(QString("WidgetAtlasGroup::WidgetAtlasGroup() could not open ") % HYGUIPATH_MetaSettings, LOGTYPE_Error);

#ifdef HYGUI_UseBinaryMetaFiles
        QJsonDocument settingsDoc = QJsonDocument::fromBinaryData(settingsFile.readAll());
#else
        QJsonDocument settingsDoc = QJsonDocument::fromJson(settingsFile.readAll());
#endif
        settingsFile.close();

        QJsonObject settingsObj = settingsDoc.object();
        m_dlgSettings.LoadSettings(settingsObj);

        // Create all the filter items first, storing their actual path in their data (for now)
        QJsonArray filtersArray = settingsObj["filters"].toArray();
        for(int i = 0; i < filtersArray.size(); ++i)
        {
            QDir filterPathDir(filtersArray.at(i).toString());

            WidgetAtlasGroupTreeWidgetItem *pNewTreeItem = new WidgetAtlasGroupTreeWidgetItem(ui->atlasList);

            pNewTreeItem->setText(0, filterPathDir.dirName());
            pNewTreeItem->setIcon(0, HyGlobal::ItemIcon(ITEM_Prefix));

            QVariant v(QString(filterPathDir.absolutePath()));
            pNewTreeItem->setData(0, Qt::UserRole, v);
        }

        // Then place the filters correctly as a parent heirarchy using the path string stored in their data
        for(int i = 0; i < ui->atlasList->topLevelItemCount(); ++i)
        {
            WidgetAtlasGroupTreeWidgetItem *pParentFilter = NULL;

            QString sFilterPath = ui->atlasList->topLevelItem(i)->data(0, Qt::UserRole).toString();
            sFilterPath.truncate(sFilterPath.lastIndexOf("/"));
            if(sFilterPath != "")
            {
                QTreeWidgetItemIterator iter2(ui->atlasList);
                while(*iter2)
                {
                    if((*iter2)->data(0, Qt::UserRole).toString() == sFilterPath)
                    {
                        pParentFilter = static_cast<WidgetAtlasGroupTreeWidgetItem *>(*iter2);
                        break;
                    }

                    ++iter2;
                }
            }

            if(pParentFilter)
            {
                pParentFilter->addChild(ui->atlasList->takeTopLevelItem(i));
                i = -1;
            }
        }

        // Finally go through all the filters and set the data string to the 'HYTREEWIDGETITEM_IsFilter' value to identify this QTreeWidgetItem as a filter
        QTreeWidgetItemIterator iter(ui->atlasList);
        while(*iter)
        {
            (*iter)->setData(0, Qt::UserRole, QVariant(QString(HYTREEWIDGETITEM_IsFilter)));
            ++iter;
        }

        QJsonArray frameArray = settingsObj["frames"].toArray();
        for(int i = 0; i < frameArray.size(); ++i)
        {
            QJsonObject frameObj = frameArray[i].toObject();

            QRect rAlphaCrop(QPoint(frameObj["cropLeft"].toInt(), frameObj["cropTop"].toInt()), QPoint(frameObj["cropRight"].toInt(), frameObj["cropBottom"].toInt()));
            HyGuiFrame *pNewFrame = m_pManager->CreateFrame(JSONOBJ_TOINT(frameObj, "checksum"),
                                                            frameObj["name"].toString(),
                                                            rAlphaCrop,
                                                            GetId(),
                                                            static_cast<eAtlasNodeType>(frameObj["type"].toInt()),
                                                            frameObj["width"].toInt(),
                                                            frameObj["height"].toInt(),
                                                            frameObj["textureIndex"].toInt(),
                                                            frameObj["x"].toInt(),
                                                            frameObj["y"].toInt(),
                                                            frameObj["errors"].toInt(0));

            QString sFilterPath = frameObj["filter"].toString();
            WidgetAtlasGroupTreeWidgetItem *pFrameParent = NULL;
            if(sFilterPath != "")
            {
                QTreeWidgetItemIterator it(ui->atlasList);
                while(*it)
                {
                    if((*it)->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter && HyGlobal::GetTreeWidgetItemPath(*it) == sFilterPath)
                    {
                        pFrameParent = static_cast<WidgetAtlasGroupTreeWidgetItem *>(*it);
                        break;
                    }

                    ++it;
                }
            }

            if(QFile::exists(m_MetaDir.absoluteFilePath(pNewFrame->ConstructImageFileName())) == false)
                pNewFrame->SetError(GUIFRAMEERROR_CannotFindMetaImg);
            else
                pNewFrame->ClearError(GUIFRAMEERROR_CannotFindMetaImg);

            CreateTreeItem(pFrameParent, pNewFrame);

            m_FrameList.append(pNewFrame);
        }

        ui->atlasList->sortItems(0, Qt::AscendingOrder);
    }
    else
    {
        WriteMetaSettings();
    }
    
    ui->lcdNumTextures->display(m_DataDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot).size());
    ui->lcdTexWidth->display(m_dlgSettings.TextureWidth());
    ui->lcdTexHeight->display(m_dlgSettings.TextureHeight());

    ui->atlasList->collapseAll();

    ResizeAtlasListColumns();
}

WidgetAtlasGroup::~WidgetAtlasGroup()
{
    delete ui;
}

bool WidgetAtlasGroup::IsMatching(QDir metaDir, QDir dataDir)
{
    return (m_MetaDir == metaDir && m_DataDir == dataDir);
}

QList<HyGuiFrame *> &WidgetAtlasGroup::GetFrameList()
{
    return m_FrameList;
}

QTreeWidget *WidgetAtlasGroup::GetTreeWidget()
{
    if(ui)
        return ui->atlasList;
    else
        return NULL;
}

QSize WidgetAtlasGroup::GetAtlasDimensions()
{
    return QSize(m_dlgSettings.TextureWidth(), m_dlgSettings.TextureHeight());
}

void WidgetAtlasGroup::GetAtlasInfoForGameData(QJsonObject &atlasObjOut)
{
    atlasObjOut.insert("id", m_DataDir.dirName().toInt());
    atlasObjOut.insert("width", m_dlgSettings.TextureWidth());
    atlasObjOut.insert("height", m_dlgSettings.TextureHeight());
    atlasObjOut.insert("num8BitClrChannels", 4);   // TODO: Actually make this configurable?
    
    QJsonArray textureArray;
    QList<QJsonArray> frameArrayList;
    
    for(int i = 0; i < m_FrameList.size(); ++i)
    {
        if(m_FrameList[i]->GetTextureIndex() < 0)
            continue;
        
        while(frameArrayList.empty() || frameArrayList.size() <= m_FrameList[i]->GetTextureIndex())
            frameArrayList.append(QJsonArray());
        
        QJsonObject frameObj;
        frameObj.insert("checksum", QJsonValue(static_cast<qint64>(m_FrameList[i]->GetChecksum())));

        frameObj.insert("left", QJsonValue(m_FrameList[i]->GetX()));
        frameObj.insert("top", QJsonValue(m_FrameList[i]->GetY()));
        
        frameObj.insert("right", QJsonValue(m_FrameList[i]->GetX() + m_FrameList[i]->GetCrop().width()));
        frameObj.insert("bottom", QJsonValue(m_FrameList[i]->GetY() + m_FrameList[i]->GetCrop().height()));
        
        frameArrayList[m_FrameList[i]->GetTextureIndex()].append(frameObj);
    }
    
    for(int i = 0; i < frameArrayList.size(); ++i)
        textureArray.append(frameArrayList[i]);
    
    atlasObjOut.insert("textures", textureArray);
}

QString WidgetAtlasGroup::GetName()
{
    return m_dlgSettings.GetName();
}

int WidgetAtlasGroup::GetId()
{
    return m_MetaDir.dirName().toInt();
}

void WidgetAtlasGroup::ResizeAtlasListColumns()
{
    if(ui->atlasList == NULL)
        return;

    int iTotalWidth = ui->atlasList->size().width();
    ui->atlasList->setColumnWidth(0, iTotalWidth - 60);
}

void WidgetAtlasGroup::WriteMetaSettings()
{
    QJsonArray frameArray;
    for(int i = 0; i < m_FrameList.size(); ++i)
    {
        QJsonObject frameObj;
        m_FrameList[i]->GetJsonObj(frameObj);
        frameArray.append(QJsonValue(frameObj));
    }

    WriteMetaSettings(frameArray);
}

void WidgetAtlasGroup::WriteMetaSettings(QJsonArray frameArray)
{
    QJsonObject settingsObj = m_dlgSettings.GetSettings();
    settingsObj.insert("frames", frameArray);

    QJsonArray filtersArray;
    QTreeWidgetItemIterator iter(ui->atlasList);
    while(*iter)
    {
        if((*iter)->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter)
        {
            QString sFilterPath = HyGlobal::GetTreeWidgetItemPath(*iter);
            filtersArray.append(QJsonValue(sFilterPath));
        }

        ++iter;
    }

    settingsObj.insert("filters", filtersArray);

    QFile settingsFile(m_MetaDir.absoluteFilePath(HYGUIPATH_MetaSettings));
    if(!settingsFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
       HyGuiLog("Couldn't open atlas settings file for writing", LOGTYPE_Error);
    }
    else
    {
        QJsonDocument settingsDoc(settingsObj);

#ifdef HYGUI_UseBinaryMetaFiles
        qint64 iBytesWritten = settingsFile.write(settingsDoc.toBinaryData());
#else
        qint64 iBytesWritten = settingsFile.write(settingsDoc.toJson());
#endif
        if(0 == iBytesWritten || -1 == iBytesWritten)
        {
            HyGuiLog("Could not write to atlas settings file: " % settingsFile.errorString(), LOGTYPE_Error);
        }

        settingsFile.close();
    }
}

void WidgetAtlasGroup::on_btnAddImages_clicked()
{
    QFileDialog dlg(this);
    dlg.setFileMode(QFileDialog::ExistingFile);
    dlg.setViewMode(QFileDialog::Detail);
    dlg.setWindowModality(Qt::ApplicationModal);
    dlg.setModal(true);

    QString sSelectedFilter(tr("PNG (*.png)"));
    QStringList sImportImgList = QFileDialog::getOpenFileNames(this,
                                                               "Import image(s) into atlases",
                                                               QString(),
                                                               tr("All files (*.*);;PNG (*.png)"),
                                                               &sSelectedFilter);

    if(sImportImgList.empty() == false)
        Repack(QSet<int>(), ImportImages(sImportImgList));
}

void WidgetAtlasGroup::on_btnAddDir_clicked()
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
        HyGlobal::RecursiveFindOfFileExt("png", sImportImgList, dirEntry);
    }

    if(sImportImgList.empty() == false)
        Repack(QSet<int>(), ImportImages(sImportImgList));
}

/*virtual*/ void WidgetAtlasGroup::enterEvent(QEvent *pEvent)
{
    m_pManager->PreviewAtlasGroup();
    QWidget::enterEvent(pEvent);
}

/*virtual*/ void WidgetAtlasGroup::leaveEvent(QEvent *pEvent)
{
    m_pManager->HideAtlasGroup();
    QWidget::leaveEvent(pEvent);
}

/*virtual*/ void WidgetAtlasGroup::resizeEvent(QResizeEvent * event)
{
    ResizeAtlasListColumns();
    QWidget::resizeEvent(event);
}

/*virtual*/ void WidgetAtlasGroup::showEvent(QShowEvent * event)
{
    ResizeAtlasListColumns();
    QWidget::showEvent(event);
}

QSet<HyGuiFrame *> WidgetAtlasGroup::ImportImages(QStringList sImportImgList)
{
    QSet<HyGuiFrame *> returnSet;

    for(int i = 0; i < sImportImgList.size(); ++i)
    {
        QFileInfo fileInfo(sImportImgList[i]);

        QImage newImage(fileInfo.absoluteFilePath());

        returnSet.insert(ImportImage(fileInfo.baseName(), newImage, ATLAS_Frame));
    }

    return returnSet;
}

HyGuiFrame *WidgetAtlasGroup::ImportImage(QString sName, QImage &newImage, eAtlasNodeType eType)
{
    quint32 uiChecksum = HyGlobal::CRCData(0, newImage.bits(), newImage.byteCount());
    
    QRect rAlphaCrop(0, 0, newImage.width(), newImage.height());
    if(eType != ATLAS_Font && eType != ATLAS_Spine) // Cannot crop 'sub-atlases' because they rely on their own UV coordinates
        rAlphaCrop = ImagePacker::crop(newImage);

    HyGuiFrame *pNewFrame = m_pManager->CreateFrame(uiChecksum, sName, rAlphaCrop, GetId(), eType, newImage.width(), newImage.height(), -1, -1, -1, 0);
    if(pNewFrame)
    {
        newImage.save(m_MetaDir.absoluteFilePath(pNewFrame->ConstructImageFileName()));

        CreateTreeItem(NULL, pNewFrame);

        m_FrameList.append(pNewFrame);
    }

    return pNewFrame;
}

void WidgetAtlasGroup::RepackAll()
{
    int iNumTotalTextures = m_DataDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files, QDir::Name).size();

    QSet<int> textureIndexSet;
    for(int i = 0; i < iNumTotalTextures; ++i)
        textureIndexSet.insert(i);

    Repack(textureIndexSet, QSet<HyGuiFrame *>());
}

void WidgetAtlasGroup::Repack(QSet<int> repackTexIndicesSet, QSet<HyGuiFrame *> newFramesSet)
{
    MainWindow::LoadSpinner(true);
    
    for(uint32 i = 0; i < m_FrameList.size(); ++i)
        m_FrameList[i]->DeleteAllDrawInst();

    // Always repack the last texture to ensure it gets filled as much as it can
    QFileInfoList existingTexturesInfoList = m_DataDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files, QDir::Name);
    for(int i = HyClamp(existingTexturesInfoList.size() - 1, 0, existingTexturesInfoList.size()); i < existingTexturesInfoList.size(); ++i)
        repackTexIndicesSet.insert(i);

    QList<int> textureIndexList = repackTexIndicesSet.toList();

    // Get all the affected frames into a list
    for(int i = 0; i < m_FrameList.size(); ++i)
    {
        for(int j = 0; j < textureIndexList.size(); ++j)
        {
            if(m_FrameList[i]->GetTextureIndex() == textureIndexList[j])
                newFramesSet.insert(m_FrameList[i]);
        }
    }

    QList<HyGuiFrame *>newFramesList = newFramesSet.toList();

    // Repack the affected frames and determine how many textures this repack took
    m_Packer.clear();
    for(int i = 0; i < newFramesList.size(); ++i)
    {
        m_Packer.addItem(newFramesList[i]->GetSize(),
                         newFramesList[i]->GetCrop(),
                         newFramesList[i]->GetChecksum(),
                         newFramesList[i],
                         m_MetaDir.absoluteFilePath(newFramesList[i]->ConstructImageFileName()));
    }
    m_dlgSettings.SetPackerSettings(&m_Packer);
    m_Packer.pack(m_dlgSettings.GetHeuristic(), m_dlgSettings.TextureWidth(), m_dlgSettings.TextureHeight());

    int iNumNewTextures = m_Packer.bins.size();

    // Delete the old textures
    for(int i = 0; i < textureIndexList.size(); ++i)
        QFile::remove(m_DataDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(textureIndexList[i]) % ".png"));
    
    // Regrab 'existingTexturesInfoList' after deleting obsolete textures
    existingTexturesInfoList = m_DataDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files, QDir::Name);

    // Using our stock of newly generated textures, fill in any gaps in the texture array. If there aren't enough new textures then shift textures (and their frames) to fill any remaining gaps in the indices.
    int iTotalNumTextures = iNumNewTextures + existingTexturesInfoList.size();
    ui->lcdNumTextures->display(iTotalNumTextures);

    int iNumNewTexturesUsed = 0;
    for(int iCurrentIndex = 0; iCurrentIndex < iTotalNumTextures; ++iCurrentIndex)
    {
        bool bFound = false;
        for(int i = 0; i < existingTexturesInfoList.size(); ++i)
        {
            if(existingTexturesInfoList[i].baseName().toInt() == iCurrentIndex)
            {
                bFound = true;
                break;
            }
        }

        if(bFound)
            continue;

        if(iNumNewTexturesUsed < iNumNewTextures)
        {
            ConstructAtlasTexture(iNumNewTexturesUsed, iCurrentIndex);
            iNumNewTexturesUsed++;
        }
        else
        {
            // There aren't enough new textures to fill all the gaps in indices. Find the next existing texture and assign it to iCurrentIndex
            bool bHandled = false;
            int iNextAvailableFoundIndex = iCurrentIndex;
            do
            {
                ++iNextAvailableFoundIndex;
                for(int i = 0; i < existingTexturesInfoList.size(); ++i)
                {
                    int iExistingTextureIndex = existingTexturesInfoList[i].baseName().toInt();

                    if(iExistingTextureIndex == iNextAvailableFoundIndex)
                    {
                        // Texture found, start migrating its frames
                        for(int j = 0; j < m_FrameList.size(); ++j)
                        {
                            if(m_FrameList[j]->GetTextureIndex() == iExistingTextureIndex)
                                m_FrameList[j]->UpdateInfoFromPacker(iCurrentIndex, m_FrameList[j]->GetX(), m_FrameList[j]->GetY());
                        }

                        // Rename the texture file to be the new index
                        QFile::rename(existingTexturesInfoList[i].absoluteFilePath(), m_DataDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(iCurrentIndex) % ".png"));
                        
                        // Regrab 'existingTexturesInfoList' after renaming a texture
                        existingTexturesInfoList = m_DataDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files, QDir::Name);

                        bHandled = true;
                        break;
                    }
                }
            }
            while(bHandled == false);
        }
    }

    Refresh();

    MainWindow::LoadSpinner(false);
}

void WidgetAtlasGroup::ConstructAtlasTexture(int iPackerBinIndex, int iTextureArrayIndex)
{
    if(m_dlgSettings.TextureWidth() != m_Packer.bins[iPackerBinIndex].width() || m_dlgSettings.TextureHeight() != m_Packer.bins[iPackerBinIndex].height())
        HyGuiLog("WidgetAtlasGroup::ConstructAtlasTexture() Mismatching texture dimentions", LOGTYPE_Error);

    QImage newTexture(m_dlgSettings.TextureWidth(), m_dlgSettings.TextureHeight(), QImage::Format_ARGB32);
    newTexture.fill(Qt::transparent);

    QPainter p(&newTexture);

    // Iterate through the images that were packed, and update their corresponding HyGuiFrame. Then draw them to the blank textures
    for(int i = 0; i < m_Packer.images.size(); ++i)
    {
        inputImage &imgInfoRef = m_Packer.images[i];
        HyGuiFrame *pFrame = reinterpret_cast<HyGuiFrame *>(imgInfoRef.id);
        bool bValidToDraw = true;

        if(imgInfoRef.pos.x() == 999999)    // This is scriptum image packer's (dumb) indication of an invalid image...
        {
            pFrame->UpdateInfoFromPacker(-1, -1, -1);
            bValidToDraw = false;
        }
        else
            pFrame->ClearError(GUIFRAMEERROR_CouldNotPack);

        if(imgInfoRef.duplicateId != NULL && m_Packer.merge)
        {
            pFrame->UpdateInfoFromPacker(iTextureArrayIndex,
                                         imgInfoRef.pos.x() + m_Packer.border.l,
                                         imgInfoRef.pos.y() + m_Packer.border.t);
            
            pFrame->SetError(GUIFRAMEERROR_Duplicate);
            bValidToDraw = false;
        }
        else
            pFrame->ClearError(GUIFRAMEERROR_Duplicate);

        if(imgInfoRef.textureId != iPackerBinIndex)
            bValidToDraw = false;

        if(bValidToDraw == false)
            continue;

        pFrame->UpdateInfoFromPacker(iTextureArrayIndex,
                                     imgInfoRef.pos.x() + m_Packer.border.l,
                                     imgInfoRef.pos.y() + m_Packer.border.t);

        QImage imgFrame(imgInfoRef.path);

        QSize size;
        QRect crop;
        if(!m_Packer.cropThreshold)
        {
            size = imgInfoRef.size;
            crop = QRect(0, 0, size.width(), size.height());
        }
        else
        {
            size = imgInfoRef.crop.size();
            crop = imgInfoRef.crop;
        }

//        if(imgInfoRef.rotated)
//        {
//            QTransform rotateTransform;
//            rotateTransform.rotate(90);
//            imgFrame = imgFrame.transformed(rotateTransform);

//            size.transpose();
//            crop = QRect(imgInfoRef.size.height() - crop.y() - crop.height(),
//                         crop.x(), crop.height(), crop.width());
//        }

        QPoint pos(pFrame->GetX(), pFrame->GetY());
        if(m_Packer.extrude)
        {
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            QColor color1 = QColor::fromRgba(imgFrame.pixel(crop.x(), crop.y()));
            p.setPen(color1);
            p.setBrush(color1);
            if(m_Packer.extrude == 1)
                p.drawPoint(QPoint(pos.x(), pos.y()));
            else
                p.drawRect(QRect(pos.x(), pos.y(), m_Packer.extrude - 1, m_Packer.extrude - 1));
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            QColor color2 = QColor::fromRgba(imgFrame.pixel(crop.x(), crop.y() + crop.height() - 1));
            p.setPen(color2);
            p.setBrush(color2);
            if(m_Packer.extrude == 1)
                p.drawPoint(QPoint(pos.x(), pos.y() + crop.height() + m_Packer.extrude));
            else
                p.drawRect(QRect(pos.x(), pos.y() + crop.height() + m_Packer.extrude, m_Packer.extrude - 1, m_Packer.extrude - 1));
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            QColor color3 = QColor::fromRgba(imgFrame.pixel(crop.x() + crop.width() - 1, crop.y()));
            p.setPen(color3);
            p.setBrush(color3);
            if(m_Packer.extrude == 1)
                p.drawPoint(QPoint(pos.x() + crop.width() + m_Packer.extrude, pos.y()));
            else
                p.drawRect(QRect(pos.x() + crop.width() + m_Packer.extrude, pos.y(), m_Packer.extrude - 1, m_Packer.extrude - 1));
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            QColor color4 = QColor::fromRgba(imgFrame.pixel(crop.x() + crop.width() - 1, crop.y() + crop.height() - 1));
            p.setPen(color4);
            p.setBrush(color4);
            if(m_Packer.extrude == 1)
                p.drawPoint(QPoint(pos.x() + crop.width() + m_Packer.extrude, pos.y() + crop.height() + m_Packer.extrude));
            else
                p.drawRect(QRect(pos.x() + crop.width() + m_Packer.extrude, pos.y() + crop.height() + m_Packer.extrude, m_Packer.extrude - 1, m_Packer.extrude - 1));
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            p.drawImage(QRect(pos.x(), pos.y() + m_Packer.extrude, m_Packer.extrude, crop.height()), imgFrame, QRect(crop.x(), crop.y(), 1, crop.height()));
            p.drawImage(QRect(pos.x() + crop.width() + m_Packer.extrude, pos.y() + m_Packer.extrude, m_Packer.extrude, crop.height()), imgFrame, QRect(crop.x() + crop.width() - 1, crop.y(), 1, crop.height()));
            p.drawImage(QRect(pos.x() + m_Packer.extrude, pos.y(), crop.width(), m_Packer.extrude), imgFrame, QRect(crop.x(), crop.y(), crop.width(), 1));
            p.drawImage(QRect(pos.x() + m_Packer.extrude, pos.y() + crop.height() + m_Packer.extrude, crop.width(), m_Packer.extrude), imgFrame, QRect(crop.x(), crop.y() + crop.height() - 1, crop.width(), 1));

            p.drawImage(pos.x() + m_Packer.extrude, pos.y() + m_Packer.extrude, imgFrame, crop.x(), crop.y(), crop.width(), crop.height());
        }
        else
            p.drawImage(pos.x(), pos.y(), imgFrame, crop.x(), crop.y(), crop.width(), crop.height());
    }

    QImage *pTexture = static_cast<QImage *>(p.device());
    pTexture->save(m_DataDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(iTextureArrayIndex) % ".png"));
}

void WidgetAtlasGroup::Refresh()
{
    QJsonArray frameArray;
    for(int i = 0; i < m_FrameList.size(); ++i)
    {
        QJsonObject frameObj;
        m_FrameList[i]->GetJsonObj(frameObj);
        frameArray.append(QJsonValue(frameObj));
    }

    WriteMetaSettings(frameArray);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // REGENERATE THE ATLAS DATA INFO FILE (HARMONY EXPORT)
    m_pManager->SaveData();
    
    MainWindow::ReloadHarmony();

    ui->atlasList->sortItems(0, Qt::AscendingOrder);
    
    ui->lcdTexWidth->display(m_dlgSettings.TextureWidth());
    ui->lcdTexHeight->display(m_dlgSettings.TextureHeight());

    HyGuiLog("Atlas Group Refresh finished", LOGTYPE_Normal);

}

void WidgetAtlasGroup::CreateTreeItem(WidgetAtlasGroupTreeWidgetItem *pParent, HyGuiFrame *pFrame)
{
    WidgetAtlasGroupTreeWidgetItem *pNewTreeItem;
    if(pParent == NULL)
        pNewTreeItem = new WidgetAtlasGroupTreeWidgetItem(ui->atlasList);
    else
        pNewTreeItem = new WidgetAtlasGroupTreeWidgetItem();

    pNewTreeItem->setText(0, pFrame->GetName());

    if(pFrame->GetTextureIndex() >= 0)
    {
        pNewTreeItem->setText(1, "Tex:" % QString::number(pFrame->GetTextureIndex()));
        pFrame->ClearError(GUIFRAMEERROR_CouldNotPack);
    }
    else
    {
        pNewTreeItem->setText(1, "Invalid");
        pFrame->SetError(GUIFRAMEERROR_CouldNotPack);
    }

    if(pFrame->GetErrors() != 0)
    {
        pNewTreeItem->setIcon(0, HyGlobal::AtlasIcon(ATLAS_Frame_Warning));
        pNewTreeItem->setToolTip(0, HyGlobal::GetGuiFrameErrors(pFrame->GetErrors()));
    }

    QVariant v; v.setValue(pFrame);
    pNewTreeItem->setData(0, Qt::UserRole, v);

    pNewTreeItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);

    if(pParent)
        pParent->addChild(pNewTreeItem);

    pFrame->SetTreeWidgetItem(pNewTreeItem);

    ResizeAtlasListColumns();
}

void WidgetAtlasGroup::on_btnSettings_clicked()
{
    if(m_dlgSettings.GetName().isEmpty())
        m_dlgSettings.SetName("Atlas Group " % QString::number(GetId()));

    m_dlgSettings.DataToWidgets();
    if(QDialog::Accepted == m_dlgSettings.exec())
    {
        m_dlgSettings.WidgetsToData();  // Save the changes

        if(m_dlgSettings.IsSettingsDirty())
            RepackAll();
        else if(m_dlgSettings.IsNameChanged())
            WriteMetaSettings();
    }
    else
        m_dlgSettings.DataToWidgets();  // Reverts changes made
}

void WidgetAtlasGroup::on_atlasList_itemSelectionChanged()
{
    int iNumSelected = ui->atlasList->selectedItems().count();
    
    ui->actionDeleteImages->setEnabled(iNumSelected != 0);
    ui->actionReplaceImages->setEnabled(iNumSelected != 0);
}

void WidgetAtlasGroup::on_actionDeleteImages_triggered()
{
    QSet<int> affectedTextureIndexSet;

    QList<QTreeWidgetItem *> selectedImageList = ui->atlasList->selectedItems();
    for(int i = 0; i < selectedImageList.count(); ++i)
    {
        HyGuiFrame *pFrame = selectedImageList[i]->data(0, Qt::UserRole).value<HyGuiFrame *>();
        QSet<ItemWidget *> sLinks = pFrame->GetLinks();
        if(sLinks.empty() == false)
        {
            QString sMessage = "'" % pFrame->GetName() % "' image cannot be deleted because it is in use by the following items: \n\n";
            for(QSet<ItemWidget *>::iterator LinksIter = sLinks.begin(); LinksIter != sLinks.end(); ++LinksIter)
                sMessage.append(HyGlobal::ItemName(HyGlobal::GetCorrespondingDirItem((*LinksIter)->GetType())) % "/" % (*LinksIter)->GetName(true) % "\n");

            HyGuiLog(sMessage, LOGTYPE_Warning);
            continue;
        }

        affectedTextureIndexSet.insert(pFrame->GetTextureIndex());

        m_FrameList.removeOne(pFrame);
        m_pManager->RemoveImage(pFrame, m_MetaDir);
        delete selectedImageList[i];
    }

    Repack(affectedTextureIndexSet, QSet<HyGuiFrame *>());
}

void WidgetAtlasGroup::on_actionReplaceImages_triggered()
{
    QSet<int> affectedTextureIndexSet;

    QList<QTreeWidgetItem *> atlasSelectedImageList = ui->atlasList->selectedItems();

    // Store a list of the frames, since 'atlasSelectedImageList' will become invalid within Refresh()
    QList<HyGuiFrame *> selectedImageList;
    for(int i = 0; i < atlasSelectedImageList.count(); ++i)
        selectedImageList.append(atlasSelectedImageList[i]->data(0, Qt::UserRole).value<HyGuiFrame *>());

    QFileDialog dlg(this);

    if(selectedImageList.count() == 1)
    {
        dlg.setFileMode(QFileDialog::ExistingFile);
        dlg.setWindowTitle("Select an image as the replacement");
    }
    else
    {
        dlg.setFileMode(QFileDialog::ExistingFiles);
        dlg.setWindowTitle("Select " % QString::number(selectedImageList.count()) % " images as replacements");
    }
    dlg.setWindowModality(Qt::ApplicationModal);
    dlg.setModal(true);
    QStringList sFilterList;
    sFilterList << "*.png" << "*.*";
    dlg.setNameFilters(sFilterList);

    QStringList sImportImgList;
    do
    {
        if(dlg.exec() == QDialog::Rejected)
            return;

        sImportImgList = dlg.selectedFiles();

        if(sImportImgList.count() != selectedImageList.count())
            HyGuiLog("You must select " % QString::number(selectedImageList.count()) % " images", LOGTYPE_Warning);
    }
    while(sImportImgList.count() != selectedImageList.count());

    for(int i = 0; i < selectedImageList.count(); ++i)
    {
        HyGuiLog("Replacing: " % selectedImageList[i]->GetName() % " -> " % sImportImgList[i], LOGTYPE_Info);

        QFileInfo fileInfo(sImportImgList[i]);
        QImage newImage(fileInfo.absoluteFilePath());

        affectedTextureIndexSet.insert(selectedImageList[i]->GetTextureIndex());

        m_pManager->ReplaceFrame(selectedImageList[i], fileInfo.fileName(), newImage, false);
    }

    Repack(affectedTextureIndexSet, QSet<HyGuiFrame *>());

    for(int i = 0; i < selectedImageList.count(); ++i)
    {
        QSet<ItemWidget *> sLinks = selectedImageList[i]->GetLinks();
        for(QSet<ItemWidget *>::iterator LinksIter = sLinks.begin(); LinksIter != sLinks.end(); ++LinksIter)
            (*LinksIter)->Relink(selectedImageList[i]);
    }
}

void WidgetAtlasGroup::on_actionAddFilter_triggered()
{
    WidgetAtlasGroupTreeWidgetItem *pNewTreeItem = new WidgetAtlasGroupTreeWidgetItem(ui->atlasList);

    DlgInputName *pDlg = new DlgInputName("Enter Atlas Group Filter Name", "New Filter");
    if(pDlg->exec() == QDialog::Accepted)
        pNewTreeItem->setText(0, pDlg->GetName());
    else
    {
        delete pDlg;
        return;
    }

    delete pDlg;

    pNewTreeItem->setIcon(0, HyGlobal::ItemIcon(ITEM_Prefix));
    pNewTreeItem->setData(0, Qt::UserRole, QVariant(QString(HYTREEWIDGETITEM_IsFilter)));

    ui->atlasList->sortItems(0, Qt::AscendingOrder);
    WriteMetaSettings();
}
