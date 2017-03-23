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
    m_HoverBackground.SetAsQuad(100.0f, 100.0f, false);
    m_HoverBackground.SetTint(0.0f, 0.0f, 0.0f);
    m_HoverBackground.alpha.Set(0.3f);
    
    AddChild(m_HoverBackground);
    
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
    for(int i = 0; i < m_CurrentPreviewList.size(); ++i)
        m_CurrentPreviewList[i]->SetEnabled(false);

    m_CurrentPreviewList.clear();
    for(int i = 0; i < selectedList.size(); ++i)
    {
        AtlasFrame *pFrame = selectedList[i]->data(0, Qt::UserRole).value<AtlasFrame *>();
        
        if(pFrame)
        {
            HyTexturedQuad2d *pQuad = m_MasterList[pFrame->GetTextureIndex()]->m_TexQuadMap[pFrame->GetChecksum()];

            pQuad->Load();
            pQuad->SetEnabled(true);
            m_CurrentPreviewList.append(pQuad);
        }
    }
}

void AtlasDraw::Update(IHyApplication &hyApp)
{
    m_pCamera->SetZoom(1.0f);
    m_pCamera->pos.Set(0.0f);
    glm::vec2 vResolution(static_cast<float>(hyApp.Window().GetResolution().x), static_cast<float>(hyApp.Window().GetResolution().y));

    glm::vec2 ptPos(vResolution.x * -0.5f, vResolution.y * 0.5f);
    float fCurMaxHeight = 0.0f;

    for(int i = 0; i < m_CurrentPreviewList.size(); ++i)
    {
        if(fCurMaxHeight < m_CurrentPreviewList[i]->GetHeight())
            fCurMaxHeight = m_CurrentPreviewList[i]->GetHeight();

        if(ptPos.x >= (vResolution.x * 0.5f) - m_CurrentPreviewList[i]->GetWidth())
        {
            ptPos.x = vResolution.x * -0.5f;
            ptPos.y -= fCurMaxHeight;
        }

        m_CurrentPreviewList[i]->pos.Tween(ptPos.x, ptPos.y - fCurMaxHeight, 0.1f, HyTween::QuadInOut);
        ptPos.x += m_CurrentPreviewList[i]->GetWidth();
    }
}

/*virtual*/ void AtlasDraw::OnShow(IHyApplication &hyApp)
{
    for(int i = 0; i < m_CurrentPreviewList.size(); ++i)
        m_CurrentPreviewList[i]->SetEnabled(true);
}

/*virtual*/ void AtlasDraw::OnHide(IHyApplication &hyApp)
{
    for(int i = 0; i < m_CurrentPreviewList.size(); ++i)
        m_CurrentPreviewList[i]->SetEnabled(false);
}
