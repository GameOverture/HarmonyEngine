#ifndef ITEMATLAS_H
#define ITEMATLAS_H

#include "Item.h"

class ItemAtlas : public Item
{
    friend class WidgetExplorer;
    
    ItemAtlas();
    
    
protected:
    
    virtual void Hide();
    virtual void Show();
    virtual void Draw(HyApp *pHyApp);
    
    virtual void Save();
};

#endif // ITEMATLAS_H
