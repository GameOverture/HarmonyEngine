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
    
    // Restore workspace
    HyGuiLog("Recovering previously opened session...", LOGTYPE_Normal);
    m_Settings.beginGroup("MainWindow");
    {
        restoreGeometry(m_Settings.value("geometry").toByteArray());
        restoreState(m_Settings.value("windowState").toByteArray());
    }
    m_Settings.endGroup();
    
    // Start with no open items. "OpenData" below will make this docking window (and Action menu item) visible if any items are to be opened
    ui->dockWidgetCurrentItem->hide();
    ui->actionViewProperties->setVisible(false);

    ui->actionViewAtlasManager->setChecked(!ui->dockWidgetAtlas->isHidden());
    ui->actionViewExplorer->setChecked(!ui->dockWidgetExplorer->isHidden());
    ui->actionViewOutputLog->setChecked(!ui->dockWidgetOutputLog->isHidden());

    m_Settings.beginGroup("OpenData");
    {
        QStringList sListOpenProjs = m_Settings.value("openProjs").toStringList();
        foreach(QString sProjPath, sListOpenProjs)
        {
            ui->explorer->AddItemProject(sProjPath);
        }
        
        //QStringList sListOpenItems = m_Settings.value("openItems").toStringList();
        //sListOpenItems.sort();  // This sort should organize each open item by project to reduce unloading/loading projects
        //foreach(QString sItemPath, sListOpenItems)
        //{
        //    Item *pItem = ui->explorer->GetItemByPath(sItemPath);
        //    if(pItem)
        //        OpenItem(pItem);
        //}
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

    if(sm_pInstance->m_pCurSelectedProj)
    {
        sm_pInstance->ui->dockWidgetAtlas->setWidget(&sm_pInstance->m_pCurSelectedProj->GetAtlasManager());
        sm_pInstance->ui->dockWidgetAtlas->widget()->show();
    }
    else
    {
        sm_pInstance->ui->dockWidgetAtlas->setWidget(NULL);
    }
}

/*static*/ void MainWindow::ReloadHarmony()
{
    ItemProject *pCurItemProj = sm_pInstance->m_pCurSelectedProj;
    sm_pInstance->m_pCurSelectedProj = NULL;    // Set m_pCurSelectedProj to 'NULL' so SetSelectedProj() doesn't imediately return
    
    SetSelectedProj(pCurItemProj);
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

        ui->explorer->AddItemProject(pDlg->GetProjFilePath());
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
    eItemType eItemDir = HyGlobal::GetCorrespondingDirItem(eItem);
    
    QString sNewItemPath = ui->explorer->GetCurProjSelected()->GetAssetsAbsPath() % HyGlobal::ItemName(eItemDir) % "/";
            
    DlgNewItem *pDlg = new DlgNewItem(sNewItemPath, eItem, this);
    if(pDlg->exec())
    {
        QString sPath = QDir::cleanPath(sNewItemPath % pDlg->GetPrefix() % "/" % pDlg->GetName() % HyGlobal::ItemExt(eItem));
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
        
        //m_Settings.setValue("openItems", QVariant(ui->renderer->GetOpenItemPaths()));
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
