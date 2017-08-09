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
#include "DataExplorerItem.h"
#include "DlgProjectSettings.h"

#include <QQueue>
#include <QJsonObject>

// Forward declaration
class AtlasWidget;
class AudioWidgetManager;

class AtlasTreeItem;

class Project : public DataExplorerItem, public IHyApplication
{
    Q_OBJECT

    ProjectDraw *                                   m_pDraw;
    DlgProjectSettings                              m_DlgProjectSettings;   // Stores the actual settings in a QJsonObject within;

    AtlasModel *                                    m_pAtlasModel;
    AtlasWidget *                                   m_pAtlasWidget;

    AudioWidgetManager *                            m_pAudioMan;
    QTabBar *                                       m_pTabBar;

    ProjectItem *                                   m_pCurOpenItem;

    QJsonObject                                     m_SaveDataObj;

    bool                                            m_bHasError;
    
public:
    Project(const QString sProjectFilePath);
    virtual ~Project();
    
    void ExecProjSettingsDlg();

    QJsonObject GetSettingsObj() const;

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

    ProjectItem *GetCurrentOpenItem();
    void OpenItem(ProjectItem *pItem);

    // IHyApplication overrides
    virtual bool Initialize();
    virtual bool Update();
    virtual void Shutdown();

    void SetRenderSize(int iWidth, int iHeight);
    
    void OnHarmonyLoaded();

    void ApplySaveEnables();

    void SaveGameData(HyGuiItemType eType, QString sPath, QJsonValue itemVal);
    void SaveGameData();
    
    void DeleteGameData(HyGuiItemType eType, QString sPath);
    void DeletePrefixAndContents(HyGuiItemType eSubDirType, QString sPrefix);

    QJsonObject GetSubDirObj(HyGuiItemType eType);
    
    // TODO: Remove this
    void RefreshCurrentItemDraw();

    bool CloseAllTabs();

public Q_SLOTS:
    void OnTabBarCurrentChanged(int iIndex);

    void OnCloseTab(int iIndex);
};

#endif // PROJECT_H
