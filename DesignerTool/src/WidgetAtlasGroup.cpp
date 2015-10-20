#include "WidgetAtlasGroup.h"
#include "ui_WidgetAtlasGroup.h"

#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStack>
#include <QPainter>

WidgetAtlasGroup::WidgetAtlasGroup(QWidget *parent) :   QWidget(parent),
                                                        ui(new Ui::WidgetAtlasGroup)
{
    ui->setupUi(this);

    // Invalid constructor. This exists so Q_OBJECT can work.
    HYLOG("WidgetAtlasGroup::WidgetAtlasGroup() invalid constructor used", LOGTYPE_Error);
}

WidgetAtlasGroup::WidgetAtlasGroup(QDir metaDir, QDir dataDir, QWidget *parent) :   QWidget(parent),
                                                                                    m_MetaDir(metaDir),
                                                                                    m_DataDir(dataDir),
                                                                                    ui(new Ui::WidgetAtlasGroup)
{
    ui->setupUi(this);

    ui->atlasList->setSelectionMode(QAbstractItemView::MultiSelection);

    QFile metaAtlasFile(m_MetaDir.absoluteFilePath(HYGUIPATH_MetaDataAtlasFileName));
    if(metaAtlasFile.exists())
    {
        QJsonDocument settingsDoc = QJsonDocument::fromBinaryData(metaAtlasFile.readAll());
        metaAtlasFile.close();

        QJsonObject settingsObj = settingsDoc.object();
        m_dlgSettings.LoadSettings(settingsObj);

        QJsonArray frameArray = settingsObj["frames"].toArray();
        for(int i = 0; i < frameArray.size(); ++i)
        {
            QJsonObject frameObj = frameArray[i].toObject();

            HyGuiFrame *pNewFrame = new HyGuiFrame(frameObj["hash"].toInt(),
                                                   frameObj["name"].toString(),
                                                   frameObj["width"].toInt(),
                                                   frameObj["height"].toInt(),
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
                    m_TextureList.append(CreateTreeItem(NULL, QString("Texture: " + m_TextureList.size()), ATLAS_Texture));

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

void WidgetAtlasGroup::ImportImages(QStringList sImportImgList)
{
    for(int i = 0; i < sImportImgList.size(); ++i)
    {
        QFileInfo fileInfo(sImportImgList[i]);

        QImage newImage;
        newImage.load(fileInfo.absoluteFilePath());
        quint32 uiHash = HyGlobal::CRCData(0, newImage.bits(), newImage.byteCount());

        HyGuiFrame *pNewFrame = new HyGuiFrame(uiHash, fileInfo.baseName());
        
        newImage.save(m_MetaDir.path() % "/" % pNewFrame->ConstructImageFileName());
        
        m_FrameList.append(pNewFrame);
    }

    Refresh();
}

void WidgetAtlasGroup::Refresh()
{
    // Wipe everything and generate

    ui->atlasList->clear();
    
    QImage *pImgList = new QImage[m_FrameList.size()];
    for(int i = 0; i < m_FrameList.size(); ++i)
    {
        QString sImageAbsoluteFilePath = m_MetaDir.absoluteFilePath(m_FrameList[i]->ConstructImageFileName());
        pImgList[i].load(sImageAbsoluteFilePath);
        m_Packer.addItem(pImgList[i], m_FrameList[i]->GetHash(), &m_FrameList[i], sImageAbsoluteFilePath);
    }



//    m_pAtlasOwner->SetPackerSettings(&m_Packer);

//    QList<QStringList> sMissingImgPaths = GetActiveTexture()->ImportFrames(sImportImgList);
//    bool bFailedToPack = false;
//    for(int i = 0; i < sMissingImgPaths.size(); ++i)
//    {
//        HyGuiTexture *pNewTexture = new HyGuiTexture(this);

//        QList<QStringList> sFailedImgPaths = pNewTexture->ImportFrames(sMissingImgPaths[i]);
//        if(sFailedImgPaths.size() > 0)
//        {
//            bFailedToPack = true;
//            for(int j = 0; j < sFailedImgPaths.size(); ++j)
//            {
//                foreach(const QString sPath, sFailedImgPaths[j])
//                    HYLOG("Could not import: " % sPath, LOGTYPE_Info);
//            }
//        }
//        else
//            m_Textures.append(pNewTexture); // Becomes active texture
//    }

//    if(bFailedToPack)
//        HYLOG("Imported image(s) failed to pack into current texture settings. Check log for info.", LOGTYPE_Warning);


    //SaveData();

    // Display texture
    //RenderAtlas();

    if(m_Packer.bins.size() == 0)
        return;

    for(int iBinIndex = 0; iBinIndex < m_Packer.bins.size(); ++iBinIndex)
    {
        if(m_dlgSettings.TextureWidth() == m_Packer.bins[iBinIndex].width() || m_dlgSettings.TextureHeight() == m_Packer.bins[iBinIndex].height())
            HYLOG("WidgetAtlasGroup::Refresh() Mismatching texture dimentions", LOGTYPE_Error);

        QImage imgTexture(m_dlgSettings.TextureWidth(), m_dlgSettings.TextureHeight(), QImage::Format_ARGB32);
        imgTexture.fill(Qt::transparent);


        QPainter p(&imgTexture);
        for(int i = 0; i < m_Packer.images.size(); ++i)
        {
            inputImage &imgInfoRef = m_Packer.images[i];

            if(imgInfoRef.textureId != iBinIndex)
                continue;

            if(imgInfoRef.duplicateId != NULL && m_Packer.merge)
                continue;

            QSize size;
            QRect crop;
            QPoint pos(imgInfoRef.pos.x() + m_Packer.border.l, imgInfoRef.pos.y() + m_Packer.border.t);

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

            QImage imgFrame(imgInfoRef.path);

            if(imgInfoRef.rotated)
            {
                QTransform rotateTransform;
                rotateTransform.rotate(90);
                imgFrame = imgFrame.transformed(rotateTransform);

                size.transpose();
                crop = QRect(imgInfoRef.size.height() - crop.y() - crop.height(),
                             crop.x(), crop.height(), crop.width());
            }

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

        imgTexture.save(m_DataDir.absolutePath() % "/" % HyGlobal::MakeFileNameFromCounter(iBinIndex) % ".png");
    }
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
