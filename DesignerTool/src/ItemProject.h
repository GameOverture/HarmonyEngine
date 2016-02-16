#ifndef ITEMPROJECT_H
#define ITEMPROJECT_H

#include "Item.h"
#include "HyGuiRenderer.h"

// Forward declaration
class WidgetAtlasManager;

class ItemProject : public Item
{
    friend class WidgetExplorer;
    
    WidgetAtlasManager *        m_pAtlasManager;

    QTabWidget *                m_pRendererTabs;
    
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

    virtual void OnDraw_Open(IHyApplication &hyApp);
    virtual void OnDraw_Close(IHyApplication &hyApp);
    virtual void OnDraw_Show(IHyApplication &hyApp);
    virtual void OnDraw_Hide(IHyApplication &hyApp);
    virtual void OnDraw_Update(IHyApplication &hyApp);
    
    void SetAtlasGroupDrawState(int iAtlasGrpId);
};

#endif // ITEMPROJECT_H
