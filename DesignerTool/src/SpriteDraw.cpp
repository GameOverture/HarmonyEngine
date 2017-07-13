#include "SpriteDraw.h"

SpriteDraw::SpriteDraw(ProjectItem *pProjItem, IHyApplication &hyApp) : IDraw(pProjItem, hyApp),
                                                                        m_pCurFrame(nullptr)
{
    std::vector<glm::vec2> lineList(2, glm::vec2());

    lineList[0].x = -2048.0f;
    lineList[0].y = 0.0f;
    lineList[1].x = 2048.0f;
    lineList[1].y = 0.0f;
    m_primOriginHorz.SetAsLineChain(lineList);

    lineList[0].x = 0.0f;
    lineList[0].y = -2048.0f;
    lineList[1].x = 0.0f;
    lineList[1].y = 2048.0f;
    m_primOriginVert.SetAsLineChain(lineList);

    m_primOriginHorz.SetTint(1.0f, 0.0f, 0.0f);
    m_primOriginVert.SetTint(1.0f, 0.0f, 0.0f);

    ChildAppend(m_primOriginHorz);
    ChildAppend(m_primOriginVert);

    for(int i = 0; i < m_pProjItem->GetModel()->GetNumStates(); ++i)
    {
        SpriteStateData *pStateData = static_cast<SpriteStateData *>(m_pProjItem->GetModel()->GetStateData(i));
        for(int j = 0; j < pStateData->GetFramesModel()->rowCount(); ++j)
        {
            AtlasFrame *pFrame = pStateData->GetFramesModel()->GetFrameAt(j)->m_pFrame;

            HyTexturedQuad2d *pNewTexturedQuad = new HyTexturedQuad2d(pFrame->GetAtlasGrpId(), pFrame->GetTextureIndex(), this);
            pNewTexturedQuad->SetTextureSource(pFrame->GetX(), pFrame->GetY(), pFrame->GetCrop().width(), pFrame->GetCrop().height());

            m_TexturedQuadIdMap.insert(pFrame->GetId(), pNewTexturedQuad);
        }
    }
}

/*virtual*/ SpriteDraw::~SpriteDraw()
{
    for(auto iter = m_TexturedQuadIdMap.begin(); iter != m_TexturedQuadIdMap.end(); ++iter)
        delete iter.value();
}

void SpriteDraw::SetFrame(quint32 uiId, glm::vec2 vOffset)
{
    if(m_pCurFrame)
        m_pCurFrame->SetEnabled(false);

    auto iter = m_TexturedQuadIdMap.find(uiId);
    if(iter != m_TexturedQuadIdMap.end())
    {
        m_pCurFrame = iter.value();
        m_pCurFrame->pos.Set(vOffset);
        m_pCurFrame->SetEnabled(true);
    }
}

/*virtual*/ void SpriteDraw::OnShow(IHyApplication &hyApp)
{
    m_primOriginHorz.SetEnabled(true);
    m_primOriginVert.SetEnabled(true);

    if(m_pCurFrame)
        m_pCurFrame->SetEnabled(true);
}

/*virtual*/ void SpriteDraw::OnHide(IHyApplication &hyApp)
{
    SetEnabled(false);
}
