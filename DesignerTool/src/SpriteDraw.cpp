#include "SpriteDraw.h"

SpriteDraw::SpriteDraw(SpriteItem *pItem) : m_pItem(pItem)
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
}

/*virtual*/ SpriteDraw::~SpriteDraw()
{
}

/*virtual*/ void SpriteDraw::OnProjLoad(IHyApplication &hyApp)
{
    m_primOriginHorz.Load();
    m_primOriginVert.Load();
}

/*virtual*/ void SpriteDraw::OnProjUnload(IHyApplication &hyApp)
{
    m_primOriginHorz.Unload();
    m_primOriginVert.Unload();

    QList<AtlasFrame *> frameList = static_cast<SpriteWidget *>(m_pItem->GetWidget())->GetAllDrawInsts();
    for(int i = 0; i < frameList.count(); i++)
        frameList[i]->DrawInst(this)->Unload();
}

/*virtual*/ void SpriteDraw::OnProjShow(IHyApplication &hyApp)
{
    m_primOriginHorz.SetEnabled(true);
    m_primOriginVert.SetEnabled(true);
}

/*virtual*/ void SpriteDraw::OnProjHide(IHyApplication &hyApp)
{
    m_primOriginHorz.SetEnabled(false);
    m_primOriginVert.SetEnabled(false);

    QList<AtlasFrame *> frameList = static_cast<SpriteWidget *>(m_pItem->GetWidget())->GetAllDrawInsts();
    for(int i = 0; i < frameList.count(); i++)
        frameList[i]->DrawInst(this)->SetEnabled(false);
}

/*virtual*/ void SpriteDraw::OnProjUpdate(IHyApplication &hyApp)
{
    QList<AtlasFrame *> frameList = static_cast<SpriteWidget *>(m_pItem->GetWidget())->GetAllDrawInsts();
    for(int i = 0; i < frameList.count(); i++)
        frameList[i]->DrawInst(this)->SetEnabled(false);

    SpriteWidgetState *pCurSpriteState = static_cast<SpriteWidget *>(m_pItem->GetWidget())->GetCurSpriteState();
    SpriteFrame *pSpriteFrame = pCurSpriteState->GetSelectedFrame();

    if(pSpriteFrame == NULL)
        return;

    AtlasFrame *pGuiFrame = pSpriteFrame->m_pFrame;
    HyTexturedQuad2d *pDrawInst = pGuiFrame->DrawInst(this);

    pDrawInst->alpha.Set(1.0f);

    QPoint ptRenderOffset = pSpriteFrame->GetRenderOffset();
    pDrawInst->pos.X(ptRenderOffset.x());
    pDrawInst->pos.Y(ptRenderOffset.y());

    pDrawInst->SetDisplayOrder(100);

    if(pDrawInst->IsLoaded() == false)
        pDrawInst->Load();

    pDrawInst->SetEnabled(true);

    pCurSpriteState->UpdateTimeStep();
}
