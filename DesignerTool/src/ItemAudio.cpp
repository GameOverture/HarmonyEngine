/**************************************************************************
 *	ItemAudio.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "ItemAudio.h"
#include "WidgetAudio.h"


ItemAudio::ItemAudio(ItemProject *pItemProj, const QString sPrefix, const QString sName, QJsonValue initVal) : ItemWidget(pItemProj, ITEM_Audio, sPrefix, sName, initVal)
{
}

/*virtual*/ ItemAudio::~ItemAudio()
{
}

/*virtual*/ void ItemAudio::OnGiveMenuActions(QMenu *pMenu)
{
}

/*virtual*/ void ItemAudio::OnLoad(IHyApplication &hyApp)
{
    m_pWidget = new WidgetAudio(this);
}

/*virtual*/ void ItemAudio::OnUnload(IHyApplication &hyApp)
{
    delete m_pWidget;
}

/*virtual*/ void ItemAudio::OnDraw_Show(IHyApplication &hyApp)
{
}

/*virtual*/ void ItemAudio::OnDraw_Hide(IHyApplication &hyApp)
{
}

/*virtual*/ void ItemAudio::OnDraw_Update(IHyApplication &hyApp)
{
}

/*virtual*/ void ItemAudio::OnLink(HyGuiFrame *pFrame)
{
}

/*virtual*/ void ItemAudio::OnReLink(HyGuiFrame *pFrame)
{
}

/*virtual*/ void ItemAudio::OnUnlink(HyGuiFrame *pFrame)
{
}

/*virtual*/ QJsonValue ItemAudio::OnSave()
{
    return QJsonObject();
}

