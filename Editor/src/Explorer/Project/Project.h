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
#include "IMimeData.h"

#include <QQueue>
#include <QJsonObject>
#include <QStandardItemModel>

// Forward declaration
class SourceModel;
class AtlasModel;
class IManagerModel;
class ManagerWidget;
class AudioManagerModel;
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

	QJsonObject											m_SettingsObj;

	ProjectDraw *										m_pDraw;

	SourceModel *										m_pSourceModel;
	ManagerWidget *										m_pSourceWidget;
	AtlasModel *										m_pAtlasModel;
	ManagerWidget *										m_pAtlasWidget;
	AudioManagerModel *									m_pAudioModel;
	ManagerWidget *										m_pAudioWidget;

	QStandardItemModel									m_FontListModel;

	ProjectTabBar *										m_pTabBar;
	ProjectItemData *									m_pCurOpenItem;

	FileDataPair										m_ProjectFileData;

	QMap<QUuid, TreeModelItemData *>					m_ItemDataUuidMap;	// Lookup map to help find tree item data pointers

	bool												m_bHasError;
	bool												m_bExplorerModelLoaded;
	
public:
	Project(const QString sProjectFilePath, ExplorerModel &modelRef);
	virtual ~Project();

	QString GetName() const;
	virtual QString GetName(bool bWithPrefix) const override;
	
	void LoadExplorerModel();

	bool HasError() const;
	
	QJsonObject GetSettingsObj() const;
	void SaveSettingsObj(const QJsonObject newSettingsObj);

	QString GetDirPath() const;
	QString GetTitle() const;

	QString GetAbsPath() const;
	QString GetAssetsAbsPath() const;
	QString GetAssetsRelPath() const;
	QString GetMetaAbsPath() const;
	QString GetMetaRelPath() const;
	QString GetSourceAbsPath() const;
	QString GetSourceRelPath() const;
	QString GetBuildAbsPath() const;
	QString GetBuildRelPath() const;
	QString GetUserAbsPath() const;

	IManagerModel *GetManagerModel(AssetManagerType eManagerType);

	SourceModel &GetSourceModel();
	ManagerWidget *GetSourceWidget();
	AtlasModel &GetAtlasModel();
	ManagerWidget *GetAtlasWidget();
	AudioManagerModel &GetAudioModel();
	ManagerWidget *GetAudioWidget();

	bool PasteAssets(ItemType ePasteItemType, QJsonArray &assetArrayRef, AssetManagerType eAssetType);

	//void SetAudioModel(QJsonObject audioObj);

	QStandardItemModel *GetFontListModel();
	void ScanMetaFontDir();

	ProjectTabBar *GetTabBar();
	ProjectItemData *GetCurrentOpenItem();

	void SetRenderSize(int iWidth, int iHeight);
	
	void ShowGridBackground(bool bShow);
	void ShowGridOrigin(bool bShow);
	void ShowGridOverlay(bool bShow);

	void SaveItemData(ItemType eType, QString sPath, const FileDataPair &itemFileDataRef, bool bWriteToDisk);
	void DeleteItemData(ItemType eType, QString sPath, bool bWriteToDisk);
	void DeletePrefixAndContents(QString sPrefix, bool bWriteToDisk);

	QString RenameItem(ItemType eType, QString sOldPath, QString sNewPath);
	QString RenamePrefix(QString sOldPath, QString sNewPath);

	bool DoesItemExist(ItemType eType, QString sPath) const;

	void SaveUserData() const;

	void AddItemDataLookup(TreeModelItemData *pItemData);
	void RemoveItemDataLookup(const QUuid &uuid);
	TreeModelItemData *FindItemData(const QUuid &uuid);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// TreeModelItemData DEPENDENCIES LINK FUNCTIONS
	QList<TreeModelItemData *> IncrementDependencies(TreeModelItemData *pItemDepender, QList<QUuid> dependeeList);
	QList<TreeModelItemData *> IncrementDependencies(TreeModelItemData *pItemDepender, QList<TreeModelItemData *> dependeeItemDataList);
	void DecrementDependencies(TreeModelItemData *pItemDepender, QList<QUuid> dependeeList);
	void DecrementDependencies(TreeModelItemData *pItemDepender, QList<TreeModelItemData *> dependeeItemDataList);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// These tab functions are only called from MainWindow
	void OpenTab(ProjectItemData *pItem);
	void CloseTab(ProjectItemData *pItem);
	bool CloseAllTabs();

	// This is called in Harmony during a reload project
	void UnloadAllTabs();

	void ApplySaveEnables();
	bool IsUnsavedOpenItems();

	void RunCMakeGui();

	// IHyEngine
	bool HarmonyInitialize();
	bool HarmonyUpdate();
	void HarmonyShutdown();

public Q_SLOTS:
	void OnTabBarCurrentChanged(int iIndex);
	void OnCloseTab(int iIndex);

private:
	QJsonObject ReadProjFile();
	void WriteGameData();
	void WriteMetaData();

	bool LoadDataObj(QString sFilePath, QJsonObject &dataObjRef);	// Return 'true' if the data obj should save to disk
	void DeleteItemInDataObj(ItemType eType, QString sPath, QJsonObject &dataObjRef);
	void RenameItemInDataObj(ItemType eType, QString sOldPath, QString sNewPath, QJsonObject &dataObjRef);
	void RenamePrefixInDataObj(QString sOldPath, QString sNewPath, QJsonObject &dataObjRef);

	void RefreshNamesOnTabs();
};

#endif // PROJECT_H
