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
#include "WidgetAtlasManager.h"

#include <QAction>
#include <QUndoView>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

ItemSprite::ItemSprite(ItemProject *pItemProj, const QString sPrefix, const QString sName, QJsonValue initVal) : ItemWidget(pItemProj, ITEM_Sprite, sPrefix, sName, initVal)
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

/*virtual*/ ItemSprite::~ItemSprite()
{
    delete m_pWidget;
}

/*virtual*/ void ItemSprite::OnGiveMenuActions(QMenu *pMenu)
{
    static_cast<WidgetSprite *>(m_pWidget)->OnGiveMenuActions(pMenu);
}

/*virtual*/ void ItemSprite::OnGuiLoad(IHyApplication &hyApp)
{
    m_pWidget = new WidgetSprite(this);
    static_cast<WidgetSprite *>(m_pWidget)->Load();

    m_primOriginHorz.Load();
    m_primOriginVert.Load();
}

/*virtual*/ void ItemSprite::OnGuiUnload(IHyApplication &hyApp)
{
    m_primOriginHorz.Unload();
    m_primOriginVert.Unload();

    QList<HyGuiFrame *> frameList = static_cast<WidgetSprite *>(m_pWidget)->GetAllDrawInsts();
    for(int i = 0; i < frameList.count(); i++)
        frameList[i]->DrawInst(this)->Unload();

    delete m_pWidget;
}

/*virtual*/ void ItemSprite::OnGuiShow(IHyApplication &hyApp)
{
    m_primOriginHorz.SetEnabled(true);
    m_primOriginVert.SetEnabled(true);
}

/*virtual*/ void ItemSprite::OnGuiHide(IHyApplication &hyApp)
{
    m_primOriginHorz.SetEnabled(false);
    m_primOriginVert.SetEnabled(false);

    QList<HyGuiFrame *> frameList = static_cast<WidgetSprite *>(m_pWidget)->GetAllDrawInsts();
    for(int i = 0; i < frameList.count(); i++)
        frameList[i]->DrawInst(this)->SetEnabled(false);
}

/*virtual*/ void ItemSprite::OnGuiUpdate(IHyApplication &hyApp)
{
    QList<HyGuiFrame *> frameList = static_cast<WidgetSprite *>(m_pWidget)->GetAllDrawInsts();
    for(int i = 0; i < frameList.count(); i++)
        frameList[i]->DrawInst(this)->SetEnabled(false);

    WidgetSpriteState *pCurSpriteState = static_cast<WidgetSprite *>(m_pWidget)->GetCurSpriteState();
    SpriteFrame *pSpriteFrame = pCurSpriteState->GetSelectedFrame();

    if(pSpriteFrame == NULL)
        return;

    HyGuiFrame *pGuiFrame = pSpriteFrame->m_pFrame;
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

/*virtual*/ void ItemSprite::OnLink(HyGuiFrame *pFrame)
{
    WidgetSpriteState *pCurSpriteState = static_cast<WidgetSprite *>(m_pWidget)->GetCurSpriteState();
    pCurSpriteState->InsertFrame(pFrame);
}

/*virtual*/ void ItemSprite::OnReLink(HyGuiFrame *pFrame)
{
    WidgetSprite *pWidgetSprite = static_cast<WidgetSprite *>(m_pWidget);
    pWidgetSprite->RefreshFrame(pFrame);
}

/*virtual*/ void ItemSprite::OnUnlink(HyGuiFrame *pFrame)
{
    WidgetSpriteState *pCurSpriteState = static_cast<WidgetSprite *>(m_pWidget)->GetCurSpriteState();
    pCurSpriteState->RemoveFrame(pFrame);
}

/*virtual*/ QJsonValue ItemSprite::OnSave()
{
    QJsonArray spriteStateArray;
    static_cast<WidgetSprite *>(m_pWidget)->GetSaveInfo(spriteStateArray);

    return spriteStateArray;
}
