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

class CheckerGrid : public HyPrimitive2d
{
    glm::vec2		m_Resolution;
public:
    CheckerGrid();
    virtual ~CheckerGrid();

    void SetResolution(int iWidth, int iHeight);

    virtual void OnUpdateUniforms(HyShaderUniforms *pShaderUniformsRef);
    virtual void OnWriteDrawBufferData(char *&pRefDataWritePos);
};

class ItemProject : public Item, public IHyApplication
{
    Q_OBJECT

    friend class WidgetExplorer;

    QTabBar *                                       m_pTabBar;
    WidgetAtlasManager *                            m_pAtlasMan;
    
    QString                                         m_sGameName;

    QString                                         m_sRelativeAssetsLocation;
    QString                                         m_sRelativeMetaDataLocation;
    QString                                         m_sRelativeSourceLocation;

    QQueue<eProjDrawState>                          m_DrawStateQueue;
    eProjDrawState                                  m_ePrevDrawState;
    eProjDrawState                                  m_eDrawState;
    bool                                            m_bDrawStateLoaded[NUMPROJDRAWSTATE];
    HyCamera2d *                                    m_pCamera;
    CheckerGrid                                     m_CheckerGridBG;

    QAction                                         m_ActionSave;
    QAction                                         m_ActionSaveAll;

    bool                                            m_bHasError;
    
    ItemProject(const QString sNewProjectFilePath);
    
public:
    ~ItemProject();

    bool HasError() const                               { return m_bHasError; }

    QString GetDirPath() const;
    QString GetGameName() const                         { return m_sGameName; }
    
    QString GetAssetsAbsPath() const                    { return QDir::cleanPath(GetDirPath() + '/' + m_sRelativeAssetsLocation) + '/'; }
    QString GetAssetsRelPath() const                    { return QDir::cleanPath(m_sRelativeAssetsLocation) + '/'; }
    QString GetMetaDataAbsPath() const                  { return QDir::cleanPath(GetDirPath() + '/' + m_sRelativeMetaDataLocation) + '/'; }
    QString GetMetaDataRelPath() const                  { return QDir::cleanPath(m_sRelativeMetaDataLocation) + '/'; }
    QString GetSourceAbsPath() const                    { return QDir::cleanPath(GetDirPath() + '/' + m_sRelativeSourceLocation) + '/'; }
    QString GetSourceRelPath() const                    { return QDir::cleanPath(m_sRelativeSourceLocation) + '/'; }

    WidgetAtlasManager &GetAtlasManager()               { return *m_pAtlasMan; }
    QTabBar *GetTabBar()                                { return m_pTabBar; }
    
    QList<QAction *> GetSaveActions();
    void SetSaveEnabled(bool bSaveEnabled, bool bSaveAllEnabled);

    // IHyApplication overrides
    virtual bool Initialize();
    virtual bool Update();
    virtual void Shutdown();

    void SetRenderSize(int iWidth, int iHeight);
    
    void SetOverrideDrawState(eProjDrawState eDrawState);
    bool IsOverrideDraw();
    void OverrideDraw();
    void Reset();

private slots:
    void on_tabBar_currentChanged(int index);

    void on_save_triggered();

    void on_saveAll_triggered();

    void on_tabBar_closeRequested(int iIndex);
};

#endif // ITEMPROJECT_H
