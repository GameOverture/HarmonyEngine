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
#include <QElapsedTimer>
#include <QByteArray>

#include <ctime>

#include "MainWindow.h"
#include "WidgetAtlasManager.h"

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

    ui->atlasList->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->atlasList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_FrameList.clear();
    ui->atlasList->clear();
    
    int iNumTextures = 0;

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

        QJsonArray filtersArray = settingsObj["filters"].toArray();
        for(int i = 0; i < filtersArray.size(); ++i)
        {
            QDir filterPath(filtersArray.at(i).toString());

            QTreeWidgetItem *pNewTreeItem = new QTreeWidgetItem(ui->atlasList);

            pNewTreeItem->setText(0, filterPath.dirName());
            pNewTreeItem->setIcon(0, HyGlobal::ItemIcon(ITEM_Prefix));

            QVariant v(QString("FILTER"));
            pNewTreeItem->setData(0, Qt::UserRole, v);

            QTreeWidgetItem *pParent = NULL;
            if(filterPath.cdUp())
            {
                QList<QTreeWidgetItem *> foundList = ui->atlasList->findItems(filterPath.dirName(), Qt::MatchExactly);
                if(foundList.empty() == false && foundList[0]->data(0, Qt::UserRole).toString() == "FILTER")
                    pParent = foundList[0];
            }
            if(pParent)
                pParent->addChild(pNewTreeItem);
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
                                                            frameObj["filter"].toString(),
                                                            frameObj["errors"].toInt(0));

            if(QFile::exists(m_MetaDir.absoluteFilePath(pNewFrame->ConstructImageFileName())) == false)
                pNewFrame->SetError(GUIFRAMEERROR_CannotFindMetaImg);
            else
                pNewFrame->ClearError(GUIFRAMEERROR_CannotFindMetaImg);

            CreateTreeItem(NULL, pNewFrame);

            m_FrameList.append(pNewFrame);
        }

        ui->atlasList->sortItems(0, Qt::AscendingOrder);
        ui->atlasList->expandAll();
    }
    else
    {
        QJsonArray emptyFramesArray;
        WriteMetaSettings(emptyFramesArray);
    }
    
    ui->lcdNumTextures->display(iNumTextures);
    ui->lcdTexWidth->display(m_dlgSettings.TextureWidth());
    ui->lcdTexHeight->display(m_dlgSettings.TextureHeight());

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

void WidgetAtlasGroup::GetAtlasInfo(QJsonObject &atlasObjOut)
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
    {
        ImportImages(sImportImgList);
        Refresh();
    }
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
    {
        ImportImages(sImportImgList);
        Refresh();
    }
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

void WidgetAtlasGroup::ImportImages(QStringList sImportImgList)
{
    for(int i = 0; i < sImportImgList.size(); ++i)
    {
        QFileInfo fileInfo(sImportImgList[i]);

        QImage newImage(fileInfo.absoluteFilePath());

        ImportImage(fileInfo.baseName(), newImage, ATLAS_Frame);
    }
}

HyGuiFrame *WidgetAtlasGroup::ImportImage(QString sName, QImage &newImage, eAtlasNodeType eType)
{
    quint32 uiChecksum = HyGlobal::CRCData(0, newImage.bits(), newImage.byteCount());
    
    QRect rAlphaCrop(0, 0, newImage.width(), newImage.height());
    if(eType != ATLAS_Font && eType != ATLAS_Spine) // Cannot crop 'sub-atlases' because they rely on their own UV coordinates
        rAlphaCrop = ImagePacker::crop(newImage);

    HyGuiFrame *pNewFrame = m_pManager->CreateFrame(uiChecksum, sName, rAlphaCrop, GetId(), eType, newImage.width(), newImage.height(), -1, -1, -1, "", 0);
    if(pNewFrame)
    {
        newImage.save(m_MetaDir.absoluteFilePath(pNewFrame->ConstructImageFileName()));
        m_FrameList.append(pNewFrame);
    }

    return pNewFrame;
}

void WidgetAtlasGroup::Refresh()
{
    MainWindow::LoadSpinner(true);

    clock_t timeStartRefresh = clock();
    QElapsedTimer timerStartRefresh;
    timerStartRefresh.start();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CLEARING EXISTING DATA
    ui->atlasList->clear();
    m_Packer.clear();
    QStringList sTextureNames = m_DataDir.entryList(QDir::NoDotAndDotDot);
    
    for(int i = 0; i < sTextureNames.size(); ++i)
        QFile::remove(sTextureNames[i]);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // REPOPULATING THE PACKER WITH 'm_FrameList'
    for(int i = 0; i < m_FrameList.size(); ++i)
    {
        m_FrameList[i]->SetTreeWidgetItem(NULL);
        m_FrameList[i]->ClearError(GUIFRAMEERROR_CouldNotPack);
        m_FrameList[i]->DeleteAllDrawInst();

        m_Packer.addItem(m_FrameList[i]->GetSize(),
                         m_FrameList[i]->GetCrop(),
                         m_FrameList[i]->GetChecksum(),
                         m_FrameList[i],
                         m_MetaDir.absoluteFilePath(m_FrameList[i]->ConstructImageFileName()));
    }

    clock_t timeStartPack = clock();
    QElapsedTimer timerStartPack;
    timerStartRefresh.start();

    m_dlgSettings.SetPackerSettings(&m_Packer);
    m_Packer.pack(m_dlgSettings.GetHeuristic(), m_dlgSettings.TextureWidth(), m_dlgSettings.TextureHeight());

    qint64 i64TimePack = timerStartPack.elapsed();
    clock_t timeEndPack = clock();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CREATE EMPTY TEXTURES
    QPainter **ppPainters = new QPainter *[m_Packer.bins.size()];
    for(int i = 0; i < m_Packer.bins.size(); ++i)
    {
        if(m_dlgSettings.TextureWidth() != m_Packer.bins[i].width() || m_dlgSettings.TextureHeight() != m_Packer.bins[i].height())
            HyGuiLog("WidgetAtlasGroup::Refresh() Mismatching texture dimentions", LOGTYPE_Error);

        QImage *pTexture = new QImage(m_dlgSettings.TextureWidth(), m_dlgSettings.TextureHeight(), QImage::Format_ARGB32);  // TODO: BitsPerPixel configurable here?
        pTexture->fill(Qt::transparent);

        ppPainters[i] = new QPainter(pTexture);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // GO THROUGH PACKER'S IMAGES AND DRAW THEM TO TEXTURES, WHILE KEEPING JSON ARRAY (FOR META-FILE)
    QJsonArray frameArray;
    for(int i = 0; i < m_Packer.images.size(); ++i)
    {
        bool bValid = true;
        inputImage &imgInfoRef = m_Packer.images[i];

        if(imgInfoRef.duplicateId != NULL && m_Packer.merge)
            continue;

        if(imgInfoRef.pos.x() == 999999)    // This is scriptum image packer's (dumb) indication of an invalid image...
            bValid = false;

        HyGuiFrame *pFrame = reinterpret_cast<HyGuiFrame *>(imgInfoRef.id);

        pFrame->UpdateInfoFromPacker(bValid ? imgInfoRef.textureId : -1,
                                    imgInfoRef.pos.x() + m_Packer.border.l,
                                    imgInfoRef.pos.y() + m_Packer.border.t);

        QJsonObject frameObj;
        pFrame->GetJsonObj(frameObj);
        frameArray.append(QJsonValue(frameObj));

        CreateTreeItem(NULL, pFrame);
        
        if(bValid == false)
            continue;

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

        QPainter &p = *ppPainters[pFrame->GetTextureIndex()];
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SAVE ALL TEXTURES OUT TO ATLAS DATA DIR
    for(int i = 0; i < m_Packer.bins.size(); ++i)
    {
        QImage *pTexture = static_cast<QImage *>(ppPainters[i]->device());
        pTexture->save(m_DataDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(i) % ".png"));

        delete ppPainters[i];
        delete pTexture;
    }
    delete [] ppPainters;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // WRITE SETTINGS FILE TO ATLAS META DIR
    WriteMetaSettings(frameArray);

    qint64 i64TimeRefresh = timerStartRefresh.elapsed();
    clock_t timeEndRefresh = clock();

    HyGuiLog("Atlas Group Refresh done in: " % QString::number(static_cast<float>(i64TimeRefresh / 1000)), LOGTYPE_Normal);
    HyGuiLog("Atlas Group Pack done in:    " % QString::number(static_cast<float>(i64TimePack / 1000)), LOGTYPE_Normal);
    HyGuiLog("Atlas Group Refresh done in: " % QString::number(static_cast<float>((timeEndRefresh - timeStartRefresh) / CLOCKS_PER_SEC)), LOGTYPE_Info);
    HyGuiLog("Atlas Group Pack done in:    " % QString::number(static_cast<float>((timeEndPack - timeStartPack) / CLOCKS_PER_SEC)), LOGTYPE_Info);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // REGENERATE THE ATLAS DATA INFO FILE (HARMONY EXPORT)
    m_pManager->SaveData();
    
    MainWindow::ReloadHarmony();

    ui->atlasList->expandAll();
    ui->atlasList->sortItems(0, Qt::AscendingOrder);
    
    ui->lcdNumTextures->display(m_Packer.bins.size());
    ui->lcdTexWidth->display(m_dlgSettings.TextureWidth());
    ui->lcdTexHeight->display(m_dlgSettings.TextureHeight());

    MainWindow::LoadSpinner(false);
}

void WidgetAtlasGroup::WriteMetaSettings(QJsonArray frameArray)
{
    QJsonObject settingsObj = m_dlgSettings.GetSettings();
    settingsObj.insert("frames", frameArray);

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

void WidgetAtlasGroup::CreateTreeItem(QTreeWidgetItem *pParent, HyGuiFrame *pFrame)
{
    QTreeWidgetItem *pNewTreeItem;
    if(pParent == NULL)
        pNewTreeItem = new QTreeWidgetItem(ui->atlasList);
    else
        pNewTreeItem = new QTreeWidgetItem();

    pNewTreeItem->setText(0, pFrame->GetName());

    if(pFrame->GetErrors() == 0)
        pNewTreeItem->setIcon(0, HyGlobal::AtlasIcon(pFrame->GetType()));
    else
    {
        pNewTreeItem->setIcon(0, HyGlobal::AtlasIcon(ATLAS_Frame_Warning));
        pNewTreeItem->setToolTip(0, HyGlobal::GetGuiFrameErrors(pFrame->GetErrors()));
    }

    if(pFrame->GetTextureIndex() >= 0)
        pNewTreeItem->setText(1, "Tex:" % QString::number(pFrame->GetTextureIndex()));
    else
        pNewTreeItem->setText(1, "Invalid");

    QVariant v; v.setValue(pFrame);
    pNewTreeItem->setData(0, Qt::UserRole, v);

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
            Refresh();
        else if(m_dlgSettings.IsNameChanged())
        {
            m_dlgSettings.GetName();
            QFile settingsFile(m_MetaDir.absoluteFilePath(HYGUIPATH_MetaSettings));
            if(!settingsFile.open(QIODevice::ReadOnly))
            {
                settingsFile.close();
                HyGuiLog("Couldn't open atlas settings file for reading", LOGTYPE_Error);
                return;
            }

#ifdef HYGUI_UseBinaryMetaFiles
            QJsonDocument settingsDoc = QJsonDocument::fromBinaryData(settingsFile.readAll());
#else
            QJsonDocument settingsDoc = QJsonDocument::fromJson(settingsFile.readAll());
#endif
            settingsFile.close();

            QJsonObject settingsObj = settingsDoc.object();
            settingsObj.insert("txtName", m_dlgSettings.GetName());
            settingsDoc.setObject(settingsObj);

            if(!settingsFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
            {
                settingsFile.close();
                HyGuiLog("Couldn't open atlas settings file for writing", LOGTYPE_Error);
                return;
            }

#ifdef HYGUI_UseBinaryMetaFiles
            qint64 iBytesWritten = settingsFile.write(settingsDoc.toBinaryData());
#else
            qint64 iBytesWritten = settingsFile.write(settingsDoc.toJson());
#endif
            if(0 == iBytesWritten || -1 == iBytesWritten)
            {
                HyGuiLog("Could not write to atlas settings file to save name: " % settingsFile.errorString(), LOGTYPE_Error);
            }
            settingsFile.close();
        }
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
    // TODO: Should save all open items - beware of the Undo/Redo stack per item that may refrence the deleted images

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

        m_FrameList.removeOne(pFrame);
        m_pManager->RemoveImage(pFrame, m_MetaDir);
        delete selectedImageList[i];
    }

    Refresh();
}

void WidgetAtlasGroup::on_actionReplaceImages_triggered()
{
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
        selectedImageList[i]->ReplaceImage(sImportImgList[i], m_MetaDir);
    }

    Refresh();

    for(int i = 0; i < selectedImageList.count(); ++i)
    {
        QSet<ItemWidget *> sLinks = selectedImageList[i]->GetLinks();
        for(QSet<ItemWidget *>::iterator LinksIter = sLinks.begin(); LinksIter != sLinks.end(); ++LinksIter)
            (*LinksIter)->Relink(selectedImageList[i]);
    }
}

void WidgetAtlasGroup::on_actionAddFilter_triggered()
{

}
