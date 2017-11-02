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

#include "Global.h"
#include "Harmony.h"
#include "Project.h"
#include "DlgSetEngineLocation.h"
#include "DlgNewProject.h"
#include "DlgNewItem.h"
#include "DlgInputName.h"
#include "DlgProjectSettings.h"
#include "ExplorerWidget.h"
#include "AtlasWidget.h"
#include "AudioWidgetManager.h"

#include <QFileDialog>
#include <QShowEvent>
#include <QStringBuilder>
#include <QVBoxLayout>
#include <QTcpSocket>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDate>
#include <QLabel>
#include <QShortcut>

/*static*/ MainWindow * MainWindow::sm_pInstance = NULL;

///*virtual*/ void SwitchRendererThread::run() /*override*/
//{
//    while(m_pCurrentRenderer && m_pCurrentRenderer->IsLoading())
//    { }

//    Q_EMIT SwitchIsReady(m_pCurrentRenderer);
//}

MainWindow::MainWindow(QWidget *parent) :   QMainWindow(parent),
                                            ui(new Ui::MainWindow),
                                            m_Settings("Overture Games", "Harmony Designer Tool"),
                                            m_LoadingSpinner(this)
{
    ui->setupUi(this);
    sm_pInstance = this;

    while(ui->stackedTabWidgets->count())
        ui->stackedTabWidgets->removeWidget(ui->stackedTabWidgets->currentWidget());

    connect(ui->menu_View, SIGNAL(aboutToShow), this, SLOT(on_menu_View_aboutToShow));

    m_Harmony = new Harmony(this);

    m_LoadingSpinner.setRoundness(50.0);
    m_LoadingSpinner.setMinimumTrailOpacity(15.0);
    m_LoadingSpinner.setTrailFadePercentage(70.0);
    m_LoadingSpinner.setNumberOfLines(20);
    m_LoadingSpinner.setLineLength(24);
    m_LoadingSpinner.setLineWidth(4);
    m_LoadingSpinner.setInnerRadius(12);
    m_LoadingSpinner.setRevolutionsPerSecond(1.5);
    m_LoadingSpinner.setColor(QColor(25, 255, 25));

//    m_pCurRenderer = new HyGuiRenderer(nullptr, this);
//    ui->centralVerticalLayout->addWidget(m_pCurRenderer);

    HyGuiLog("Harmony Designer Tool", LOGTYPE_Title);
    HyGuiLog("Initializing...", LOGTYPE_Normal);
    
    ui->actionProjectSettings->setEnabled(false);
    ui->actionCloseProject->setEnabled(false);
    ui->actionNewSprite->setEnabled(false);
    ui->actionNewFont->setEnabled(false);
    ui->actionNewParticle->setEnabled(false);
    ui->actionNewAudio->setEnabled(false);
    ui->actionNewEntity->setEnabled(false);
    ui->actionSave->setEnabled(false);
    ui->actionSaveAll->setEnabled(false);
    ui->actionLaunchIDE->setEnabled(false);
    
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
    ui->explorer->addAction(ui->actionNewEntity);
    ui->explorer->addAction(ui->actionOpenProject);
    ui->explorer->addAction(ui->actionPaste);
    ui->explorer->addAction(ui->actionRemove);
    ui->explorer->addAction(ui->actionRename);
    ui->explorer->addAction(ui->actionLaunchIDE);
    
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
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Start with no open items. "OpenData" below will make this docking window (and Action menu item) visible if any items are to be opened
    ui->dockWidgetCurrentItem->hide();

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

    // TODO: Restore opened items/tabs here
    //

    m_LoadingMsg.setText("Ready");
    statusBar()->addWidget(&m_LoadingMsg);
    statusBar()->addWidget(&m_LoadingBar);

    QPixmap *pPixmap = new QPixmap(":/icons16x16/smiley-sad.gif");
    QLabel *pSvnStatusIcon = new QLabel;
    pSvnStatusIcon->setPixmap(*pPixmap);
    statusBar()->addPermanentWidget(pSvnStatusIcon);

    QLabel *pSvnLoginLabel = new QLabel;
    pSvnLoginLabel->setText("SVN Not Detected");
    statusBar()->addPermanentWidget(pSvnLoginLabel);

    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Tab), this, SLOT(OnCtrlTab()));

    //setStyleSheet("background-color:black;");

    HyGuiLog("Ready to go!", LOGTYPE_Normal);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*static*/ MainWindow *MainWindow::GetInstance()
{
    return sm_pInstance;
}

void MainWindow::SetHarmonyWidget(HarmonyWidget *pWidget)
{
    ui->centralVerticalLayout->addWidget(pWidget);
}

void MainWindow::SetLoading(QString sMsg)
{
    m_LoadingMsg.setText(sMsg);

    if(m_LoadingSpinner.isSpinning() == false)
        m_LoadingSpinner.start();
}

void MainWindow::ClearLoading()
{
    statusBar()->showMessage("Ready");
    m_LoadingSpinner.stop();
}

void MainWindow::SetCurrentProject(Project &newCurrentProjectRef)
{
    // Insert the project's TabBar
    bool bTabsFound = false;
    for(int i = 0; i < ui->stackedTabWidgets->count(); ++i)
    {
        if(ui->stackedTabWidgets->widget(i) == newCurrentProjectRef.GetTabBar())
        {
            ui->stackedTabWidgets->setCurrentIndex(i);
            bTabsFound = true;
        }
    }
    if(bTabsFound == false)
    {
        ui->stackedTabWidgets->addWidget(newCurrentProjectRef.GetTabBar());
        ui->stackedTabWidgets->setCurrentWidget(newCurrentProjectRef.GetTabBar());
        newCurrentProjectRef.GetTabBar()->setParent(ui->stackedTabWidgets);
    }

    // Project manager widgets
    ui->dockWidgetAtlas->setWidget(newCurrentProjectRef.GetAtlasWidget());
    ui->dockWidgetAtlas->widget()->show();

    ui->dockWidgetAudio->setWidget(newCurrentProjectRef.GetAudioWidget());
    ui->dockWidgetAudio->widget()->show();

    ui->centralVerticalLayout->addWidget(Harmony::GetWidget(newCurrentProjectRef));
}

/*static*/ QString MainWindow::EngineSrcLocation()
{
    return sm_pInstance->m_sEngineLocation;
}

/*static*/ void MainWindow::PasteItemSrc(QByteArray sSrc, Project *pProject)
{
    sm_pInstance->ui->explorer->PasteItemSrc(sSrc, pProject);
}

/*static*/ void MainWindow::ApplySaveEnables(bool bCurItemDirty, bool bAnyItemDirty)
{
    sm_pInstance->ui->actionSave->setEnabled(bCurItemDirty);
    sm_pInstance->ui->actionSaveAll->setEnabled(bAnyItemDirty);
}

/*static*/ void MainWindow::OpenItem(ProjectItem *pItem)
{
    if(pItem == nullptr || pItem->GetType() == ITEM_Project)
        return;

    Harmony::GetProject()->OpenTab(pItem);

    sm_pInstance->ui->explorer->SelectItem(pItem);

    // Setup the item properties docking window to be the current item
    QString sWindowTitle = pItem->GetName(true) % " Properties";

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

    if(pItem->IsSaveClean() == false)
    {
        int iDlgReturn = QMessageBox::question(nullptr, "Save Changes", pItem->GetName(true) % " has unsaved changes. Do you want to save before closing?", QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

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
        pItem->BlockAllWidgetSignals(true);

        sm_pInstance->ui->dockWidgetCurrentItem->hide();

        QList<QAction *> editActionList = sm_pInstance->ui->menu_Edit->actions();
        for(uint i = 0; i < editActionList.size(); ++i)
            sm_pInstance->ui->mainToolBar->removeAction(editActionList[i]);

        sm_pInstance->ui->menu_Edit->clear();

        pItem->BlockAllWidgetSignals(false);
    }

    Harmony::GetProject()->CloseTab(pItem);

    if(pItem->IsExistencePendingSave() && pItem->GetTreeItem()->parent() != nullptr)
        pItem->GetTreeItem()->parent()->removeChild(pItem->GetTreeItem());
}

/*virtual*/ void MainWindow::closeEvent(QCloseEvent *pEvent) /*override*/
{
    // This will ensure that the user has a chance to save all unsaved open documents, or cancel which will abort the close
    if(Harmony::GetProject() && Harmony::GetProject()->CloseAllTabs() == false)
    {
        pEvent->ignore();
        return;
    }

    SaveSettings();
    QMainWindow::closeEvent(pEvent);
}

void MainWindow::OnCtrlTab()
{
//    if(pCurProject == nullptr)
//        return;

//    ProjectTabBar *pTabBar = pCurProject->GetTabBar();
//    //pTabBar
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
        ui->explorer->GetCurProjSelected()->GetAtlasModel().RepackAll(0);
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
    Harmony::CloseProject();

    ui->dockWidgetAtlas->setWidget(nullptr);
    ui->dockWidgetAudio->setWidget(nullptr);

    ui->explorer->RemoveItem(ui->explorer->GetCurProjSelected());
}

void MainWindow::on_actionNewAudio_triggered()
{
    NewItem(ITEM_Audio);
}

void MainWindow::on_actionNewEntity_triggered()
{
    NewItem(ITEM_Entity);
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

void MainWindow::on_actionSave_triggered()
{
    Project *pCurProject = Harmony::GetProject();
    if(pCurProject == nullptr)
    {
        HyGuiLog("No valid project is active to save.", LOGTYPE_Error);
        return;
    }

    ProjectTabBar *pTabBar = pCurProject->GetTabBar();
    int iIndex = pTabBar->currentIndex();
    if(iIndex < 0)
    {
        HyGuiLog("on_actionSave triggered with tab index of '-1'. Aborting save.", LOGTYPE_Error);
        return;
    }

    QVariant v = pTabBar->tabData(iIndex);
    ProjectItem *pItem = v.value<ProjectItem *>();
    pItem->Save();

    HyGuiLog(pItem->GetName(true) % " was saved", LOGTYPE_Normal);
}

void MainWindow::on_actionSaveAll_triggered()
{
    Project *pCurProject = Harmony::GetProject();
    if(pCurProject == nullptr)
    {
        HyGuiLog("No valid project is active to save all.", LOGTYPE_Error);
        return;
    }

    ProjectTabBar *pTabBar = pCurProject->GetTabBar();
    for(int i = 0; i < pTabBar->count(); ++i)
    {
        ProjectItem *pItem = pTabBar->tabData(i).value<ProjectItem *>();
        if(pItem->IsSaveClean() == false)
        {
            pItem->Save();
            HyGuiLog(pItem->GetName(true) % " was saved", LOGTYPE_Normal);
        }
    }
}

void MainWindow::on_menu_View_aboutToShow()
{
    QMenu *pPopupMenu = this->createPopupMenu();

    ui->menu_View->clear();
    ui->menu_View->addActions(pPopupMenu->actions());
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
    bool bUseVs2017 = false;
    bool bUseVs2015 = false;
    bool bUseVs2013 = false;

    QSettings windowsRegEntryVS2017("HKEY_CLASSES_ROOT\\VisualStudio.DTE.15.0", QSettings::NativeFormat);
    if(windowsRegEntryVS2017.childKeys().empty() == false)
        bUseVs2017 = true;

    QSettings windowsRegEntryVS2015("HKEY_CLASSES_ROOT\\VisualStudio.DTE.14.0", QSettings::NativeFormat);
    if(windowsRegEntryVS2015.childKeys().empty() == false)
        bUseVs2015 = true;

    QSettings windowsRegEntryVS2013("HKEY_CLASSES_ROOT\\VisualStudio.DTE.12.0", QSettings::NativeFormat);
    if(windowsRegEntryVS2013.childKeys().empty() == false)
        bUseVs2013 = true;

    // Use the newest version
    if(bUseVs2017)
        bUseVs2013 = bUseVs2015 = false;
    else if(bUseVs2015)
        bUseVs2013 = false;
    else if(bUseVs2013 == false) {
        HyGuiLog("No appropriate IDE was detected on this machine.", LOGTYPE_Error);
    }

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

                if(line.contains("# Visual Studio 15", Qt::CaseSensitive))
                {
                    if(bUseVs2017)
                    {
                        file.close();
                        if(false == QDesktopServices::openUrl(QUrl(ideFileInfoList[i].absoluteFilePath())))
                            HyGuiLog("Could not open IDE at:\n" % ideFileInfoList[i].absoluteFilePath(), LOGTYPE_Error);
                        return;
                    }
                    else
                        break;
                }

                if(line.contains("# Visual Studio 14", Qt::CaseSensitive))
                {
                    if(bUseVs2015)
                    {
                        file.close();
                        if(false == QDesktopServices::openUrl(QUrl(ideFileInfoList[i].absoluteFilePath())))
                            HyGuiLog("Could not open IDE at:\n" % ideFileInfoList[i].absoluteFilePath(), LOGTYPE_Error);
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
                        if(false == QDesktopServices::openUrl(QUrl(ideFileInfoList[i].absoluteFilePath())))
                            HyGuiLog("Could not open IDE at:\n" % ideFileInfoList[i].absoluteFilePath(), LOGTYPE_Error);
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

void MainWindow::on_actionConnect_triggered()
{
//    // Network initialization
//    m_pTcpServer = new QTcpServer(this);
//    connect(m_pTcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
//    if(!m_pTcpServer->listen(QHostAddress::LocalHost, HY_TCP_PORT))
//        HYLOG("Cannot start TCP server", LOGTYPE_Error)
//    else
//        HYLOG("TCP server initialized", LOGTYPE_Normal);

    //m_pDebugConnection->Connect();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, HyDesignerToolName, "Harmony Engine and Designer Tool\n\nJason Knobler " % QString::number(QDate::currentDate().year()));
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionProjectSettings_triggered()
{
    if(ui->explorer->GetCurProjSelected() == nullptr)
        return;

    ui->explorer->GetCurProjSelected()->ExecProjSettingsDlg();
}

void MainWindow::NewItem(HyGuiItemType eItem)
{
    DlgNewItem *pDlg = new DlgNewItem(Harmony::GetProject(), eItem, this);
    if(pDlg->exec())
        ui->explorer->AddNewItem(ui->explorer->GetCurProjSelected(), eItem, pDlg->GetPrefix(), pDlg->GetName(), true, QJsonValue());

    delete pDlg;
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
