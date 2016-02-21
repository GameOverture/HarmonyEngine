#ifndef ITEMPROJECT_H
#define ITEMPROJECT_H

#include "Item.h"
#include "HyGuiRenderer.h"

// Forward declaration
class WidgetAtlasManager;

class ItemProject : public Item
{
    friend class WidgetExplorer;
    
    QString                     m_sRelativeAssetsLocation;
    QString                     m_sRelativeMetaDataLocation;
    QString                     m_sRelativeSourceLocation;
    
    WidgetAtlasManager *        m_pAtlasManager;
    
    enum eDrawState
    {
        DRAWSTATE_Nothing,
        DRAWSTATE_AtlasManager,
    };
    eDrawState          m_eState;
    
    ItemProject(const QString sNewProjectFilePath);
    
public:
    ~ItemProject();

    QString GetDirPath() const;
    
    QString GetAssetsAbsPath() const                    { return QDir::cleanPath(GetDirPath() + '/' + m_sRelativeAssetsLocation) + '/'; }
    QString GetAssetsRelPath() const                    { return m_sRelativeAssetsLocation; }
    QString GetMetaDataAbsPath() const                  { return QDir::cleanPath(GetDirPath() + '/' + m_sRelativeMetaDataLocation) + '/'; }
    QString GetMetaDataRelPath() const                  { return m_sRelativeMetaDataLocation; }
    QString GetSourceAbsPath() const                    { return QDir::cleanPath(GetDirPath() + '/' + m_sRelativeSourceLocation) + '/'; }
    QString GetSourceRelPath() const                    { return m_sRelativeSourceLocation; }

    WidgetAtlasManager *GetAtlasManager()               { return m_pAtlasManager; }

    virtual void OnDraw_Open(IHyApplication &hyApp);
    virtual void OnDraw_Close(IHyApplication &hyApp);
    virtual void OnDraw_Show(IHyApplication &hyApp);
    virtual void OnDraw_Hide(IHyApplication &hyApp);
    virtual void OnDraw_Update(IHyApplication &hyApp);
    
    void SetAtlasGroupDrawState(int iAtlasGrpId);
};

#endif // ITEMPROJECT_H
