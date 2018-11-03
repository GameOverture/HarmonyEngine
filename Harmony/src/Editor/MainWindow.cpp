/**************************************************************************
 *	MainWindow.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Harmony.h"
#include "HarmonyWidget.h"
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

/*static*/ MainWindow *MainWindow::sm_pInstance = nullptr;

MainWindow::MainWindow(QWidget *pParent) :  QMainWindow(pParent),
											ui(new Ui::MainWindow),
											m_Harmony(*this),
											m_eTheme(THEME_Lappy486),
											m_Settings(HyOrganizationName, HyEditorToolName)
{
	ui->setupUi(this);
	sm_pInstance = this;

	while(ui->stackedTabWidgets->count())
		ui->stackedTabWidgets->removeWidget(ui->stackedTabWidgets->currentWidget());

	SetHarmonyWidget(m_Harmony.GetWidget(nullptr));
	SetCurrentProject(nullptr);

	connect(ui->menu_View, SIGNAL(aboutToShow), this, SLOT(on_menu_View_aboutToShow));
	new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Tab), this, SLOT(OnCtrlTab()));

	HyGuiLog(HyEditorToolName, LOGTYPE_Title);
	HyGuiLog("Initializing...", LOGTYPE_Normal);
	
	// TODO: Don't copy action pointers to other widgets, have function here that manipulates them instead
	ui->explorer->addAction(ui->actionProjectSettings);
	ui->explorer->addAction(ui->actionCloseProject);
	ui->explorer->addAction(ui->actionCopy);
	ui->explorer->addAction(ui->actionNewProject);
	ui->explorer->addAction(ui->actionNewAudio);
	ui->explorer->addAction(ui->actionNewParticle);
	ui->explorer->addAction(ui->actionNewFont);
	ui->explorer->addAction(ui->actionNewSprite);
	ui->explorer->addAction(ui->actionNewEntity);
	ui->explorer->addAction(ui->actionNewPrefab);
	ui->explorer->addAction(ui->actionNewEntity3d);
	ui->explorer->addAction(ui->actionOpenProject);
	ui->explorer->addAction(ui->actionPaste);
	ui->explorer->addAction(ui->actionRemove);
	ui->explorer->addAction(ui->actionRename);
	ui->explorer->addAction(ui->actionLaunchIDE);

	ui->explorer->SetItemMenuPtr(ui->menu_New_Item);
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// FIRST RUN CHECK - Ensure Harmony Engine propject location has been specified
	HyGuiLog("Checking required initialization parameters...", LOGTYPE_Normal);
	m_Settings.beginGroup("RequiredParams");
	{
		QString sEngineDir = m_Settings.value("engineLocation").toString();
		bool bFirstRun = sEngineDir.isEmpty();

		QDir engineDir(sEngineDir);
		while(HyGlobal::IsEngineDirValid(engineDir) == false)
		{
			if(bFirstRun)
				QMessageBox::information(parentWidget(), HyEditorToolName, "First run initialization: Please specify where the Harmony Engine project location is on your machine");
			else
				QMessageBox::warning(parentWidget(), HyEditorToolName, "Harmony Engine Directory is Invalid: Please specify where the Harmony Engine project location is on your machine");
			
			DlgSetEngineLocation *pDlg = new DlgSetEngineLocation(this);
			if(pDlg->exec() == QDialog::Accepted)
			{
				engineDir.setPath(pDlg->SelectedDir());
				m_Settings.setValue("engineLocation", QVariant(pDlg->SelectedDir()));
				setWindowState(Qt::WindowMaximized);
			}
			else
			{
				if(QMessageBox::Retry != QMessageBox::critical(parentWidget(), HyEditorToolName, "You must specify the Harmony Engine project location to continue", QMessageBox::Retry | QMessageBox::Close, QMessageBox::Retry))
					exit(-1);
			}
			delete pDlg;
		}
		
		m_sEnginePath = engineDir.absolutePath();
		m_sEnginePath += "/";
	}
	m_Settings.endGroup();
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Restore workspace
	ui->dockWidgetExplorer->show();
	ui->dockWidgetAtlas->show();
	ui->dockWidgetAudio->hide();
	ui->dockWidgetProperties->hide();

	HyGuiLog("Recovering previously opened session...", LOGTYPE_Normal);
	m_Settings.beginGroup("MainWindow");
	{
		restoreGeometry(m_Settings.value("geometry").toByteArray());
		restoreState(m_Settings.value("windowState").toByteArray());
	}
	m_Settings.endGroup();
	
	m_Settings.beginGroup("OpenData");
	{
		QStringList sListOpenProjs = m_Settings.value("openProjs").toStringList();
		for(int i = 0; i < sListOpenProjs.size(); ++i)
			ui->explorer->AddProject(sListOpenProjs[i]);
	}
	m_Settings.endGroup();

	m_Settings.beginGroup("Misc");
	{
		m_sDefaultProjectLocation = m_Settings.value("defaultProjectLocation").toString();
		QDir defaultProjDir(m_sDefaultProjectLocation);
		if(m_sDefaultProjectLocation.isEmpty() || defaultProjDir.exists() == false)
		{
			QDir setDefaultProjectDir(m_sEnginePath);
			setDefaultProjectDir.cdUp();
			m_sDefaultProjectLocation = setDefaultProjectDir.absolutePath();
		}

		bool bThemeFound = false;
		QString sTheme = m_Settings.value("theme").toString();
		for(int i = 0; i < NUMTHEMES; ++i)
		{
			if(sTheme == HyGlobal::ThemeString(static_cast<Theme>(i)))
			{
				SelectTheme(static_cast<Theme>(i));
				bThemeFound = true;
				break;
			}
		}

		if(bThemeFound == false)
			SelectTheme(m_eTheme);  // Default theme
	}
	m_Settings.endGroup();


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Status bar (and loading indication) initialization
	// Create a loading spinner per docking window. The number of docking windows is predefined and their widgets are contextually replaced to what project/item is active.
	m_LoadingSpinnerList.append(new WaitingSpinnerWidget(ui->dockWidgetAtlas));
	m_LoadingSpinnerList.append(new WaitingSpinnerWidget(ui->dockWidgetAudio));
	m_LoadingSpinnerList.append(new WaitingSpinnerWidget(ui->dockWidgetExplorer));
	//m_LoadingSpinnerList.append(new WaitingSpinnerWidget(ui->dockWidgetOutputLog));   // No need
	m_LoadingSpinnerList.append(new WaitingSpinnerWidget(ui->dockWidgetProperties));

	for(int i = 0; i < m_LoadingSpinnerList.size(); ++i)
	{
		WaitingSpinnerWidget *pLoadingSpinner = m_LoadingSpinnerList[i];

		pLoadingSpinner->setRoundness(50.0);
		pLoadingSpinner->setMinimumTrailOpacity(15.0);
		pLoadingSpinner->setTrailFadePercentage(70.0);
		pLoadingSpinner->setNumberOfLines(20);
		pLoadingSpinner->setLineLength(24);
		pLoadingSpinner->setLineWidth(4);
		pLoadingSpinner->setInnerRadius(12);
		pLoadingSpinner->setRevolutionsPerSecond(1.5);
		pLoadingSpinner->setColor(QColor(25, 255, 25));
	}

	m_LoadingMsg.setText("Ready");
	m_LoadingBar.setRange(0, 100);
	m_LoadingBar.reset();
	statusBar()->addWidget(&m_LoadingMsg);
	statusBar()->addWidget(&m_LoadingBar);

	QPixmap *pPixmap = new QPixmap(":/icons16x16/smiley-sad.gif");
	QLabel *pSvnStatusIcon = new QLabel;
	pSvnStatusIcon->setPixmap(*pPixmap);
	statusBar()->addPermanentWidget(pSvnStatusIcon);

	QLabel *pSvnLoginLabel = new QLabel;
	pSvnLoginLabel->setText("SVN Not Detected");
	statusBar()->addPermanentWidget(pSvnLoginLabel);

	// Manual connections (QtCreator used its black magic to generate these automatically back when I used it)
	connect(ui->actionNewPrefix, SIGNAL(triggered(QAction*)), this, SLOT(on_actionNewPrefix_triggered));

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

void MainWindow::SetCurrentProject(Project *pProject)
{
	if(pProject == nullptr)
	{
		ui->dockWidgetAtlas->setWidget(nullptr);
		ui->dockWidgetAudio->setWidget(nullptr);
		return;
	}

	// Insert the project's TabBar
	bool bTabsFound = false;
	for(int i = 0; i < ui->stackedTabWidgets->count(); ++i)
	{
		if(ui->stackedTabWidgets->widget(i) == pProject->GetTabBar())
		{
			ui->stackedTabWidgets->setCurrentIndex(i);
			bTabsFound = true;
		}
	}
	if(bTabsFound == false)
	{
		ui->stackedTabWidgets->addWidget(pProject->GetTabBar());
		ui->stackedTabWidgets->setCurrentWidget(pProject->GetTabBar());
		pProject->GetTabBar()->setParent(ui->stackedTabWidgets);
	}

	// Project manager widgets
	ui->dockWidgetAtlas->setWidget(pProject->GetAtlasWidget());
	ui->dockWidgetAtlas->widget()->show();

	ui->dockWidgetAudio->setWidget(pProject->GetAudioWidget());
	ui->dockWidgetAudio->widget()->show();
}

/*static*/ void MainWindow::SetLoading(QString sMsg, int iPercentComplete)
{
	sm_pInstance->statusBar()->clearMessage();

	sm_pInstance->m_LoadingMsg.setText(sMsg);

	if(iPercentComplete >= 0)
	{
		sm_pInstance->m_LoadingBar.setVisible(true);
		sm_pInstance->m_LoadingBar.setValue(iPercentComplete);
	}
	else
		sm_pInstance->m_LoadingBar.setVisible(false);

	for(int i = 0; i < sm_pInstance->m_LoadingSpinnerList.size(); ++i)
	{
		if(sm_pInstance->m_LoadingSpinnerList[i]->isSpinning() == false)
			sm_pInstance->m_LoadingSpinnerList[i]->start();
	}

	sm_pInstance->ui->mainToolBar->setEnabled(false);
	sm_pInstance->ui->menuBar->setEnabled(false);
}

/*static*/ void MainWindow::ClearLoading()
{
	sm_pInstance->statusBar()->showMessage("Ready");

	for(int i = 0; i < sm_pInstance->m_LoadingSpinnerList.size(); ++i)
		sm_pInstance->m_LoadingSpinnerList[i]->stop();

	sm_pInstance->ui->mainToolBar->setEnabled(true);
	sm_pInstance->ui->menuBar->setEnabled(true);
}

/*static*/ QString MainWindow::EngineSrcLocation()
{
	return sm_pInstance->m_sEnginePath;
}

/*static*/ void MainWindow::PasteItemSrc(QByteArray sSrc, Project *pProject, QString sPrefixOverride)
{
	sm_pInstance->ui->explorer->PasteItemSrc(sSrc, pProject, sPrefixOverride);
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

	sm_pInstance->ui->dockWidgetProperties->show();
	sm_pInstance->ui->dockWidgetProperties->setWindowTitle(sWindowTitle);
	sm_pInstance->ui->dockWidgetProperties->setWidget(pItem->GetWidget());

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
	if(sm_pInstance->ui->dockWidgetProperties->widget() == pItem->GetWidget())
	{
		pItem->BlockAllWidgetSignals(true);

		sm_pInstance->ui->dockWidgetProperties->hide();

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

		ui->explorer->SelectItem(ui->explorer->AddProject(pDlg->GetProjFilePath()));
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
    pDlg->setDirectory(m_sDefaultProjectLocation);

	if(pDlg->exec() == QDialog::Accepted)
	{
		Project *pAddedProject = ui->explorer->AddProject(pDlg->selectedFiles()[0]);
		if(pAddedProject)
			m_sDefaultProjectLocation = pAddedProject->GetDirPath();
	}
	delete pDlg;
}

void MainWindow::on_actionCloseProject_triggered()
{
	Harmony::SetProject(nullptr);
	ui->explorer->RemoveItem(ui->explorer->GetCurProjSelected());
}

void MainWindow::on_actionNewPrefix_triggered()
{
	NewItem(ITEM_Prefix);
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

void MainWindow::on_actionNewPrefab_triggered()
{
	NewItem(ITEM_Prefab);
}

void MainWindow::on_actionNewEntity3d_triggered()
{
	NewItem(ITEM_Entity3d);
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

	QMenu *pThemesMenu = new QMenu("Themes");
	pThemesMenu->addAction(ui->actionTheme_Lappy486);
	pThemesMenu->addAction(ui->actionTheme_Compe);

	ui->menu_View->addMenu(pThemesMenu);
	ui->menu_View->addSeparator();
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
	QMessageBox::about(this, HyEditorToolName, "Harmony Engine and Editor Tool\n\nJason Knobler " % QString::number(QDate::currentDate().year()));
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

void MainWindow::on_actionTheme_Lappy486_triggered()
{
	SelectTheme(THEME_Lappy486);
}

void MainWindow::on_actionTheme_Compe_triggered()
{
	SelectTheme(THEME_Compe);
}

void MainWindow::NewItem(HyGuiItemType eItem)
{
	ExplorerItem *pCurSelectedItem = ui->explorer->GetCurItemSelected();
	QString sDefaultPrefix = pCurSelectedItem->GetType() == ITEM_Prefix ? pCurSelectedItem->GetName(true) : pCurSelectedItem->GetPrefix();

	DlgNewItem *pDlg = new DlgNewItem(Harmony::GetProject(), eItem, sDefaultPrefix, this);
	if(pDlg->exec())
	{
		ui->explorer->AddItem(ui->explorer->GetCurProjSelected(),
							  eItem,
							  pDlg->GetPrefix(),
							  pDlg->GetName(),
							  true,
							  pDlg->GetImportFile().isEmpty() ? QJsonValue() : QJsonValue(pDlg->GetImportFile()));
	}

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
		m_Settings.setValue("theme", HyGlobal::ThemeString(m_eTheme));
	}
	m_Settings.endGroup();
}

void MainWindow::SelectTheme(Theme eTheme)
{
	m_eTheme = eTheme;
	switch(m_eTheme)
	{
	case THEME_Lappy486:
		setStyleSheet("");
		break;
	case THEME_Compe:
		setStyleSheet("background-color:black;");
		break;
	default:
		HyGuiLog("MainWindow::SelectTheme was given unknown theme", LOGTYPE_Error);
		break;
	}
}
