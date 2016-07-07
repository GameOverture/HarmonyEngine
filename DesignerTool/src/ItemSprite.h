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

#include "ItemWidget.h"
#include "WidgetSprite.h"

class WidgetAtlasManager;

class ItemSprite : public ItemWidget
{
    Q_OBJECT

    friend class WidgetExplorer;
    
    HyPrimitive2d       m_primOriginHorz;
    HyPrimitive2d       m_primOriginVert;
    
    ItemSprite(const QString sPath, WidgetAtlasManager &atlasManRef);
    
protected:
    
    virtual void OnDraw_Load(IHyApplication &hyApp);
    virtual void OnDraw_Unload(IHyApplication &hyApp);

    virtual void OnDraw_Show(IHyApplication &hyApp);
    virtual void OnDraw_Hide(IHyApplication &hyApp);
    virtual void OnDraw_Update(IHyApplication &hyApp);
    
    virtual void Save();

    virtual void OnLink(HyGuiFrame *pFrame, QVariant param);
    virtual void OnUnlink(HyGuiFrame *pFrame);
};

#endif // ITEMSPRITE_H
