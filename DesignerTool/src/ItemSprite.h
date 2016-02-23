/**************************************************************************
 *	ItemSprite.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ITEMSPRITE_H
#define ITEMSPRITE_H

#include "Item.h"
#include "WidgetRenderer.h"

class ItemSprite : public Item
{
    friend class WidgetExplorer;
    
    HyPrimitive2d       m_primOriginHorz;
    HyPrimitive2d       m_primOriginVert;
    
    ItemSprite(const QString sPath);
    
protected:
    
    virtual void Hide();
    virtual void Show();
    virtual void Draw(WidgetRenderer &renderer);
    
    virtual void Save();
};

#endif // ITEMSPRITE_H
