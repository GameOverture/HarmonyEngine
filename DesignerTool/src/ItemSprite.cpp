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

ItemSprite::ItemSprite(const QString sPath, WidgetAtlasManager &atlasManRef) : ItemWidget(ITEM_Sprite, sPath, atlasManRef)
{
    m_pWidget = new WidgetSprite(this);
    static_cast<WidgetSprite *>(m_pWidget)->LoadAndInit();

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

/*virtual*/ ItemSprite::~ItemSprite()
{
}

/*virtual*/ QList<QAction *> ItemSprite::GetActionsForToolBar()
{
    QList<QAction *> returnList;
    
    returnList.append(FindAction(m_pEditMenu->actions(), "Undo"));
    returnList.append(FindAction(m_pEditMenu->actions(), "Redo"));
    returnList.append(FindAction(m_pEditMenu->actions(), "UndoSeparator"));
    
    static_cast<WidgetSprite *>(m_pWidget)->AppendActionsForToolBar(returnList);
    
    return returnList;
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
    
    QList<HyGuiFrame *> frameList = static_cast<WidgetSprite *>(m_pWidget)->GetAllDrawInsts();
    for(int i = 0; i < frameList.count(); i++)
        frameList[i]->DrawInst(this)->Unload();
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
    
    QList<HyGuiFrame *> frameList = static_cast<WidgetSprite *>(m_pWidget)->GetAllDrawInsts();
    for(int i = 0; i < frameList.count(); i++)
        frameList[i]->DrawInst(this)->SetEnabled(false);
}

/*virtual*/ void ItemSprite::OnDraw_Update(IHyApplication &hyApp)
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
    
    pDrawInst->pos.X(pGuiFrame->GetCrop().x() + pSpriteFrame->m_ptOffset.x());
    pDrawInst->pos.Y(pGuiFrame->GetSize().height() - pGuiFrame->GetCrop().bottom() + pSpriteFrame->m_ptOffset.y());
    
    if(pDrawInst->IsLoaded() == false)
        pDrawInst->Load();
    
    pDrawInst->SetEnabled(true);
    
    
}

/*virtual*/ void ItemSprite::OnLink(HyGuiFrame *pFrame)
{
    WidgetSpriteState *pCurSpriteState = static_cast<WidgetSprite *>(m_pWidget)->GetCurSpriteState();
    pCurSpriteState->InsertFrame(pFrame);
}

/*virtual*/ void ItemSprite::OnUnlink(HyGuiFrame *pFrame)
{
    WidgetSpriteState *pCurSpriteState = static_cast<WidgetSprite *>(m_pWidget)->GetCurSpriteState();
    pCurSpriteState->RemoveFrame(pFrame);
}

/*virtual*/ void ItemSprite::OnSave()
{
    QJsonArray spriteStateArray;
    static_cast<WidgetSprite *>(m_pWidget)->GetSpriteStateInfo(spriteStateArray);

    QJsonDocument settingsDoc(spriteStateArray);

    QFile spriteFile(GetAbsPath());
    if(spriteFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qint64 iBytesWritten = spriteFile.write(settingsDoc.toJson());
        if(0 == iBytesWritten || -1 == iBytesWritten)
            HyGuiLog("Could not write to atlas settings file: " % spriteFile.errorString(), LOGTYPE_Error);
    }
    else
        HyGuiLog("Couldn't open item file " % GetAbsPath() % ": " % spriteFile.errorString(), LOGTYPE_Error);

    spriteFile.close();
}
