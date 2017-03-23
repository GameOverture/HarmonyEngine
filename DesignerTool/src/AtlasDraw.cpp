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

#define DISPLAYORDER_AtlasSelectedFrames 1000
#define DISPLAYORDER_AtlasHoverBG 1001
#define DISPLAYORDER_AtlasHoverFrame 1002

AtlasDraw::AtlasDraw(AtlasModel *pModelRef, IHyApplication *pHyApp) :   IDraw(*pHyApp),
                                                                        m_ModelRef(*pModelRef),
                                                                        m_pHoverTexQuad(nullptr)
{
    m_HoverBackground.SetAsQuad(100.0f, 100.0f, false);
    m_HoverBackground.SetTint(0.0f, 0.0f, 0.0f);
    m_HoverBackground.SetDisplayOrder(DISPLAYORDER_AtlasHoverBG);
    m_HoverBackground.SetEnabled(false);
    m_HoverBackground.Load();
    
    
    QList<AtlasFrame *> frameList = m_ModelRef.GetFrames();
    for(int i = 0; i < frameList.size(); ++i)
    {
        uint32 uiTextureIndex = frameList[i]->GetTextureIndex();
        
        while(m_MasterList.size() <= uiTextureIndex)
            m_MasterList.append(new TextureEnt(this));
        
        HyTexturedQuad2d *pNewTexQuad = new HyTexturedQuad2d(uiTextureIndex, m_MasterList[uiTextureIndex]);
        pNewTexQuad->SetTextureSource(frameList[i]->GetX(), frameList[i]->GetY(), frameList[i]->GetCrop().width(), frameList[i]->GetCrop().height());
        pNewTexQuad->SetDisplayOrder(DISPLAYORDER_AtlasSelectedFrames);
        
        m_MasterList[uiTextureIndex]->m_TexQuadMap.insert(frameList[i]->GetChecksum(), pNewTexQuad);
    }
}

/*virtual*/ AtlasDraw::~AtlasDraw()
{

}

void AtlasDraw::SetHover(QTreeWidgetItem *pHoverItem)
{
    HyTexturedQuad2d *pNewHoverTexQuad = nullptr;

    if(pHoverItem)
    {
        AtlasFrame *pFrame = pHoverItem->data(0, Qt::UserRole).value<AtlasFrame *>();
        if(pFrame)
            pNewHoverTexQuad = m_MasterList[pFrame->GetTextureIndex()]->m_TexQuadMap[pFrame->GetChecksum()];
    }

    if(m_pHoverTexQuad)
    {
        m_pHoverTexQuad->SetDisplayOrder(DISPLAYORDER_AtlasSelectedFrames);
        m_pHoverTexQuad->SetEnabled(false);
        m_pHoverTexQuad = nullptr;
    }
    m_HoverBackground.SetEnabled(false);

    if(pNewHoverTexQuad)
    {
        m_pHoverTexQuad = pNewHoverTexQuad;
        m_pHoverTexQuad->Load();
        m_pHoverTexQuad->SetEnabled(true);
        m_pHoverTexQuad->SetDisplayOrder(DISPLAYORDER_AtlasHoverFrame);

        m_HoverBackground.SetEnabled(true);
        m_HoverBackground.SetAsQuad(static_cast<float>(m_pHoverTexQuad->GetWidth()), static_cast<float>(m_pHoverTexQuad->GetHeight()), false);

        m_pHoverTexQuad->AddChild(m_HoverBackground);
    }
}

void AtlasDraw::SetSelected(QList<QTreeWidgetItem *> selectedList)
{
    for(int i = 0; i < m_SelectedTexQuadList.size(); ++i)
        m_SelectedTexQuadList[i]->SetEnabled(false);

    m_SelectedTexQuadList.clear();
    for(int i = 0; i < selectedList.size(); ++i)
    {
        AtlasFrame *pFrame = selectedList[i]->data(0, Qt::UserRole).value<AtlasFrame *>();
        if(pFrame)
        {
            HyTexturedQuad2d *pQuad = m_MasterList[pFrame->GetTextureIndex()]->m_TexQuadMap[pFrame->GetChecksum()];
            pQuad->Load();
            pQuad->SetEnabled(true);

            if(pQuad == m_pHoverTexQuad)
                pQuad->SetDisplayOrder(DISPLAYORDER_AtlasSelectedFrames);

            m_SelectedTexQuadList.append(pQuad);
        }
    }

    if(m_pHoverTexQuad)
        m_pHoverTexQuad->pos.Set(m_pHoverTexQuad->GetWidth() * -0.5f, m_pHoverTexQuad->GetHeight() * -0.5f);
}

void AtlasDraw::Update(IHyApplication &hyApp)
{
    m_pCamera->SetZoom(1.0f);
    m_pCamera->pos.Set(0.0f);
    glm::vec2 vResolution(static_cast<float>(hyApp.Window().GetResolution().x), static_cast<float>(hyApp.Window().GetResolution().y));

    glm::vec2 ptPos(vResolution.x * -0.5f, vResolution.y * 0.5f);
    float fCurMaxHeight = 0.0f;

    for(int i = 0; i < m_SelectedTexQuadList.size(); ++i)
    {
        if(fCurMaxHeight < m_SelectedTexQuadList[i]->GetHeight())
            fCurMaxHeight = m_SelectedTexQuadList[i]->GetHeight();

        if(ptPos.x >= (vResolution.x * 0.5f) - m_SelectedTexQuadList[i]->GetWidth())
        {
            ptPos.x = vResolution.x * -0.5f;
            ptPos.y -= fCurMaxHeight;
        }

        m_SelectedTexQuadList[i]->pos.Tween(ptPos.x, ptPos.y - fCurMaxHeight, 0.1f, HyTween::QuadInOut);
        ptPos.x += m_SelectedTexQuadList[i]->GetWidth();
    }
}

/*virtual*/ void AtlasDraw::OnShow(IHyApplication &hyApp)
{
    for(int i = 0; i < m_SelectedTexQuadList.size(); ++i)
        m_SelectedTexQuadList[i]->SetEnabled(true);
}

/*virtual*/ void AtlasDraw::OnHide(IHyApplication &hyApp)
{
    for(int i = 0; i < m_SelectedTexQuadList.size(); ++i)
        m_SelectedTexQuadList[i]->SetEnabled(false);
}
