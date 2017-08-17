/**************************************************************************
 *	HyGuiFrame.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "AtlasFrame.h"
#include "scriptum/imagepacker.h"

#include "AtlasWidget.h"

AtlasFrame::AtlasFrame(quint32 uiId,
                       quint32 uiChecksum,
                       quint32 uiAtlasGrpId,
                       QString sN,
                       QRect rAlphaCrop,
                       HyGuiItemType eType,
                       int iW,
                       int iH,
                       int iX,
                       int iY,
                       int iTextureIndex,
                       uint uiErrors) : m_uiUNIQUE_ID(uiId),
                                        m_eType(eType),
                                        m_uiAtlasGrpId(uiAtlasGrpId),
                                        m_pTreeWidgetItem(nullptr),
                                        m_uiImageChecksum(uiChecksum),
                                        m_sName(sN),
                                        m_iWidth(iW),
                                        m_iHeight(iH),
                                        m_rAlphaCrop(rAlphaCrop),
                                        m_iPosX(iX),
                                        m_iPosY(iY),
                                        m_iTextureIndex(iTextureIndex),
                                        m_uiErrors(uiErrors)
{
}

AtlasFrame::~AtlasFrame()
{
}

AtlasTreeItem *AtlasFrame::GetTreeItem()
{
    if(m_pTreeWidgetItem)
        return m_pTreeWidgetItem;

    m_pTreeWidgetItem = new AtlasTreeItem((QTreeWidgetItem *)nullptr, QTreeWidgetItem::Type);
    m_pTreeWidgetItem->setText(0, GetName());

    if(m_iTextureIndex >= 0)
    {
        m_pTreeWidgetItem->setText(1, "Grp:" % QString::number(m_uiAtlasGrpId));
        ClearError(GUIFRAMEERROR_CouldNotPack);
    }
    else
    {
        m_pTreeWidgetItem->setText(1, "Invalid");
        SetError(GUIFRAMEERROR_CouldNotPack);
    }

    UpdateTreeItemIconAndToolTip();

    QVariant v; v.setValue(this);
    m_pTreeWidgetItem->setData(0, Qt::UserRole, v);
    m_pTreeWidgetItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);

    return m_pTreeWidgetItem;
}

quint32 AtlasFrame::GetId()
{
    return m_uiUNIQUE_ID;
}

quint32 AtlasFrame::GetAtlasGrpId()
{
    return m_uiAtlasGrpId;
}

void AtlasFrame::SetAtlasGrpId(quint32 uiNewAtlasGrpId)
{
    m_uiAtlasGrpId = uiNewAtlasGrpId;
    if(m_pTreeWidgetItem)
        m_pTreeWidgetItem->setText(1, "Grp:" % QString::number(uiNewAtlasGrpId));
}

quint32 AtlasFrame::GetImageChecksum()
{
    return m_uiImageChecksum;
}

QString AtlasFrame::GetName()
{
    return m_sName;
}

void AtlasFrame::SetName(QString sNewName)
{
    m_sName = sNewName;
    m_pTreeWidgetItem->setText(0, m_sName);
}

QSize AtlasFrame::GetSize()
{
    return QSize(m_iWidth, m_iHeight);
}

QRect AtlasFrame::GetCrop()
{
    return m_rAlphaCrop;
}

QPoint AtlasFrame::GetPosition()
{
    return QPoint(m_iPosX, m_iPosY);
}

QSet<ProjectItem *> AtlasFrame::GetLinks()
{
    return m_DependencySet;
}

HyGuiItemType AtlasFrame::GetType()
{
    return m_eType;
}

int AtlasFrame::GetTextureIndex()
{
    return m_iTextureIndex;
}

int AtlasFrame::GetX()
{
    return m_iPosX;
}

int AtlasFrame::GetY()
{
    return m_iPosY;
}

void AtlasFrame::UpdateInfoFromPacker(int iTextureIndex, int iX, int iY)
{
    m_iTextureIndex = iTextureIndex;
    m_iPosX = iX;
    m_iPosY = iY;

    if(m_iTextureIndex != -1)
    {
        ClearError(GUIFRAMEERROR_CouldNotPack);

        if(m_pTreeWidgetItem)
            m_pTreeWidgetItem->setText(1, "Grp:" % QString::number(m_uiAtlasGrpId));
    }
    else
    {
        SetError(GUIFRAMEERROR_CouldNotPack);
        if(m_pTreeWidgetItem)
            m_pTreeWidgetItem->setText(1, "Invalid");
    }
}

QString AtlasFrame::ConstructImageFileName()
{
    QString sMetaImgName;
    sMetaImgName = sMetaImgName.sprintf("%010u", m_uiImageChecksum);
    sMetaImgName += ".png";

    return sMetaImgName;
}

void AtlasFrame::GetJsonObj(QJsonObject &frameObj)
{
    frameObj.insert("id", QJsonValue(static_cast<qint64>(GetId())));
    frameObj.insert("atlasGrpId", QJsonValue(static_cast<qint64>(GetAtlasGrpId())));
    frameObj.insert("checksum", QJsonValue(static_cast<qint64>(GetImageChecksum())));
    frameObj.insert("name", QJsonValue(GetName()));
    frameObj.insert("width", QJsonValue(GetSize().width()));
    frameObj.insert("height", QJsonValue(GetSize().height()));
    frameObj.insert("textureIndex", QJsonValue(GetTextureIndex()));
    frameObj.insert("type", QJsonValue(GetType()));
    frameObj.insert("x", QJsonValue(GetX()));
    frameObj.insert("y", QJsonValue(GetY()));
    frameObj.insert("cropLeft", QJsonValue(GetCrop().left()));
    frameObj.insert("cropTop", QJsonValue(GetCrop().top()));
    frameObj.insert("cropRight", QJsonValue(GetCrop().right()));
    frameObj.insert("cropBottom", QJsonValue(GetCrop().bottom()));
    frameObj.insert("errors", QJsonValue(static_cast<int>(GetErrors())));

    QString sFilterPath = "";
    if(m_pTreeWidgetItem)
    {
        QTreeWidgetItem *pTreeParent = m_pTreeWidgetItem->parent();
        while(pTreeParent)
        {
            if(pTreeParent->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter)
                break;

            pTreeParent = pTreeParent->parent();
        }
        if(pTreeParent)
            sFilterPath = HyGlobal::GetTreeWidgetItemPath(pTreeParent);
    }

    frameObj.insert("filter", QJsonValue(sFilterPath));
}

void AtlasFrame::SetError(eGuiFrameError eError)
{
    if(eError == GUIFRAMEERROR_CannotFindMetaImg)
        HyGuiLog(m_sName % " - GUIFRAMEERROR_CannotFindMetaImg", LOGTYPE_Error);
            
    m_uiErrors |= (1 << eError);
    
    UpdateTreeItemIconAndToolTip();
}

void AtlasFrame::ClearError(eGuiFrameError eError)
{
    m_uiErrors &= ~(1 << eError);

    UpdateTreeItemIconAndToolTip();
}

uint AtlasFrame::GetErrors()
{
    return m_uiErrors;
}

void AtlasFrame::UpdateTreeItemIconAndToolTip()
{
    if(m_pTreeWidgetItem)
    {
        // Duplicates are not considered and error so don't mark the icon as a warning (if only error)
        if(m_uiErrors == 0)
            m_pTreeWidgetItem->setIcon(0, HyGlobal::ItemIcon(m_eType, SUBICON_None));
        else
            m_pTreeWidgetItem->setIcon(0, HyGlobal::ItemIcon(m_eType, SUBICON_Warning));
        
        m_pTreeWidgetItem->setToolTip(0, HyGlobal::GetGuiFrameErrors(m_uiErrors));
    }
}

bool AtlasFrame::DeleteMetaImage(QDir metaDir)
{
    QFile imageFile(metaDir.path() % "/" % ConstructImageFileName());
    if(imageFile.remove() == false)
        return false;

    return true;
}

void AtlasFrame::ReplaceImage(QString sName, quint32 uiChecksum, QImage &newImage, QDir metaDir)
{
    m_sName = sName;

    if(m_pTreeWidgetItem)
        m_pTreeWidgetItem->setText(0, m_sName);

    m_uiImageChecksum = uiChecksum;
    m_iWidth = newImage.width();
    m_iHeight = newImage.height();

    if(m_eType != ITEM_Font && m_eType != ITEM_Spine) // Cannot crop 'sub-atlases' because they rely on their own UV coordinates
        m_rAlphaCrop = ImagePacker::crop(newImage);
    else
        m_rAlphaCrop = QRect(0, 0, newImage.width(), newImage.height());

    // DO NOT clear 'm_iTextureIndex' as it's needed in the WidgetAtlasGroup::Repack()

    if(newImage.save(metaDir.path() % "/" % ConstructImageFileName()) == false)
        HyGuiLog("Could not save frame image to meta directory: " % m_sName, LOGTYPE_Error);
}

QDataStream &operator<<(QDataStream &out, AtlasFrame *const &rhs)
{
    out.writeRawData(reinterpret_cast<const char*>(&rhs), sizeof(rhs));
    return out;
}

QDataStream &operator>>(QDataStream &in, AtlasFrame *rhs)
{
    in.readRawData(reinterpret_cast<char *>(rhs), sizeof(rhs));
    return in;
}
