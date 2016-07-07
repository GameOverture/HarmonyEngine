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
#include <QAction>
#include <QUndoView>
#include "WidgetAtlasManager.h"

ItemSprite::ItemSprite(const QString sPath, WidgetAtlasManager &atlasManRef) : ItemWidget(ITEM_Sprite, sPath, atlasManRef)
{
    m_pWidget = new WidgetSprite(this);

    glm::vec2 vLinePts[2];
    
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
}

/*virtual*/ void ItemSprite::OnDraw_Load(IHyApplication &hyApp)
{
    m_primOriginHorz.Load();
    m_primOriginVert.Load();
}

/*virtual*/ void ItemSprite::OnDraw_Unload(IHyApplication &hyApp)
{
    m_primOriginHorz.Unload();
    m_primOriginVert.Unload();
}

/*virtual*/ void ItemSprite::OnDraw_Show(IHyApplication &hyApp)
{
    m_primOriginHorz.SetEnabled(true);
    m_primOriginVert.SetEnabled(true);
}

/*virtual*/ void ItemSprite::OnDraw_Hide(IHyApplication &hyApp)
{
    m_primOriginHorz.SetEnabled(false);
    m_primOriginVert.SetEnabled(false);
}

/*virtual*/ void ItemSprite::OnDraw_Update(IHyApplication &hyApp)
{
}

/*virtual*/ void ItemSprite::Save()
{
    
}

/*virtual*/ void ItemSprite::OnLink(HyGuiFrame *pFrame, QVariant param)
{
    static_cast<WidgetSprite *>(m_pWidget)->InsertFrame(pFrame, param);
}

/*virtual*/ void ItemSprite::OnUnlink(HyGuiFrame *pFrame)
{
    static_cast<WidgetSprite *>(m_pWidget)->RemoveFrame(pFrame);
}
