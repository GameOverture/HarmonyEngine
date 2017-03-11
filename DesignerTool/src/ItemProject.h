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
#include <QJsonObject>

// Forward declaration
class WidgetAtlasManager;
class WidgetAudioManager;

class CheckerGrid : public HyPrimitive2d
{
    glm::vec2		m_Resolution;
public:
    CheckerGrid();
    virtual ~CheckerGrid();

    void SetSurfaceSize(int iWidth, int iHeight);

    virtual void OnUpdateUniforms();
    virtual void OnWriteDrawBufferData(char *&pRefDataWritePos);
};

class ItemProject : public Item, public IHyApplication
{
    Q_OBJECT

    WidgetAtlasManager *                            m_pAtlasMan;
    WidgetAudioManager *                            m_pAudioMan;
    QTabBar *                                       m_pTabBar;
    
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
    QJsonObject                                     m_SaveDataObj;

    bool                                            m_bHasError;
    
public:
    ItemProject(const QString sNewProjectFilePath);
    ~ItemProject();

    void WidgetLoad();

    bool HasError() const                               { return m_bHasError; }

    QString GetDirPath() const;
    QString GetGameName() const                         { return m_sGameName; }
    
    QString GetAbsPath() const                          { return m_sPATH; }
    QString GetAssetsAbsPath() const                    { return QDir::cleanPath(GetDirPath() + '/' + m_sRelativeAssetsLocation) + '/'; }
    QString GetAssetsRelPath() const                    { return QDir::cleanPath(m_sRelativeAssetsLocation) + '/'; }
    QString GetMetaDataAbsPath() const                  { return QDir::cleanPath(GetDirPath() + '/' + m_sRelativeMetaDataLocation) + '/'; }
    QString GetMetaDataRelPath() const                  { return QDir::cleanPath(m_sRelativeMetaDataLocation) + '/'; }
    QString GetSourceAbsPath() const                    { return QDir::cleanPath(GetDirPath() + '/' + m_sRelativeSourceLocation) + '/'; }
    QString GetSourceRelPath() const                    { return QDir::cleanPath(m_sRelativeSourceLocation) + '/'; }

    WidgetAtlasManager &GetAtlasManager()               { return *m_pAtlasMan; }
    WidgetAudioManager &GetAudioManager()               { return *m_pAudioMan; }
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

    void SaveGameData(eItemType eType, QString sPath, QJsonValue itemVal);
    void SaveGameData();
    void SaveUserData();

    QJsonObject GetSubDirObj(eItemType eType);

private Q_SLOTS:
    void on_tabBar_currentChanged(int index);

    void on_save_triggered();

    void on_saveAll_triggered();

    void on_tabBar_closeRequested(int iIndex);

Q_SIGNALS:
    void LoadFinished(ItemProject *pSelf);
};

#endif // ITEMPROJECT_H
