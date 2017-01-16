/**************************************************************************
 *	HyGuiFrame.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "HyGuiFrame.h"
#include "scriptum/imagepacker.h"

HyGuiFrame::HyGuiFrame(quint32 uiChecksum, QString sN, QRect rAlphaCrop, uint uiAtlasGroupId, eAtlasNodeType eType, int iW, int iH, int iTexIndex, int iX, int iY, uint uiErrors) : m_uiATLAS_GROUP_ID(uiAtlasGroupId),
                                                                                                                                                                                    m_eType(eType),
                                                                                                                                                                                    m_pTreeWidgetItem(NULL),
                                                                                                                                                                                    m_uiChecksum(uiChecksum),
                                                                                                                                                                                    m_sName(sN),
                                                                                                                                                                                    m_iWidth(iW),
                                                                                                                                                                                    m_iHeight(iH),
                                                                                                                                                                                    m_rAlphaCrop(rAlphaCrop),
                                                                                                                                                                                    m_iTextureIndex(iTexIndex),
                                                                                                                                                                                    m_iPosX(iX),
                                                                                                                                                                                    m_iPosY(iY),
                                                                                                                                                                                    m_uiErrors(uiErrors)
{
}

HyGuiFrame::~HyGuiFrame()
{
    QMapIterator<void *, HyTexturedQuad2d *> iter(m_DrawInstMap);
    while(iter.hasNext())
    {
        iter.next();
        delete iter.value();
    }
}

HyTexturedQuad2d *HyGuiFrame::DrawInst(void *pKey)
{
    QMap<void *, HyTexturedQuad2d *>::iterator iter = m_DrawInstMap.find(pKey);
    if(iter != m_DrawInstMap.end())
        return iter.value();
    
    // Not found, create a new HyTexturedQuad2d based on key
    HyTexturedQuad2d *pDrawInst = new HyTexturedQuad2d(m_uiATLAS_GROUP_ID);
    pDrawInst->SetTextureSource(m_iTextureIndex, GetX(), GetY(), m_rAlphaCrop.width(), m_rAlphaCrop.height());

    pDrawInst->SetEnabled(false);
    pDrawInst->SetTag(reinterpret_cast<int64>(this));

    m_DrawInstMap.insert(pKey, pDrawInst);

    return pDrawInst;
}

void HyGuiFrame::DeleteDrawInst(void *pKey)
{
    QMap<void *, HyTexturedQuad2d *>::iterator iter = m_DrawInstMap.find(pKey);
    if(iter != m_DrawInstMap.end())
    {
        iter.value()->Unload();
        m_DrawInstMap.remove(pKey);
    }
}

void HyGuiFrame::DeleteAllDrawInst()
{
    QMap<void *, HyTexturedQuad2d *>::iterator iter = m_DrawInstMap.begin();
    for(; iter != m_DrawInstMap.end(); ++iter)
    {
        delete iter.value();
    }
    
    m_DrawInstMap.clear();
}

void HyGuiFrame::SetTreeWidgetItem(QTreeWidgetItem *pTreeItem)
{
    m_pTreeWidgetItem = pTreeItem;

    if(m_pTreeWidgetItem == NULL)
        return;

    if(m_uiErrors == 0)
        m_pTreeWidgetItem->setIcon(0, HyGlobal::AtlasIcon(m_eType));
    else
    {
        m_pTreeWidgetItem->setIcon(0, HyGlobal::AtlasIcon(ATLAS_Frame_Warning));
        m_pTreeWidgetItem->setToolTip(0, HyGlobal::GetGuiFrameErrors(m_uiErrors));
    }
}

void HyGuiFrame::UpdateInfoFromPacker(int iTextureIndex, int iX, int iY)
{
    DeleteAllDrawInst();

    m_iTextureIndex = iTextureIndex;
    m_iPosX = iX;
    m_iPosY = iY;

    if(m_iTextureIndex != -1)
    {
        ClearError(GUIFRAMEERROR_CouldNotPack);

        QMapIterator<void *, HyTexturedQuad2d *> iter(m_DrawInstMap);
        while(iter.hasNext())
        {
            iter.next();
            iter.value()->SetTextureSource(m_iTextureIndex, GetX(), GetY(), m_rAlphaCrop.width(), m_rAlphaCrop.height());
        }

        if(m_pTreeWidgetItem)
            m_pTreeWidgetItem->setText(1, "Tex:" % QString::number(m_iTextureIndex));
    }
    else
    {
        SetError(GUIFRAMEERROR_CouldNotPack);
        if(m_pTreeWidgetItem)
            m_pTreeWidgetItem->setText(1, "Invalid");
    }
}

QString HyGuiFrame::ConstructImageFileName()
{
    QString sMetaImgName;
    sMetaImgName = sMetaImgName.sprintf("%010u-%s", m_uiChecksum, m_sName.toStdString().c_str());
    sMetaImgName += ".png";

    return sMetaImgName;
}

void HyGuiFrame::GetJsonObj(QJsonObject &frameObj)
{
    frameObj.insert("checksum", QJsonValue(static_cast<qint64>(GetChecksum())));
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
    QTreeWidgetItem *pTreeParent = m_pTreeWidgetItem->parent();
    while(pTreeParent)
    {
        if(pTreeParent->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter)
            break;

        pTreeParent = pTreeParent->parent();
    }
    if(pTreeParent)
        sFilterPath = HyGlobal::GetTreeWidgetItemPath(pTreeParent);

    frameObj.insert("filter", QJsonValue(sFilterPath));
}

void HyGuiFrame::SetError(eGuiFrameError eError)
{
    m_uiErrors |= (1 << eError);

    if(m_pTreeWidgetItem)
    {
        m_pTreeWidgetItem->setIcon(0, HyGlobal::AtlasIcon(ATLAS_Frame_Warning));
        m_pTreeWidgetItem->setToolTip(0, HyGlobal::GetGuiFrameErrors(m_uiErrors));
    }
}

void HyGuiFrame::ClearError(eGuiFrameError eError)
{
    m_uiErrors &= ~(1 << eError);

    if(m_pTreeWidgetItem)
    {
        if(m_uiErrors == 0)
        {
            m_pTreeWidgetItem->setIcon(0, HyGlobal::AtlasIcon(m_eType));
            m_pTreeWidgetItem->setToolTip(0, "");
        }
        else
        {
            m_pTreeWidgetItem->setIcon(0, HyGlobal::AtlasIcon(ATLAS_Frame_Warning));
            m_pTreeWidgetItem->setToolTip(0, HyGlobal::GetGuiFrameErrors(m_uiErrors));
        }
    }
}

uint HyGuiFrame::GetErrors()
{
    return m_uiErrors;
}

bool HyGuiFrame::DeleteMetaImage(QDir metaDir)
{
    if(0 != (m_uiErrors & GUIFRAMEERROR_Duplicate))
        return true;

    QFile imageFile(metaDir.path() % "/" % ConstructImageFileName());
    if(imageFile.remove() == false)
        return false;

    return true;
}

void HyGuiFrame::ReplaceImage(QString sName, quint32 uiChecksum, QImage &newImage, QDir metaDir)
{
    DeleteMetaImage(metaDir);

    m_sName = sName;
    m_pTreeWidgetItem->setText(0, m_sName);

    m_uiChecksum = uiChecksum;
    m_iWidth = newImage.width();
    m_iHeight = newImage.height();

    if(m_eType != ATLAS_Font && m_eType != ATLAS_Spine) // Cannot crop 'sub-atlases' because they rely on their own UV coordinates
        m_rAlphaCrop = ImagePacker::crop(newImage);
    else
        m_rAlphaCrop = QRect(0, 0, newImage.width(), newImage.height());

    m_iTextureIndex = -1;
    m_iPosX = -1;
    m_iPosY = -1;

    if(newImage.save(metaDir.path() % "/" % ConstructImageFileName()) == false)
        HyGuiLog("Could not save frame image to meta directory: " % m_sName, LOGTYPE_Error);
}

QDataStream &operator<<(QDataStream &out, HyGuiFrame *const &rhs)
{
    out.writeRawData(reinterpret_cast<const char*>(&rhs), sizeof(rhs));
    return out;
}

QDataStream &operator>>(QDataStream &in, HyGuiFrame *rhs)
{
    in.readRawData(reinterpret_cast<char *>(rhs), sizeof(rhs));
    return in;
}
