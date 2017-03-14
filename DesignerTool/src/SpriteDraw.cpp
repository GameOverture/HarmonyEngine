#include "SpriteDraw.h"

SpriteDraw::SpriteDraw(SpriteItem *pItem) : m_pItem(pItem)
{

}

/*virtual*/ SpriteDraw::~SpriteDraw()
{
}

/*virtual*/ void SpriteDraw::OnGuiLoad(IHyApplication &hyApp)
{
    m_primOriginHorz.Load();
    m_primOriginVert.Load();
}

/*virtual*/ void SpriteDraw::OnGuiUnload(IHyApplication &hyApp)
{
    m_primOriginHorz.Unload();
    m_primOriginVert.Unload();

    QList<AtlasFrame *> frameList = static_cast<SpriteWidget *>(m_pItem->GetWidget())->GetAllDrawInsts();
    for(int i = 0; i < frameList.count(); i++)
        frameList[i]->DrawInst(this)->Unload();
}

/*virtual*/ void SpriteDraw::OnGuiShow(IHyApplication &hyApp)
{
    m_primOriginHorz.SetEnabled(true);
    m_primOriginVert.SetEnabled(true);
}

/*virtual*/ void SpriteDraw::OnGuiHide(IHyApplication &hyApp)
{
    m_primOriginHorz.SetEnabled(false);
    m_primOriginVert.SetEnabled(false);

    QList<AtlasFrame *> frameList = static_cast<SpriteWidget *>(m_pItem->GetWidget())->GetAllDrawInsts();
    for(int i = 0; i < frameList.count(); i++)
        frameList[i]->DrawInst(this)->SetEnabled(false);
}

/*virtual*/ void SpriteDraw::OnGuiUpdate(IHyApplication &hyApp)
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
