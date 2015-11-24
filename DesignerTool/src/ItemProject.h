#ifndef ITEMPROJECT_H
#define ITEMPROJECT_H

#include "Item.h"
#include "HyGuiRenderer.h"

// Forward declaration
class WidgetAtlasManager;

class ItemProject : public Item, public IHyApplication
{
    friend class WidgetExplorer;
    
    HyGuiRenderer *         m_pRenderer;
    
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
    
    HyGuiRenderer *GetRenderer()                        { return m_pRenderer; }
    
    WidgetAtlasManager *GetAtlasManager()               { return m_pAtlasManager; }
    QString GetPath(QString sAppendRelativePath) const  { return m_sPath % sAppendRelativePath; }
    QString GetPath() const                             { return m_sPath; }
    
    // IHyApplication overrides
    virtual bool Initialize();
    virtual bool Update();
    virtual bool Shutdown();
    
    // Item overrides
    virtual void Show();
    virtual void Hide();
    virtual void Draw(WidgetRenderer &renderer);
    
    void SetAtlasGroupDrawState(int iAtlasGrpId, bool bForceLoad);
};

#endif // ITEMPROJECT_H
