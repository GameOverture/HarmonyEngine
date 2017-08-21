#include "SpriteDraw.h"
#include "SpriteWidget.h"

SpriteDraw::SpriteDraw(ProjectItem *pProjItem, IHyApplication &hyApp) : IDraw(pProjItem, hyApp),
                                                                        m_Sprite("", "+GuiPreview", this)
{
}

/*virtual*/ SpriteDraw::~SpriteDraw()
{
}

void SpriteDraw::SetFrame(quint32 uiStateIndex, quint32 uiFrameIndex)
{
    m_Sprite.AnimSetState(uiStateIndex);
    m_Sprite.AnimSetFrame(uiFrameIndex);
}

/*virtual*/ void SpriteDraw::OnKeyPressEvent(QKeyEvent *pEvent) /*override*/
{
    IDraw::OnKeyPressEvent(pEvent);
}

/*virtual*/ void SpriteDraw::OnKeyReleaseEvent(QKeyEvent *pEvent) /*override*/
{
    IDraw::OnKeyReleaseEvent(pEvent);
}

/*virtual*/ void SpriteDraw::OnMousePressEvent(QMouseEvent *pEvent) /*override*/
{
    IDraw::OnMousePressEvent(pEvent);
}

/*virtual*/ void SpriteDraw::OnMouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
    IDraw::OnMouseReleaseEvent(pEvent);
}

/*virtual*/ void SpriteDraw::OnMouseWheelEvent(QWheelEvent *pEvent) /*override*/
{
    IDraw::OnMouseWheelEvent(pEvent);
}

/*virtual*/ void SpriteDraw::OnMouseMoveEvent(QMouseEvent *pEvent) /*override*/
{
    IDraw::OnMouseMoveEvent(pEvent);
}

/*virtual*/ void SpriteDraw::OnApplyJsonData(jsonxx::Value &valueRef) /*override*/
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

    m_Sprite.GuiOverrideData<HySprite2dData>(valueRef);
    m_Sprite.AnimCtrl(HYANIMCTRL_Reset);
    
    SpriteWidget *pWidget = static_cast<SpriteWidget *>(m_pProjItem->GetWidget());
    m_Sprite.AnimSetPause(pWidget->IsPlayingAnim() == false);
}

/*virtual*/ void SpriteDraw::OnShow(IHyApplication &hyApp) /*override*/
{
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