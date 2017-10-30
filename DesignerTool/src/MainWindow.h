/**************************************************************************
 *	MainWindow.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Global.h"
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

class Harmony;
class Project;
class ProjectItem;

//class SwitchRendererThread : public QThread
//{
//    Q_OBJECT

//    HyGuiRenderer *m_pCurrentRenderer;

//public:
//    SwitchRendererThread(HyGuiRenderer *pCurrentRenderer, QObject *pParent) :   QThread(pParent),
//                                                                                m_pCurrentRenderer(pCurrentRenderer)
//    { }

//    virtual void run() override;

//Q_SIGNALS:
//      void SwitchIsReady(HyGuiRenderer *pRenderer);
//};

class MainWindow : public QMainWindow
{
    Q_OBJECT

    static MainWindow *     sm_pInstance;

    Harmony *               m_Harmony;

    QSettings               m_Settings;
    QString                 m_sEngineLocation;
    QString                 m_sDefaultProjectLocation;

    bool                    m_bIsInitialized;   // TODO: Get rid of this

    WaitingSpinnerWidget    m_LoadingSpinner;
    QLabel                  m_LoadingMsg;
    QProgressBar            m_LoadingBar;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void SetLoading(QString sMsg);
    void ClearLoading();

    void SetCurrentProject(Project &newCurrentProjectRef);

    void showEvent(QShowEvent *pEvent);

    static MainWindow *GetInstance();   // Should only be used to set QWidget parents // TODO: Check if messageboxes even care about their parent set, if not then remove this
    
    static QString EngineSrcLocation();

    static void PasteItemSrc(QByteArray sSrc, Project *pProject);

    static void ApplySaveEnables(bool bCurItemDirty, bool bAnyItemDirty);
    static void OpenItem(ProjectItem *pItem);
    static void CloseItem(ProjectItem *pItem);

private Q_SLOTS:
    void OnCtrlTab();

    void on_actionNewProject_triggered();
    void on_actionOpenProject_triggered();
    void on_actionCloseProject_triggered();

    void on_actionNewSprite_triggered();
    void on_actionNewFont_triggered();
    void on_actionNewParticle_triggered();
    void on_actionNewAudio_triggered();
    void on_actionNewEntity_triggered();
    
    void on_actionViewExplorer_triggered();
    void on_actionViewAtlasManager_triggered();
    void on_actionViewOutputLog_triggered();
    
    void on_actionConnect_triggered();

    void on_actionViewProperties_triggered();
    
    void on_actionSave_triggered();
    void on_actionSaveAll_triggered();
    
    void on_actionLaunchIDE_triggered();
    
    void on_actionAbout_triggered();

    void on_actionAudioManager_triggered();
    
    void on_actionExit_triggered();

    void on_actionProjectSettings_triggered();

private:
    Ui::MainWindow *ui;

    void NewItem(HyGuiItemType eItem);
    
    void closeEvent(QCloseEvent *pEvent);

    void SaveSettings();
};

#endif // MAINWINDOW_H
