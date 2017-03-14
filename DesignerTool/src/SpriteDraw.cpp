#include "SpriteDraw.h"

SpriteDraw::SpriteDraw()
{

}

/*virtual*/ SpriteDraw::~SpriteDraw()
{
}

///*virtual*/ void SpriteDraw::OnGuiLoad(IHyApplication &hyApp)
//{
//    m_pWidget = new WidgetSprite(this);
//    static_cast<WidgetSprite *>(m_pWidget)->Load();

//    m_primOriginHorz.Load();
//    m_primOriginVert.Load();
//}

///*virtual*/ void SpriteDraw::OnGuiUnload(IHyApplication &hyApp)
//{
//    m_primOriginHorz.Unload();
//    m_primOriginVert.Unload();

//    QList<HyGuiFrame *> frameList = static_cast<WidgetSprite *>(m_pWidget)->GetAllDrawInsts();
//    for(int i = 0; i < frameList.count(); i++)
//        frameList[i]->DrawInst(this)->Unload();

//    delete m_pWidget;
//}

///*virtual*/ void SpriteDraw::OnGuiShow(IHyApplication &hyApp)
//{
//    m_primOriginHorz.SetEnabled(true);
//    m_primOriginVert.SetEnabled(true);
//}

///*virtual*/ void SpriteDraw::OnGuiHide(IHyApplication &hyApp)
//{
//    m_primOriginHorz.SetEnabled(false);
//    m_primOriginVert.SetEnabled(false);

//    QList<HyGuiFrame *> frameList = static_cast<WidgetSprite *>(m_pWidget)->GetAllDrawInsts();
//    for(int i = 0; i < frameList.count(); i++)
//        frameList[i]->DrawInst(this)->SetEnabled(false);
//}

///*virtual*/ void SpriteDraw::OnGuiUpdate(IHyApplication &hyApp)
//{
//    QList<HyGuiFrame *> frameList = static_cast<WidgetSprite *>(m_pWidget)->GetAllDrawInsts();
//    for(int i = 0; i < frameList.count(); i++)
//        frameList[i]->DrawInst(this)->SetEnabled(false);

//    WidgetSpriteState *pCurSpriteState = static_cast<WidgetSprite *>(m_pWidget)->GetCurSpriteState();
//    SpriteFrame *pSpriteFrame = pCurSpriteState->GetSelectedFrame();

//    if(pSpriteFrame == NULL)
//        return;

//    HyGuiFrame *pGuiFrame = pSpriteFrame->m_pFrame;
//    HyTexturedQuad2d *pDrawInst = pGuiFrame->DrawInst(this);

//    pDrawInst->alpha.Set(1.0f);

//    QPoint ptRenderOffset = pSpriteFrame->GetRenderOffset();
//    pDrawInst->pos.X(ptRenderOffset.x());
//    pDrawInst->pos.Y(ptRenderOffset.y());

//    pDrawInst->SetDisplayOrder(100);

//    if(pDrawInst->IsLoaded() == false)
//        pDrawInst->Load();

//    pDrawInst->SetEnabled(true);

//    pCurSpriteState->UpdateTimeStep();
//}
