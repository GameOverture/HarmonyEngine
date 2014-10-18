#ifndef HYGUI_H
#define HYGUI_H

#include <QMainWindow>
#include <QSettings>

#include "Item.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    static MainWindow * sm_pInstance;

    QSettings           m_Settings;
    QString             m_sDefaultProjLocation;

    bool                m_bIsInitialized;
    
    //QMenu *             m_pEditContextMenu;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void showEvent(QShowEvent *pEvent);
    
    static void OpenItem(Item *pItem);
    static void CloseItem(Item *pItem);

private slots:
    void on_actionNewProject_triggered();
    void on_actionOpenProject_triggered();
    void on_actionCloseProject_triggered();
    void on_actionNewSprite_triggered();
    void on_actionNewFont_triggered();
    
    
    void on_actionNewParticle_triggered();
    
    void on_actionNewAudio_triggered();
    
    void on_actionViewExplorer_triggered();
    
private:
    Ui::MainWindow *ui;

    void NewItem(eItemType eItem);
    
    void closeEvent(QCloseEvent * event);

    void SaveSettings();
    void LoadSettings();
    
    void UpdateActions();
    
};

#endif // HYGUI_H
