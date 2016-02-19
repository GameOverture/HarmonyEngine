#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "DlgSetEngineLocation.h"
#include "DlgNewProject.h"
#include "DlgNewItem.h"
#include "DlgInputName.h"

#include "WidgetExplorer.h"
#include "WidgetAtlasManager.h"

#include "ItemSprite.h"

#include "HyGlobal.h"

#include <QFileDialog>
#include <QShowEvent>
#include <QStringBuilder>
#include <QVBoxLayout>
#include <QTcpSocket>
#include <QMessageBox>

#define HYGUIVERSION_STRING "v0.0.1"

/*static*/ MainWindow * MainWindow::sm_pInstance = NULL;

MainWindow::MainWindow(QWidget *parent) :   QMainWindow(parent),
                                            ui(new Ui::MainWindow),
                                            m_Settings("Overture Games", "Harmony Designer Tool"),
                                            m_bIsInitialized(false)
{
    ui->setupUi(this);
    sm_pInstance = this;
    
    SetSelectedProj(NULL);

    HyGuiLog("Harmony Designer Tool " % QString(HYGUIVERSION_STRING), LOGTYPE_Title);
    HyGuiLog("Initializing...", LOGTYPE_Normal);
    
    ui->actionCloseProject->setEnabled(false);
    ui->actionNewSprite->setEnabled(false);
    ui->actionNewFont->setEnabled(false);
    ui->actionNewParticle->setEnabled(false);
    ui->actionNewAudio->setEnabled(false);
    
    // Link the actions to their proper widgets
    ui->explorer->addAction(ui->actionCloseProject);
    ui->explorer->addAction(ui->actionCopy);
    ui->explorer->addAction(ui->actionCut);
    ui->explorer->addAction(ui->actionNewProject);
    ui->explorer->addAction(ui->actionNewAudio);
    ui->explorer->addAction(ui->actionNewParticle);
    ui->explorer->addAction(ui->actionNewFont);
    ui->explorer->addAction(ui->actionNewSprite);
    ui->explorer->addAction(ui->actionOpenProject);
    ui->explorer->addAction(ui->actionPaste);
    ui->explorer->addAction(ui->actionRemove);
    ui->explorer->addAction(ui->actionRename);
    ui->explorer->addAction(ui->actionSave);

    m_pDebugConnection = new HyGuiDebugger(*ui->actionConnect, this);
    
    ui->dockWidgetAtlas->hide();
    ui->dockWidgetGlyphCreator->hide();
    
    HyGuiLog("Checking required initialization parameters...", LOGTYPE_Normal);
    m_Settings.beginGroup("RequiredParams");
    {
        QDir engineDir(m_Settings.value("engineLocation").toString());
        
        while(HyGlobal::IsEngineDirValid(engineDir) == false)
        {
            QMessageBox::information(parentWidget(), HyDesignerToolName, "First run initialization: Please specify where the Harmony Engine project location is on your machine");
            
            DlgSetEngineLocation *pDlg = new DlgSetEngineLocation(this);
            if(pDlg->exec() == QDialog::Accepted)
            {
                engineDir.setPath(pDlg->SelectedDir());
                m_Settings.setValue("engineLocation", QVariant(pDlg->SelectedDir()));
            }
            else
            {
                if(QMessageBox::Retry != QMessageBox::critical(parentWidget(), HyDesignerToolName, "You must specify the Harmony Engine project location to continue", QMessageBox::Retry | QMessageBox::Cancel, QMessageBox::Retry))
                    exit(-1);
            }
            delete pDlg;
        }
    }
    m_Settings.endGroup();
    
    // Restore workspace
    HyGuiLog("Recovering previously opened session...", LOGTYPE_Normal);
    m_Settings.beginGroup("MainWindow");
    {
        restoreGeometry(m_Settings.value("geometry").toByteArray());
        restoreState(m_Settings.value("windowState").toByteArray());
    }
    m_Settings.endGroup();

    ui->actionViewAtlasManager->setChecked(!ui->dockWidgetAtlas->isHidden());
    ui->actionViewExplorer->setChecked(!ui->dockWidgetExplorer->isHidden());
    ui->actionViewOutputLog->setChecked(!ui->dockWidgetOutputLog->isHidden());

    m_Settings.beginGroup("OpenData");
    {
        QStringList sListOpenProjs = m_Settings.value("openProjs").toStringList();
        foreach(QString sProjPath, sListOpenProjs)
        {
            ui->explorer->AddItem(ITEM_Project, sProjPath, false);
        }
    }
    m_Settings.endGroup();

    // Restore opened items/tabs

    // Append version to window title
    setWindowTitle(windowTitle() % " " % HYGUIVERSION_STRING);

    HyGuiLog("Ready to go!", LOGTYPE_Normal);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showEvent(QShowEvent *pEvent)
{
    QMainWindow::showEvent(pEvent);
    if(pEvent->spontaneous())
        return;

    if(m_bIsInitialized == false)
    {
        ui->actionViewExplorer->setChecked(ui->dockWidgetExplorer->isVisible());
        ui->actionViewProperties->setChecked(ui->dockWidgetGlyphCreator->isVisible());
        ui->actionViewOutputLog->setChecked(ui->dockWidgetOutputLog->isVisible());

        m_bIsInitialized = true;
    }
}

/*static*/ void MainWindow::OpenItem(Item *pItem)
{
    sm_pInstance->ui->renderer->RenderItem(pItem);
    
    if(pItem->GetType() != ITEM_Project)
        sm_pInstance->ui->explorer->SelectItem(pItem);
}

/*static*/ void MainWindow::CloseItem(Item *pItem)
{
    // TODO: Ask to save file if changes have been made
    sm_pInstance->ui->renderer->CloseItem(pItem);
}

/*static*/ void MainWindow::SetSelectedProj(ItemProject *pProj)
{
    if(sm_pInstance->m_pCurSelectedProj == pProj)
        return;
        
    sm_pInstance->m_pCurSelectedProj = pProj;
    if(sm_pInstance->m_pCurSelectedProj)
    {
        sm_pInstance->ui->renderer->GetRenderer()->Reload(sm_pInstance->m_pCurSelectedProj->GetPath(HY_DATA_DIR));
        
        sm_pInstance->ui->dockWidgetAtlas->setWidget(sm_pInstance->m_pCurSelectedProj->GetAtlasManager());
        sm_pInstance->m_pCurSelectedProj->GetAtlasManager()->show();
    }
    else
    {
        sm_pInstance->ui->dockWidgetAtlas->setWidget(NULL);
    }
}

/*static*/ void MainWindow::ReloadItems(QStringList &sPaths, bool bRefreshAssets)
{
    sm_pInstance->ui->renderer->GetRenderer()->Reload(sPaths, bRefreshAssets);
    sm_pInstance->m_pDebugConnection->WriteReloadPacket(sPaths);
}

void MainWindow::on_actionNewProject_triggered()
{
    DlgNewProject *pDlg = new DlgNewProject(QDir::current().path(), this);
    if(pDlg->exec())
    {
        QString sProjDirPath = pDlg->GetProjPath();

        ui->explorer->AddItem(ITEM_Project, sProjDirPath, true);
    }
    delete pDlg;
}

void MainWindow::on_actionOpenProject_triggered()
{
//    DlgSetEngineLocation *pDlg = new DlgSetEngineLocation(this);
//    if(pDlg->exec() == QDialog::Accepted)
//    {
//        ui->explorer->AddItem(ITEM_Project, pDlg->SelectedDir(), true);
//    }
//    delete pDlg;
}

void MainWindow::on_actionCloseProject_triggered()
{
    ui->explorer->RemoveItem(ui->explorer->GetCurProjSelected());
}

void MainWindow::on_actionNewAudio_triggered()
{
    NewItem(ITEM_Audio);
}

void MainWindow::on_actionNewParticle_triggered()
{
    NewItem(ITEM_Particles);
}

void MainWindow::on_actionNewSprite_triggered()
{
    NewItem(ITEM_Sprite);
}

void MainWindow::on_actionNewFont_triggered()
{
    NewItem(ITEM_Font);
}

void MainWindow::NewItem(eItemType eItem)
{
    QString sProjPath = ui->explorer->GetCurProjSelected()->GetPath();
    QString sSpritePath = sProjPath % HYGUIPATH_RelDataDir % HyGlobal::ItemName(eItem) % "/";
            
    DlgNewItem *pDlg = new DlgNewItem(sSpritePath, eItem, this);
    if(pDlg->exec())
    {
        QString sPath = QDir::cleanPath(sSpritePath % pDlg->GetPrefix() % "/" % pDlg->GetName() % HyGlobal::ItemExt(eItem));
        ui->explorer->AddItem(eItem, sPath, true);
    }
    delete pDlg;
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    SaveSettings();
    QMainWindow::closeEvent(event);
}

void MainWindow::SaveSettings()
{
    m_Settings.beginGroup("MainWindow");
    {
        m_Settings.setValue("geometry", saveGeometry());
        m_Settings.setValue("windowState", saveState());
    }
    m_Settings.endGroup();

    m_Settings.beginGroup("OpenData");
    {
        m_Settings.setValue("openProjs", QVariant(ui->explorer->GetOpenProjectPaths()));
    }
    m_Settings.endGroup();
}

void MainWindow::on_actionViewExplorer_triggered()
{
    ui->dockWidgetExplorer->setHidden(!ui->dockWidgetExplorer->isHidden());
}

void MainWindow::on_actionViewAtlasManager_triggered()
{
    ui->dockWidgetAtlas->setHidden(!ui->dockWidgetAtlas->isHidden());
}

void MainWindow::on_actionViewOutputLog_triggered()
{
    ui->dockWidgetOutputLog->setHidden(!ui->dockWidgetOutputLog->isHidden());
}

void MainWindow::on_actionConnect_triggered()
{
//    // Network initialization
//    m_pTcpServer = new QTcpServer(this);
//    connect(m_pTcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
//    if(!m_pTcpServer->listen(QHostAddress::LocalHost, HY_TCP_PORT))
//        HYLOG("Cannot start TCP server", LOGTYPE_Error)
//    else
//        HYLOG("TCP server initialized", LOGTYPE_Normal);

    m_pDebugConnection->Connect();
}
