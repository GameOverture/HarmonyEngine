#ifndef ITEMPROJECT_H
#define ITEMPROJECT_H

#include "Item.h"

// Forward declaration
class WidgetAtlasManager;

class ItemProject : public Item
{
    friend class WidgetExplorer;
    
    WidgetAtlasManager *    m_pAtlasManager;
    HyTexturedQuad2d *      m_pCurAtlas;
    HyCamera2d *            m_pCam;
    
    enum eDrawState
    {
        DRAWSTATE_Nothing,
        DRAWSTATE_AtlasManager,
    };
    
    eDrawState          m_eState;
    
    ItemProject(const QString sPath);
    
public:
    ~ItemProject();
    
    WidgetAtlasManager *GetAtlasManager()               { return m_pAtlasManager; }
    QString GetPath(QString sAppendRelativePath) const  { return m_sPath % sAppendRelativePath; }
    QString GetPath() const                             { return m_sPath; }
    
    virtual void Show();
    virtual void Hide();
    virtual void Draw(WidgetRenderer &renderer);
    
    void SetAtlasGroupDrawState(int iAtlasGrpId, bool bForceLoad);
};

#endif // ITEMPROJECT_H
