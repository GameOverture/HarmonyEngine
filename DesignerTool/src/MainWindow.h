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

#include "ItemProject.h"
#include "HyGuiDebugger.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    static MainWindow * sm_pInstance;

    QSettings           m_Settings;
    QString             m_sDefaultProjectLocation;

    bool                m_bIsInitialized;

    ItemProject *       m_pCurSelectedProj;
    HyGuiRenderer *     m_pCurRenderer;
    QMenu *             m_pCurEditMenu;
    
    HyGuiDebugger *     m_pDebugConnection;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void showEvent(QShowEvent *pEvent);

    static MainWindow *GetInstance();   // Should only be used to set QWidget parents
    
    // This only requests to the WidgetRenderer to open/close the item. It will eventually do so, after re-loading any resources if need be
    static void OpenItem(ItemWidget *pItem);
    static void CloseItem(ItemWidget *pItem);
    
    static void SetSelectedProj(ItemProject *pProj);
    static void ReloadHarmony();
    
    static void SetSaveEnabled(bool bSaveEnabled, bool bSaveAllEnabled);

private slots:
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
    
private:
    Ui::MainWindow *ui;

    void NewItem(eItemType eItem);
    
    void closeEvent(QCloseEvent * event);

    void SaveSettings();
};

#endif // HYGUI_H
