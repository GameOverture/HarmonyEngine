#include "SpriteDraw.h"

SpriteDraw::SpriteDraw(SpriteItem *pItem) : m_pItem(pItem),
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
}

/*virtual*/ SpriteDraw::~SpriteDraw()
{
}

/*virtual*/ void SpriteDraw::Relink(AtlasFrame *pFrame)
{
    auto iter = m_FrameMap.find(pFrame->GetChecksum());
    if(iter != m_FrameMap.end())
        iter.value()->SetTextureSource(pFrame->GetX(), pFrame->GetY(), pFrame->GetCrop().width(), pFrame->GetCrop().height());
}

/*virtual*/ void SpriteDraw::OnPreLoad(IHyApplication &hyApp)
{
    QList<AtlasFrame *> frameList = static_cast<SpriteWidget *>(m_pItem->GetWidget())->GetAllDrawInsts();
    for(int i = 0; i < frameList.size(); ++i)
    {
        if(false == m_FrameMap.contains(frameList[i]->GetChecksum()))
        {
            HyTexturedQuad2d *pNewFrame = new HyTexturedQuad2d(frameList[i]->GetTextureIndex(), this);
            pNewFrame->SetTextureSource(frameList[i]->GetX(), frameList[i]->GetY(), frameList[i]->GetCrop().width(), frameList[i]->GetCrop().height());
            m_FrameMap.insert(frameList[i]->GetChecksum(), pNewFrame);
        }
    }
}

/*virtual*/ void SpriteDraw::OnPostUnload(IHyApplication &hyApp)
{
    for(auto iter = m_FrameMap.begin(); iter != m_FrameMap.end(); ++iter)
        delete iter.value();

    m_FrameMap.clear();
    m_pCurFrame = nullptr;
}

/*virtual*/ void SpriteDraw::OnProjShow(IHyApplication &hyApp)
{
    m_primOriginHorz.SetEnabled(true);
    m_primOriginVert.SetEnabled(true);

    SpriteWidgetState *pCurSpriteState = static_cast<SpriteWidget *>(m_pItem->GetWidget())->GetCurSpriteState();
    SpriteFrame *pSpriteFrame = pCurSpriteState->GetSelectedFrame();

    if(pSpriteFrame == NULL)
        return;

    if(m_pCurFrame)
        m_pCurFrame->SetEnabled(false);

    auto iter = m_FrameMap.find(pSpriteFrame->m_pFrame->GetChecksum());
    if(iter != m_FrameMap.end())
        m_pCurFrame = iter.value();

    m_pCurFrame->SetEnabled(true);
}

/*virtual*/ void SpriteDraw::OnProjHide(IHyApplication &hyApp)
{
    m_primOriginHorz.SetEnabled(false);
    m_primOriginVert.SetEnabled(false);

    m_pCurFrame->SetEnabled(false);
}

/*virtual*/ void SpriteDraw::OnProjUpdate(IHyApplication &hyApp)
{
    SpriteWidgetState *pCurSpriteState = static_cast<SpriteWidget *>(m_pItem->GetWidget())->GetCurSpriteState();
    SpriteFrame *pSpriteFrame = pCurSpriteState->GetSelectedFrame();

    if(pSpriteFrame == NULL)
        return;

    auto iter = m_FrameMap.find(pSpriteFrame->m_pFrame->GetChecksum());
    if(iter != m_FrameMap.end() && m_pCurFrame != iter.value())
    {
        m_pCurFrame->SetEnabled(false);
        m_pCurFrame = iter.value();
        m_pCurFrame->SetEnabled(true);
    }

    QPoint ptRenderOffset = pSpriteFrame->GetRenderOffset();
    m_pCurFrame->pos.X(ptRenderOffset.x());
    m_pCurFrame->pos.Y(ptRenderOffset.y());

    pCurSpriteState->UpdateTimeStep();
}
