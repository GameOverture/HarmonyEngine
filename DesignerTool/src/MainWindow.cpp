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

#include "WidgetExplorer.h"
#include "WidgetAtlasManager.h"
#include "WidgetAudioManager.h"

#include "ItemSprite.h"

#include "HyGlobal.h"

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
                                            m_pCurSelectedProj(NULL),
                                            m_pCurRenderer(NULL),
                                            m_pCurEditMenu(NULL)
{
    ui->setupUi(this);
    sm_pInstance = this;

    while(ui->stackedTabWidgets->count())
        ui->stackedTabWidgets->removeWidget(ui->stackedTabWidgets->currentWidget());

    m_pCurRenderer = new HyGuiRenderer(NULL, this);
    ui->centralVerticalLayout->addWidget(m_pCurRenderer);

    m_pLoadingSpinner = new WaitingSpinnerWidget(this);
    m_pLoadingSpinner->setRoundness(50.0);
    m_pLoadingSpinner->setMinimumTrailOpacity(15.0);
    m_pLoadingSpinner->setTrailFadePercentage(70.0);
    m_pLoadingSpinner->setNumberOfLines(20);
    m_pLoadingSpinner->setLineLength(35);
    m_pLoadingSpinner->setLineWidth(15);
    m_pLoadingSpinner->setInnerRadius(65);
    m_pLoadingSpinner->setRevolutionsPerSecond(3.0);
    m_pLoadingSpinner->setColor(QColor(25, 255, 25));
    
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
            ui->explorer->AddItemProject(sListOpenProjs[i], false);
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

/*static*/ void MainWindow::OpenItem(ItemWidget *pItem)
{
    if(pItem == NULL || pItem->GetType() == ITEM_Project)
        return;
    
    ItemProject *pItemProj = sm_pInstance->ui->explorer->GetCurProjSelected();
    QTabBar *pTabBar = pItemProj->GetTabBar();

    if(pItem->IsLoaded() == false)
    {
        pItem->Load(*pItemProj);

        pTabBar->blockSignals(true);
        int iIndex = pTabBar->addTab(pItem->GetIcon(), pItem->GetName(false));
        QVariant v;
        v.setValue(pItem);
        pTabBar->setTabData(iIndex, v);
        pTabBar->setCurrentIndex(iIndex);
        pTabBar->blockSignals(false);
    }
    else
    {
        for(int i = 0; i < pTabBar->count(); ++i)
        {
            if(pTabBar->tabData(i).value<ItemWidget *>() == pItem)
            {
                pTabBar->blockSignals(true);
                pTabBar->setCurrentIndex(i);
                pTabBar->blockSignals(false);
                break;
            }
        }
    }
    
    // Hide everything
    for(int i = 0; i < pTabBar->count(); ++i)
        pTabBar->tabData(i).value<ItemWidget *>()->DrawHide(*pItemProj);

    // Then show
    pItem->DrawShow(*pItemProj);
    
    sm_pInstance->ui->explorer->SelectItem(pItem);

    // Setup the item properties docking window to be the current item
    QString sWindowTitle = HyGlobal::ItemName(pItem->GetType()) % " Properties";
    
    sm_pInstance->ui->actionViewProperties->setVisible(true);
    sm_pInstance->ui->actionViewProperties->setText(sWindowTitle);

    sm_pInstance->ui->dockWidgetCurrentItem->setVisible(true);
    sm_pInstance->ui->dockWidgetCurrentItem->setWindowTitle(sWindowTitle);
    sm_pInstance->ui->dockWidgetCurrentItem->setWidget(pItem->GetWidget());

    // Remove any "Edit" menu, and replace it with the current item's "Edit" menu
    if(sm_pInstance->m_pCurEditMenu)
        sm_pInstance->ui->menuBar->removeAction(sm_pInstance->m_pCurEditMenu->menuAction());

    sm_pInstance->m_pCurEditMenu = pItem->GetEditMenu();
    if(sm_pInstance->m_pCurEditMenu)
        sm_pInstance->ui->menuBar->insertMenu(sm_pInstance->ui->menu_View->menuAction(), sm_pInstance->m_pCurEditMenu);
    
    // Remove any item specific actions in the main toolbar, and add the current item's desired actions to be shown to the main toolbar
    for(int i = 0; i < sm_pInstance->m_ToolBarItemActionsList.count(); ++i)
        sm_pInstance->ui->mainToolBar->removeAction(sm_pInstance->m_ToolBarItemActionsList[i]);
    
    sm_pInstance->m_ToolBarItemActionsList.clear();
    sm_pInstance->m_ToolBarItemActionsList = pItem->GetActionsForToolBar();
    
    sm_pInstance->ui->mainToolBar->addActions(sm_pInstance->m_ToolBarItemActionsList);
}

/*static*/ void MainWindow::CloseItem(ItemWidget *pItem)
{
    if(pItem == NULL || pItem->GetType() == ITEM_Project)
        return;

    if(pItem->IsSaveClean() == false)
    {
        int iDlgReturn = QMessageBox::question(sm_pInstance, "Save Changes", pItem->GetName(true) % " has unsaved changes. Do you want to save before closing?", QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if(iDlgReturn == QMessageBox::Save)
            pItem->Save();
        else if(iDlgReturn == QMessageBox::Discard)
            pItem->DiscardChanges();
        else if(iDlgReturn == QMessageBox::Cancel)
            return;
    }

    // If this is the item that is currently being shown, unhook all its actions and widget
    if(sm_pInstance->ui->dockWidgetCurrentItem->widget() == pItem->GetWidget())
    {
        sm_pInstance->ui->menuBar->removeAction(sm_pInstance->m_pCurEditMenu->menuAction());
        sm_pInstance->m_pCurEditMenu = NULL;
        
        sm_pInstance->ui->dockWidgetCurrentItem->hide();
        
        for(int i = 0; i < sm_pInstance->m_ToolBarItemActionsList.count(); ++i)
            sm_pInstance->ui->mainToolBar->removeAction(sm_pInstance->m_ToolBarItemActionsList[i]);
        
        sm_pInstance->m_ToolBarItemActionsList.clear();
    }
    
    ItemProject *pItemProj = sm_pInstance->ui->explorer->GetCurProjSelected();
    QTabBar *pTabBar = pItemProj->GetTabBar();
    for(int i = 0; i < pTabBar->count(); ++i)
    {
        if(pTabBar->tabData(i).value<ItemWidget *>() == pItem)
        {
            pTabBar->removeTab(i);
            break;
        }
    }
    
    pItem->Unload(*pItemProj);
}

/*static*/ void MainWindow::SetSelectedProj(ItemProject *pProj)
{
    if(sm_pInstance->m_pCurSelectedProj == pProj)
        return;

    bool bSelfSpinner = false;
    if(sm_pInstance->m_pLoadingSpinner->isSpinning() == false)
    {
        sm_pInstance->m_pLoadingSpinner->start();
        bSelfSpinner = true;
    }

    sm_pInstance->m_pCurSelectedProj = pProj;
    if(sm_pInstance->m_pCurSelectedProj)
    {
        sm_pInstance->m_pCurSelectedProj->Reset();

        // Insert the project's TabBar
        bool bTabsFound = false;
        for(int i = 0; i < sm_pInstance->ui->stackedTabWidgets->count(); ++i)
        {
            if(sm_pInstance->ui->stackedTabWidgets->widget(i) == pProj->GetTabBar())
            {
                sm_pInstance->ui->stackedTabWidgets->setCurrentIndex(i);
                bTabsFound = true;
            }
        }
        if(bTabsFound == false)
        {
            sm_pInstance->ui->stackedTabWidgets->addWidget(pProj->GetTabBar());
            sm_pInstance->ui->stackedTabWidgets->setCurrentWidget(pProj->GetTabBar());
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
    }

    // Swap the harmony engine renderers
    delete sm_pInstance->m_pCurRenderer;
    sm_pInstance->m_pCurRenderer = new HyGuiRenderer(pProj, sm_pInstance);
    sm_pInstance->ui->centralVerticalLayout->addWidget(sm_pInstance->m_pCurRenderer);

    ItemProject *pTest = sm_pInstance->m_pCurSelectedProj;

    if(sm_pInstance->m_pCurSelectedProj)
    {
        sm_pInstance->ui->dockWidgetAtlas->setWidget(&sm_pInstance->m_pCurSelectedProj->GetAtlasManager());
        sm_pInstance->ui->dockWidgetAtlas->widget()->show();
        
        sm_pInstance->ui->dockWidgetAudio->setWidget(&sm_pInstance->m_pCurSelectedProj->GetAudioManager());
        sm_pInstance->ui->dockWidgetAudio->widget()->show();
    }
    else
    {
        sm_pInstance->ui->dockWidgetAtlas->setWidget(NULL);
        sm_pInstance->ui->dockWidgetAudio->setWidget(NULL);
    }

    if(bSelfSpinner)
        sm_pInstance->m_pLoadingSpinner->stop();
}

/*static*/ void MainWindow::ReloadHarmony()
{
    delete sm_pInstance->m_pCurRenderer;
    sm_pInstance->m_pCurRenderer = NULL;
    
    ItemProject *pCurItemProj = sm_pInstance->m_pCurSelectedProj;
    sm_pInstance->m_pCurSelectedProj = NULL;    // Set m_pCurSelectedProj to 'NULL' so SetSelectedProj() doesn't imediately return
    
    SetSelectedProj(pCurItemProj);
}

/*static*/ void MainWindow::LoadSpinner(bool bEnabled)
{
    if(bEnabled)
        sm_pInstance->m_pLoadingSpinner->start();
    else
        sm_pInstance->m_pLoadingSpinner->stop();
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

        ui->explorer->AddItemProject(pDlg->GetProjFilePath(), true);
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
        ui->explorer->AddItemProject(pDlg->selectedFiles()[0], true);
    }
    delete pDlg;
}

void MainWindow::on_actionCloseProject_triggered()
{
    delete m_pCurRenderer;
    m_pCurRenderer = 0;

    ui->explorer->GetCurProjSelected()->SaveUserData();
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
    DlgNewItem *pDlg = new DlgNewItem(m_pCurSelectedProj, eItem, this);
    if(pDlg->exec())
        ui->explorer->AddItem(eItem, pDlg->GetPrefix(), pDlg->GetName(), true);

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
