/**************************************************************************
 *	ItemFont.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "ItemFont.h"

#include <QMenu>
#include <QAction>
#include <QUndoView>

#include "WidgetFont.h"

ItemFont::ItemFont(const QString sPath, WidgetAtlasManager &atlasManRef) : ItemWidget(ITEM_Font, sPath, atlasManRef)
{
}

/*virtual*/ ItemFont::~ItemFont()
{
}

/*virtual*/ QList<QAction *> ItemFont::GetActionsForToolBar()
{
    QList<QAction *> returnList;
    
    returnList.append(FindAction(m_pEditMenu->actions(), "Undo"));
    returnList.append(FindAction(m_pEditMenu->actions(), "Redo"));
    returnList.append(FindAction(m_pEditMenu->actions(), "UndoSeparator"));
    
    return returnList;
}

/*virtual*/ void ItemFont::OnLoad(IHyApplication &hyApp)
{
    m_pWidget = new WidgetFont(this);
}

/*virtual*/ void ItemFont::OnUnload(IHyApplication &hyApp)
{
}

/*virtual*/ void ItemFont::OnDraw_Show(IHyApplication &hyApp)
{
}

/*virtual*/ void ItemFont::OnDraw_Hide(IHyApplication &hyApp)
{
}

/*virtual*/ void ItemFont::OnDraw_Update(IHyApplication &hyApp)
{
}

/*virtual*/ void ItemFont::OnLink(HyGuiFrame *pFrame)
{
}

/*virtual*/ void ItemFont::OnReLink(HyGuiFrame *pFrame)
{
}

/*virtual*/ void ItemFont::OnUnlink(HyGuiFrame *pFrame)
{
}

/*virtual*/ void ItemFont::OnSave()
{
}
