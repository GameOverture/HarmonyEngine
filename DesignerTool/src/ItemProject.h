/**************************************************************************
 *	ItemProject.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ITEMPROJECT_H
#define ITEMPROJECT_H

#include "ItemWidget.h"
#include "HyGuiRenderer.h"

#include <QQueue>

// Forward declaration
class WidgetAtlasManager;
class WidgetTabsManager;

class ItemProject : public ItemWidget
{
    friend class WidgetExplorer;
    
    QString                     m_sRelativeAssetsLocation;
    QString                     m_sRelativeMetaDataLocation;
    QString                     m_sRelativeSourceLocation;
    
    WidgetTabsManager *         m_pTabsManager;
    
    QQueue<eProjDrawState>      m_DrawStateQueue;
    eProjDrawState              m_ePrevDrawState;
    eProjDrawState              m_eDrawState;
    bool                        m_bDrawStateLoaded[NUMPROJDRAWSTATE];

    bool                        m_bHasError;
    
    ItemProject(const QString sNewProjectFilePath);
    
public:
    ~ItemProject();

    bool HasError() const                               { return m_bHasError; }

    QString GetDirPath() const;
    
    QString GetAssetsAbsPath() const                    { return QDir::cleanPath(GetDirPath() + '/' + m_sRelativeAssetsLocation) + '/'; }
    QString GetAssetsRelPath() const                    { return QDir::cleanPath(m_sRelativeAssetsLocation) + '/'; }
    QString GetMetaDataAbsPath() const                  { return QDir::cleanPath(GetDirPath() + '/' + m_sRelativeMetaDataLocation) + '/'; }
    QString GetMetaDataRelPath() const                  { return QDir::cleanPath(m_sRelativeMetaDataLocation) + '/'; }
    QString GetSourceAbsPath() const                    { return QDir::cleanPath(GetDirPath() + '/' + m_sRelativeSourceLocation) + '/'; }
    QString GetSourceRelPath() const                    { return QDir::cleanPath(m_sRelativeSourceLocation) + '/'; }

    WidgetTabsManager *GetTabsManager()                 { return m_pTabsManager; }

    virtual void OnDraw_Open(IHyApplication &hyApp);
    virtual void OnDraw_Close(IHyApplication &hyApp);
    virtual void OnDraw_Show(IHyApplication &hyApp);
    virtual void OnDraw_Hide(IHyApplication &hyApp);
    virtual void OnDraw_Update(IHyApplication &hyApp);
    
    void SetOverrideDrawState(eProjDrawState eDrawState);
    bool IsOverrideDraw();

    void Reset();
};

#endif // ITEMPROJECT_H
