/**************************************************************************
 *	Project.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Designer Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef PROJECT_H
#define PROJECT_H

#include "ExplorerWidget.h"
#include "ProjectDraw.h"
#include "ProjectItem.h"
#include "AtlasModel.h"
#include "ExplorerTreeItem.h"
#include "DlgProjectSettings.h"

#include <QQueue>
#include <QJsonObject>

// Forward declaration
class AtlasWidget;
class AudioWidgetManager;
class Harmony;

class AtlasTreeItem;

class ProjectTabBar : public QTabBar
{
	Project *           m_pProjectOwner;

public:
	ProjectTabBar(Project *pProjectOwner);
	virtual ~ProjectTabBar();

protected:
	virtual void dragEnterEvent(QDragEnterEvent *pEvent) override;
	virtual void dropEvent(QDropEvent *pEvent) override;
};

class Project : public ExplorerTreeItem, public IHyApplication
{
	Q_OBJECT

	ExplorerWidget *                                m_pWidget;
	ProjectDraw *                                   m_pDraw;
	DlgProjectSettings                              m_DlgProjectSettings;   // Stores the actual settings in a QJsonObject within;

	AtlasModel *                                    m_pAtlasModel;
	AtlasWidget *                                   m_pAtlasWidget;

	AudioWidgetManager *                            m_pAudioMan;
	ProjectTabBar *                                 m_pTabBar;

	ProjectItem *                                   m_pCurOpenItem;

	QJsonObject                                     m_SaveDataObj;

	bool                                            m_bHasError;
	
public:
	Project(ExplorerWidget *pProjWidget, const QString sProjectFilePath);
	virtual ~Project();

	bool HasError() const;
	
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

	ProjectTabBar *GetTabBar();

	ExplorerWidget *GetExplorerWidget();
	ProjectItem *GetCurrentOpenItem();

	void SetRenderSize(int iWidth, int iHeight);

	void SaveGameData(HyGuiItemType eType, QString sPath, QJsonValue itemVal);
	
	void DeleteGameData(HyGuiItemType eType, QString sPath);
	void DeletePrefixAndContents(QString sPrefix);

	void RenameGameData(HyGuiItemType eType, QString sOldPath, QString sNewPath, QJsonValue itemVal);
	void RenamePrefix(QString sOldPath, QString sNewPath);
	void RefreshNamesOnTabs();

	void WriteGameData();

	QJsonObject GetSavedItemsObj(HyGuiItemType eType);

	// These tab functions are only called from MainWindow
	void OpenTab(ProjectItem *pItem);
	void CloseTab(ProjectItem *pItem);
	bool CloseAllTabs();

	// This is called in Harmony during a reload project
	void UnloadAllTabs();

	void ApplySaveEnables();

	// IHyApplication overrides
	virtual bool Initialize();
	virtual bool Update();
	virtual void Shutdown();

public Q_SLOTS:
	void OnTabBarCurrentChanged(int iIndex);

	void OnCloseTab(int iIndex);
};

#endif // PROJECT_H
