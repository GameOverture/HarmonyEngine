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

HyGuiFrame::HyGuiFrame(quint32 uiCRC, QString sN, QRect rAlphaCrop, uint uiAtlasGroupId, int iW, int iH, int iTexIndex, bool bRot, int iX, int iY) :    m_uiHASH(uiCRC),
                                                                                                                                                        m_sNAME(sN),
                                                                                                                                                        m_iWIDTH(iW),
                                                                                                                                                        m_iHEIGHT(iH),
                                                                                                                                                        m_rALPHA_CROP(rAlphaCrop),
                                                                                                                                                        m_uiATLAS_GROUP_ID(uiAtlasGroupId),
                                                                                                                                                        m_iTextureIndex(iTexIndex),
                                                                                                                                                        m_bRotation(bRot),
                                                                                                                                                        m_iPosX(iX),
                                                                                                                                                        m_iPosY(iY)
{
    // This draw instance is reserved for AtlasManager and accessed by DrawInst() function
    RequestDrawInst();
}

HyGuiFrame::~HyGuiFrame()
{
    for(int i = 0; i < m_DrawInstList.count(); ++i)
        delete m_DrawInstList[i];
}

HyTexturedQuad2d *HyGuiFrame::RequestDrawInst()
{
    HyTexturedQuad2d *pDrawInst = new HyTexturedQuad2d(m_uiATLAS_GROUP_ID);
    if(m_bRotation == false)
        pDrawInst->SetTextureSource(m_iTextureIndex, GetX(), GetY(), m_rALPHA_CROP.width(), m_rALPHA_CROP.height());
    else
    {
        pDrawInst->SetTextureSource(m_iTextureIndex, GetX(), GetY(), m_rALPHA_CROP.height(), m_rALPHA_CROP.width());
        pDrawInst->rot_pivot.Set(m_rALPHA_CROP.height() * 0.5f, m_rALPHA_CROP.width() * 0.5f);
        pDrawInst->rot.Z(90);
    }

    pDrawInst->SetEnabled(false);
    pDrawInst->SetTag(reinterpret_cast<int64>(this));

    m_DrawInstList.append(pDrawInst);

    return pDrawInst;
}

void HyGuiFrame::UpdateInfoFromPacker(int iTextureIndex, bool bRotation, int iX, int iY)
{
    m_iTextureIndex = iTextureIndex;
    m_bRotation = bRotation;
    m_iPosX = iX;
    m_iPosY = iY;

    if(m_iTextureIndex != -1)
    {
        for(int i = 0; i < m_DrawInstList.count(); ++i)
        {
            if(m_bRotation == false)
                m_DrawInstList[i]->SetTextureSource(m_iTextureIndex, GetX(), GetY(), m_rALPHA_CROP.width(), m_rALPHA_CROP.height());
            else
            {
                m_DrawInstList[i]->SetTextureSource(m_iTextureIndex, GetX(), GetY(), m_rALPHA_CROP.height(), m_rALPHA_CROP.width());
                m_DrawInstList[i]->rot_pivot.Set(m_rALPHA_CROP.height() * 0.5f, m_rALPHA_CROP.width() * 0.5f);
                m_DrawInstList[i]->rot.Z(90);
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
    sMetaImgName = sMetaImgName.sprintf("%010u-%s", m_uiHASH, m_sNAME.toStdString().c_str());
    sMetaImgName += ".png";

    return sMetaImgName;
}
