/**************************************************************************
 *	MainWindow.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "DlgSetEngineLocation.h"
#include "DlgNewProject.h"
#include "DlgNewItem.h"
#include "DlgInputName.h"
#include "DlgProjectSettings.h"

#include "ExplorerWidget.h"
#include "AtlasWidget.h"
#include "AudioWidgetManager.h"

#include "HyGuiRenderer.h"
#include "HyGuiGlobal.h"

#include <QFileDialog>
#include <QShowEvent>
#include <QStringBuilder>
#include <QVBoxLayout>
#include <QTcpSocket>
#include <QMessageBox>
#include <QDesktopServices>

#include <QLabel>

#define HYGUIVERSION_STRING "v0.0.1"

/*static*/ MainWindow * MainWindow::sm_pInstance = NULL;

MainWindow::MainWindow(QWidget *parent) :   QMainWindow(parent),
                                            ui(new Ui::MainWindow),
                                            m_Settings("Overture Games", "Harmony Designer Tool"),
                                            m_bIsInitialized(false),
                                            m_pCurSelectedProj(nullptr),
                                            m_pCurRenderer(nullptr)
{
    ui->setupUi(this);
    sm_pInstance = this;

    while(ui->stackedTabWidgets->count())
        ui->stackedTabWidgets->removeWidget(ui->stackedTabWidgets->currentWidget());

    m_pCurRenderer = new HyGuiRenderer(nullptr, this);
    ui->centralVerticalLayout->addWidget(m_pCurRenderer);

    m_pLoadingSpinners[0 /*MDI_MainWindow*/] = new WaitingSpinnerWidget(this, true, true);
    m_pLoadingSpinners[1 /*MDI_Explorer*/] = new WaitingSpinnerWidget(ui->dockWidgetExplorer, true, true);
    m_pLoadingSpinners[2 /*MDI_AtlasManager*/] = new WaitingSpinnerWidget(ui->dockWidgetAtlas, true, true);
    m_pLoadingSpinners[3 /*MDI_AudioManager*/] = new WaitingSpinnerWidget(ui->dockWidgetAudio, true, true);
    m_pLoadingSpinners[4 /*MDI_ItemProperties*/] = new WaitingSpinnerWidget(ui->dockWidgetCurrentItem, true, true);
    m_pLoadingSpinners[5 /*MDI_Output*/] = new WaitingSpinnerWidget(ui->dockWidgetOutputLog, true, true);

    for(uint i = 0; i < NUM_MDI; ++i)
    {
        m_pLoadingSpinners[i]->setRoundness(50.0);
        m_pLoadingSpinners[i]->setMinimumTrailOpacity(15.0);
        m_pLoadingSpinners[i]->setTrailFadePercentage(70.0);
        m_pLoadingSpinners[i]->setNumberOfLines(20);
        m_pLoadingSpinners[i]->setLineLength(24);
        m_pLoadingSpinners[i]->setLineWidth(4);
        m_pLoadingSpinners[i]->setInnerRadius(12);
        m_pLoadingSpinners[i]->setRevolutionsPerSecond(1.5);
        m_pLoadingSpinners[i]->setColor(QColor(25, 255, 25));

        m_uiLoadingSpinnerRefCounts[i] = 0;
    }

    //StartLoading(MDI_Explorer | MDI_AtlasManager | MDI_AudioManager | MDI_ItemProperties);
    
    SetSelectedProj(NULL);

    HyGuiLog("Harmony Designer Tool " % QString(HYGUIVERSION_STRING), LOGTYPE_Title);
    HyGuiLog("Initializing...", LOGTYPE_Normal);
    
    ui->actionCloseProject->setEnabled(false);
    ui->actionNewSprite->setEnabled(false);
    ui->actionNewFont->setEnabled(false);
    ui->actionNewParticle->setEnabled(false);
    ui->actionNewAudio->setEnabled(false);
    ui->actionSave->setEnabled(false);
    ui->actionSaveAll->setEnabled(false);
    ui->actionLaunchIDE->setEnabled(false);
    
    m_pCurSaveAction = ui->actionSave;
    m_pCurSaveAllAction = ui->actionSaveAll;
    
    // Link the actions to their proper widgets
    ui->explorer->addAction(ui->actionProjectSettings);
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
    ui->explorer->addAction(ui->actionLaunchIDE);

    m_pDebugConnection = new HyGuiDebugger(*ui->actionConnect, this);
    
    ui->dockWidgetAtlas->hide();
    ui->dockWidgetCurrentItem->hide();
    
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
        
        m_sEngineLocation = engineDir.absolutePath();
        m_sEngineLocation += "/";
    }
    m_Settings.endGroup();
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    ui->actionAudioManager->setChecked(!ui->dockWidgetAudio->isHidden());
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Start with no open items. "OpenData" below will make this docking window (and Action menu item) visible if any items are to be opened
    ui->dockWidgetCurrentItem->hide();
    ui->actionViewProperties->setVisible(false);

    m_Settings.beginGroup("OpenData");
    {
        QStringList sListOpenProjs = m_Settings.value("openProjs").toStringList();
        for(int i = 0; i < sListOpenProjs.size(); ++i)
            ui->explorer->AddItemProject(sListOpenProjs[i]);
    }
    m_Settings.endGroup();

    m_Settings.beginGroup("Misc");
    {
        m_sDefaultProjectLocation = m_Settings.value("defaultProjectLocation").toString();
        QDir defaultProjDir(m_sDefaultProjectLocation);
        if(m_sDefaultProjectLocation.isEmpty() || defaultProjDir.exists() == false)
            m_sDefaultProjectLocation = QDir::current().path();
    }
    m_Settings.endGroup();

    // Restore opened items/tabs

    // Append version to window title
    setWindowTitle(windowTitle() % " " % HYGUIVERSION_STRING);


    QLabel *pStatusLbl = new QLabel;
    statusBar()->showMessage("Ready");

    QPixmap *pPixmap = new QPixmap(":/icons16x16/smiley-sad.gif");
    QLabel *pSvnStatusIcon = new QLabel;
    pSvnStatusIcon->setPixmap(*pPixmap);
    statusBar()->addPermanentWidget(pSvnStatusIcon);

    QLabel *pSvnLoginLabel = new QLabel;
    pSvnLoginLabel->setText("SVN Not Detected");
    statusBar()->addPermanentWidget(pSvnLoginLabel);

    //setStyleSheet("background-color:black;");

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
        ui->actionViewProperties->setChecked(ui->dockWidgetCurrentItem->isVisible());
        ui->actionViewOutputLog->setChecked(ui->dockWidgetOutputLog->isVisible());

        m_bIsInitialized = true;
    }
}

/*static*/ MainWindow *MainWindow::GetInstance()
{
    return sm_pInstance;
}

/*static*/ QString MainWindow::EngineLocation()
{
    return sm_pInstance->m_sEngineLocation;
}

/*static*/ void MainWindow::OpenItem(ProjectItem *pItem)
{
    if(pItem == nullptr || pItem->GetType() == ITEM_Project)
        return;
    
    Project *pItemProj = sm_pInstance->ui->explorer->GetCurProjSelected();
    pItemProj->OpenItem(pItem);
    
    sm_pInstance->ui->explorer->SelectItem(pItem);

    // Setup the item properties docking window to be the current item
    QString sWindowTitle = HyGlobal::ItemName(pItem->GetType()) % " Properties";
    
    sm_pInstance->ui->actionViewProperties->setVisible(true);
    sm_pInstance->ui->actionViewProperties->setText(sWindowTitle);

    sm_pInstance->ui->dockWidgetCurrentItem->show();
    sm_pInstance->ui->dockWidgetCurrentItem->setWindowTitle(sWindowTitle);
    sm_pInstance->ui->dockWidgetCurrentItem->setWidget(pItem->GetWidget());

    // Remove all the actions in the "Edit" menu, and replace it with the current item's actions
    QList<QAction *> editActionList = sm_pInstance->ui->menu_Edit->actions();
    for(uint i = 0; i < editActionList.size(); ++i)
        sm_pInstance->ui->mainToolBar->removeAction(editActionList[i]);

    sm_pInstance->ui->menu_Edit->clear();

    pItem->GiveMenuActions(sm_pInstance->ui->menu_Edit);
    sm_pInstance->ui->mainToolBar->addActions(sm_pInstance->ui->menu_Edit->actions());
}

/*static*/ void MainWindow::CloseItem(ProjectItem *pItem)
{
    if(pItem == nullptr || pItem->GetType() == ITEM_Project)
        return;

    // If this is the item that is currently being shown, unhook all its actions and widget
    if(sm_pInstance->ui->dockWidgetCurrentItem->widget() == pItem->GetWidget())
    {
        pItem->BlockAllWidgetSignals(true);

        sm_pInstance->ui->dockWidgetCurrentItem->hide();

        QList<QAction *> editActionList = sm_pInstance->ui->menu_Edit->actions();
        for(uint i = 0; i < editActionList.size(); ++i)
            sm_pInstance->ui->mainToolBar->removeAction(editActionList[i]);

        sm_pInstance->ui->menu_Edit->clear();
        
        pItem->BlockAllWidgetSignals(false);
    }
}

/*static*/ void MainWindow::SetSelectedProj(Project *pProj)
{
    if(sm_pInstance->m_pCurSelectedProj == pProj)
        return;

    sm_pInstance->m_pCurSelectedProj = pProj;

    // Swap the harmony engine renderers
    delete sm_pInstance->m_pCurRenderer;
    sm_pInstance->m_pCurRenderer = new HyGuiRenderer(sm_pInstance->m_pCurSelectedProj, sm_pInstance);
    sm_pInstance->ui->centralVerticalLayout->addWidget(sm_pInstance->m_pCurRenderer);

    // Below will be set when HyEngine is fully loaded
    sm_pInstance->ui->dockWidgetAtlas->setWidget(nullptr);
    sm_pInstance->ui->dockWidgetAudio->setWidget(nullptr);
}

/*static*/ void MainWindow::SetSelectedProjWidgets(Project *pProj)
{
    if(sm_pInstance->m_pCurSelectedProj != pProj)
        HyGuiLog("MainWindow::SetSelectedProjWidgets was passed a project that wasn't the currently selected one", LOGTYPE_Error);
    
    // Insert the project's TabBar
    bool bTabsFound = false;
    for(int i = 0; i < sm_pInstance->ui->stackedTabWidgets->count(); ++i)
    {
        if(sm_pInstance->ui->stackedTabWidgets->widget(i) == sm_pInstance->m_pCurSelectedProj->GetTabBar())
        {
            sm_pInstance->ui->stackedTabWidgets->setCurrentIndex(i);
            bTabsFound = true;
        }
    }
    if(bTabsFound == false)
    {
        sm_pInstance->ui->stackedTabWidgets->addWidget(sm_pInstance->m_pCurSelectedProj->GetTabBar());
        sm_pInstance->ui->stackedTabWidgets->setCurrentWidget(sm_pInstance->m_pCurSelectedProj->GetTabBar());
        sm_pInstance->m_pCurSelectedProj->GetTabBar()->setParent(sm_pInstance->ui->stackedTabWidgets);
    }
    
    // Replace the save actions in the 'File' menu
    QList<QAction *> projSaveActionList = sm_pInstance->m_pCurSelectedProj->GetSaveActions();
    if(sm_pInstance->m_pCurSaveAction != projSaveActionList[0])
    {
        sm_pInstance->ui->menu_File->insertActions(sm_pInstance->m_pCurSaveAction, projSaveActionList);
        sm_pInstance->ui->menu_File->removeAction(sm_pInstance->m_pCurSaveAction);
        sm_pInstance->ui->menu_File->removeAction(sm_pInstance->m_pCurSaveAllAction);
        sm_pInstance->m_pCurSaveAction = projSaveActionList[0];
        sm_pInstance->m_pCurSaveAllAction = projSaveActionList[1];
    }
    
    // Project manager widgets
    sm_pInstance->ui->dockWidgetAtlas->setWidget(sm_pInstance->m_pCurSelectedProj->GetAtlasWidget());
    sm_pInstance->ui->dockWidgetAtlas->widget()->show();
    
    sm_pInstance->ui->dockWidgetAudio->setWidget(sm_pInstance->m_pCurSelectedProj->GetAudioWidget());
    sm_pInstance->ui->dockWidgetAudio->widget()->show();
}

/*static*/ void MainWindow::ReloadHarmony()
{
    delete sm_pInstance->m_pCurRenderer;
    sm_pInstance->m_pCurRenderer = nullptr;
    
    Project *pCurItemProj = sm_pInstance->m_pCurSelectedProj;
    sm_pInstance->m_pCurSelectedProj = nullptr;    // Set m_pCurSelectedProj to 'nullptr' so SetSelectedProj() doesn't imediately return
    
    SetSelectedProj(pCurItemProj);
}

/*static*/ void MainWindow::StartLoading(uint uiAreaFlags)
{
    for(uint i = 0; i < NUM_MDI; ++i)
    {
        if((uiAreaFlags & (1 << i)) != 0)
            sm_pInstance->m_pLoadingSpinners[i]->start();
    }
}

/*static*/ void MainWindow::StopLoading(uint uiAreaFlags)
{
    for(uint i = 0; i < NUM_MDI; ++i)
    {
        if((uiAreaFlags & (1 << i)) != 0)
            sm_pInstance->m_pLoadingSpinners[i]->stop();
    }
}

/*static*/ HyRendererInterop *MainWindow::GetCurrentRenderer()
{
    if(sm_pInstance->m_pCurRenderer)
        sm_pInstance->m_pCurRenderer->GetHarmonyRenderer();
    else
        return NULL;
}

void MainWindow::on_actionNewProject_triggered()
{
    DlgNewProject *pDlg = new DlgNewProject(m_sDefaultProjectLocation, this);
    if(pDlg->exec() == QDialog::Accepted)
    {
        if(pDlg->IsCreatingGameDir())
        {
            QDir defaultProjDir(pDlg->GetProjDirPath());
            defaultProjDir.cdUp();
            m_sDefaultProjectLocation = defaultProjDir.absolutePath();
        }

        ui->explorer->SelectItem(ui->explorer->AddItemProject(pDlg->GetProjFilePath()));
        ui->explorer->GetCurProjSelected()->GetAtlasModel().RepackAll(0, true);
    }
    delete pDlg;
}

void MainWindow::on_actionOpenProject_triggered()
{
    //DlgSetEngineLocation *pDlg = new DlgSetEngineLocation(this);
    QFileDialog *pDlg = new QFileDialog();
    pDlg->setNameFilter(tr("Harmony Project File (*.hyproj)"));
    pDlg->setModal(true);

    if(pDlg->exec() == QDialog::Accepted)
    {
        ui->explorer->AddItemProject(pDlg->selectedFiles()[0]);
    }
    delete pDlg;
}

void MainWindow::on_actionCloseProject_triggered()
{
    delete m_pCurRenderer;
    m_pCurRenderer = new HyGuiRenderer(nullptr, this);
    ui->centralVerticalLayout->addWidget(m_pCurRenderer);

    ui->explorer->GetCurProjSelected()->SaveUserData();
    ui->explorer->RemoveItem(ui->explorer->GetCurProjSelected());

    m_pCurSelectedProj = nullptr;
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
    DlgNewItem *pDlg = new DlgNewItem(m_pCurSelectedProj, eItem, this);
    if(pDlg->exec())
        ui->explorer->AddItem(eItem, pDlg->GetPrefix(), pDlg->GetName(), true);

    delete pDlg;
}

void MainWindow::closeEvent(QCloseEvent *pEvent)
{
    // This will ensure that the user has a chance to save all unsaved open documents, or cancel which will abort the close
    if(m_pCurSelectedProj && m_pCurSelectedProj->CloseAllTabs() == false)
    {
        pEvent->ignore();
        return;
    }
    
    SaveSettings();
    QMainWindow::closeEvent(pEvent);
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

    m_Settings.beginGroup("Misc");
    {
        m_Settings.setValue("defaultProjectLocation", QVariant(m_sDefaultProjectLocation));
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

void MainWindow::on_actionViewProperties_triggered()
{
    ui->dockWidgetCurrentItem->setHidden(!ui->dockWidgetCurrentItem->isHidden());
}

void MainWindow::on_actionSave_triggered()
{
    
}

void MainWindow::on_actionSaveAll_triggered()
{
    
}

void MainWindow::on_actionLaunchIDE_triggered()
{
    QStringList sFilterList;

#if defined(Q_OS_WIN)
    sFilterList << "*.sln";
#endif
    
    QDir srcDir(ui->explorer->GetCurProjSelected()->GetSourceAbsPath());
    srcDir.setNameFilters(sFilterList);
    QFileInfoList ideFileInfoList = srcDir.entryInfoList();
    
    if(ideFileInfoList.empty())
    {
        HyGuiLog("Could not find appropriate IDE file to launch", LOGTYPE_Error);
        return;
    }

#if defined(Q_OS_WIN)
    bool bUseVs2015 = false;
    bool bUseVs2013 = false;

    QSettings windowsRegEntryVS2015("HKEY_CLASSES_ROOT\\VisualStudio.DTE.14.0", QSettings::NativeFormat);
    if(windowsRegEntryVS2015.childKeys().empty() == false)
        bUseVs2015 = true;

    QSettings windowsRegEntryVS2013("HKEY_CLASSES_ROOT\\VisualStudio.DTE.12.0", QSettings::NativeFormat);
    if(windowsRegEntryVS2013.childKeys().empty() == false)
        bUseVs2013 = true;

    // Use the newer version
    if(bUseVs2013 && bUseVs2015)
        bUseVs2013 = false;

    for(int i = 0; i < ideFileInfoList.size(); ++i)
    {
        QFile file(ideFileInfoList[i].absoluteFilePath());
        if(file.open(QFile::ReadOnly))
        {
            QTextStream in(&file);
            QString line;
            do
            {
                line = in.readLine();
                if(line.contains("# Visual Studio 14", Qt::CaseSensitive))
                {
                    if(bUseVs2015)
                    {
                        file.close();
                        QDesktopServices::openUrl(QUrl(ideFileInfoList[i].absoluteFilePath()));
                        return;
                    }
                    else
                        break;
                }

                if(line.contains("# Visual Studio 2013", Qt::CaseSensitive))
                {
                    if(bUseVs2013)
                    {
                        file.close();
                        QDesktopServices::openUrl(QUrl(ideFileInfoList[i].absoluteFilePath()));
                        return;
                    }
                    else
                        break;
                }
            } while (!line.isNull());

            file.close();
        }
    }
#endif

    QDesktopServices::openUrl(QUrl(ideFileInfoList[0].absoluteFilePath()));
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, HyDesignerToolName, "Harmony Engine and Designer Tool\nCopyright (c) 2016 Jason Knobler");
}

void MainWindow::on_actionAudioManager_triggered()
{
    ui->dockWidgetAudio->setHidden(!ui->dockWidgetAudio->isHidden());
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionProjectSettings_triggered()
{
    if(ui->explorer->GetCurProjSelected() == nullptr)
        return;

    DlgProjectSettings *pDlg = new DlgProjectSettings(*ui->explorer->GetCurProjSelected(), this);
    if(pDlg->exec() == QDialog::Accepted)
    {
        pDlg->SaveSettings();
    }

    delete pDlg;
}
