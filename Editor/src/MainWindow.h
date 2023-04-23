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

class HarmonyWidget;
class Project;
class ProjectItemData;

class MainWindow : public QMainWindow
{
	Q_OBJECT

	static MainWindow *				sm_pInstance;

	ExplorerModel					m_ExplorerModel;

	Harmony							m_Harmony;
	Theme							m_eTheme;

	QSettings						m_Settings;

	QString							m_sEnginePath;
	QString							m_sDefaultProjectLocation;

	QList<WaitingSpinnerWidget *>	m_LoadingSpinnerList;
	QLabel							m_LoadingMsg;
	QProgressBar					m_LoadingBar;

	QLabel 							m_StatusBarMouseIcon;
	QLabel 							m_StatusBarMouse;
	QLabel 							m_StatusBarSizeIcon;
	QLabel							m_StatusBarSize;
	QLabel 							m_StatusBarZoomIcon;
	QLabel							m_StatusBarZoom;

public:
	explicit MainWindow(QWidget *pParent = 0);
	~MainWindow();

	static MainWindow *GetInstance();   // Should only be used to set QWidget parents // TODO: Check if messageboxes even care about their parent set, if not then remove this

	void SetHarmonyWidget(HarmonyWidget *pWidget);
	void SetCurrentProject(Project *pProject);

	static void SetLoading(QString sMsg, int iPercentComplete);
	static void ClearLoading();

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

	static void SetStatus(const QString &sMessage, int iTimeoutMs);
	static void ClearStatus();
	static void SetDrawStatus(QString sMouse, QString sSize, QString sZoom);

	static void GetGridStatus(bool &bShowBackgroundOut, bool &bShowOriginOut, bool &bShowOverlayOut);

protected:
	virtual void closeEvent(QCloseEvent *pEvent) override;

private Q_SLOTS:
	void OnCtrlTab();
	void OnProcessStdOut();
	void OnProcessErrorOut();

	void on_tabWidgetAux_currentChanged(int iIndex);

	void on_actionNewProject_triggered();
	void on_actionOpenProject_triggered();
	void on_actionCloseProject_triggered();
	void on_actionProjectSettings_triggered();
	void on_actionOpenFolderExplorer_triggered();

	void on_actionNewPrefix_triggered();
	void on_actionNewAudio_triggered();
	void on_actionNewEntity_triggered();
	void on_actionNewParticle_triggered();
	void on_actionNewSprite_triggered();
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
	
	void on_actionBuildSettings_triggered();
	void on_actionNewBuild_triggered();
	void on_actionNewPackage_triggered();

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

	void NewItem(ItemType eItem);
	void RefreshBuildMenu();
	void SaveSettings();
	void SelectTheme(Theme eTheme);
};

#endif // MAINWINDOW_H
