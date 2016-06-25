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

WidgetAtlasGroup::WidgetAtlasGroup(QDir metaDir, QDir dataDir, QWidget *parent) :   QWidget(parent),
                                                                                    m_MetaDir(metaDir),
                                                                                    m_DataDir(dataDir),
                                                                                    ui(new Ui::WidgetAtlasGroup)
{    
    ui->setupUi(this);

    ui->atlasList->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->atlasList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_FrameList.clear();
    ui->atlasList->clear();

    QFile metaAtlasFile(m_MetaDir.absoluteFilePath(HYGUIPATH_MetaAtlasSettings));
    if(metaAtlasFile.exists())
    {
        if(!metaAtlasFile.open(QIODevice::ReadOnly))
            HyGuiLog(QString("WidgetAtlasGroup::WidgetAtlasGroup() could not open ") % HYGUIPATH_MetaAtlasSettings, LOGTYPE_Error);

#ifdef HYGUI_UseBinaryMetaFiles
        QJsonDocument settingsDoc = QJsonDocument::fromBinaryData(metaAtlasFile.readAll());
#else
        QJsonDocument settingsDoc = QJsonDocument::fromJson(metaAtlasFile.readAll());
#endif
        metaAtlasFile.close();

        QJsonObject settingsObj = settingsDoc.object();
        m_dlgSettings.LoadSettings(settingsObj);

        QJsonArray frameArray = settingsObj["frames"].toArray();
        for(int i = 0; i < frameArray.size(); ++i)
        {
            QJsonObject frameObj = frameArray[i].toObject();

            QRect rAlphaCrop(QPoint(frameObj["cropLeft"].toInt(), frameObj["cropTop"].toInt()), QPoint(frameObj["cropRight"].toInt(), frameObj["cropBottom"].toInt()));
            HyGuiFrame *pNewFrame = GetManager()->CreateFrame(frameObj["hash"].toInt(),
                                                              frameObj["name"].toString(),
                                                              rAlphaCrop,
                                                              GetId(),
                                                              frameObj["width"].toInt(),
                                                              frameObj["height"].toInt(),
                                                              frameObj["textureIndex"].toInt(),
                                                              frameObj["rotate"].toBool(),
                                                              frameObj["x"].toInt(),
                                                              frameObj["y"].toInt());

//            QJsonArray frameLinksArray = frameObj["links"].toArray();
//            for(int k = 0; k < frameLinksArray.size(); ++k)
//                pNewFrame->SetLink(frameLinksArray[k].toString());

            eAtlasNodeType eIconType = ATLAS_Frame_Warning;
            int iTexIndex = frameObj["textureIndex"].toInt();
            if(iTexIndex >= 0)
            {
                //while(m_TextureList.empty() || m_TextureList.size() <= frameObj["textureIndex"].toInt())
                //    m_TextureList.append(CreateTreeItem(NULL, "Texture: " % QString::number(m_TextureList.size()), ATLAS_Texture));

                //pTextureTreeItem = m_TextureList[];
                eIconType = ATLAS_Frame;
            }

            CreateTreeItem(NULL, frameObj["name"].toString(), iTexIndex, eIconType, pNewFrame);

            m_FrameList.append(pNewFrame);
        }

        ui->groupBox->setTitle(m_dlgSettings.GetName());

        ui->atlasList->sortItems(0, Qt::AscendingOrder);
        ui->atlasList->expandAll();
    }
}

WidgetAtlasGroup::~WidgetAtlasGroup()
{
    delete ui;

    for(int i = 0; i < m_FrameList.size(); ++i)
        GetManager()->RemoveFrame(m_FrameList[i]);
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

void WidgetAtlasGroup::GetAtlasInfo(QJsonObject &atlasObj)
{
    atlasObj.insert("id", m_DataDir.dirName().toInt());
    atlasObj.insert("width", m_dlgSettings.TextureWidth());
    atlasObj.insert("height", m_dlgSettings.TextureHeight());
    atlasObj.insert("num8BitClrChannels", 4);   // TODO: Actually make this configurable?
    
    QJsonArray textureArray;
    QList<QJsonArray> frameArrayList;
    
    for(int i = 0; i < m_FrameList.size(); ++i)
    {
        if(m_FrameList[i]->GetTextureIndex() < 0)
            continue;
        
        while(frameArrayList.empty() || frameArrayList.size() <= m_FrameList[i]->GetTextureIndex())
            frameArrayList.append(QJsonArray());
        
        QJsonObject frameObj;
        frameObj.insert("right", QJsonValue(m_FrameList[i]->GetX() + m_FrameList[i]->GetCrop().right()));
        frameObj.insert("bottom", QJsonValue(m_FrameList[i]->GetY() + m_FrameList[i]->GetCrop().bottom()));
        frameObj.insert("rotate", QJsonValue(m_FrameList[i]->IsRotated()));
        frameObj.insert("left", QJsonValue(m_FrameList[i]->GetX() + m_FrameList[i]->GetCrop().left()));
        frameObj.insert("top", QJsonValue(m_FrameList[i]->GetY() + m_FrameList[i]->GetCrop().top()));
        
        frameArrayList[m_FrameList[i]->GetTextureIndex()].append(frameObj);
    }
    
    foreach(QJsonArray frameArray, frameArrayList)
        textureArray.append(frameArray);
    
    atlasObj.insert("textures", textureArray);
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
        ImportImages(sImportImgList);
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

    ImportImages(sImportImgList);
}

/*virtual*/ void WidgetAtlasGroup::enterEvent(QEvent *pEvent)
{
    HyGuiLog("AtlasGroup mouseMoveEvent(): Enter", LOGTYPE_Normal);
    WidgetAtlasManager *pAtlasMan = static_cast<WidgetAtlasManager *>(parent()->parent());

    pAtlasMan->PreviewAtlasGroup();
    QWidget::enterEvent(pEvent);
}

/*virtual*/ void WidgetAtlasGroup::leaveEvent(QEvent *pEvent)
{
    HyGuiLog("AtlasGroup mouseMoveEvent(): Leave", LOGTYPE_Normal);
    WidgetAtlasManager *pAtlasMan = static_cast<WidgetAtlasManager *>(parent()->parent());
    
    pAtlasMan->HideAtlasGroup();
    QWidget::leaveEvent(pEvent);
}

WidgetAtlasManager *WidgetAtlasGroup::GetManager()
{
    return reinterpret_cast<WidgetAtlasManager *>(this->parent()->parent());
}

void WidgetAtlasGroup::ImportImages(QStringList sImportImgList)
{
    for(int i = 0; i < sImportImgList.size(); ++i)
    {
        QFileInfo fileInfo(sImportImgList[i]);

        QImage newImage(fileInfo.absoluteFilePath());
        quint32 uiHash = HyGlobal::CRCData(0, newImage.bits(), newImage.byteCount());
        QRect rAlphaCrop = m_Packer.crop(newImage);

        HyGuiFrame *pNewFrame = GetManager()->CreateFrame(uiHash, fileInfo.baseName(), rAlphaCrop, GetId(), newImage.width(), newImage.height(), -1, false, -1, -1);
        
        newImage.save(m_MetaDir.path() % "/" % pNewFrame->ConstructImageFileName());
        
        m_FrameList.append(pNewFrame);
    }

    Refresh();
}

void WidgetAtlasGroup::Refresh()
{
    clock_t timeStartRefresh = clock();
    QElapsedTimer timerStartRefresh;
    timerStartRefresh.start();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CLEARING EXISTING DATA
    ui->atlasList->clear();
    m_Packer.clear();
    QStringList sTextureNames = m_DataDir.entryList(QDir::NoDotAndDotDot);
    foreach(QString sTexName, sTextureNames)
        QFile::remove(sTexName);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // REPOPULATING THE PACKER WITH 'm_FrameList'
    for(int i = 0; i < m_FrameList.size(); ++i)
    {
        m_Packer.addItem(m_FrameList[i]->GetSize(),
                         m_FrameList[i]->GetCrop(),
                         m_FrameList[i]->GetHash(),
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

        pFrame->SetInfoFromPacker(bValid ? imgInfoRef.textureId : -1,
                                  imgInfoRef.rotated,
                                  imgInfoRef.pos.x() + m_Packer.border.l,
                                  imgInfoRef.pos.y() + m_Packer.border.t);

        QJsonObject frameObj;
        frameObj.insert("hash", QJsonValue(static_cast<qint64>(pFrame->GetHash())));
        frameObj.insert("name", QJsonValue(pFrame->GetName()));
        frameObj.insert("width", QJsonValue(pFrame->GetSize().width()));
        frameObj.insert("height", QJsonValue(pFrame->GetSize().height()));
        frameObj.insert("textureIndex", QJsonValue(pFrame->GetTextureIndex()));
        frameObj.insert("rotate", QJsonValue(pFrame->IsRotated()));
        frameObj.insert("x", QJsonValue(pFrame->GetX()));
        frameObj.insert("y", QJsonValue(pFrame->GetY()));
        frameObj.insert("cropLeft", QJsonValue(pFrame->GetCrop().left()));
        frameObj.insert("cropTop", QJsonValue(pFrame->GetCrop().top()));
        frameObj.insert("cropRight", QJsonValue(pFrame->GetCrop().right()));
        frameObj.insert("cropBottom", QJsonValue(pFrame->GetCrop().bottom()));

//        QJsonArray frameLinksArray;
//        QStringList sLinks = pFrame->GetLinks();
//        for(int i = 0; i < sLinks.size(); ++i)
//            frameLinksArray.append(QJsonValue(sLinks[i]));
//        frameObj.insert("links", QJsonValue(frameLinksArray));

        frameArray.append(QJsonValue(frameObj));

        if(bValid == false)
        {
            CreateTreeItem(NULL, pFrame->GetName(), -1, ATLAS_Frame_Warning, pFrame);
            continue;
        }
        else
            CreateTreeItem(NULL, pFrame->GetName(), pFrame->GetTextureIndex(), ATLAS_Frame, pFrame);

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

        if(imgInfoRef.rotated)
        {
            QTransform rotateTransform;
            rotateTransform.rotate(90);
            imgFrame = imgFrame.transformed(rotateTransform);

            size.transpose();
            crop = QRect(imgInfoRef.size.height() - crop.y() - crop.height(),
                         crop.x(), crop.height(), crop.width());
        }

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
        pTexture->save(m_DataDir.absolutePath() % "/" % HyGlobal::MakeFileNameFromCounter(i) % ".png");

        delete ppPainters[i];
        delete pTexture;
    }
    delete [] ppPainters;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // WRITE SETTINGS FILE TO ATLAS META DIR
    QJsonObject settingsObj = m_dlgSettings.GetSettings();
    settingsObj.insert("frames", frameArray);

    QFile settingsFile(m_MetaDir.absolutePath() % "/" % HYGUIPATH_MetaAtlasSettings);
    if(!settingsFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
       HyGuiLog("Couldn't open atlas settings file for writing", LOGTYPE_Error);
    }
    else
    {
        QJsonDocument settingsDoc(settingsObj);
        qint64 iBytesWritten = settingsFile.write(settingsDoc.toJson());//toBinaryData());
        if(0 == iBytesWritten || -1 == iBytesWritten)
        {
            HyGuiLog("Could not write to atlas settings file: " % settingsFile.errorString(), LOGTYPE_Error);
        }

        settingsFile.close();
    }

    qint64 i64TimeRefresh = timerStartRefresh.elapsed();
    clock_t timeEndRefresh = clock();

    HyGuiLog("Atlas Group Refresh done in: " % QString::number(static_cast<float>(i64TimeRefresh / 1000)), LOGTYPE_Normal);
    HyGuiLog("Atlas Group Pack done in:    " % QString::number(static_cast<float>(i64TimePack / 1000)), LOGTYPE_Normal);
    HyGuiLog("Atlas Group Refresh done in: " % QString::number(static_cast<float>((timeEndRefresh - timeStartRefresh) / CLOCKS_PER_SEC)), LOGTYPE_Info);
    HyGuiLog("Atlas Group Pack done in:    " % QString::number(static_cast<float>((timeEndPack - timeStartPack) / CLOCKS_PER_SEC)), LOGTYPE_Info);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // REGENERATE THE ATLAS DATA INFO FILE (HARMONY EXPORT)
    WidgetAtlasManager *pAtlasManager = GetManager();
    pAtlasManager->SaveData();
    
//    QStringList sReloadPaths;
//    for(int i = 0; i < m_FrameList.size(); ++i)
//    {
//        QStringList sLinks = m_FrameList[i]->GetLinks();
//        foreach(QString sLink, sLinks)
//            sReloadPaths.append(sLink);
//    }

    MainWindow::ReloadHarmony();

    ui->atlasList->expandAll();
    ui->atlasList->sortItems(0, Qt::AscendingOrder);
}

void WidgetAtlasGroup::CreateTreeItem(QTreeWidgetItem *pParent, QString sName, int iTextureIndex, eAtlasNodeType eType, HyGuiFrame *pFrame)
{
    QTreeWidgetItem *pNewTreeItem;
    if(pParent == NULL)
        pNewTreeItem = new QTreeWidgetItem(ui->atlasList);
    else
        pNewTreeItem = new QTreeWidgetItem();

    pNewTreeItem->setText(0, sName);
    pNewTreeItem->setIcon(0, HyGlobal::AtlasIcon(eType));

    if(iTextureIndex >= 0)
        pNewTreeItem->setText(1, "Tex:" % QString::number(iTextureIndex));
    else
        pNewTreeItem->setText(1, "Invalid");

    QVariant v; v.setValue(pFrame);
    pNewTreeItem->setData(0, Qt::UserRole, v);

    if(pParent)
        pParent->addChild(pNewTreeItem);

    ResizeAtlasListColumns();
}

void WidgetAtlasGroup::on_btnSettings_clicked()
{
    if(m_dlgSettings.GetName().isEmpty())
        m_dlgSettings.SetName("Atlas Group " % QString::number(GetId()));

    m_dlgSettings.DataToWidgets();
    m_dlgSettings.exec();
}

void WidgetAtlasGroup::on_atlasList_itemSelectionChanged()
{
    //WidgetAtlasManager *pAtlasManager = reinterpret_cast<WidgetAtlasManager *>(this->parent()->parent());
    ui->actionDeleteImage->setEnabled(ui->atlasList->selectedItems().count() != 0);
}

void WidgetAtlasGroup::on_actionDeleteImage_triggered()
{
    QList<QTreeWidgetItem *> selectedItems = ui->atlasList->selectedItems();

    for(int i = 0; i < selectedItems.count(); ++i)
    {
        HyGuiFrame *pFrame = selectedItems[i]->data(0, Qt::UserRole).value<HyGuiFrame *>();
        QStringList sLinks = pFrame->GetLinks();
        for(int i = 0; i < sLinks.count(); ++i)
        {
            sLinks[i];
        }

        m_FrameList.removeOne(pFrame);
        GetManager()->RemoveFrame(pFrame);
        delete selectedItems[i];
    }

    Refresh();
}
