#ifndef ITEMPROJECT_H
#define ITEMPROJECT_H

#include "Item.h"

// Forward declaration
class WidgetAtlas;

class ItemProject : public Item
{
    friend class WidgetExplorer;
    
    WidgetAtlas *       m_pAtlases;
    HyPrimitive2d       m_CurAtlas;
    
    enum eDrawState
    {
        DRAWSTATE_AtlasManager,
    };
    eDrawState          m_eState;
    
    ItemProject(const QString sPath);
    
public:
    ~ItemProject();
    
    WidgetAtlas *GetAtlasWidget()       { return m_pAtlases; }
    
    virtual void Hide();
    virtual void Show();
    virtual void Draw(HyApp *pHyApp);
};

#endif // ITEMPROJECT_H
