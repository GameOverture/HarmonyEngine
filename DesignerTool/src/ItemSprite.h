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
#include "WidgetAtlasManager.h"

class ItemSprite : public Item
{
    friend class WidgetExplorer;
    
    WidgetAtlasManager *m_pAtlasMan;
    
    HyPrimitive2d       m_primOriginHorz;
    HyPrimitive2d       m_primOriginVert;
    
    ItemSprite(WidgetAtlasManager *pAtlasMan, const QString sPath);
    
protected:
    
    virtual void OnDraw_Open(IHyApplication &hyApp);
    virtual void OnDraw_Close(IHyApplication &hyApp);

    virtual void OnDraw_Show(IHyApplication &hyApp);
    virtual void OnDraw_Hide(IHyApplication &hyApp);
    virtual void OnDraw_Update(IHyApplication &hyApp);
    
    virtual void Save();
    
public:
    WidgetAtlasManager *GetAtlasMan();
};

#endif // ITEMSPRITE_H
