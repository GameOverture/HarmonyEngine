/**************************************************************************
 *	MainWindow.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HYGUI_H
#define HYGUI_H

#include <QMainWindow>
#include <QSettings>
#include <QTcpServer>

#include "Project.h"
#include "HyGuiDebugger.h"

#include "QtWaitingSpinner/waitingspinnerwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    static MainWindow *     sm_pInstance;

    WaitingSpinnerWidget *  m_pLoadingSpinners[NUM_MDI];
    uint                    m_uiLoadingSpinnerRefCounts[NUM_MDI];

    QSettings               m_Settings;
    QString                 m_sEngineLocation;
    QString                 m_sDefaultProjectLocation;

    bool                    m_bIsInitialized;

    Project *               m_pCurSelectedProj;
    HyGuiRenderer *         m_pCurRenderer;

    QAction *               m_pCurSaveAction;
    QAction *               m_pCurSaveAllAction;

    HyGuiDebugger *         m_pDebugConnection;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void showEvent(QShowEvent *pEvent);

    static MainWindow *GetInstance();   // Should only be used to set QWidget parents
    
    static QString EngineLocation();
    
    static void OpenItem(IProjItem *pItem);
    static void CloseItem(IProjItem *pItem);
    
    static void SetSelectedProj(Project *pProj);
    static void ReloadHarmony();

    static void StartLoading(uint uiAreaFlags);
    static void StopLoading(uint uiAreaFlags);
    
    static HyRendererInterop *GetCurrentRenderer();

private Q_SLOTS:
    void on_actionNewProject_triggered();
    void on_actionOpenProject_triggered();
    void on_actionCloseProject_triggered();
    void on_actionNewSprite_triggered();
    void on_actionNewFont_triggered();
    
    void on_actionNewParticle_triggered();
    
    void on_actionNewAudio_triggered();
    
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
    
private:
    Ui::MainWindow *ui;

    void NewItem(eItemType eItem);
    
    void closeEvent(QCloseEvent * event);

    void SaveSettings();
};

#endif // HYGUI_H
