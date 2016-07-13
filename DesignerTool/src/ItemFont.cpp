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

ItemFont::ItemFont(const QString sPath) : Item(ITEM_Font, sPath)
{
}

/*virtual*/ ItemFont::~ItemFont()
{
}

/*virtual*/ void ItemFont::Draw(WidgetRenderer &renderer)
{
    
}

