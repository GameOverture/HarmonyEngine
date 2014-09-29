#ifndef ITEMSPRITE_H
#define ITEMSPRITE_H

#include "Item.h"

class ItemSprite : public Item
{
    friend class WidgetExplorer;
    
    ItemSprite();
public:
    
protected:
    
    virtual void ReadJson(const QJsonObject &json);
    virtual void WriteJson(QJsonObject &json) const;
    
    virtual void Draw();
};

#endif // ITEMSPRITE_H
