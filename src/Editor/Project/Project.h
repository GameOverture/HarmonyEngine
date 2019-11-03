/**************************************************************************
 *	Project.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef PROJECT_H
#define PROJECT_H

#include "ExplorerWidget.h"
#include "ProjectDraw.h"
#include "ProjectItem.h"
#include "ExplorerItem.h"
#include "DlgProjectSettings.h"

#include <QQueue>
#include <QJsonObject>
#include <QStandardItemModel>

// Forward declaration
class AtlasModel;
class AtlasWidget;
class GltfModel;
class GltfWidget;
class AudioAssetsWidget;
class Harmony;
class ExplorerModel;

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

class Project : public ExplorerItem
{
	Q_OBJECT

	ExplorerModel &									m_ModelRef;

	ProjectDraw *									m_pDraw;
	DlgProjectSettings								m_DlgProjectSettings;   // Stores the actual settings in a QJsonObject within;

	AtlasModel *									m_pAtlasModel;
	AtlasWidget *									m_pAtlasWidget;

	GltfModel *										m_pGltfModel;
	GltfWidget *									m_pGltfWidget;

	QStandardItemModel								m_FontListModel;

	AudioAssetsWidget *								m_pAudioWidget;

	ProjectTabBar *									m_pTabBar;
	ProjectItem *									m_pCurOpenItem;

	QJsonObject										m_SaveDataObj;

	bool											m_bHasError;
	
public:
	Project(const QString sProjectFilePath, ExplorerModel &modelRef);
	virtual ~Project();

	virtual QString GetName(bool bWithPrefix) const override;
	
	void LoadExplorerModel();

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

	ExplorerModel &GetExplorerModel();
	AtlasModel &GetAtlasModel();
	AtlasWidget *GetAtlasWidget();

	GltfModel *GetGltfModel();
	GltfWidget *GetGltfWidget();

	void SetAudioModel(QJsonObject audioObj);
	AudioAssetsWidget *GetAudioWidget();

	QStandardItemModel *GetFontListModel();
	void ScanMetaFontDir();

	ProjectTabBar *GetTabBar();

	ProjectItem *GetCurrentOpenItem();

	void SetRenderSize(int iWidth, int iHeight);

	void SaveGameData(HyGuiItemType eType, QString sPath, QJsonValue itemVal);
	
	void DeleteGameData(HyGuiItemType eType, QString sPath);
	void DeletePrefixAndContents(QString sPrefix);

	QString RenameItem(HyGuiItemType eType, QString sOldPath, QString sNewPath);
	QString RenamePrefix(QString sOldPath, QString sNewPath);
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

	// IHyEngine
	bool HarmonyInitialize();
	bool HarmonyUpdate();
	void HarmonyShutdown();

public Q_SLOTS:
	void OnTabBarCurrentChanged(int iIndex);

	void OnCloseTab(int iIndex);
};

#endif // PROJECT_H
