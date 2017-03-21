#include "SpriteDraw.h"

SpriteDraw::SpriteDraw(SpriteModel &modelRef, IHyApplication &hyApp) :  IDraw(hyApp),
                                                                        m_ModelRef(modelRef),
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

    AddChild(m_primOriginHorz);
    AddChild(m_primOriginVert);

    for(int i = 0; i < modelRef.GetNumStates(); ++i)
    {
        SpriteStateData *pStateData = static_cast<SpriteStateData *>(modelRef.GetStateData(i));
        for(int j = 0; j < pStateData->GetFramesModel()->rowCount(); ++j)
        {
            AtlasFrame *pFrame = pStateData->GetFramesModel()->GetFrameAt(j)->m_pFrame;

            HyTexturedQuad2d *pNewTexturedQuad = new HyTexturedQuad2d(pFrame->GetTextureIndex(), this);
            pNewTexturedQuad->SetTextureSource(pFrame->GetX(), pFrame->GetY(), pFrame->GetCrop().width(), pFrame->GetCrop().height());

            m_TexturedQuadMap.insert(pFrame->GetChecksum(), pNewTexturedQuad);
        }
    }
}

/*virtual*/ SpriteDraw::~SpriteDraw()
{
    for(auto iter = m_TexturedQuadMap.begin(); iter != m_TexturedQuadMap.end(); ++iter)
        delete iter.value();
}

///*virtual*/ void SpriteDraw::Relink(AtlasFrame *pFrame)
//{
//    auto iter = m_FrameMap.find(pFrame->GetChecksum());
//    if(iter != m_FrameMap.end())
//        iter.value()->SetTextureSource(pFrame->GetX(), pFrame->GetY(), pFrame->GetCrop().width(), pFrame->GetCrop().height());
//}

void SpriteDraw::SetFrame(quint32 uiChecksum, glm::vec2 vOffset)
{
    if(m_pCurFrame)
        m_pCurFrame->SetEnabled(false);

    auto iter = m_TexturedQuadMap.find(uiChecksum);
    if(iter != m_TexturedQuadMap.end())
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
