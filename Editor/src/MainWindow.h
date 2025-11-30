/**************************************************************************
 *	MainWindow.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Global.h"
#include "Harmony.h"
#include "Explorer/ExplorerModel.h"
#include "_Dependencies/QtWaitingSpinner/waitingspinnerwidget.h"

#include <QMainWindow>
#include <QSettings>
#include <QTcpServer>
#include <QStackedWidget>
#include <QThread>
#include <QLabel>
#include <QProgressBar>

namespace Ui {
class MainWindow;
}

class WgtHarmony;
class Project;
class ProjectItemData;

class HarmonyStatusBar : public QStatusBar
{
	Q_OBJECT

public:
	HarmonyStatusBar(QWidget *pParent = nullptr) :
		QStatusBar(pParent)
	{
	}
	virtual ~HarmonyStatusBar() { }
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

	static MainWindow *					sm_pInstance;

	ExplorerModel						m_ExplorerModel;

	Harmony								m_Harmony;
	Theme								m_eTheme;

	QSettings							m_Settings;

	QString								m_sEnginePath;
	QString								m_sDefaultProjectLocation;

	QList<WaitingSpinnerWidget *>		m_LoadingSpinnerList;
	QMap<LoadingType, QPair<int, int>>	m_LoadingMap;
	QLabel								m_LoadingMsg;
	QProgressBar						m_LoadingBar;

	QLabel 								m_StatusBarMouse;
	QLabel								m_StatusBarZoom;

public:
	explicit MainWindow(QWidget *pParent = 0);
	~MainWindow();

	static MainWindow *GetInstance();   // Should only be used to set QWidget parents // TODO: Check if messageboxes even care about their parent set, if not then remove this	

	void SetHarmonyWidget(HarmonyWidget *pWidget);
	void SetCurrentProject(Project *pProject);

	static Theme GetTheme();
	static QList<LoadingType> GetCurrentLoading();
	static void SetLoading(LoadingType eLoadingType, int iLoadedBlocks, int iTotalBlocks);
	static void ClearLoading(LoadingType eLoadingType);

	static QString EngineSrcLocation();

	static void ApplySaveEnables(bool bCurItemDirty, bool bAnyItemDirty);
	static void OpenItem(ProjectItemData *pItem);
	static void CloseItem(ProjectItemData *pItem);

	static QMenu *GetNewItemMenu();

	static ExplorerModel &GetExplorerModel();
	static ExplorerWidget &GetExplorerWidget();

	static int GetAssetManagerTabIndex();

	static IWidget *GetItemProperties();

	static QWidget *GetAuxWidget(AuxTab eTabIndex);
	static void FocusAuxWidget(AuxTab eTabIndex);
	static void HideAuxWidget(AuxTab eTabIndex);

	static void SetStatus(const QString &sMessage, int iTimeoutMs);
	static void SetTempStatus(const QString &sMessage);
	static void ClearStatus();
	static void SetDrawStatus(QString sMouse, QString sZoom);

	static void GetGridStatus(bool &bShowBackgroundOut, bool &bShowOriginOut, bool &bShowOverlayOut);

	virtual QMenu *createPopupMenu() override;

protected:
	virtual void closeEvent(QCloseEvent *pEvent) override;

	void RefreshLoading();

private Q_SLOTS:
	void OnCtrlTab();
	void OnCtrlShiftTab();
	void OnCtrlF4();

	void on_tabWidgetAssetManager_currentChanged(int iIndex);
	void on_tabWidgetAux_currentChanged(int iIndex);

	void on_actionNewProject_triggered();
	void on_actionOpenProject_triggered();
	void on_actionCloseProject_triggered();
	void on_actionProjectSettings_triggered();
	void on_actionOpenFolderExplorer_triggered();

	void on_dockWidgetAssets_visibilityChanged(bool visible);

	void on_actionNewPrefix_triggered();
	void on_actionNewAudio_triggered();
	void on_actionNewEntity_triggered();
	void on_actionNewParticle_triggered();
	void on_actionNewSprite_triggered();
	void on_actionNewTileMap_triggered();
	void on_actionNewSpine_triggered();
	void on_actionNewText_triggered();
    void on_actionNewEntity3d_triggered();
    void on_actionNewPrefab_triggered();

	void on_actionSave_triggered();
	void on_actionSaveAll_triggered();

	void on_menu_View_aboutToShow();
	
	void on_actionShowGridBackground_triggered();
	void on_actionShowGridOrigin_triggered();
	void on_actionShowGridOverlay_triggered();
	void on_actionSnappingSettings_triggered();

	void on_actionChangeHarmonyLocation_triggered();
	void on_actionConnect_triggered();
	
	void on_actionAbout_triggered();
	
	void on_actionExit_triggered();

    void on_actionTheme_Decemberween_triggered();
	void on_actionTheme_CorpyNT6_triggered();
	void on_actionTheme_Compe_triggered();

	void on_actionActivateProject_triggered();

private:
	Ui::MainWindow *ui;

	void DeserializeColors(const QByteArray &serializedColors);
	void SerializeColors(const QByteArray &serializedColors);

	void NewItem(ItemType eItem);
	void SaveSettings();
	void SelectTheme(Theme eTheme);
};

#endif // MAINWINDOW_H
