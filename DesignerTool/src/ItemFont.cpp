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

ItemFont::ItemFont(const QString sPath, HyGuiDependencies *pDependencies) : Item(ITEM_Font, sPath, pDependencies)
{
}

/*virtual*/ void ItemFont::Draw(WidgetRenderer &renderer)
{
    
}

