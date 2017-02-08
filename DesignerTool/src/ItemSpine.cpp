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

ItemSpine::ItemSpine(const QString sPath, WidgetAtlasManager &atlasManRef, WidgetAudioManager &audioManRef) :   ItemWidget(ITEM_Spine, sPath, atlasManRef, audioManRef)
{
    m_pWidget = new WidgetSpine(this);
}

/*virtual*/ ItemSpine::~ItemSpine()
{
}

/*virtual*/ QList<QAction *> ItemSpine::GetActionsForToolBar()
{
    QList<QAction *> returnList;
    
    returnList.append(FindAction(m_pEditMenu->actions(), "Undo"));
    returnList.append(FindAction(m_pEditMenu->actions(), "Redo"));
    returnList.append(FindAction(m_pEditMenu->actions(), "UndoSeparator"));
    
    //static_cast<WidgetSprite *>(m_pWidget)->AppendActionsForToolBar(returnList);
    
    return returnList;
}

/*virtual*/ void ItemSpine::OnLoad(IHyApplication &hyApp)
{
}

/*virtual*/ void ItemSpine::OnUnload(IHyApplication &hyApp)
{
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

/*virtual*/ QJsonObject ItemSpine::OnSave()
{
    return QJsonObject();
}
