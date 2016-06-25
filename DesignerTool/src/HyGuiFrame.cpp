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
                                                                                                                                                        m_iTextureIndex(iTexIndex),
                                                                                                                                                        m_bRotation(bRot),
                                                                                                                                                        m_iPosX(iX),
                                                                                                                                                        m_iPosY(iY)
{
    m_pDrawInst = new HyTexturedQuad2d(uiAtlasGroupId);
    if(m_bRotation == false)
        m_pDrawInst->SetTextureSource(m_iTextureIndex, GetX(), GetY(), m_rALPHA_CROP.width(), m_rALPHA_CROP.height());
    else
    {
        m_pDrawInst->SetTextureSource(m_iTextureIndex, GetX(), GetY(), m_rALPHA_CROP.height(), m_rALPHA_CROP.width());
        m_pDrawInst->rot_pivot.Set(m_rALPHA_CROP.height() * 0.5f, m_rALPHA_CROP.width() * 0.5f);
        m_pDrawInst->rot.Z(90);
    }

    m_pDrawInst->SetDisplayOrder(1);
    m_pDrawInst->SetEnabled(false);
}

HyGuiFrame::~HyGuiFrame()
{
    delete m_pDrawInst;
}
