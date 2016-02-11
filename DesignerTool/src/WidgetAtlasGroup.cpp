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
#include "WidgetRenderer.h"

WidgetAtlasGroup::WidgetAtlasGroup(QWidget *parent) :   QWidget(parent),
                                                        m_DrawInst(0),
                                                        ui(new Ui::WidgetAtlasGroup)
{
    ui->setupUi(this);

    // Invalid constructor. This exists so Q_OBJECT can work.
    HYLOG("WidgetAtlasGroup::WidgetAtlasGroup() invalid constructor used", LOGTYPE_Error);
}

WidgetAtlasGroup::WidgetAtlasGroup(QDir metaDir, QDir dataDir, QWidget *parent) :   QWidget(parent),
                                                                                    m_MetaDir(metaDir),
                                                                                    m_DataDir(dataDir),
                                                                                    m_DrawInst(GetId()),
                                                                                    m_pCam(NULL),
                                                                                    ui(new Ui::WidgetAtlasGroup)
{
    ui->setupUi(this);

    ui->atlasList->setSelectionMode(QAbstractItemView::MultiSelection);

    QFile metaAtlasFile(m_MetaDir.absoluteFilePath(HYGUIPATH_MetaDataAtlasFileName));
    if(metaAtlasFile.exists())
    {
        if(!metaAtlasFile.open(QIODevice::ReadOnly))
        {
            QString sErrorMsg("WidgetAtlasGroup::WidgetAtlasGroup() could not open ");
            sErrorMsg += HYGUIPATH_MetaDataAtlasFileName;
            HYLOG(sErrorMsg, LOGTYPE_Error);
        }

        QJsonDocument settingsDoc = QJsonDocument::fromBinaryData(metaAtlasFile.readAll());
        metaAtlasFile.close();

        QJsonObject settingsObj = settingsDoc.object();
        m_dlgSettings.LoadSettings(settingsObj);

        QJsonArray frameArray = settingsObj["frames"].toArray();
        for(int i = 0; i < frameArray.size(); ++i)
        {
            QJsonObject frameObj = frameArray[i].toObject();

            QRect rAlphaCrop(frameObj["cropLeft"].toInt(), frameObj["cropTop"].toInt(), frameObj["cropRight"].toInt(), frameObj["cropBottom"].toInt());
            HyGuiFrame *pNewFrame = new HyGuiFrame(frameObj["hash"].toInt(),
                                                   frameObj["name"].toString(),
                                                   rAlphaCrop,
                                                   frameObj["width"].toInt(),
                                                   frameObj["height"].toInt(),
                                                   frameObj["textureIndex"].toInt(),
                                                   frameObj["rotate"].toBool(),
                                                   frameObj["x"].toInt(),
                                                   frameObj["y"].toInt());

            QJsonArray frameLinksArray = frameObj["links"].toArray();
            for(int k = 0; k < frameLinksArray.size(); ++k)
                pNewFrame->SetLink(frameLinksArray[k].toString());

            QTreeWidgetItem *pTextureTreeItem = NULL;
            eAtlasNodeType eIconType = ATLAS_Frame_Warning;
            if(frameObj["textureIndex"].toInt() >= 0)
            {
                while(m_TextureList.empty() || m_TextureList.size() <= frameObj["textureIndex"].toInt())
                    m_TextureList.append(CreateTreeItem(NULL, "Texture: " % QString::number(m_TextureList.size()), ATLAS_Texture));

                pTextureTreeItem = m_TextureList[frameObj["textureIndex"].toInt()];
                eIconType = ATLAS_Frame;
            }

            pNewFrame->SetTreeWidgetItem(CreateTreeItem(pTextureTreeItem, frameObj["name"].toString(), eIconType));
            
            m_FrameList.append(pNewFrame);
        }
    }
}

WidgetAtlasGroup::~WidgetAtlasGroup()
{
    delete ui;
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
        while(frameArrayList.empty() || frameArrayList.size() <= m_FrameList[i]->GetTextureIndex())
            frameArrayList.append(QJsonArray());
        
        QJsonObject frameObj;
        frameObj.insert("width", QJsonValue(m_FrameList[i]->GetSize().width()));
        frameObj.insert("height", QJsonValue(m_FrameList[i]->GetSize().height()));
        frameObj.insert("rotate", QJsonValue(m_FrameList[i]->IsRotated()));
        frameObj.insert("x", QJsonValue(m_FrameList[i]->GetX()));
        frameObj.insert("y", QJsonValue(m_FrameList[i]->GetY()));
        
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

/*virtual*/ void WidgetAtlasGroup::Show()
{
    LoadDrawInst();
    m_DrawInst.SetEnabled(true);

    if(m_pCam)
         m_pCam->SetEnabled(true);
}

/*virtual*/ void WidgetAtlasGroup::Hide()
{
    m_DrawInst.SetEnabled(false);
    
    if(m_pCam)
        m_pCam->SetEnabled(false);
}

/*virtual*/ void WidgetAtlasGroup::Draw(IHyApplication &hyApp)
{
    //m_pCam
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
    HYLOG("AtlasGroup mouseMoveEvent(): Enter", LOGTYPE_Normal);
    WidgetAtlasManager *pAtlasMan = static_cast<WidgetAtlasManager *>(parent()->parent());
    
    pAtlasMan->PreviewAtlasGroup();
    QWidget::enterEvent(pEvent);
}

/*virtual*/ void WidgetAtlasGroup::leaveEvent(QEvent *pEvent)
{
    HYLOG("AtlasGroup mouseMoveEvent(): Leave", LOGTYPE_Normal);
    WidgetAtlasManager *pAtlasMan = static_cast<WidgetAtlasManager *>(parent()->parent());
    
    pAtlasMan->HideAtlasGroup();
    QWidget::leaveEvent(pEvent);
}

void WidgetAtlasGroup::ImportImages(QStringList sImportImgList)
{
    for(int i = 0; i < sImportImgList.size(); ++i)
    {
        QFileInfo fileInfo(sImportImgList[i]);

        QImage newImage(fileInfo.absoluteFilePath());
        quint32 uiHash = HyGlobal::CRCData(0, newImage.bits(), newImage.byteCount());
        QRect rAlphaCrop = m_Packer.crop(newImage);

        HyGuiFrame *pNewFrame = new HyGuiFrame(uiHash, fileInfo.baseName(), rAlphaCrop, newImage.width(), newImage.height(), -1, false, -1, -1);
        
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

    m_TextureList.clear();
    ui->atlasList->clear();
    m_Packer.clear();
    QStringList sTextureNames = m_DataDir.entryList();
    foreach(QString sTexName, sTextureNames)
        QFile::remove(sTexName);

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

    QPainter **ppPainters = new QPainter *[m_Packer.bins.size()];
    for(int i = 0; i < m_Packer.bins.size(); ++i)
    {
        if(m_dlgSettings.TextureWidth() != m_Packer.bins[i].width() || m_dlgSettings.TextureHeight() != m_Packer.bins[i].height())
            HYLOG("WidgetAtlasGroup::Refresh() Mismatching texture dimentions", LOGTYPE_Error);

        QImage *pTexture = new QImage(m_dlgSettings.TextureWidth(), m_dlgSettings.TextureHeight(), QImage::Format_ARGB32);
        pTexture->fill(Qt::transparent);

        ppPainters[i] = new QPainter(pTexture);

        m_TextureList.append(CreateTreeItem(NULL, "Texture: " % QString::number(i), ATLAS_Texture));
    }

    QJsonArray frameArray;
    for(int i = 0; i < m_Packer.images.size(); ++i)
    {
        bool bValid = true;
        inputImage &imgInfoRef = m_Packer.images[i];

        if(imgInfoRef.duplicateId != NULL && m_Packer.merge)
            continue;

        if(imgInfoRef.pos.x() == 999999)    // This is scriptum image packer's indication of an invalid image...
            bValid = false;

        HyGuiFrame *pFrame = reinterpret_cast<HyGuiFrame *>(imgInfoRef.id);

        pFrame->SetPackerInfo(bValid ? imgInfoRef.textureId : -1,
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

        QJsonArray frameLinksArray;
        QStringList sLinks = pFrame->GetLinks();
        for(int i = 0; i < sLinks.size(); ++i)
            frameLinksArray.append(QJsonValue(sLinks[i]));
        frameObj.insert("links", QJsonValue(frameLinksArray));

        frameArray.append(QJsonValue(frameObj));

        if(bValid == false)
        {
            pFrame->SetTreeWidgetItem(CreateTreeItem(NULL, pFrame->GetName(), ATLAS_Frame_Warning));
            continue;
        }
        else
            pFrame->SetTreeWidgetItem(CreateTreeItem(m_TextureList[pFrame->GetTextureIndex()], pFrame->GetName(), ATLAS_Frame));

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

    // Save all textures out to atlas data dir
    for(int i = 0; i < m_Packer.bins.size(); ++i)
    {
        QImage *pTexture = static_cast<QImage *>(ppPainters[i]->device());
        pTexture->save(m_DataDir.absolutePath() % "/" % HyGlobal::MakeFileNameFromCounter(i) % ".png");
    }

    // Write settings file to atlas meta dir
    QJsonObject settingsObj = m_dlgSettings.GetSettings();
    settingsObj.insert("frames", frameArray);

    QFile settingsFile(m_MetaDir.absolutePath() % "/" % HYGUIPATH_MetaDataAtlasFileName);
    if(!settingsFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
       HYLOG("Couldn't open atlas settings file for writing", LOGTYPE_Error);
    }
    else
    {
        QJsonDocument settingsDoc(settingsObj);
        qint64 iBytesWritten = settingsFile.write(settingsDoc.toBinaryData());
        if(0 == iBytesWritten || -1 == iBytesWritten)
        {
            HYLOG("Could not write to atlas settings file: " % settingsFile.errorString(), LOGTYPE_Error);
        }

        settingsFile.close();
    }

    qint64 i64TimeRefresh = timerStartRefresh.elapsed();
    clock_t timeEndRefresh = clock();

    HYLOG("Atlas Group Refresh done in: " % QString::number(static_cast<float>(i64TimeRefresh / 1000)), LOGTYPE_Normal);
    HYLOG("Atlas Group Pack done in:    " % QString::number(static_cast<float>(i64TimePack / 1000)), LOGTYPE_Normal);
    HYLOG("Atlas Group Refresh done in: " % QString::number(static_cast<float>((timeEndRefresh - timeStartRefresh) / CLOCKS_PER_SEC)), LOGTYPE_Info);
    HYLOG("Atlas Group Pack done in:    " % QString::number(static_cast<float>((timeEndPack - timeStartPack) / CLOCKS_PER_SEC)), LOGTYPE_Info);

    // Regenerate the atlas data info file
    WidgetAtlasManager *pAtlasManager = reinterpret_cast<WidgetAtlasManager *>(this->parent()->parent());
    pAtlasManager->SaveData();
    
    QStringList sReloadPaths;
    for(int i = 0; i < m_FrameList.size(); ++i)
    {
        QStringList sLinks = m_FrameList[i]->GetLinks();
        foreach(QString sLink, sLinks)
            sReloadPaths.append(sLink);
    }
    
    m_DrawInst.Unload();
    m_DrawInst.Load();
    
    MainWindow::ReloadItems(sReloadPaths);
    
    pAtlasManager->PreviewAtlasGroup();
}

void WidgetAtlasGroup::LoadDrawInst()
{
    m_DrawInst.Load();
    
    foreach(HyGuiFrame *pFrame, m_FrameList)
        pFrame->LoadDrawInst();
}

QTreeWidgetItem *WidgetAtlasGroup::CreateTreeItem(QTreeWidgetItem *pParent, QString sName, eAtlasNodeType eType)
{
    QTreeWidgetItem *pNewTreeItem;
    if(pParent == NULL)
        pNewTreeItem = new QTreeWidgetItem(ui->atlasList);
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
