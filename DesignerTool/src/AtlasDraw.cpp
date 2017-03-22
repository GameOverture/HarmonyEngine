/**************************************************************************
 *	AtlasDraw.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "AtlasDraw.h"

AtlasDraw::AtlasDraw(AtlasModel *pModelRef, IHyApplication *pHyApp) :   IDraw(*pHyApp),
                                                                        m_ModelRef(*pModelRef)
{
    m_ShadeBackground.SetAsQuad(100.0f, 100.0f, false);
    m_ShadeBackground.SetTint(0.0f, 0.0f, 0.0f);
    m_ShadeBackground.alpha.Set(0.3f);
    
    AddChild(m_ShadeBackground);
    
    QList<AtlasFrame *> frameList = m_ModelRef.GetFrames();
    for(int i = 0; i < frameList.size(); ++i)
    {
        uint32 uiTextureIndex = frameList[i]->GetTextureIndex();
        
        while(m_TextureEntList.size() <= uiTextureIndex)
            m_TextureEntList.append(new TextureEnt(this));
        
        HyTexturedQuad2d *pNewTexQuad = new HyTexturedQuad2d(uiTextureIndex, m_TextureEntList[uiTextureIndex]);
        pNewTexQuad->SetTextureSource(frameList[i]->GetX(), frameList[i]->GetY(), frameList[i]->GetSize().width(), frameList[i]->GetSize().height());
        m_TextureEntList[uiTextureIndex]->m_PreviewQuadList.append(pNewTexQuad);
    }
}

/*virtual*/ AtlasDraw::~AtlasDraw()
{

}

/*virtual*/ void AtlasDraw::OnShow(IHyApplication &hyApp)
{
}

/*virtual*/ void AtlasDraw::OnHide(IHyApplication &hyApp)
{
}
