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

SpriteItem::SpriteItem(Project *pItemProj, const QString sPrefix, const QString sName, QJsonArray stateArray) : IProjItem(pItemProj, ITEM_Sprite, sPrefix, sName)
{
    m_pStatesModel = new SpriteStatesModel(this);

    // If item's init value is defined, parse and initalize with it, otherwise make default empty sprite
    if(stateArray.empty() == false)
    {
        for(int i = 0; i < stateArray.size(); ++i)
            m_pStatesModel->AppendState(stateArray[i].toObject());
    }
    else
        m_pStatesModel->AppendState(QJsonObject());
}

/*virtual*/ SpriteItem::~SpriteItem()
{
    delete m_pWidget;
}

SpriteStatesModel *SpriteItem::GetSpritesModel()
{
    return m_pStatesModel;
}

/*virtual*/ void SpriteItem::OnGiveMenuActions(QMenu *pMenu)
{
    static_cast<SpriteWidget *>(m_pWidget)->OnGiveMenuActions(pMenu);
}

/*virtual*/ void SpriteItem::OnLink(AtlasFrame *pFrame)
{
    // TODO: Don't access Widget, store in 'this' data
    SpriteWidgetState *pCurSpriteState = static_cast<SpriteWidget *>(m_pWidget)->GetCurSpriteState();
    pCurSpriteState->InsertFrame(pFrame);
}

/*virtual*/ void SpriteItem::OnUnlink(AtlasFrame *pFrame)
{
    // TODO: Don't access Widget, store in 'this' data
    SpriteWidgetState *pCurSpriteState = static_cast<SpriteWidget *>(m_pWidget)->GetCurSpriteState();
    pCurSpriteState->RemoveFrame(pFrame);
}

/*virtual*/ QJsonValue SpriteItem::OnSave()
{
    QJsonArray spriteStateArray;
    static_cast<SpriteWidget *>(m_pWidget)->GetSaveInfo(spriteStateArray);

    return spriteStateArray;
}
