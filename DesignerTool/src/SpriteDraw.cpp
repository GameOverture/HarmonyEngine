#include "SpriteDraw.h"
#include "SpriteWidget.h"

SpriteDraw::SpriteDraw(ProjectItem *pProjItem, IHyApplication &hyApp) : IDraw(pProjItem, hyApp),
                                                                        m_Sprite("", "+GuiPreview", this),
                                                                        m_primOriginHorz(this),
                                                                        m_primOriginVert(this)
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

	ApplyJsonData(true);

//    for(int i = 0; i < m_pProjItem->GetModel()->GetNumStates(); ++i)
//    {
//        SpriteStateData *pStateData = static_cast<SpriteStateData *>(m_pProjItem->GetModel()->GetStateData(i));
//        for(int j = 0; j < pStateData->GetFramesModel()->rowCount(); ++j)
//        {
//            AtlasFrame *pFrame = pStateData->GetFramesModel()->GetFrameAt(j)->m_pFrame;

//            HyTexturedQuad2d *pNewTexturedQuad = new HyTexturedQuad2d(pFrame->GetAtlasGrpId(), pFrame->GetTextureIndex(), this);
//            pNewTexturedQuad->SetTextureSource(pFrame->GetX(), pFrame->GetY(), pFrame->GetCrop().width(), pFrame->GetCrop().height());

//            m_TexturedQuadIdMap.insert(pFrame->GetId(), pNewTexturedQuad);
//        }
//    }
}

/*virtual*/ SpriteDraw::~SpriteDraw()
{
//    for(auto iter = m_TexturedQuadIdMap.begin(); iter != m_TexturedQuadIdMap.end(); ++iter)
//        delete iter.value();
}

void SpriteDraw::SetFrame(quint32 uiStateIndex, quint32 uiFrameIndex)
{
    m_Sprite.AnimSetState(uiStateIndex);
    m_Sprite.AnimSetFrame(uiFrameIndex);
}

/*virtual*/ void SpriteDraw::OnKeyPressEvent(QKeyEvent *pEvent) /*override*/
{
}

/*virtual*/ void SpriteDraw::OnKeyReleaseEvent(QKeyEvent *pEvent) /*override*/
{
}

/*virtual*/ void SpriteDraw::OnMousePressEvent(QMouseEvent *pEvent) /*override*/
{
}

/*virtual*/ void SpriteDraw::OnMouseWheelEvent(QWheelEvent *pEvent) /*override*/
{
    QPoint numPixels = pEvent->pixelDelta();
    QPoint numDegrees = pEvent->angleDelta() / 8;

    /*if(!numPixels.isNull())
    {
        //scrollWithPixels(numPixels);
    }
    else */if(!numDegrees.isNull())
    {
        QPoint numSteps = numDegrees / 15;
        m_pCamera->scale.TweenOffset(numSteps.y() * 0.2f, numSteps.y() * 0.2f, 0.5f, HyTween::QuadInOut);
        //scrollWithDegrees(numSteps);
    }

    pEvent->accept();
}

/*virtual*/ void SpriteDraw::OnMouseMoveEvent(QMouseEvent *pEvent) /*override*/
{
}

/*virtual*/ void SpriteDraw::OnMouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
}

/*virtual*/ void SpriteDraw::OnApplyJsonData(jsonxx::Value &valueRef, bool bReloadInAssetManager) /*override*/
{
    if(m_Sprite.AcquireData() != nullptr)
    {
        // Clear whatever anim ctrl was set since it will only set the proper attributes from GuiOverrideData, leaving stale flags behind
        for(uint32 i = 0; i < m_Sprite.AnimGetNumStates(); ++i)
        {
            m_Sprite.AnimCtrl(HYANIMCTRL_DontLoop, i);
            m_Sprite.AnimCtrl(HYANIMCTRL_DontBounce, i);
            m_Sprite.AnimCtrl(HYANIMCTRL_Play, i);
        }
    }

    m_Sprite.GuiOverrideData<HySprite2dData>(valueRef, bReloadInAssetManager);
    m_Sprite.AnimCtrl(HYANIMCTRL_Reset);
    
    SpriteWidget *pWidget = static_cast<SpriteWidget *>(m_pProjItem->GetWidget());
    m_Sprite.AnimSetPause(pWidget->IsPlayingAnim() == false);
}

/*virtual*/ void SpriteDraw::OnShow(IHyApplication &hyApp) /*override*/
{
    m_primOriginHorz.SetEnabled(true);
    m_primOriginVert.SetEnabled(true);

    m_Sprite.SetEnabled(true);
    
//    if(m_pCurFrame)
//        m_pCurFrame->SetEnabled(true);
}

/*virtual*/ void SpriteDraw::OnHide(IHyApplication &hyApp) /*override*/
{
    SetEnabled(false);
}

/*virtual*/ void SpriteDraw::OnResizeRenderer() /*override*/
{

}

/*virtual*/ void SpriteDraw::OnUpdate() /*override*/
{
    SpriteWidget *pWidget = static_cast<SpriteWidget *>(m_pProjItem->GetWidget());
    
    m_Sprite.AnimSetPause(pWidget->IsPlayingAnim() == false);

    // NOTE: Data in sprite may be invalid/null because of GUI usage
    if(m_Sprite.AcquireData() == nullptr)
        return;
    
    if(m_Sprite.AnimIsPaused())
    {
        int iStateIndex, iFrameIndex;
        pWidget->GetSpriteInfo(iStateIndex, iFrameIndex);
        
        if(iStateIndex < 0)
            iStateIndex = 0;

        if(iFrameIndex < 0)
            iFrameIndex = 0;

        m_Sprite.AnimSetState(static_cast<uint32>(iStateIndex));
        m_Sprite.AnimSetFrame(static_cast<uint32>(iFrameIndex));
    }
    else
        pWidget->SetSelectedFrame(m_Sprite.AnimGetFrame());
}
