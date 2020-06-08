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
#include "ProjectItemData.h"
#include "ExplorerItemData.h"
#include "DlgProjectSettings.h"

#include <QQueue>
#include <QJsonObject>
#include <QStandardItemModel>

// Forward declaration
class AtlasModel;
class IManagerWidget;
class GltfModel;
class GltfWidget;
class AudioAssetsWidget;
class Harmony;
class ExplorerModel;

class ProjectTabBar : public QTabBar
{
	Project *										m_pProjectOwner;

public:
	ProjectTabBar(Project *pProjectOwner);
	virtual ~ProjectTabBar();

protected:
	virtual void dragEnterEvent(QDragEnterEvent *pEvent) override;
	virtual void dropEvent(QDropEvent *pEvent) override;
};

class Project : public ExplorerItemData
{
	Q_OBJECT

	ExplorerModel &										m_ModelRef;

	ProjectDraw *										m_pDraw;
	DlgProjectSettings									m_DlgProjectSettings;   // Stores the actual settings in a QJsonObject within;

	AtlasModel *										m_pAtlasModel;
	AtlasWidget *										m_pAtlasWidget;

	GltfModel *											m_pGltfModel;
	GltfWidget *										m_pGltfWidget;

	QStandardItemModel									m_FontListModel;

	AudioAssetsWidget *									m_pAudioWidget;

	ProjectTabBar *										m_pTabBar;
	ProjectItemData *									m_pCurOpenItem;

	FileDataPair										m_ProjectFileData;

	QMap<ProjectItemData *, QSet<ProjectItemData *>>	m_ItemOwnerMap;

	bool												m_bHasError;
	
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
	QString GetBuildAbsPath() const;
	QString GetBuildRelPath() const;

	ExplorerModel &GetExplorerModel();
	AtlasModel &GetAtlasModel();
	IManagerWidget *GetAtlasWidget();

	GltfModel *GetGltfModel();
	GltfWidget *GetGltfWidget();

	void SetAudioModel(QJsonObject audioObj);
	AudioAssetsWidget *GetAudioWidget();

	QStandardItemModel *GetFontListModel();
	void ScanMetaFontDir();

	ProjectTabBar *GetTabBar();
	ProjectItemData *GetCurrentOpenItem();

	void SetRenderSize(int iWidth, int iHeight);

	void SaveItemData(HyGuiItemType eType, QString sPath, const FileDataPair &itemFileDataRef, bool bWriteToDisk);
	void DeleteItemData(HyGuiItemType eType, QString sPath, bool bWriteToDisk);
	void DeletePrefixAndContents(QString sPrefix, bool bWriteToDisk);

	QString RenameItem(HyGuiItemType eType, QString sOldPath, QString sNewPath);
	QString RenamePrefix(QString sOldPath, QString sNewPath);

	bool DoesItemExist(HyGuiItemType eType, QString sPath) const;

	// Dependency links between items
	QList<ProjectItemData *> RegisterItemsById(ProjectItemData *pItemOwner, QList<QUuid> requestList);
	QList<ProjectItemData *> RegisterItems(ProjectItemData *pItemOwner, QList<ProjectItemData *> requestList);
	void RelinquishItems(ProjectItemData *pItemOwner, QList<ProjectItemData *> relinquishList);
	QList<ProjectItemData *> GetItemOwners(ProjectItemData *pItem);

	// These tab functions are only called from MainWindow
	void OpenTab(ProjectItemData *pItem);
	void CloseTab(ProjectItemData *pItem);
	bool CloseAllTabs();

	// This is called in Harmony during a reload project
	void UnloadAllTabs();

	void ApplySaveEnables();

	void RunCMakeGui();

	// IHyEngine
	bool HarmonyInitialize();
	bool HarmonyUpdate();
	void HarmonyShutdown();

public Q_SLOTS:
	void OnTabBarCurrentChanged(int iIndex);
	void OnCloseTab(int iIndex);

private:
	void WriteGameData();
	void WriteMetaData();

	bool LoadDataObj(QString sFilePath, QJsonObject &dataObjRef);	// Return 'true' if the data obj should save to disk
	void DeleteItemInDataObj(HyGuiItemType eType, QString sPath, QJsonObject &dataObjRef);
	void RenameItemInDataObj(HyGuiItemType eType, QString sOldPath, QString sNewPath, QJsonObject &dataObjRef);
	void RenamePrefixInDataObj(QString sOldPath, QString sNewPath, QJsonObject &dataObjRef);

	void RefreshNamesOnTabs();
};

#endif // PROJECT_H
