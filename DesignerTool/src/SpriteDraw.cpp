#include "SpriteDraw.h"

SpriteDraw::SpriteDraw(SpriteModel &modelRef, IHyApplication &hyApp) :  IDraw(hyApp),
                                                                        m_ModelRef(modelRef),
                                                                        m_iCurStateIndex(0)
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
        SpriteStateData *pStateData = modelRef.GetStateData(i);
        AnimState *pNewState = new AnimState();

        for(int j = 0; j < pStateData->pFramesModel->rowCount(); ++j)
        {
            AtlasFrame *pFrame = pStateData->pFramesModel->GetFrameAt(j)->m_pFrame;

            HyTexturedQuad2d *pNewTexturedQuad = new HyTexturedQuad2d(pFrame->GetTextureIndex(), pNewState);
            pNewTexturedQuad->SetTextureSource(pFrame->GetX(), pFrame->GetY(), pFrame->GetCrop().width(), pFrame->GetCrop().height());

            pNewState->m_FrameMap.insert(pFrame->GetChecksum(), pNewTexturedQuad);
        }

        AddChild(*pNewState);
        m_AnimStates.append(pNewState);
    }

    SetEnabled(false);
}

/*virtual*/ SpriteDraw::~SpriteDraw()
{
    for(int i = 0; i < m_AnimStates.size(); ++i)
        delete m_AnimStates[i];
}

///*virtual*/ void SpriteDraw::Relink(AtlasFrame *pFrame)
//{
//    auto iter = m_FrameMap.find(pFrame->GetChecksum());
//    if(iter != m_FrameMap.end())
//        iter.value()->SetTextureSource(pFrame->GetX(), pFrame->GetY(), pFrame->GetCrop().width(), pFrame->GetCrop().height());
//}

/*virtual*/ void SpriteDraw::OnProjShow(IHyApplication &hyApp)
{
    m_primOriginHorz.SetEnabled(true);
    m_primOriginVert.SetEnabled(true);

    if(m_AnimStates.empty())
        return;

    if(m_AnimStates[m_iCurStateIndex]->m_pCurFrame)
        m_AnimStates[m_iCurStateIndex]->m_pCurFrame->SetEnabled(true);
}

/*virtual*/ void SpriteDraw::OnProjHide(IHyApplication &hyApp)
{
    SetEnabled(false);
}

/*virtual*/ void SpriteDraw::OnProjUpdate(IHyApplication &hyApp)
{
//    SpriteWidgetState *pCurSpriteState = static_cast<SpriteWidget *>(m_pItem->GetWidget())->GetCurSpriteState();
//    SpriteFrame *pSpriteFrame = pCurSpriteState->GetSelectedFrame();

//    if(pSpriteFrame == NULL)
//        return;

//    auto iter = m_FrameMap.find(pSpriteFrame->m_pFrame->GetChecksum());
//    if(iter != m_FrameMap.end() && m_pCurFrame != iter.value())
//    {
//        m_pCurFrame->SetEnabled(false);
//        m_pCurFrame = iter.value();
//        m_pCurFrame->SetEnabled(true);
//    }

//    QPoint ptRenderOffset = pSpriteFrame->GetRenderOffset();
//    m_pCurFrame->pos.X(ptRenderOffset.x());
//    m_pCurFrame->pos.Y(ptRenderOffset.y());

//    pCurSpriteState->UpdateTimeStep();
}
