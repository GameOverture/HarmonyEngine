/**************************************************************************
 *	MainWindow.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Designer Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Global.h"
#include "Harmony.h"
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
class ProjectItem;

class MainWindow : public QMainWindow
{
	Q_OBJECT

	static MainWindow *             sm_pInstance;

	Harmony                         m_Harmony;
	Theme                           m_eTheme;

	QSettings                       m_Settings;

	QString                         m_sEngineLocation;
	QString                         m_sDefaultProjectLocation;

	QList<WaitingSpinnerWidget *>   m_LoadingSpinnerList;
	QLabel                          m_LoadingMsg;
	QProgressBar                    m_LoadingBar;

public:
	explicit MainWindow(QWidget *pParent = 0);
	~MainWindow();

	static MainWindow *GetInstance();   // Should only be used to set QWidget parents // TODO: Check if messageboxes even care about their parent set, if not then remove this

	void SetLoading(QString sMsg);
	void ClearLoading();

	void SetHarmonyWidget(HarmonyWidget *pWidget);
	void SetCurrentProject(Project *pProject);

	static QString EngineSrcLocation();

	static void PasteItemSrc(QByteArray sSrc, Project *pProject);

	static void ApplySaveEnables(bool bCurItemDirty, bool bAnyItemDirty);
	static void OpenItem(ProjectItem *pItem);
	static void CloseItem(ProjectItem *pItem);

protected:
	virtual void closeEvent(QCloseEvent *pEvent) override;

private Q_SLOTS:
	void OnCtrlTab();

	void on_actionNewProject_triggered();
	void on_actionOpenProject_triggered();
	void on_actionCloseProject_triggered();

	void on_actionNewAudio_triggered();
	void on_actionNewEntity_triggered();
	void on_actionNewParticle_triggered();
	void on_actionNewSprite_triggered();
	void on_actionNewFont_triggered();

	void on_actionSave_triggered();
	void on_actionSaveAll_triggered();

	void on_menu_View_aboutToShow();
	
	void on_actionLaunchIDE_triggered();
	void on_actionConnect_triggered();    
	
	void on_actionAbout_triggered();
	
	void on_actionExit_triggered();

	void on_actionProjectSettings_triggered();

	void on_actionTheme_Lappy486_triggered();

	void on_actionTheme_Compe_triggered();

private:
	Ui::MainWindow *ui;

	void NewItem(HyGuiItemType eItem);

	void SaveSettings();
	void SelectTheme(Theme eTheme);
};

#endif // MAINWINDOW_H
