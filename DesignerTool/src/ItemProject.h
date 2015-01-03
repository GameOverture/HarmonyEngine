#ifndef ITEMPROJECT_H
#define ITEMPROJECT_H

#include "Item.h"

// Forward declaration
class WidgetAtlas;

class ItemProject : public Item
{
    WidgetAtlas *       m_pAtlases;
    HyPrimitive2d       m_CurAtlas;
    
    enum eDrawState
    {
        DRAWSTATE_AtlasManager,
    };
    eDrawState          m_eState;
    
public:
    ItemProject();
    ~ItemProject();
    
    QString GetDataPath() const         { return GetPath() % HYGUIPATH_RelDataDir; }
    QString GetMetaDataPath() const     { return GetPath() % HYGUIPATH_RelMetaDataDir; }
    
    WidgetAtlas *GetAtlasWidget()       { return m_pAtlases; }
    
    virtual void Hide();
    virtual void Show();
    virtual void Draw(HyApp *pHyApp);
};

#endif // ITEMPROJECT_H
