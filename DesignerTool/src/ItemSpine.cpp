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

ItemSpine::ItemSpine(const QString sPath, WidgetAtlasManager &atlasManRef, WidgetAudioManager &audioManRef)
{
    m_pWidget = new WidgetSpine(this);
}
