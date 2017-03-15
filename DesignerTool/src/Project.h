/**************************************************************************
 *	ItemProject.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef PROJECT_H
#define PROJECT_H

#include "IProjItem.h"
#include "AtlasesData.h"
#include "ExplorerItem.h"

#include <QQueue>
#include <QJsonObject>

// Forward declaration
class AtlasesWidget;
class AudioWidgetManager;

class AtlasTreeItem;

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

class Project : public ExplorerItem, public IHyApplication
{
    Q_OBJECT

    AtlasesData *                                   m_pAtlasesData;
    AtlasesWidget *                                 m_pAtlasMan;

    AudioWidgetManager *                            m_pAudioMan;
    QTabBar *                                       m_pTabBar;

    IProjItem *                                     m_pCurOpenItem;
    
    QString                                         m_sGameName;

    QString                                         m_sRelativeAssetsLocation;
    QString                                         m_sRelativeMetaDataLocation;
    QString                                         m_sRelativeSourceLocation;

    HyCamera2d *                                    m_pCamera;
    CheckerGrid                                     m_CheckerGridBG;

    QAction                                         m_ActionSave;
    QAction                                         m_ActionSaveAll;
    QJsonObject                                     m_SaveDataObj;

    bool                                            m_bHasError;
    
public:
    Project(const QString sNewProjectFilePath);
    virtual ~Project();

    void LoadWidgets();
    bool HasError() const;

    QList<AtlasTreeItem *> GetAtlasTreeItemList();

    QString GetDirPath() const;
    QString GetGameName() const                         { return m_sGameName; }
    
    QString GetAbsPath() const                          { return m_sPATH; }
    QString GetAssetsAbsPath() const                    { return QDir::cleanPath(GetDirPath() + '/' + m_sRelativeAssetsLocation) + '/'; }
    QString GetAssetsRelPath() const                    { return QDir::cleanPath(m_sRelativeAssetsLocation) + '/'; }
    QString GetMetaDataAbsPath() const                  { return QDir::cleanPath(GetDirPath() + '/' + m_sRelativeMetaDataLocation) + '/'; }
    QString GetMetaDataRelPath() const                  { return QDir::cleanPath(m_sRelativeMetaDataLocation) + '/'; }
    QString GetSourceAbsPath() const                    { return QDir::cleanPath(GetDirPath() + '/' + m_sRelativeSourceLocation) + '/'; }
    QString GetSourceRelPath() const                    { return QDir::cleanPath(m_sRelativeSourceLocation) + '/'; }

    AtlasesData &GetAtlasesData()                       { return *m_pAtlasesData; }
    AtlasesWidget &GetAtlasManager()                    { return *m_pAtlasMan; }
    AudioWidgetManager &GetAudioManager()               { return *m_pAudioMan; }
    QTabBar *GetTabBar()                                { return m_pTabBar; }
    
    QList<QAction *> GetSaveActions();
    void SetSaveEnabled(bool bSaveEnabled, bool bSaveAllEnabled);

    void OpenItem(IProjItem *pItem);

    // IHyApplication overrides
    virtual bool Initialize();
    virtual bool Update();
    virtual void Shutdown();

    void SetRenderSize(int iWidth, int iHeight);
    
    void Reset();

    void SaveGameData(eItemType eType, QString sPath, QJsonValue itemVal);
    void SaveGameData();
    void SaveUserData();

    QJsonObject GetSubDirObj(eItemType eType);

public Q_SLOTS:
    void OnTabBarCurrentChanged(int iIndex);

    void on_save_triggered();

    void on_saveAll_triggered();

    void on_tabBar_closeRequested(int iIndex);
};

#endif // PROJECT_H
