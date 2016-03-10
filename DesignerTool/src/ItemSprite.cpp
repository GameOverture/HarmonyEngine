/**************************************************************************
 *	ItemSprite.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "ItemSprite.h"


ItemSprite::ItemSprite(const QString sPath) : Item(ITEM_Sprite, sPath)
{
    vec2 vLinePts[2];
    
    vLinePts[0].x = -2048.0f;
    vLinePts[0].y = 0.0f;
    vLinePts[1].x = 2048.0f;
    vLinePts[1].y = 0.0f;
    m_primOriginHorz.SetAsEdgeChain(vLinePts, 2, false);
    
    vLinePts[0].x = 100.0f;
    vLinePts[0].y = -2048.0f;
    vLinePts[1].x = 100.0f;
    vLinePts[1].y = 2048.0f;
    m_primOriginVert.SetAsEdgeChain(vLinePts, 2, false);
    
    m_primOriginHorz.color.Set(1.0f, 0.0f, 0.0f, 1.0f);
    m_primOriginVert.color.Set(1.0f, 0.0f, 0.0f, 1.0f);
}

/*virtual*/ void ItemSprite::OnDraw_Open(IHyApplication &hyApp)
{
    m_primOriginHorz.Load();
    m_primOriginVert.Load();
}

/*virtual*/ void ItemSprite::OnDraw_Close(IHyApplication &hyApp)
{
}

/*virtual*/ void ItemSprite::OnDraw_Show(IHyApplication &hyApp)
{
}

/*virtual*/ void ItemSprite::OnDraw_Hide(IHyApplication &hyApp)
{
}

/*virtual*/ void ItemSprite::OnDraw_Update(IHyApplication &hyApp)
{
}

/*virtual*/ void ItemSprite::Save()
{
    
}
