#ifndef ITEMSPRITE_H
#define ITEMSPRITE_H

#include "Item.h"
#include "WidgetRenderer.h"

class ItemSprite : public Item
{
    friend class WidgetExplorer;
    
    HyPrimitive2d       m_primOriginHorz;
    HyPrimitive2d       m_primOriginVert;
    
    ItemSprite();
    
protected:
    
    virtual void Hide();
    virtual void Show();
    virtual void Draw(HyApp *pHyApp);
    
    virtual void Save();
};

#endif // ITEMSPRITE_H
