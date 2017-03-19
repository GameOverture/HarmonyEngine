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

#include "ProjectItem.h"
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

    ProjectItem *                                     m_pCurOpenItem;
    
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
    
    //void InitAtlasDependencies(QTreeWidgetItem *pTreeItem);

    bool HasError() const;

    QList<AtlasTreeItem *> GetAtlasTreeItemList();

    QString GetDirPath() const;
    QString GetGameName() const;

    QString GetAbsPath() const;
    QString GetAssetsAbsPath() const;
    QString GetAssetsRelPath() const;
    QString GetMetaDataAbsPath() const;
    QString GetMetaDataRelPath() const;
    QString GetSourceAbsPath() const;
    QString GetSourceRelPath() const;

    AtlasesData &GetAtlasesData();
    AtlasesWidget &GetAtlasManager();
    AudioWidgetManager &GetAudioManager();
    QTabBar *GetTabBar();
    
    QList<QAction *> GetSaveActions();
    void SetSaveEnabled(bool bSaveEnabled, bool bSaveAllEnabled);

    void OpenItem(ProjectItem *pItem);

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
    
    bool CloseAllTabs();

public Q_SLOTS:
    void OnTabBarCurrentChanged(int iIndex);

    void on_save_triggered();

    void on_saveAll_triggered();

    void on_tabBar_closeRequested(int iIndex);
};

#endif // PROJECT_H
