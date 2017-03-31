/**************************************************************************
 *	AtlasModel.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "AtlasModel.h"

#include "Project.h"
#include "AtlasWidget.h"
#include "MainWindow.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPainter>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AtlasModel::FrameLookup::AddLookup(AtlasFrame *pFrame)
{
    m_FrameIdMap[pFrame->GetId()] = pFrame;

    uint32 uiChecksum = pFrame->GetImageChecksum();
    
    if(m_FrameChecksumMap.contains(uiChecksum))
    {
        m_FrameChecksumMap.find(uiChecksum).value().append(pFrame);
        HyGuiLog("'" % pFrame->GetName() % "' is a duplicate of '" % m_FrameChecksumMap.find(uiChecksum).value()[0]->GetName() % "' with the checksum: " % QString::number(uiChecksum) % " totaling: " % QString::number(m_FrameChecksumMap.find(uiChecksum).value().size()), LOGTYPE_Info);
    }
    else
    {
        QList<AtlasFrame *> newFrameList;
        newFrameList.append(pFrame);
        m_FrameChecksumMap[uiChecksum] = newFrameList;
    }
}
bool AtlasModel::FrameLookup::RemoveLookup(AtlasFrame *pFrame)  // Returns true if no remaining duplicates exist
{
    m_FrameIdMap.remove(pFrame->GetId());

    auto iter = m_FrameChecksumMap.find(pFrame->GetImageChecksum());
    if(iter == m_FrameChecksumMap.end())
        HyGuiLog("AtlasModel::RemoveLookup could not find frame", LOGTYPE_Error);
    
    iter.value().removeOne(pFrame);
    if(iter.value().size() == 0)
    {
        m_FrameChecksumMap.remove(pFrame->GetImageChecksum());
        return true;
    }
    
    return false;
}
AtlasFrame *AtlasModel::FrameLookup::Find(quint32 uiId)
{
    auto iter = m_FrameIdMap.find(uiId);
    if(iter == m_FrameIdMap.end())
        return nullptr;
    else
        return iter.value();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AtlasModel::AtlasModel(Project *pProjOwner) :   m_pProjOwner(pProjOwner),
                                                m_MetaDir(m_pProjOwner->GetMetaDataAbsPath() + HyGlobal::ItemName(ITEM_DirAtlases) + HyGlobal::ItemExt(ITEM_DirAtlases)),
                                                m_DataDir(m_pProjOwner->GetAssetsAbsPath() + HyGlobal::ItemName(ITEM_DirAtlases) + HyGlobal::ItemExt(ITEM_DirAtlases))
{
    if(m_MetaDir.exists() == false)
    {
        HyGuiLog("Meta atlas directory is missing, recreating", LOGTYPE_Info);
        m_MetaDir.mkpath(m_MetaDir.absolutePath());
    }
    if(m_DataDir.exists() == false)
    {
        HyGuiLog("Data atlas directory is missing, recreating", LOGTYPE_Info);
        m_DataDir.mkpath(m_DataDir.absolutePath());
    }

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

        m_uiNextFrameId = JSONOBJ_TOINT(settingsObj, "startFrameId");

        m_PackerSettings = settingsObj["settings"].toObject();

        // Create all the filter items first, storing their actual path in their data (for now)
        QJsonArray filtersArray = settingsObj["filters"].toArray();
        for(int i = 0; i < filtersArray.size(); ++i)
        {
            QDir filterPathDir(filtersArray.at(i).toString());

            AtlasTreeItem *pNewTreeItem = new AtlasTreeItem((QTreeWidgetItem *)nullptr, QTreeWidgetItem::Type);

            pNewTreeItem->setText(0, filterPathDir.dirName());
            pNewTreeItem->setIcon(0, HyGlobal::ItemIcon(ITEM_Prefix));

            QVariant v(QString(filterPathDir.absolutePath()));
            pNewTreeItem->setData(0, Qt::UserRole, v);

            m_TopLevelTreeItemList.append(pNewTreeItem);
        }

        // Then place the filters correctly as a parent heirarchy using the path string stored in their data
        QList<AtlasTreeItem *> atlasFiltersTreeItemList(m_TopLevelTreeItemList);
        for(int i = 0; i < m_TopLevelTreeItemList.size(); ++i)
        {
            AtlasTreeItem *pParentFilter = NULL;

            QString sFilterPath = m_TopLevelTreeItemList[i]->data(0, Qt::UserRole).toString();
            sFilterPath.truncate(sFilterPath.lastIndexOf("/"));
            if(sFilterPath != "")
            {
                for(int j = 0; j < atlasFiltersTreeItemList.size(); ++j)
                {
                    if(atlasFiltersTreeItemList[j]->data(0, Qt::UserRole).toString() == sFilterPath)
                    {
                        pParentFilter = atlasFiltersTreeItemList[j];
                        break;
                    }
                }
            }

            if(pParentFilter)
            {
                pParentFilter->addChild(m_TopLevelTreeItemList.takeAt(i));
                i = -1;
            }
        }

        // Finally go through all the filters and set the data string to the 'HYTREEWIDGETITEM_IsFilter' value to identify this QTreeWidgetItem as a filter
        for(int i = 0; i < atlasFiltersTreeItemList.size(); ++i)
            atlasFiltersTreeItemList[i]->setData(0, Qt::UserRole, QVariant(QString(HYTREEWIDGETITEM_IsFilter)));

        QJsonArray frameArray = settingsObj["frames"].toArray();
        for(int i = 0; i < frameArray.size(); ++i)
        {
            QJsonObject frameObj = frameArray[i].toObject();

            // TODO: Put this in parameter list, once snow white updated
            quint32 uiFrameId;
            if(false == frameObj.contains("id"))
                uiFrameId = ATLASFRAMEID_NotSet;
            else
                uiFrameId = JSONOBJ_TOINT(frameObj, "id");

            QRect rAlphaCrop(QPoint(frameObj["cropLeft"].toInt(), frameObj["cropTop"].toInt()), QPoint(frameObj["cropRight"].toInt(), frameObj["cropBottom"].toInt()));
            AtlasFrame *pNewFrame = CreateFrame(uiFrameId,
                                                JSONOBJ_TOINT(frameObj, "checksum"),
                                                frameObj["name"].toString(),
                                                rAlphaCrop,
                                                static_cast<eAtlasNodeType>(frameObj["type"].toInt()),
                                                frameObj["width"].toInt(),
                                                frameObj["height"].toInt(),
                                                frameObj["x"].toInt(),
                                                frameObj["y"].toInt(),
                                                frameObj["textureIndex"].toInt(),
                                                frameObj["errors"].toInt(0));

            QString sFilterPath = frameObj["filter"].toString();
            AtlasTreeItem *pFrameParent = NULL;
            if(sFilterPath != "")
            {
                for(int j = 0; j < atlasFiltersTreeItemList.size(); ++j)
                {
                    if(atlasFiltersTreeItemList[j]->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter && HyGlobal::GetTreeWidgetItemPath(atlasFiltersTreeItemList[j]) == sFilterPath)
                    {
                        pFrameParent = atlasFiltersTreeItemList[j];
                        break;
                    }
                }
            }

            if(QFile::exists(m_MetaDir.absoluteFilePath(pNewFrame->ConstructImageFileName())) == false)
                pNewFrame->SetError(GUIFRAMEERROR_CannotFindMetaImg);
            else
                pNewFrame->ClearError(GUIFRAMEERROR_CannotFindMetaImg);

            if(pFrameParent)
                pFrameParent->addChild(pNewFrame->GetTreeItem());
            else
                m_TopLevelTreeItemList.append(pNewFrame->GetTreeItem());
        }
    }

    if(m_TopLevelTreeItemList.empty())
        WriteMetaSettings();
}

/*virtual*/ AtlasModel::~AtlasModel()
{
    for(int i = 0; i < m_FrameList.size(); ++i)
        delete m_FrameList[i];
}

QList<AtlasFrame *> AtlasModel::GetFrames()
{
    return m_FrameList;
}

QJsonObject AtlasModel::GetPackerSettings()
{
    return m_PackerSettings;
}

void AtlasModel::TakeTreeWidgets(QList<AtlasTreeItem *> treeItemList)
{
    m_TopLevelTreeItemList = treeItemList;
}

QList<AtlasTreeItem *> AtlasModel::GetTopLevelTreeItemList()
{
    return m_TopLevelTreeItemList;
}

QSize AtlasModel::GetAtlasDimensions()
{
    int iWidth = m_PackerSettings["sbTextureWidth"].toInt();
    int iHeight = m_PackerSettings["sbTextureHeight"].toInt();
    return QSize(iWidth, iHeight);
}

int AtlasModel::GetNumTextures()
{
    return m_DataDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot).size() - 1;  // - 1 because don't include atlasInfo.json
}

void AtlasModel::WriteMetaSettings()
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

void AtlasModel::WriteMetaSettings(QJsonArray frameArray)
{
    QJsonObject settingsObj;
    settingsObj.insert("startFrameId", QJsonValue(static_cast<qint64>(m_uiNextFrameId)));
    settingsObj.insert("settings", m_PackerSettings);
    settingsObj.insert("frames", frameArray);

    QJsonArray filtersArray;
    if(m_pProjOwner->GetAtlasWidget())
    {
        QTreeWidgetItemIterator iter(m_pProjOwner->GetAtlasWidget()->GetFramesTreeWidget());
        while(*iter)
        {
            if((*iter)->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter)
            {
                QString sFilterPath = HyGlobal::GetTreeWidgetItemPath(*iter);
                filtersArray.append(QJsonValue(sFilterPath));
            }

            ++iter;
        }
    }
    else
    {
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
            filtersArray = settingsObj["filters"].toArray();
        }
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

AtlasFrame *AtlasModel::CreateFrame(quint32 uiId, quint32 uiChecksum, QString sN, QRect rAlphaCrop, eAtlasNodeType eType, int iW, int iH, int iX, int iY, int iAtlasIndex, uint uiErrors)
{
    if(uiId == ATLASFRAMEID_NotSet)
    {
        uiId = m_uiNextFrameId;
        m_uiNextFrameId++;
    }

    AtlasFrame *pNewFrame = new AtlasFrame(uiId, uiChecksum, sN, rAlphaCrop, eType, iW, iH, iX, iY, iAtlasIndex, uiErrors);

    m_FrameLookup.AddLookup(pNewFrame);
    m_FrameList.append(pNewFrame);
    
    return pNewFrame;
}

void AtlasModel::RemoveFrame(AtlasFrame *pFrame)
{
    if(m_FrameLookup.RemoveLookup(pFrame))
        pFrame->DeleteMetaImage(m_MetaDir);
    
    m_FrameList.removeOne(pFrame);

    delete pFrame;
}

AtlasFrame *AtlasModel::GenerateFrame(ProjectItem *pItem, QString sName, QImage &newImage, eAtlasNodeType eType)
{
    // This will create a meta image
    AtlasFrame *pFrame = ImportImage(sName, newImage, eType);

    QSet<AtlasFrame *> newFrameSet;
    newFrameSet.insert(pFrame);
    Repack(QSet<int>(), newFrameSet);

    // This retrieves the newly created AtlasFrame and links it to its ProjectItem
    QList<quint32> idList;
    idList.append(pFrame->GetId());
    QList<AtlasFrame *> returnList = RequestFramesById(pItem, idList);

    if(returnList.empty() == false)
        return returnList[0];

    return nullptr;
}

void AtlasModel::ReplaceFrame(AtlasFrame *pFrame, QString sName, QImage &newImage, bool bDoAtlasGroupRepack)
{
    QSet<int> textureIndexToReplaceSet;
    textureIndexToReplaceSet.insert(pFrame->GetTextureIndex());

    // First remove the frame from the map
    if(m_FrameLookup.RemoveLookup(pFrame))
        pFrame->DeleteMetaImage(m_MetaDir);

    // Determine the new checksum into the map
    quint32 uiChecksum = HyGlobal::CRCData(0, newImage.bits(), newImage.byteCount());
    pFrame->ReplaceImage(sName, uiChecksum, newImage, m_MetaDir);

    // Re-enter the frame into the map
    m_FrameLookup.AddLookup(pFrame);

    if(bDoAtlasGroupRepack)
        Repack(textureIndexToReplaceSet, QSet<AtlasFrame *>());
}

QList<AtlasFrame *> AtlasModel::RequestFrames(ProjectItem *pItem)
{
    QList<QTreeWidgetItem *> selectedItems;
    if(m_pProjOwner->GetAtlasWidget())
    {
        selectedItems = m_pProjOwner->GetAtlasWidget()->GetFramesTreeWidget()->selectedItems();
        qSort(selectedItems.begin(), selectedItems.end(), SortTreeWidgetsPredicate());

        m_pProjOwner->GetAtlasWidget()->GetFramesTreeWidget()->clearSelection();
    }

    QList<AtlasFrame *> frameRequestList;
    for(int i = 0; i < selectedItems.size(); ++i)
    {
        AtlasFrame *pFrame = selectedItems[i]->data(0, Qt::UserRole).value<AtlasFrame *>();
        if(pFrame == NULL)
            continue;

        frameRequestList.append(pFrame);
    }

    if(frameRequestList.empty())
        return QList<AtlasFrame *>();

    return RequestFrames(pItem, frameRequestList);
}

QList<AtlasFrame *> AtlasModel::RequestFramesById(ProjectItem *pItem, QList<quint32> requestList)
{
    if(requestList.empty())
        return QList<AtlasFrame *>();

    QList<AtlasFrame *> frameRequestList;
    for(int i = 0; i < requestList.size(); ++i)
    {
        AtlasFrame *pFoundFrame = m_FrameLookup.Find(requestList[i]);
        if(pFoundFrame == nullptr)
        {
            // TODO: Support a "Yes to all" dialog functionality here. Also note that the request list will not == the return list
            HyGuiLog("Cannot find image with checksum: " % QString::number(requestList[i]) % "\nIt may have been removed, or is invalid in the Atlas Manager.", LOGTYPE_Warning);
        }
        else
        {
            frameRequestList.append(pFoundFrame);
        }
    }

    return RequestFrames(pItem, frameRequestList);
}

QList<AtlasFrame *> AtlasModel::RequestFrames(ProjectItem *pItem, QList<AtlasFrame *> requestList)
{
    if(requestList.empty())
        return RequestFrames(pItem);

    QList<AtlasFrame *> returnList;
    for(int i = 0; i < requestList.size(); ++i)
    {
        requestList[i]->m_DependencySet.insert(pItem);
        //SetDependency(requestList[i], pItem);
        returnList.append(requestList[i]);
    }

    return returnList;
}

void AtlasModel::RelinquishFrames(ProjectItem *pItem, QList<AtlasFrame *> relinquishList)
{
    for(int i = 0; i < relinquishList.size(); ++i)
        relinquishList[i]->m_DependencySet.remove(pItem);

//    RemoveDependency(relinquishList[i], pItem);
}

QSet<AtlasFrame *> AtlasModel::ImportImages(QStringList sImportImgList)
{
    QSet<AtlasFrame *> returnSet;

    for(int i = 0; i < sImportImgList.size(); ++i)
    {
        QFileInfo fileInfo(sImportImgList[i]);

        QImage newImage(fileInfo.absoluteFilePath());

        returnSet.insert(ImportImage(fileInfo.baseName(), newImage, ATLAS_Frame));
    }

    return returnSet;
}

AtlasFrame *AtlasModel::ImportImage(QString sName, QImage &newImage, eAtlasNodeType eType)
{
    quint32 uiChecksum = HyGlobal::CRCData(0, newImage.bits(), newImage.byteCount());

    QRect rAlphaCrop(0, 0, newImage.width(), newImage.height());
    if(eType != ATLAS_Font && eType != ATLAS_Spine) // Cannot crop 'sub-atlases' because they rely on their own UV coordinates
        rAlphaCrop = ImagePacker::crop(newImage);

    AtlasFrame *pNewFrame = CreateFrame(ATLASFRAMEID_NotSet, uiChecksum, sName, rAlphaCrop, eType, newImage.width(), newImage.height(), -1, -1, -1, 0);
    if(pNewFrame)
    {
        // TODO: Should I care about overwriting a duplicate image?
        newImage.save(m_MetaDir.absoluteFilePath(pNewFrame->ConstructImageFileName()));
        m_pProjOwner->GetAtlasWidget()->GetFramesTreeWidget()->addTopLevelItem(pNewFrame->GetTreeItem());
    }

    return pNewFrame;
}

void AtlasModel::SaveData()
{
    QJsonObject atlasObj;
    GetAtlasInfoForGameData(atlasObj);

    QJsonDocument atlasInfoDoc;
    atlasInfoDoc.setObject(atlasObj);

    QFile atlasInfoFile(m_DataDir.absolutePath() % "/" % HYGUIPATH_DataAtlases);
    if(atlasInfoFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
    {
       HyGuiLog("Couldn't open atlas data info file for writing", LOGTYPE_Error);
    }
    else
    {
        qint64 iBytesWritten = atlasInfoFile.write(atlasInfoDoc.toJson());
        if(0 == iBytesWritten || -1 == iBytesWritten)
        {
            HyGuiLog("Could not write to atlas settings file: " % atlasInfoFile.errorString(), LOGTYPE_Error);
        }

        atlasInfoFile.close();
    }
}

void AtlasModel::GetAtlasInfoForGameData(QJsonObject &atlasObjOut)
{
    atlasObjOut.insert("id", m_DataDir.dirName().toInt());
    atlasObjOut.insert("width", m_PackerSettings["sbTextureWidth"].toInt());
    atlasObjOut.insert("height", m_PackerSettings["sbTextureHeight"].toInt());
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
        frameObj.insert("checksum", QJsonValue(static_cast<qint64>(m_FrameList[i]->GetImageChecksum())));
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

void AtlasModel::SetPackerSettings()
{
    m_Packer.sortOrder = m_PackerSettings["cmbSortOrder"].toInt();// m_iSortOrderIndex;//ui->cmbSortOrder->currentIndex();
    m_Packer.border.t = m_PackerSettings["sbFrameMarginTop"].toInt();// m_iFrameMarginTop;//ui->sbFrameMarginTop->value();
    m_Packer.border.l = m_PackerSettings["sbFrameMarginLeft"].toInt();// m_iFrameMarginLeft;//ui->sbFrameMarginLeft->value();
    m_Packer.border.r = m_PackerSettings["sbFrameMarginRight"].toInt();// m_iFrameMarginRight;//ui->sbFrameMarginRight->value();
    m_Packer.border.b = m_PackerSettings["sbFrameMarginBottom"].toInt();// m_iFrameMarginBottom;//ui->sbFrameMarginBottom->value();
    m_Packer.extrude = m_PackerSettings["extrude"].toInt();// m_iExtrude;//ui->extrude->value();
    m_Packer.merge = m_PackerSettings["chkMerge"].toBool();// m_bMerge;//ui->chkMerge->isChecked();
    m_Packer.square = m_PackerSettings["chkSquare"].toBool();// m_bSquare;//ui->chkSquare->isChecked();
    m_Packer.autosize = m_PackerSettings["chkAutosize"].toBool();// m_bAutoSize;//ui->chkAutosize->isChecked();
    m_Packer.minFillRate = m_PackerSettings["minFillRate"].toInt();// m_iFillRate;//ui->minFillRate->value();
    m_Packer.mergeBF = false;
    m_Packer.rotate = ImagePacker::NEVER;
}

QFileInfoList AtlasModel::GetExistingTextureInfoList()
{
    QStringList sNameFilterList;
    sNameFilterList << "*.png";
    
    return m_DataDir.entryInfoList(sNameFilterList, QDir::NoDotAndDotDot | QDir::Files, QDir::Name);
}

void AtlasModel::RepackAll()
{
    int iNumTotalTextures = GetNumTextures();

    QSet<int> textureIndexSet;
    for(int i = 0; i < iNumTotalTextures; ++i)
        textureIndexSet.insert(i);

    Repack(textureIndexSet, QSet<AtlasFrame *>());
}

void AtlasModel::Repack(QSet<int> repackTexIndicesSet, QSet<AtlasFrame *> newFramesSet)
{
    // Always repack the last texture to ensure it gets filled as much as it can
    QFileInfoList existingTexturesInfoList = GetExistingTextureInfoList();
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

    QList<AtlasFrame *>newFramesList = newFramesSet.toList();

    // Repack the affected frames and determine how many textures this repack took
    m_Packer.clear();
    for(int i = 0; i < newFramesList.size(); ++i)
    {
        m_Packer.addItem(newFramesList[i]->GetSize(),
                         newFramesList[i]->GetCrop(),
                         newFramesList[i]->GetImageChecksum(),
                         newFramesList[i],
                         m_MetaDir.absoluteFilePath(newFramesList[i]->ConstructImageFileName()));
    }
    SetPackerSettings();
    m_Packer.pack(m_PackerSettings["cmbHeuristic"].toInt(), m_PackerSettings["sbTextureWidth"].toInt(), m_PackerSettings["sbTextureHeight"].toInt());

    // Subtract '1' from the number of new textures because we want to ensure the last generated (and likely least filled) texture is last
    int iNumNewTextures = m_Packer.bins.size() - 1;

    // Delete the old textures
    for(int i = 0; i < textureIndexList.size(); ++i)
        QFile::remove(m_DataDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(textureIndexList[i]) % ".png"));

    // Regrab 'existingTexturesInfoList' after deleting obsolete textures
    existingTexturesInfoList = GetExistingTextureInfoList();

    // Using our stock of newly generated textures, fill in any gaps in the texture array. If there aren't enough new textures then shift textures (and their frames) to fill any remaining gaps in the indices.
    int iTotalNumTextures = iNumNewTextures + existingTexturesInfoList.size();

    int iNumNewTexturesUsed = 0;
    int iCurrentIndex = 0;
    for(; iCurrentIndex < iTotalNumTextures; ++iCurrentIndex)
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
                        existingTexturesInfoList = GetExistingTextureInfoList();

                        bHandled = true;
                        break;
                    }
                }
            }
            while(bHandled == false);
        }
    }

    // Place the last generated texture at the end of the array
    ConstructAtlasTexture(m_Packer.bins.size() - 1, iCurrentIndex);
    
    
    // Assign all the duplicate frames
    for(int i = 0; i < m_Packer.images.size(); ++i)
    {
        inputImage &imgInfoRef = m_Packer.images[i];
        if(imgInfoRef.duplicateId != nullptr)
        {
            AtlasFrame *pFrame = reinterpret_cast<AtlasFrame *>(imgInfoRef.id);
            AtlasFrame *pDupFrame = reinterpret_cast<AtlasFrame *>(imgInfoRef.duplicateId);
            
            pFrame->UpdateInfoFromPacker(pDupFrame->GetTextureIndex(), pDupFrame->GetX(), pDupFrame->GetY());
        }
    }

    Refresh();
}

void AtlasModel::ConstructAtlasTexture(int iPackerBinIndex, int iTextureArrayIndex)
{
    if(m_PackerSettings["sbTextureWidth"].toInt() != m_Packer.bins[iPackerBinIndex].width() || m_PackerSettings["sbTextureHeight"].toInt() != m_Packer.bins[iPackerBinIndex].height())
        HyGuiLog("WidgetAtlasGroup::ConstructAtlasTexture() Mismatching texture dimentions", LOGTYPE_Error);

    QImage newTexture(m_PackerSettings["sbTextureWidth"].toInt(), m_PackerSettings["sbTextureHeight"].toInt(), QImage::Format_ARGB32);
    newTexture.fill(Qt::transparent);

    QPainter p(&newTexture);

    // Iterate through the images that were packed, and update their corresponding HyGuiFrame. Then draw them to the blank textures
    for(int i = 0; i < m_Packer.images.size(); ++i)
    {
        inputImage &imgInfoRef = m_Packer.images[i];
        AtlasFrame *pFrame = reinterpret_cast<AtlasFrame *>(imgInfoRef.id);
        bool bValidToDraw = true;

        if(imgInfoRef.pos.x() == 999999)    // This is scriptum image packer's (dumb) indication of an invalid image...
        {
            pFrame->UpdateInfoFromPacker(-1, -1, -1);
            bValidToDraw = false;
        }
        else
            pFrame->ClearError(GUIFRAMEERROR_CouldNotPack);

        if(imgInfoRef.duplicateId != nullptr)
            bValidToDraw = false;

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

void AtlasModel::Refresh()
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
    SaveData();

    MainWindow::ReloadHarmony();

//    ui->lcdTexWidth->display(m_dlgSettings.TextureWidth());
//    ui->lcdTexHeight->display(m_dlgSettings.TextureHeight());

    HyGuiLog("Atlas Refresh finished", LOGTYPE_Normal);

}

