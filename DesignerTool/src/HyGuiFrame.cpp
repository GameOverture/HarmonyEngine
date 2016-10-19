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

HyGuiFrame::HyGuiFrame(quint32 uiChecksum, QString sN, QRect rAlphaCrop, uint uiAtlasGroupId, int iW, int iH, int iTexIndex, bool bRot, int iX, int iY) :   m_uiATLAS_GROUP_ID(uiAtlasGroupId),
                                                                                                                                                            m_uiChecksum(uiChecksum),
                                                                                                                                                            m_sName(sN),
                                                                                                                                                            m_iWidth(iW),
                                                                                                                                                            m_iHeight(iH),
                                                                                                                                                            m_rAlphaCrop(rAlphaCrop),
                                                                                                                                                            m_iTextureIndex(iTexIndex),
                                                                                                                                                            m_bRotation(bRot),
                                                                                                                                                            m_iPosX(iX),
                                                                                                                                                            m_iPosY(iY)
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

void HyGuiFrame::ReplaceImage(QString sImgPath, QDir metaDir)
{
    QFileInfo fileInfo(sImgPath);
    QImage newImage(fileInfo.absoluteFilePath());

    ReplaceImage(fileInfo.baseName(), newImage, metaDir);
}

void HyGuiFrame::ReplaceImage(QString sName, QImage &newImage, QDir metaDir)
{
    QFile oldImageFile(metaDir.path() % "/" % ConstructImageFileName());
    if(oldImageFile.remove() == false)
        HyGuiLog("Could not remove old meta image file when replacing " % sName, LOGTYPE_Error);

    m_uiChecksum = HyGlobal::CRCData(0, newImage.bits(), newImage.byteCount());
    m_sName = sName;
    m_iWidth = newImage.width();
    m_iHeight = newImage.height();
    m_rAlphaCrop = ImagePacker::crop(newImage);
    m_iTextureIndex = -1;
    m_bRotation = false;
    m_iPosX = -1;
    m_iPosY = -1;

    if(newImage.save(metaDir.path() % "/" % ConstructImageFileName()) == false)
        HyGuiLog("Could not save frame image to meta directory: " % m_sName, LOGTYPE_Error);
}

HyTexturedQuad2d *HyGuiFrame::DrawInst(void *pKey)
{
    QMap<void *, HyTexturedQuad2d *>::iterator iter = m_DrawInstMap.find(pKey);
    if(iter != m_DrawInstMap.end())
        return iter.value();
    
    // Not found, create a new HyTexturedQuad2d based on key
    HyTexturedQuad2d *pDrawInst = new HyTexturedQuad2d(m_uiATLAS_GROUP_ID);
    if(m_bRotation == false)
        pDrawInst->SetTextureSource(m_iTextureIndex, GetX(), GetY(), m_rAlphaCrop.width(), m_rAlphaCrop.height());
    else
    {
        pDrawInst->SetTextureSource(m_iTextureIndex, GetX(), GetY(), m_rAlphaCrop.height(), m_rAlphaCrop.width());
        pDrawInst->rot_pivot.Set(m_rAlphaCrop.height() * 0.5f, m_rAlphaCrop.width() * 0.5f);
        pDrawInst->rot.Set(90);
    }

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

void HyGuiFrame::UpdateInfoFromPacker(int iTextureIndex, bool bRotation, int iX, int iY)
{
    m_iTextureIndex = iTextureIndex;
    m_bRotation = bRotation;
    m_iPosX = iX;
    m_iPosY = iY;

    if(m_iTextureIndex != -1)
    {
        QMapIterator<void *, HyTexturedQuad2d *> iter(m_DrawInstMap);
        while(iter.hasNext())
        {
            iter.next();
            
            if(m_bRotation == false)
                iter.value()->SetTextureSource(m_iTextureIndex, GetX(), GetY(), m_rAlphaCrop.width(), m_rAlphaCrop.height());
            else
            {
                iter.value()->SetTextureSource(m_iTextureIndex, GetX(), GetY(), m_rAlphaCrop.height(), m_rAlphaCrop.width());
                iter.value()->rot_pivot.Set(m_rAlphaCrop.height() * 0.5f, m_rAlphaCrop.width() * 0.5f);
                iter.value()->rot.Set(90);
            }
        }
    }

    for(int i = 0; i < m_Links.count(); ++i)
    {
        // TODO:
        //m_Links[i]
    }
}

QString HyGuiFrame::ConstructImageFileName()
{
    QString sMetaImgName;
    sMetaImgName = sMetaImgName.sprintf("%010u-%s", m_uiChecksum, m_sName.toStdString().c_str());
    sMetaImgName += ".png";

    return sMetaImgName;
}
