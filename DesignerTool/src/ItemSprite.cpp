#include "ItemSprite.h"


ItemSprite::ItemSprite(const QString sPath) : Item(ITEM_Sprite, sPath)
{
    vec2 vLinePts[2];
    
    vLinePts[0].x = -2048.0f;
    vLinePts[0].y = 0.0f;
    vLinePts[1].x = 2048.0f;
    vLinePts[1].y = 0.0f;
    m_primOriginHorz.SetAsEdgeChain(vLinePts, 2, false);
    
    vLinePts[0].x = 0.0f;
    vLinePts[0].y = -2048.0f;
    vLinePts[1].x = 0.0f;
    vLinePts[1].y = 2048.0f;
    m_primOriginVert.SetAsEdgeChain(vLinePts, 2, false);
    
    m_primOriginHorz.color.Set(1.0f, 0.0f, 0.0f, 1.0f);
    m_primOriginVert.color.Set(1.0f, 0.0f, 0.0f, 1.0f);
    
    m_primOriginHorz.Load();
    m_primOriginVert.Load();
}

/*virtual*/ void ItemSprite::Hide()
{
    m_primOriginHorz.SetEnabled(false);
    m_primOriginVert.SetEnabled(false);
}

/*virtual*/ void ItemSprite::Show()
{
    m_primOriginHorz.SetEnabled(true);
    m_primOriginVert.SetEnabled(true);
}

/*virtual*/ void ItemSprite::Draw(WidgetRenderer &renderer)
{
    
    //pHyApp->GetViewport().GetResolution().iWidth
    //pHyApp->GetViewport().GetResolution().iHeight
}

/*virtual*/ void ItemSprite::Save()
{
    
}
