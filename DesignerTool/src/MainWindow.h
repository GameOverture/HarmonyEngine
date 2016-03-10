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

    bool                m_bIsInitialized;
    
    ItemProject *       m_pCurSelectedProj;
    
    HyGuiDebugger *     m_pDebugConnection;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void showEvent(QShowEvent *pEvent);
    
    static void OpenItem(Item *pItem);
    static void CloseItem(Item *pItem);
    static void SetCurrentItem(Item *pItem);
    
    static void SetSelectedProj(ItemProject *pProj);
    
    static void ReloadItems(QStringList &sPaths, bool bRefreshAssets);

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

private:
    Ui::MainWindow *ui;

    void NewItem(eItemType eItem);
    
    void closeEvent(QCloseEvent * event);

    void SaveSettings();
};

#endif // HYGUI_H
