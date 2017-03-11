/**************************************************************************
 *	ItemSpine.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "ItemSpine.h"

#include <QMenu>

ItemSpine::ItemSpine(const QString sPrefix, const QString sName, QJsonValue initVal, WidgetAtlasManager &atlasManRef, WidgetAudioManager &audioManRef) :   ItemWidget(ITEM_Spine, sPrefix, sName, initVal, atlasManRef, audioManRef)
{
}

/*virtual*/ ItemSpine::~ItemSpine()
{
}

/*virtual*/ void ItemSpine::OnGiveMenuActions(QMenu *pMenu)
{
}

/*virtual*/ void ItemSpine::OnLoad(IHyApplication &hyApp)
{
    m_pWidget = new WidgetSpine(this);
}

/*virtual*/ void ItemSpine::OnUnload(IHyApplication &hyApp)
{
    delete m_pWidget;
}

/*virtual*/ void ItemSpine::OnDraw_Show(IHyApplication &hyApp)
{
}

/*virtual*/ void ItemSpine::OnDraw_Hide(IHyApplication &hyApp)
{
}

/*virtual*/ void ItemSpine::OnDraw_Update(IHyApplication &hyApp)
{
}

/*virtual*/ void ItemSpine::OnLink(HyGuiFrame *pFrame)
{
}

/*virtual*/ void ItemSpine::OnReLink(HyGuiFrame *pFrame)
{
}

/*virtual*/ void ItemSpine::OnUnlink(HyGuiFrame *pFrame)
{
}

/*virtual*/ QJsonValue ItemSpine::OnSave()
{
    return QJsonObject();
}
