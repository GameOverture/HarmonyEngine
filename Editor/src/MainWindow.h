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

protected:
	virtual void closeEvent(QCloseEvent *pEvent) override;

private Q_SLOTS:
	void OnCtrlTab();
	void OnProcessStdOut();
	void OnProcessErrorOut();

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
	
	void on_actionNewBuild_triggered();
	void on_actionOpenIde_triggered(QAction *pAction);
	void on_actionNewPackage_triggered();

	void on_actionConnect_triggered();

	void on_actionImportTileSheet_triggered();
	
	void on_actionAbout_triggered();
	
	void on_actionExit_triggered();

	void on_actionTheme_Lappy486_triggered();

	void on_actionTheme_Compe_triggered();

	void on_actionActivateProject_triggered();


private:
	Ui::MainWindow *ui;

	void NewItem(HyGuiItemType eItem);
	void RefreshBuildMenu();
	void SaveSettings();
	void SelectTheme(Theme eTheme);
};

#endif // MAINWINDOW_H
