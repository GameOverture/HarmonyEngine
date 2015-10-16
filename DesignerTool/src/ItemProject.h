#ifndef ITEMPROJECT_H
#define ITEMPROJECT_H

#include "Item.h"

// Forward declaration
class WidgetAtlasManager;

class ItemProject : public Item
{
    friend class WidgetExplorer;
    
    WidgetAtlasManager *    m_pAtlases;
    HyPrimitive2d           m_CurAtlas;
    
public:
    enum eDrawState
    {
        DRAWSTATE_AtlasManager,
    };
    
private:
    eDrawState          m_eState;
    int                 m_iDrawStateIndex;
    
    ItemProject(const QString sPath);
    
public:
    ~ItemProject();
    
    WidgetAtlasManager *GetAtlasWidget()       { return m_pAtlases; }
    
    virtual void Hide();
    virtual void Show();
    virtual void Draw(WidgetRenderer &renderer);
    
    void SetDrawState(eDrawState eState, int iDrawStateIndex);
};

#endif // ITEMPROJECT_H
