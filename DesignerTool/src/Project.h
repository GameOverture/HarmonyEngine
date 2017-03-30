/**************************************************************************
 *	Project.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef PROJECT_H
#define PROJECT_H

#include "ProjectDraw.h"
#include "ProjectItem.h"
#include "AtlasModel.h"
#include "ExplorerItem.h"

#include <QQueue>
#include <QJsonObject>

// Forward declaration
class AtlasWidget;
class AudioWidgetManager;

class AtlasTreeItem;

class Project : public ExplorerItem, public IHyApplication
{
    Q_OBJECT

    ProjectDraw *                                   m_pDraw;

    AtlasModel *                                    m_pAtlasModel;
    AtlasWidget *                                   m_pAtlasWidget;

    AudioWidgetManager *                            m_pAudioMan;
    QTabBar *                                       m_pTabBar;

    ProjectItem *                                   m_pCurOpenItem;
    
    QString                                         m_sGameName;

    QString                                         m_sRelativeAssetsLocation;
    QString                                         m_sRelativeMetaDataLocation;
    QString                                         m_sRelativeSourceLocation;

    QAction                                         m_ActionSave;
    QAction                                         m_ActionSaveAll;
    QJsonObject                                     m_SaveDataObj;

    bool                                            m_bHasError;
    
public:
    Project(const QString sNewProjectFilePath);
    virtual ~Project();
    
    bool HasError() const;

    QString GetDirPath() const;
    QString GetGameName() const;

    QString GetAbsPath() const;
    QString GetAssetsAbsPath() const;
    QString GetAssetsRelPath() const;
    QString GetMetaDataAbsPath() const;
    QString GetMetaDataRelPath() const;
    QString GetSourceAbsPath() const;
    QString GetSourceRelPath() const;

    AtlasModel &GetAtlasModel();
    AtlasWidget *GetAtlasWidget();
    AudioWidgetManager *GetAudioWidget();

    QTabBar *GetTabBar();
    
    QList<QAction *> GetSaveActions();
    void SetSaveEnabled(bool bSaveEnabled, bool bSaveAllEnabled);

    void OpenItem(ProjectItem *pItem);

    // IHyApplication overrides
    virtual bool Initialize();
    virtual bool Update();
    virtual void Shutdown();

    void SetRenderSize(int iWidth, int iHeight);
    
    void OnHarmonyLoaded();

    void SaveGameData(eItemType eType, QString sPath, QJsonValue itemVal);
    void SaveGameData();
    void SaveUserData();

    QJsonObject GetSubDirObj(eItemType eType);
    
    void RefreshCurrentItemDraw();

    bool CloseAllTabs();

public Q_SLOTS:
    void OnTabBarCurrentChanged(int iIndex);

    void on_save_triggered();

    void on_saveAll_triggered();

    void OnCloseTab(int iIndex);
};

#endif // PROJECT_H
