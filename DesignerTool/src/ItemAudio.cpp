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

ItemAudio::ItemAudio(const QString sPath, WidgetAtlasManager &atlasManRef, WidgetAudioManager &audioManRef) : ItemWidget(ITEM_Audio, sPath, atlasManRef, audioManRef)
{
    m_pWidget = new WidgetAudio(this);
}

/*virtual*/ ItemAudio::~ItemAudio()
{
    delete m_pWidget;
}

/*virtual*/ QList<QAction *> ItemAudio::GetActionsForToolBar()
{
    QList<QAction *> returnList;
    
//    returnList.append(FindAction(m_pEditMenu->actions(), "Undo"));
//    returnList.append(FindAction(m_pEditMenu->actions(), "Redo"));
//    returnList.append(FindAction(m_pEditMenu->actions(), "UndoSeparator"));
    
    return returnList;
}

/*virtual*/ void ItemAudio::OnLoad(IHyApplication &hyApp)
{
}

/*virtual*/ void ItemAudio::OnUnload(IHyApplication &hyApp)
{
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

/*virtual*/ void ItemAudio::OnSave()
{
}

