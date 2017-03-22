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
        
        while(m_MasterList.size() <= uiTextureIndex)
            m_MasterList.append(new TextureEnt(this));
        
        HyTexturedQuad2d *pNewTexQuad = new HyTexturedQuad2d(uiTextureIndex, m_MasterList[uiTextureIndex]);
        pNewTexQuad->SetTextureSource(frameList[i]->GetX(), frameList[i]->GetY(), frameList[i]->GetCrop().width(), frameList[i]->GetCrop().height());
        
        m_MasterList[uiTextureIndex]->m_TexQuadMap.insert(frameList[i]->GetChecksum(), pNewTexQuad);
    }
}

/*virtual*/ AtlasDraw::~AtlasDraw()
{

}

void AtlasDraw::SetSelected(QList<QTreeWidgetItem *> selectedList)
{
    for(int i = 0; i < selectedList.size(); ++i)
    {
        AtlasFrame *pFrame = selectedList[i]->data(0, Qt::UserRole).value<AtlasFrame *>();
        
        if(pFrame)
        {
            if(m_CurrentPreviewMap.contains(pFrame->GetChecksum()) ==  false)
            {
                HyTexturedQuad2d *pQuad = m_MasterList[pFrame->GetTextureIndex()]->m_TexQuadMap[pFrame->GetChecksum()];
                
                pQuad->Load();
                m_CurrentPreviewMap.insert(pFrame->GetChecksum(), pQuad);
            }
        }
    }
}

/*virtual*/ void AtlasDraw::OnShow(IHyApplication &hyApp)
{
}

/*virtual*/ void AtlasDraw::OnHide(IHyApplication &hyApp)
{
}
