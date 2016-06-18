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
#include "WidgetSprite.h"

class ItemSprite : public Item
{
    friend class WidgetExplorer;
    
    HyPrimitive2d       m_primOriginHorz;
    HyPrimitive2d       m_primOriginVert;
    
    ItemSprite(const QString sPath, HyGuiDependencies *pDependencies);
    
protected:
    
    virtual void OnDraw_Open(IHyApplication &hyApp);
    virtual void OnDraw_Close(IHyApplication &hyApp);

    virtual void OnDraw_Show(IHyApplication &hyApp);
    virtual void OnDraw_Hide(IHyApplication &hyApp);
    virtual void OnDraw_Update(IHyApplication &hyApp);
    
    virtual void Save();
};

#endif // ITEMSPRITE_H
