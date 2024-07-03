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
#include "Project.h"
#include "DlgSetEngineLocation.h"
#include "DlgNewProject.h"
#include "DlgNewItem.h"
#include "DlgNewBuild.h"
#include "DlgNewPackage.h"
#include "DlgInputName.h"
#include "DlgProjectSettings.h"
#include "DlgSnappingSettings.h"
#include "DlgTabCycle.h"
#include "ExplorerWidget.h"
#include "AudioAssetsWidget.h"
#include "ManagerWidget.h"
#include "SourceModel.h"
#include "Themes.h"
#include "EntityModel.h"
#include "WgtCodeEditor.h"

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
#include <QProcess>

/*static*/ MainWindow *MainWindow::sm_pInstance = nullptr;

MainWindow::MainWindow(QWidget *pParent) :
	QMainWindow(pParent),
	ui(new Ui::MainWindow),
	m_Harmony(*this),
	m_eTheme(THEME_Decemberween),
	m_Settings(HyOrganizationName, HyEditorToolName)
{
	ui->setupUi(this);
	sm_pInstance = this;

	while(ui->stackedTabWidgets->count())
		ui->stackedTabWidgets->removeWidget(ui->stackedTabWidgets->currentWidget());

	// Initialize Code Editor static theme stuff
	WgtCodeEditor::InitThemeStyleMap();

	SetHarmonyWidget(m_Harmony.GetHarmonyWidget(nullptr));
	SetCurrentProject(nullptr);

	connect(ui->menu_View, SIGNAL(aboutToShow), this, SLOT(on_menu_View_aboutToShow));
	new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Tab), this, SLOT(OnCtrlTab()));
	new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Tab), this, SLOT(OnCtrlShiftTab()));

	HyGuiLog(HyEditorToolName, LOGTYPE_Title);
	HyGuiLog("Initializing...", LOGTYPE_Normal);

	ui->explorer->SetModel(m_ExplorerModel);
	
	ui->explorer->addAction(ui->actionProjectSettings);
	ui->explorer->addAction(ui->actionCloseProject);
	ui->explorer->addAction(ui->actionCopy);
	ui->explorer->addAction(ui->actionNewProject);
	ui->explorer->addAction(ui->actionNewPrefix);
	ui->explorer->addAction(ui->actionNewAudio);
	ui->explorer->addAction(ui->actionNewParticle);
	ui->explorer->addAction(ui->actionNewText);
	ui->explorer->addAction(ui->actionNewSprite);
	ui->explorer->addAction(ui->actionNewSpine);
	ui->explorer->addAction(ui->actionNewEntity);
	ui->explorer->addAction(ui->actionNewPrefab);
	ui->explorer->addAction(ui->actionNewEntity3d);
	ui->explorer->addAction(ui->actionOpenProject);
	ui->explorer->addAction(ui->actionPaste);
	ui->explorer->addAction(ui->actionRemove);
	ui->explorer->addAction(ui->actionRename);
	ui->explorer->addAction(ui->actionBuildSettings);
	ui->explorer->addAction(ui->actionNewBuild);
	ui->explorer->addAction(ui->actionActivateProject);
	ui->explorer->addAction(ui->actionOpenFolderExplorer);
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// FIRST RUN CHECK - Ensure Harmony Engine project location has been specified
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

	// If ui->dockWidgetAssets is shown or hidden, set a slot to also show/hide ui->tabWidgetAux's Asset Manager tab
	//connect(ui->dockWidgetAssets, &QDockWidget::visibilityChanged, this, &MainWindow::on_dockWidgetAssets_visibilityChanged);
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Restore workspace
	ui->dockWidgetExplorer->show();
	ui->dockWidgetAssets->show();
	ui->dockWidgetProperties->setWidget(nullptr);
	ui->dockWidgetProperties->hide();

	for(int i = 0; i < ui->tabWidgetAux->count(); ++i)
		ui->tabWidgetAux->setTabVisible(i, false);
	ui->tabWidgetAux->setTabVisible(AUXTAB_Log, true);
	ui->tabWidgetAux->setCurrentIndex(AUXTAB_Log);

	HyGuiLog("Recovering previously opened session...", LOGTYPE_Normal);
	m_Settings.beginGroup("MainWindow");
	{
		restoreGeometry(m_Settings.value("geometry").toByteArray());
		restoreState(m_Settings.value("windowState").toByteArray());

		DeserializeColors(m_Settings.value("colors", QByteArray()).toByteArray());
	}
	m_Settings.endGroup();
	
	m_Settings.beginGroup("OpenData");
	{
		QStringList sListOpenProjs = m_Settings.value("openProjs").toStringList();
		for(int i = 0; i < sListOpenProjs.size(); ++i)
			m_ExplorerModel.AddProject(sListOpenProjs[i]);
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
		for(int i = 0; i < NUM_THEMES; ++i)
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
	m_LoadingSpinnerList.append(new WaitingSpinnerWidget(ui->dockWidgetAssets));
	m_LoadingSpinnerList.append(new WaitingSpinnerWidget(ui->dockWidgetExplorer));
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

	m_LoadingBar.reset();
	m_LoadingBar.setFormat("%v/%m (%p%)");

	//QPixmap *pPixmap = new QPixmap(":/icons16x16/smiley-sad.gif");
	//QLabel *pSvnStatusIcon = new QLabel;
	//pSvnStatusIcon->setPixmap(*pPixmap);
	//ui->statusBar->addPermanentWidget(&pSvnStatusIcon);

	//QLabel *pSvnLoginLabel = new QLabel;
	//pSvnLoginLabel->setText("SVN Not Detected");
	//ui->statusBar->addPermanentWidget(pSvnLoginLabel);

	QPixmap *pPixmap = new QPixmap(":/icons16x16/StatusMouse.png");
	m_StatusBarMouseIcon.setPixmap(*pPixmap);
	pPixmap = new QPixmap(":/icons16x16/StatusSize.png");
	m_StatusBarSizeIcon.setPixmap(*pPixmap);
	pPixmap = new QPixmap(":/icons16x16/search.png");
	m_StatusBarZoomIcon.setPixmap(*pPixmap);

	ui->statusBar->addPermanentWidget(&m_StatusBarMouseIcon);
	ui->statusBar->addPermanentWidget(&m_StatusBarMouse);
	ui->statusBar->addPermanentWidget(&m_StatusBarSizeIcon);
	ui->statusBar->addPermanentWidget(&m_StatusBarSize);
	ui->statusBar->addPermanentWidget(&m_StatusBarZoomIcon);
	ui->statusBar->addPermanentWidget(&m_StatusBarZoom);

	HyGuiLog("Ready to go!", LOGTYPE_Normal);

	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	on_menu_View_aboutToShow();
}

MainWindow::~MainWindow()
{
	WgtCodeEditor::DestroyThemeStyleMap();
	//if(Harmony::GetProject())
	//	delete Harmony::GetProject()->GetAtlasWidget();
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
		ui->tabWidgetAssetManager->clear();
		ui->actionCloseProject->setEnabled(false);
		ui->actionProjectSettings->setEnabled(false);
		return;
	}

	ui->actionCloseProject->setEnabled(true);
	ui->actionProjectSettings->setEnabled(true);

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
	ui->tabWidgetAssetManager->clear();
	ui->tabWidgetAssetManager->addTab(pProject->GetSourceWidget(), QIcon(":/icons16x16/code.png"), "Source");
	ui->tabWidgetAssetManager->addTab(pProject->GetAtlasWidget(), QIcon(":/icons16x16/atlas-file.png"), "Atlases");
	//ui->tabWidgetAssetManager->addTab(pProject->GetGltfWidget(), HyGlobal::ItemIcon(ITEM_Prefab, SUBICON_None), "Prefabs");
	ui->tabWidgetAssetManager->addTab(pProject->GetAudioWidget(), HyGlobal::ItemIcon(ITEM_Audio, SUBICON_None), "Audio");

	// Restore asset manager widgets to their previous state
	QSettings settings(pProject->GetUserAbsPath(), QSettings::IniFormat);
	settings.beginGroup("AssetManagers");
	{
		int iAssetManagerTabIndex = settings.value("TabIndex").toInt();
		ui->tabWidgetAssetManager->setCurrentIndex(iAssetManagerTabIndex);

		QStringList expandedSourceList = settings.value(HyGlobal::AssetName(ASSETMAN_Source)).toStringList();
		pProject->GetSourceWidget()->RestoreExpandedState(expandedSourceList);

		int iAtlasBankIndex = settings.value(HyGlobal::AssetName(ASSETMAN_Atlases) + "BankIndex").toInt();
		pProject->GetAtlasWidget()->SetSelectedBankIndex(iAtlasBankIndex);
		QStringList expandedAtlasList = settings.value(HyGlobal::AssetName(ASSETMAN_Atlases)).toStringList();
		pProject->GetAtlasWidget()->RestoreExpandedState(expandedAtlasList);

		int iAudioBankIndex = settings.value(HyGlobal::AssetName(ASSETMAN_Audio) + "BankIndex").toInt();
		pProject->GetAudioWidget()->SetSelectedBankIndex(iAudioBankIndex);
		QStringList expandedAudioList = settings.value(HyGlobal::AssetName(ASSETMAN_Audio)).toStringList();
		pProject->GetAudioWidget()->RestoreExpandedState(expandedAudioList);
	}
	settings.endGroup();
	settings.beginGroup("ProjectSettings");
	{
		ui->actionShowGridBackground->setChecked(settings.value("ShowGridBackground", true).toBool());
		ui->actionShowGridOrigin->setChecked(settings.value("ShowGridOrigin", true).toBool());
		ui->actionShowGridOverlay->setChecked(settings.value("ShowGridOverlay", false).toBool());
		pProject->ShowGridBackground(ui->actionShowGridBackground->isChecked());
		pProject->ShowGridOrigin(ui->actionShowGridOrigin->isChecked());
		pProject->ShowGridOverlay(ui->actionShowGridOverlay->isChecked());

		pProject->SetSnappingSettings(settings.value("SnappingSettings", QVariant(static_cast<uint>(SNAPSETTING_DefaultSettings))).toUInt(), false);
	}
	settings.endGroup();

	RefreshBuildMenu();
}

/*static*/ Theme MainWindow::GetTheme()
{
	return sm_pInstance->m_eTheme;
}

/*static*/ QList<LoadingType> MainWindow::GetCurrentLoading()
{
	return sm_pInstance->m_LoadingMap.keys();
}

/*static*/ void MainWindow::SetLoading(LoadingType eLoadingType, int iLoadedBlocks, int iTotalBlocks)
{
	sm_pInstance->m_LoadingMap[eLoadingType] = QPair<int, int>(iLoadedBlocks, iTotalBlocks);
	sm_pInstance->RefreshLoading();
}

/*static*/ void MainWindow::ClearLoading(LoadingType eLoadingType)
{
	sm_pInstance->m_LoadingMap.remove(eLoadingType);
	sm_pInstance->RefreshLoading();
}

/*static*/ QString MainWindow::EngineSrcLocation()
{
	return sm_pInstance->m_sEnginePath;
}

/*static*/ void MainWindow::ApplySaveEnables(bool bCurItemDirty, bool bAnyItemDirty)
{
	sm_pInstance->ui->actionSave->setEnabled(bCurItemDirty);
	sm_pInstance->ui->actionSaveAll->setEnabled(bAnyItemDirty);
}

/*static*/ void MainWindow::OpenItem(ProjectItemData *pItem)
{
	if(pItem == nullptr || pItem->GetType() == ITEM_Project)
		return;

	if(Harmony::GetProject() != &pItem->GetProject())
	{
		HyGuiLog("Cannot open " % HyGlobal::ItemName(pItem->GetType(), false) % " '" % pItem->GetName(true) % "' because its current project is not activated.", LOGTYPE_Warning);
		return;
	}

	Harmony::GetProject()->OpenTab(pItem);

	// Setup the item properties docking window to be the current item
	QString sWindowTitle = pItem->GetName(false) % " Properties";

	sm_pInstance->ui->dockWidgetProperties->show();
	sm_pInstance->ui->dockWidgetProperties->setWindowTitle(sWindowTitle);
	sm_pInstance->ui->dockWidgetProperties->setWidget(pItem->GetWidget());

	if(pItem->GetType() == ITEM_Entity)
	{
		sm_pInstance->ui->tabWidgetAux->setTabVisible(AUXTAB_DopeSheet, true);
		sm_pInstance->ui->tabWidgetAux->setCurrentIndex(AUXTAB_DopeSheet);
		int iStateIndex = 0;
		if(pItem->GetWidget())
			iStateIndex = pItem->GetWidget()->GetCurStateIndex();
		sm_pInstance->ui->dopeSheet->SetEntityStateModel(static_cast<EntityStateData *>(pItem->GetModel()->GetStateData(iStateIndex)));
	}
	else
		sm_pInstance->ui->tabWidgetAux->setTabVisible(AUXTAB_DopeSheet, false);

	// Remove all the actions in the "Edit" menu, and replace it with the current item's actions
	QList<QAction *> editActionList = sm_pInstance->ui->menu_Edit->actions();
	for(int i = 0; i < editActionList.size(); ++i)
		sm_pInstance->ui->mainToolBar->removeAction(editActionList[i]);

	sm_pInstance->ui->menu_Edit->clear();

	pItem->GiveMenuActions(sm_pInstance->ui->menu_Edit);
	sm_pInstance->ui->mainToolBar->addActions(sm_pInstance->ui->menu_Edit->actions());
}

/*static*/ void MainWindow::CloseItem(ProjectItemData *pItem)
{
	if(pItem == nullptr || pItem->GetType() == ITEM_Project)
		return;

	if(pItem->IsSaveClean() == false)
	{
		int iDlgReturn = QMessageBox::question(nullptr, "Save Changes", pItem->GetName(true) % " has unsaved changes. Do you want to save before closing?", QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

		if(iDlgReturn == QMessageBox::Save)
		{
			if(pItem->Save(true) == false)
				return;
		}
		else if(iDlgReturn == QMessageBox::Discard)
			pItem->DiscardChanges();
		else if(iDlgReturn == QMessageBox::Cancel)
			return;
	}

	// If this is the item that is currently being shown, unhook all its actions and widget
	if(pItem->GetType() == ITEM_Entity)
	{
		sm_pInstance->ui->dopeSheet->SetEntityStateModel(nullptr);
		sm_pInstance->ui->tabWidgetAux->setTabVisible(AUXTAB_DopeSheet, false);
	}

	if(sm_pInstance->ui->dockWidgetProperties->widget() == pItem->GetWidget())
	{
		pItem->BlockAllWidgetSignals(true);

		sm_pInstance->ui->dockWidgetProperties->setWindowTitle("Properties");
		sm_pInstance->ui->dockWidgetProperties->hide();

		QList<QAction *> editActionList = sm_pInstance->ui->menu_Edit->actions();
		for(int i = 0; i < editActionList.size(); ++i)
			sm_pInstance->ui->mainToolBar->removeAction(editActionList[i]);

		sm_pInstance->ui->menu_Edit->clear();

		pItem->BlockAllWidgetSignals(false);
	}

	Harmony::GetProject()->CloseTab(pItem);

	if(pItem->IsExistencePendingSave())
		sm_pInstance->m_ExplorerModel.RemoveItem(pItem);
}

/*static*/ QMenu *MainWindow::GetNewItemMenu()
{
	return sm_pInstance->ui->menu_New_Item;
}

/*static*/ ExplorerModel &MainWindow::GetExplorerModel()
{
	return sm_pInstance->m_ExplorerModel;
}

/*static*/ ExplorerWidget &MainWindow::GetExplorerWidget()
{
	return *sm_pInstance->ui->explorer;
}

/*static*/ int MainWindow::GetAssetManagerTabIndex()
{
	return sm_pInstance->ui->tabWidgetAssetManager->currentIndex();
}

/*static*/ IWidget *MainWindow::GetItemProperties()
{
	return static_cast<IWidget *>(sm_pInstance->ui->dockWidgetProperties->widget());
}

/*static*/ QWidget *MainWindow::GetAuxWidget(AuxTab eTabIndex)
{
	switch(eTabIndex)
	{
	case AUXTAB_Log:
		return sm_pInstance->ui->outputLog;
	case AUXTAB_AssetInspector:
		return sm_pInstance->ui->assetInspector;
	case AUXTAB_DopeSheet:
		return sm_pInstance->ui->dopeSheet;
	case AUXTAB_ShaderEditor:
		return sm_pInstance->ui->shaderEditor;

	default:
		HyGuiLog("MainWindow::GetAuxWidget() - Unknown tab index: " % QString::number(eTabIndex), LOGTYPE_Error);
		break;
	}

	return nullptr;
}

/*static*/ void MainWindow::FocusAuxWidget(AuxTab eTabIndex)
{
	sm_pInstance->ui->tabWidgetAux->setCurrentIndex(eTabIndex);
}

/*static*/ void MainWindow::SetStatus(const QString &sMessage, int iTimeoutMs)
{
	sm_pInstance->ui->statusBar->showMessage(sMessage, iTimeoutMs);
}

/*static*/ void MainWindow::ClearStatus()
{
	sm_pInstance->ui->statusBar->clearMessage();
}

/*static*/ void MainWindow::SetDrawStatus(QString sMouse, QString sSize, QString sZoom)
{
	sm_pInstance->m_StatusBarMouse.setText(sMouse);
	sm_pInstance->m_StatusBarSize.setText(sSize);
	sm_pInstance->m_StatusBarZoom.setText(sZoom);
}

/*static*/ void MainWindow::GetGridStatus(bool &bShowBackgroundOut, bool &bShowOriginOut, bool &bShowOverlayOut)
{
	bShowBackgroundOut = sm_pInstance->ui->actionShowGridBackground->isChecked();
	bShowOriginOut = sm_pInstance->ui->actionShowGridOrigin->isChecked();
	bShowOverlayOut = sm_pInstance->ui->actionShowGridOverlay->isChecked();
}

/*virtual*/ QMenu *MainWindow::createPopupMenu() /*override*/
{
	QMenu *pMenu = QMainWindow::createPopupMenu();

	for(QAction *pAction : pMenu->actions())
	{
		if(pAction->text() == "Aux")
		{
			pAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Space));
			break;
		}
	}

	return pMenu;
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

	delete Harmony::GetProject();
	Harmony::SetProject(nullptr);

	QMainWindow::closeEvent(pEvent);
}

void MainWindow::RefreshLoading()
{
	if(m_LoadingMap.empty() == false)
	{
		int iSumOfLoadedBlocks = 0;
		int iSumOfTotalBlocks = 0;
		for(LoadingType eCurLoadingType : m_LoadingMap.keys())
		{
			QPair<int, int> &curPair = m_LoadingMap[eCurLoadingType];
			iSumOfLoadedBlocks += curPair.first;
			iSumOfTotalBlocks += curPair.second;
		}

		bool bLockUI = true;

		if(m_LoadingMap.size() > 1)
			m_LoadingMsg.setText("Multi-Processing");
		else
		{
			switch(m_LoadingMap.firstKey())
			{
			case LOADINGTYPE_Project:			m_LoadingMsg.setText(iSumOfTotalBlocks == 0 ? "Loading Project" : "Version Patcher"); break;
			case LOADINGTYPE_ImportAssets:		m_LoadingMsg.setText(iSumOfTotalBlocks == 0 ? "Validating Assets" : "Importing Assets"); break;
			case LOADINGTYPE_AtlasManager:		m_LoadingMsg.setText(iSumOfTotalBlocks == 0 ? "Repacking Atlases" : "Constructing Atlas Textures"); break;
			case LOADINGTYPE_AudioManager:		m_LoadingMsg.setText("Constructing Audio Banks"); break;
			case LOADINGTYPE_ReloadHarmony:		m_LoadingMsg.setText("Reloading Harmony"); break;
			case LOADINGTYPE_HarmonyStreaming:	m_LoadingMsg.setText("Harmony Streaming"); bLockUI = false; break;

			default:
				HyGuiLog("MainWindow::SetLoading() - Unhandled LoadingType: " % QString::number(m_LoadingMap.firstKey()), LOGTYPE_Error);
				break;
			}
		}

		m_LoadingBar.setRange(0, iSumOfTotalBlocks);
		m_LoadingBar.setValue(iSumOfLoadedBlocks);
		
		statusBar()->addWidget(&m_LoadingMsg);
		statusBar()->addWidget(&m_LoadingBar);
		m_LoadingMsg.show();
		m_LoadingBar.show();

		if(bLockUI)
		{
			// Start spinners and disable UI
			for(int i = 0; i < m_LoadingSpinnerList.size(); ++i)
			{
				if(m_LoadingSpinnerList[i]->isSpinning() == false)
					m_LoadingSpinnerList[i]->start();
			}
			ui->mainToolBar->setEnabled(false);
			ui->menuBar->setEnabled(false);
		}
	}
	else // Loading Complete
	{
		statusBar()->showMessage("Loading Complete", 2000);

		// Stop spinners and enable UI
		statusBar()->removeWidget(&m_LoadingMsg);
		statusBar()->removeWidget(&m_LoadingBar);
		for(int i = 0; i < m_LoadingSpinnerList.size(); ++i)
			m_LoadingSpinnerList[i]->stop();

		ui->mainToolBar->setEnabled(true);
		ui->menuBar->setEnabled(true);
	}

	Project *pCurProject = Harmony::GetProject();
	if(pCurProject)
		m_Harmony.GetHarmonyWidget(pCurProject)->OnRefreshLoading();
}

void MainWindow::OnCtrlTab()
{
	if(Harmony::GetProject() && Harmony::GetProject()->GetTabBar())
	{
		DlgTabCycle *pDlg = new DlgTabCycle(Harmony::GetProject()->GetTabBar(), true, this);
		pDlg->exec();

		delete pDlg;
	}
}

void MainWindow::OnCtrlShiftTab()
{
	if(Harmony::GetProject() && Harmony::GetProject()->GetTabBar())
	{
		DlgTabCycle *pDlg = new DlgTabCycle(Harmony::GetProject()->GetTabBar(), false, this);
		pDlg->exec();

		delete pDlg;
	}
}

void MainWindow::OnProcessStdOut()
{
	QProcess *p = (QProcess *)sender();
	HyGuiLog(p->readAllStandardOutput(), LOGTYPE_Normal);
}

void MainWindow::OnProcessErrorOut()
{
	QProcess *p = (QProcess *)sender();
	HyGuiLog(p->readAllStandardError(), LOGTYPE_Info);
}

void MainWindow::on_tabWidgetAssetManager_currentChanged(int iIndex)
{
	if(Harmony::GetProject() == nullptr)
		return;

	ManagerWidget *pManagerWidget = static_cast<ManagerWidget *>(sm_pInstance->ui->tabWidgetAssetManager->widget(iIndex));
	if(pManagerWidget)
		ui->assetInspector->SetAssetManager(pManagerWidget->GetModel().GetAssetType());
}

void MainWindow::on_tabWidgetAux_currentChanged(int iIndex)
{
	//switch(iIndex)
	//{
	//case AUXTAB_Log:
	//	ui->dockWidgetAuxiliary->setWindowTitle("Log");
	//	break;

	//case AUXTAB_AssetManager:
	//	ui->dockWidgetAuxiliary->setWindowTitle("Asset Manager");
	//	break;

	//case AUXTAB_DopeSheet:
	//	ui->dockWidgetAuxiliary->setWindowTitle("Entity Dope Sheet");
	//	break;

	//case AUXTAB_ShaderEditor:
	//	ui->dockWidgetAuxiliary->setWindowTitle("Shader Editor");
	//	break;

	//default:
	//	HyGuiLog("MainWindow::on_tabWidgetAux_currentChanged() - Unhandled tab index: " % QString::number(iIndex), LOGTYPE_Error);
	//	break;
	//}
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

		m_ExplorerModel.AddProject(pDlg->GetProjFilePath());
		SaveSettings();
	}
	delete pDlg;
}

void MainWindow::on_actionOpenProject_triggered()
{
	QFileDialog *pDlg = new QFileDialog();
	pDlg->setNameFilter(tr("Harmony Project File (*.hyproj)"));
	pDlg->setModal(true);
	pDlg->setDirectory(m_sDefaultProjectLocation);

	if(pDlg->exec() == QDialog::Accepted)
	{
		m_ExplorerModel.AddProject(pDlg->selectedFiles()[0]);
		SaveSettings();
	}

	delete pDlg;
}

void MainWindow::on_actionCloseProject_triggered()
{
	Project *pProj = nullptr;
	ExplorerItemData *pFirstSelected = ui->explorer->GetSelected();
	if(pFirstSelected == nullptr)
		pProj = Harmony::GetProject();
	else
		pProj = &pFirstSelected->GetProject();

	if(pProj == nullptr)
		return;

	bool bClearCurrentProject = Harmony::GetProject() == pProj;
	m_ExplorerModel.RemoveItem(pProj);

	if(bClearCurrentProject)
		Harmony::SetProject(nullptr);
}

void MainWindow::on_actionProjectSettings_triggered()
{
	Project *pProj = nullptr;
	ExplorerItemData *pFirstSelected = ui->explorer->GetSelected();
	if(pFirstSelected == nullptr)
		pProj = Harmony::GetProject();
	else
		pProj = &pFirstSelected->GetProject();

	if(pProj == nullptr)
		return;

	DlgProjectSettings dlg(*pProj, this);
	if(dlg.exec() == QDialog::Accepted && dlg.HasSettingsChanged())
		pProj->SaveSettingsObj(dlg.GetNewSettingsObj());
}

void MainWindow::on_actionOpenFolderExplorer_triggered()
{
	Project *pProj = nullptr;
	ExplorerItemData *pFirstSelected = ui->explorer->GetSelected();
	if(pFirstSelected != nullptr)
		pProj = &pFirstSelected->GetProject();

	if(pProj == nullptr)
		return;

	HyGlobal::OpenFileInExplorer(pProj->GetAbsPath());
}

void MainWindow::on_dockWidgetAssets_visibilityChanged(bool bVisible)
{
	ui->tabWidgetAux->setTabVisible(AUXTAB_AssetInspector, bVisible);
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

void MainWindow::on_actionNewSpine_triggered()
{
	NewItem(ITEM_Spine);
}

void MainWindow::on_actionNewText_triggered()
{
	NewItem(ITEM_Text);
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
	ProjectItemData *pItem = v.value<ProjectItemData *>();
	
	if(pItem->Save(true))
		HyGuiLog(pItem->GetName(true) % " was saved", LOGTYPE_Normal);
	else
		HyGuiLog(pItem->GetName(true) % " was NOT saved", LOGTYPE_Warning);
}

void MainWindow::on_actionSaveAll_triggered()
{
	Project *pCurProject = Harmony::GetProject();
	if(pCurProject == nullptr)
	{
		HyGuiLog("No valid project is active to save all.", LOGTYPE_Error);
		return;
	}

	QList<ProjectItemData *> dirtyItemList;

	ProjectTabBar *pTabBar = pCurProject->GetTabBar();
	for(int i = 0; i < pTabBar->count(); ++i)
	{
		ProjectItemData *pItem = pTabBar->tabData(i).value<ProjectItemData *>();
		if(pItem->IsSaveClean() == false)
			dirtyItemList.append(pItem);
	}

	for(int i = 0; i < dirtyItemList.size(); ++i)
	{
		if(dirtyItemList[i]->Save(i == (dirtyItemList.size() - 1)))
			HyGuiLog(dirtyItemList[i]->GetName(true) % " was saved", LOGTYPE_Normal);
		else
			HyGuiLog(dirtyItemList[i]->GetName(true) % " did NOT save", LOGTYPE_Warning);
	}
}

void MainWindow::on_menu_View_aboutToShow()
{
	ui->menu_View->clear();

	QMenu *pThemesMenu = new QMenu("Themes");
	pThemesMenu->addAction(ui->actionTheme_Decemberween);
	pThemesMenu->addAction(ui->actionTheme_CorpyNT6);
	//pThemesMenu->addAction(ui->actionTheme_Compe);

	ui->menu_View->addMenu(pThemesMenu);
	ui->menu_View->addSeparator();
	ui->menu_View->addAction(ui->actionShowGridBackground);
	ui->menu_View->addAction(ui->actionShowGridOrigin);
	ui->menu_View->addAction(ui->actionShowGridOverlay);
	
	ui->menu_View->addSeparator();
	QMenu *pPopupMenu = this->createPopupMenu();
	ui->menu_View->addActions(pPopupMenu->actions());
	delete pPopupMenu;
}

void MainWindow::on_actionShowGridBackground_triggered()
{
	if(Harmony::GetProject())
		Harmony::GetProject()->ShowGridBackground(ui->actionShowGridBackground->isChecked());
}

void MainWindow::on_actionShowGridOrigin_triggered()
{
	if(Harmony::GetProject())
		Harmony::GetProject()->ShowGridOrigin(ui->actionShowGridOrigin->isChecked());
}

void MainWindow::on_actionShowGridOverlay_triggered()
{
	if(Harmony::GetProject())
		Harmony::GetProject()->ShowGridOverlay(ui->actionShowGridOverlay->isChecked());
}

void MainWindow::on_actionSnappingSettings_triggered()
{
	Project *pCurProject = Harmony::GetProject();
	if(pCurProject == nullptr)
		return;

	DlgSnappingSettings *pNewDlg = new DlgSnappingSettings(pCurProject->GetSnappingSettings(), this);

	if(pNewDlg->exec() == QDialog::Accepted)
		pCurProject->SetSnappingSettings(pNewDlg->GetSnappingSettings(), true);

	delete pNewDlg;
}

void MainWindow::on_actionBuildSettings_triggered()
{
	if(Harmony::GetProject() == nullptr)
	{
		HyGuiLog("on_actionBuildSettings_triggered invoked with no loaded project", LOGTYPE_Error);
		return;
	}

	Harmony::GetProject()->GetSourceModel().OnBankSettingsDlg(0);
}

void MainWindow::on_actionNewBuild_triggered()
{
	if(Harmony::GetProject() == nullptr)
	{
		HyGuiLog("on_actionNewBuild_triggered invoked with no loaded project", LOGTYPE_Error);
		return;
	}

	DlgNewBuild *pDlg = new DlgNewBuild(*Harmony::GetProject(), this);
	if(pDlg->exec() == QDialog::Accepted)
	{
		// TODO: FILE IO FAILS TO DELETE AND IMMEDIATELY CREATE!
		QString sBuildPath = Harmony::GetProject()->GetBuildAbsPath();
		QDir rootBuildDir(sBuildPath);
		QDir buildDir(pDlg->GetAbsBuildDir());
		if(rootBuildDir.exists())
		{
			if(buildDir.exists())
			{
				QFileInfoList tempFileInfoList = buildDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
				if(tempFileInfoList.isEmpty() == false &&
					QMessageBox::Yes == QMessageBox::question(MainWindow::GetInstance(), "Clean existing build", "Do you want to generate a new IDE and override existing build?", QMessageBox::Yes, QMessageBox::No))
				{
					buildDir.removeRecursively();
					QThread::sleep(2);
				}
			}
		}
		else if(false == rootBuildDir.mkpath("."))
		{
			HyGuiLog("Could not create root build directory", LOGTYPE_Error);
			return;
		}

		QProcess *pBuildProcess = new QProcess(this);
		QObject::connect(pBuildProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(OnProcessStdOut()));
		QObject::connect(pBuildProcess, SIGNAL(readyReadStandardError()), this, SLOT(OnProcessErrorOut()));

		connect(pBuildProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
			[=](int exitCode, QProcess::ExitStatus exitStatus) { RefreshBuildMenu(); QMessageBox::information(nullptr, "Build Complete", "Build '" % buildDir.dirName() % "' has completed."); });

		QString sProc = pDlg->GetProc();
		QStringList sArgList = pDlg->GetProcOptions();
		pBuildProcess->start(sProc, sArgList);
	}
	delete pDlg;

	//Harmony::GetProject()->RunCMakeGui();

//	QStringList sFilterList;
//
//#if defined(Q_OS_WIN)
//	sFilterList << "*.sln";
//#endif
//
//	QDir srcDir(Harmony::GetProject()->GetSourceAbsPath());
//	srcDir.setNameFilters(sFilterList);
//	QFileInfoList ideFileInfoList = srcDir.entryInfoList();
//
//	if(ideFileInfoList.empty())
//	{
//		HyGuiLog("Could not find appropriate IDE file to launch", LOGTYPE_Error);
//		return;
//	}
//
//	QDesktopServices::openUrl(QUrl(ideFileInfoList[0].absoluteFilePath()));
}

void MainWindow::on_actionNewPackage_triggered()
{
	if(Harmony::GetProject() == nullptr)
	{
		HyGuiLog("on_actionNewPackage_triggered invoked with no loaded project", LOGTYPE_Error);
		return;
	}

	DlgNewPackage *pDlg = new DlgNewPackage(*Harmony::GetProject(), this);
	if(pDlg->exec() == QDialog::Accepted)
	{
		QProcess *pBuildProcess = new QProcess(this);
		QObject::connect(pBuildProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(OnProcessStdOut()));
		QObject::connect(pBuildProcess, SIGNAL(readyReadStandardError()), this, SLOT(OnProcessErrorOut()));

		QString sProc = pDlg->GetProc();
		QStringList sArgList = pDlg->GetProcOptions();
		pBuildProcess->start(sProc, sArgList);
	}
	delete pDlg;
}

void MainWindow::on_actionChangeHarmonyLocation_triggered()
{
	QDir engineDir(m_Settings.value("engineLocation").toString());
	DlgSetEngineLocation *pDlg = new DlgSetEngineLocation(this);
	if(pDlg->exec() == QDialog::Accepted)
	{
		engineDir.setPath(pDlg->SelectedDir());
		if(HyGlobal::IsEngineDirValid(engineDir))
		{
			m_Settings.setValue("engineLocation", QVariant(pDlg->SelectedDir()));
			SaveSettings();
		}
		else
			HyGuiLog(engineDir.absolutePath() % " is an invalid Harmony path", LOGTYPE_Error);
	}

	delete pDlg;
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
	QMessageBox::about(this, HyEditorToolName, "Harmony Engine and Editor Tool\n\nJason Knobler 2012-" % QString::number(QDate::currentDate().year()) %
											   "\n\nFile Version: " % QString::number(HYGUI_FILE_VERSION) %
											   "\nHarmony: " % m_sEnginePath);
}

void MainWindow::on_actionExit_triggered()
{
	close();
}

void MainWindow::on_actionTheme_Decemberween_triggered()
{
	SelectTheme(THEME_Decemberween);
}

void MainWindow::on_actionTheme_CorpyNT6_triggered()
{
	SelectTheme(THEME_CorpyNT6);
}

void MainWindow::on_actionTheme_Compe_triggered()
{
	SelectTheme(THEME_Compe);
}

void MainWindow::on_actionActivateProject_triggered()
{
	ExplorerItemData *pFirstSelected = ui->explorer->GetSelected();

	if(Harmony::GetProject() &&
	   Harmony::GetProject() != &pFirstSelected->GetProject() &&
	   Harmony::GetProject()->IsUnsavedOpenItems())
	{
		HyGuiLog("Cannot activate project because there are unsaved items open", LOGTYPE_Warning);
		return;
	}
	
	bool bNewProject = Harmony::GetProject() != &pFirstSelected->GetProject();
	if(bNewProject)
	{
		sm_pInstance->ui->dockWidgetProperties->setWindowTitle("Properties");
		sm_pInstance->ui->dockWidgetProperties->setWidget(nullptr);
	}

	m_Harmony.SetProject(&pFirstSelected->GetProject());

	if(Harmony::GetProject() && bNewProject && Harmony::GetProject()->GetCurrentOpenItem())
		OpenItem(Harmony::GetProject()->GetCurrentOpenItem());
}

void MainWindow::DeserializeColors(const QByteArray &serializedColors)
{
	
}

void MainWindow::SerializeColors(const QByteArray &serializedColors)
{
	m_Settings.setValue("colors", serializedColors);
}

void MainWindow::NewItem(ItemType eItem)
{
	ExplorerItemData *pFirstSelected = ui->explorer->GetSelected();
	if(pFirstSelected == nullptr)
	{
		HyGuiLog("Nothing selected (no project) to add item to.", LOGTYPE_Error);
		return;
	}

	Project *pProj = Harmony::GetProject();

	if(pProj != &pFirstSelected->GetProject())
	{
		HyGuiLog("Current project does not equal the selected item's project", LOGTYPE_Error);
		return;
	}

#ifndef HY_USE_SPINE
	if(eItem == ITEM_Spine)
	{
		HyGuiLog("This Editor was not built with CMake option 'HYBUILD_SPINE' enabled.", LOGTYPE_Warning);
		return;
	}
#endif

	QString sDefaultPrefix = pFirstSelected->GetType() == ITEM_Prefix ? pFirstSelected->GetName(true) : pFirstSelected->GetPrefix();
	if(sDefaultPrefix.isEmpty() == false && sDefaultPrefix[sDefaultPrefix.size() - 1] == '/')
		sDefaultPrefix = sDefaultPrefix.left(sDefaultPrefix.size() - 1);

	DlgNewItem *pDlg = new DlgNewItem(Harmony::GetProject(), eItem, sDefaultPrefix, this);
	if(pDlg->exec())
	{
		ExplorerItemData *pNewItem = m_ExplorerModel.AddItem(pProj,
															 eItem,
															 pDlg->GetPrefix(),
															 pDlg->GetName(),
															 HyGlobal::GenerateNewItemFileData(pDlg->GetImportFile()),
															 true);//pDlg->GetImportFile().isEmpty()); // Blank items are pending save

		if(pNewItem->IsProjectItem())
		{
			static_cast<ProjectItemData *>(pNewItem)->LoadModel();
			MainWindow::OpenItem(static_cast<ProjectItemData *>(pNewItem));
		}
	}

	delete pDlg;
}

void MainWindow::RefreshBuildMenu()
{
	// Clean out existing actionOpenIde's
	ui->menu_Project->clear();
	ui->menu_Project->addAction(ui->actionProjectSettings);
	ui->menu_Project->addSeparator();
	ui->menu_Project->addAction(ui->actionBuildSettings);
	ui->menu_Project->addAction(ui->actionNewBuild);

	if(Harmony::GetProject() == nullptr)
		return;

	QMenu *pBuildsMenu = nullptr;
	Project *pProject = Harmony::GetProject();
	QString sAbsBuildDir = pProject->GetBuildAbsPath();
	QDir buildDir(sAbsBuildDir);
	QStringList buildDirList = buildDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	if(buildDirList.empty() == false)
	{
		ui->menu_Project->addSeparator();

		for(auto sDirName : buildDirList)
		{
			QDir dir(buildDir.absolutePath() % "/" % sDirName);
			QFileInfoList buildFileInfoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

			QString sBestAbsFilePath = dir.absolutePath();
			bool bIdeFileFound = false;
			for(auto fileInfo : buildFileInfoList)
			{
				// Look for .sln (Visual Studio)
				if(fileInfo.suffix().compare("sln", Qt::CaseInsensitive) == 0)
				{
					sBestAbsFilePath = fileInfo.absoluteFilePath();
					bIdeFileFound = true;
					break;
				}
				// TODO: scan for other popular IDE's
			}

			// Setup the action and its trigger
			if(pBuildsMenu == nullptr)
				pBuildsMenu = ui->menu_Project->addMenu(QIcon(":/icons16x16/items/Build-Open.png"), "Builds");

			QAction *pActionOpenIde = new QAction(pBuildsMenu);
			pActionOpenIde->setText(sDirName);
			pActionOpenIde->setIcon(QIcon(":/icons16x16/code.png"));
			if(bIdeFileFound)
				connect(pBuildsMenu, &QMenu::triggered, this, [this, sBestAbsFilePath]() { QDesktopServices::openUrl(QUrl(sBestAbsFilePath)); });
			else // Couldn't determine the exact IDE file, so just open the build directory in explorer
				connect(pBuildsMenu, &QMenu::triggered, this, [this, sBestAbsFilePath]() { HyGlobal::OpenFileInExplorer(sBestAbsFilePath); });

			pBuildsMenu->addAction(pActionOpenIde);
		}
	}

	pProject->GetSourceWidget()->SetSettingsAction(pProject->GetTitle() % " Build Settings:", ui->actionNewBuild, ui->actionBuildSettings);

	//ui->menu_Build->addSeparator();
	//ui->menu_Build->addAction(ui->actionNewPackage);
}

void MainWindow::SaveSettings()
{
	m_Settings.beginGroup("MainWindow");
	{
		m_Settings.setValue("geometry", saveGeometry());
		m_Settings.setValue("windowState", saveState());
		//m_Settings.setValue("colors", m_Settings.value("colors", QByteArray()).toByteArray());
	}
	m_Settings.endGroup();

	m_Settings.beginGroup("OpenData");
	{
		m_Settings.setValue("openProjs", QVariant(m_ExplorerModel.GetOpenProjectPaths()));
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
	case THEME_Decemberween:
		setStyleSheet(QString(szDECEMBERWEEN_STYLESHEET));
		break;
	case THEME_CorpyNT6:
	case THEME_Lappy486:
	case THEME_Compe:
		setStyleSheet("");
		break;
	default:
		HyGuiLog("MainWindow::SelectTheme was given unknown theme", LOGTYPE_Error);
		break;
	}

	static_cast<AuxAssetInspector *>(GetAuxWidget(AUXTAB_AssetInspector))->GetCodeEditor().SetTheme(m_eTheme);
	static_cast<AuxShaderEditor *>(GetAuxWidget(AUXTAB_ShaderEditor))->GetCodeEditor().SetTheme(m_eTheme);
}
