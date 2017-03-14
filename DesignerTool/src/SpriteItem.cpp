/**************************************************************************
 *	ItemSprite.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "SpriteItem.h"
#include "AtlasesWidget.h"

#include <QAction>
#include <QUndoView>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

SpriteItem::SpriteItem(Project *pItemProj, const QString sPrefix, const QString sName, QJsonValue initVal) : IProjItem(pItemProj, ITEM_Sprite, sPrefix, sName, initVal)
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

/*virtual*/ SpriteItem::~SpriteItem()
{
    delete m_pWidget;
}

/*virtual*/ void SpriteItem::OnGiveMenuActions(QMenu *pMenu)
{
    static_cast<SpriteWidget *>(m_pWidget)->OnGiveMenuActions(pMenu);
}

/*virtual*/ void SpriteItem::OnLink(AtlasFrame *pFrame)
{
    SpriteWidgetState *pCurSpriteState = static_cast<SpriteWidget *>(m_pWidget)->GetCurSpriteState();
    pCurSpriteState->InsertFrame(pFrame);
}

/*virtual*/ void SpriteItem::OnReLink(AtlasFrame *pFrame)
{
    SpriteWidget *pWidgetSprite = static_cast<SpriteWidget *>(m_pWidget);
    pWidgetSprite->RefreshFrame(pFrame);
}

/*virtual*/ void SpriteItem::OnUnlink(AtlasFrame *pFrame)
{
    SpriteWidgetState *pCurSpriteState = static_cast<SpriteWidget *>(m_pWidget)->GetCurSpriteState();
    pCurSpriteState->RemoveFrame(pFrame);
}

/*virtual*/ QJsonValue SpriteItem::OnSave()
{
    QJsonArray spriteStateArray;
    static_cast<SpriteWidget *>(m_pWidget)->GetSaveInfo(spriteStateArray);

    return spriteStateArray;
}
