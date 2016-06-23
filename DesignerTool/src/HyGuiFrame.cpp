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

HyGuiFrame::HyGuiFrame()
{
    
}

HyGuiFrame::~HyGuiFrame()
{
    
}


HyTexturedQuad2d *HyGuiFrame::CreateHyTexturedQuad2d()
{
    HyTexturedQuad2d *pNewHyTexQuad2d = new HyTexturedQuad2d(m_uiATLAS_GROUP_ID);
    if(m_bRotation == false)
        pNewHyTexQuad2d->SetTextureSource(iTexIndex, GetX(), GetY(), m_rALPHA_CROP.width(), m_rALPHA_CROP.height());
    else
    {
        pNewHyTexQuad2d->SetTextureSource(iTexIndex, GetX(), GetY(), m_rALPHA_CROP.height(), m_rALPHA_CROP.width());
        pNewHyTexQuad2d->rot_pivot.Set(m_rALPHA_CROP.height() * 0.5f, m_rALPHA_CROP.width() * 0.5f);
        pNewHyTexQuad2d->rot.Z(90);
    }

    pNewHyTexQuad2d->SetDisplayOrder(1);
    pNewHyTexQuad2d->SetEnabled(false);

    return pNewHyTexQuad2d;
}
