/**************************************************************************
 *	DlgNewProject.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "DlgNewProject.h"
#include "ui_DlgNewProject.h"
#include "MainWindow.h"

#include <QDir>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QUuid>

DlgNewProject::DlgNewProject(QString &sDefaultLocation, QWidget *parent) :
	QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
	ui(new Ui::DlgNewProject)
{
	ui->setupUi(this);

	ui->txtTitleName->blockSignals(true);
	ui->txtCodeName->blockSignals(true);
	ui->txtGameLocation->blockSignals(true);
	{
		ui->txtTitleName->setText("New Game");
		ui->txtTitleName->setFocus();
		ui->txtTitleName->selectAll();
		ui->txtTitleName->setValidator(HyGlobal::FreeFormValidator());

		ui->txtCodeName->setText("NewGame");
		ui->txtCodeName->setValidator(HyGlobal::CodeNameValidator());

		ui->txtGameLocation->setText(sDefaultLocation);

		on_txtTitleName_textChanged("New Game");
	}
	ui->txtTitleName->blockSignals(false);
	ui->txtCodeName->blockSignals(false);
	ui->txtGameLocation->blockSignals(false);
	
	UpdateProjectDir();

	ui->lblError->setStyleSheet("QLabel { background-color : red; color : black; }");
	connect(ui->wgtDataDir, &WgtMakeRelDir::OnDirty, this, &DlgNewProject::ErrorCheck);
	connect(ui->wgtMetaDir, &WgtMakeRelDir::OnDirty, this, &DlgNewProject::ErrorCheck);
	connect(ui->wgtSourceDir, &WgtMakeRelDir::OnDirty, this, &DlgNewProject::ErrorCheck);
	connect(ui->wgtBuildDir, &WgtMakeRelDir::OnDirty, this, &DlgNewProject::ErrorCheck);
	ErrorCheck();
}

DlgNewProject::~DlgNewProject()
{
	delete ui;
}

QString DlgNewProject::GetProjFilePath()
{
	return GetProjDirPath() % GetProjFileName();
}

QString DlgNewProject::GetProjFileName()
{
	return ui->txtCodeName->text() % HyGlobal::ItemExt(ITEM_Project);
}

QString DlgNewProject::GetProjDirPath()
{
	if(ui->chkCreateGameDir->isChecked())
		return ui->txtGameLocation->text() + '/' + ui->txtCodeName->text() + '/';
	else
		return ui->txtGameLocation->text() + '/';
}

bool DlgNewProject::IsCreatingGameDir()
{
	return ui->chkCreateGameDir->isChecked();
}

void DlgNewProject::on_buttonBox_accepted()
{
	///////////////////////////////////////////////////////////////////////////////////////////////
	// Create workspace file tree
	QDir buildDir(GetProjDirPath());
	buildDir.mkpath(".");

	// DATA
	buildDir.mkdir(ui->wgtDataDir->GetRelPath());
	buildDir.cd(ui->wgtDataDir->GetRelPath());
	buildDir.mkdir(HyGlobal::AssetName(ASSETMAN_Atlases));
	buildDir.mkdir(HyGlobal::AssetName(ASSETMAN_Audio));

	// META-DATA
	buildDir.setPath(GetProjDirPath());
	buildDir.mkdir(ui->wgtMetaDir->GetRelPath());
	buildDir.cd(ui->wgtMetaDir->GetRelPath());
	buildDir.mkdir(HyGlobal::AssetName(ASSETMAN_Source));
	buildDir.mkdir(HyGlobal::AssetName(ASSETMAN_Atlases));
	buildDir.mkdir(HyGlobal::AssetName(ASSETMAN_Audio));

	// SOURCE
	buildDir.setPath(GetProjDirPath());
	buildDir.mkdir(ui->wgtSourceDir->GetRelPath());

	//// BUILD
	//buildDir.setPath(GetProjDirPath());
	//buildDir.mkdir(ui->wgtBuildDir->GetRelPath());

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Generate HyProj file. Insert the minimum required fields for it.
	// The project's DlgProjectSettings will fill in the rest of the defaults
	HarmonyInit hyInit; // Use default values of struct;
	QJsonObject jsonObj;
	jsonObj.insert("$fileVersion", HYGUI_FILE_VERSION);
	jsonObj.insert("Title", ui->txtTitleName->text());
	jsonObj.insert("DataPath", QString(ui->wgtDataDir->GetRelPath() + "/"));
	jsonObj.insert("MetaPath", QString(ui->wgtMetaDir->GetRelPath() + "/"));
	jsonObj.insert("SourcePath", QString(ui->wgtSourceDir->GetRelPath() + "/"));
	jsonObj.insert("BuildPath", QString(ui->wgtBuildDir->GetRelPath() + "/"));

	jsonObj.insert("UpdatesPerSec", static_cast<qint64>(hyInit.uiUpdatesPerSec));
	jsonObj.insert("VSync", hyInit.iVSync);
	jsonObj.insert("NumInputMaps", static_cast<qint64>(hyInit.uiNumInputMaps));
	jsonObj.insert("ShowCursor", hyInit.bShowCursor);

	QJsonArray gravity2dArray;
	gravity2dArray.append(hyInit.vGravity2d.x);
	gravity2dArray.append(hyInit.vGravity2d.y);
	jsonObj.insert("Gravity2d", gravity2dArray);
	jsonObj.insert("PixelsPerMeter", hyInit.fPixelsPerMeter);

	QJsonArray windowInfoArray;
	QJsonObject windowInfoObj;
	windowInfoObj.insert("Name", ui->txtTitleName->text());
	windowInfoObj.insert("Type", hyInit.windowInfo[0].eMode);
	windowInfoObj.insert("ResolutionX", hyInit.windowInfo[0].vSize.x);
	windowInfoObj.insert("ResolutionY", hyInit.windowInfo[0].vSize.y);
	windowInfoObj.insert("LocationX", hyInit.windowInfo[0].ptLocation.x);
	windowInfoObj.insert("LocationY", hyInit.windowInfo[0].ptLocation.y);
	windowInfoArray.append(windowInfoObj);
	jsonObj.insert("WindowInfo", windowInfoArray);

	jsonObj.insert("UseConsole", true);
	QJsonObject consoleInfoObj;
	consoleInfoObj.insert("LocationX", hyInit.consoleInfo.ptLocation.x);
	consoleInfoObj.insert("LocationY", hyInit.consoleInfo.ptLocation.y);
	consoleInfoObj.insert("Name", hyInit.consoleInfo.sName.c_str());
	consoleInfoObj.insert("ResolutionX", hyInit.consoleInfo.vSize.x);
	consoleInfoObj.insert("ResolutionY", hyInit.consoleInfo.vSize.y);
	consoleInfoObj.insert("Type", hyInit.consoleInfo.eMode);
	jsonObj.insert("ConsoleInfo", consoleInfoObj);

	QFile newProjectFile(GetProjFilePath());
	if(newProjectFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
	{
		HyGuiLog("Couldn't open new project file for writing", LOGTYPE_Error);
	}
	else
	{
		QJsonDocument newProjectDoc(jsonObj);
		qint64 iBytesWritten = newProjectFile.write(newProjectDoc.toJson());
		if(0 == iBytesWritten || -1 == iBytesWritten)
		{
			HyGuiLog("Could not write new project file: " % newProjectFile.errorString(), LOGTYPE_Error);
		}

		newProjectFile.close();
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// Copy standard git files to new project location
	if(ui->chkUseGit->isChecked())
	{
		QDir projGenDir(MainWindow::EngineSrcLocation() % HYGUIPATH_ProjGenDir);
		QFileInfoList fileInfoList = projGenDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
		for(int i = 0; i < fileInfoList.size(); ++i)
			QFile::copy(fileInfoList[i].absoluteFilePath(), QDir(GetProjDirPath()).absoluteFilePath(fileInfoList[i].fileName()));
	}
}

void DlgNewProject::on_btnBrowse_clicked()
{
	QFileDialog *pDlg = new QFileDialog(this, "Choose the \"root\" location of the game project where the game project file will appear");
	pDlg->setFileMode(QFileDialog::Directory);
	pDlg->setOption(QFileDialog::ShowDirsOnly, true);

	pDlg->setViewMode(QFileDialog::Detail);
	pDlg->setWindowModality( Qt::ApplicationModal );
	pDlg->setModal(true);
	pDlg->setDirectory(ui->txtGameLocation->text());

	if(pDlg->exec() == QDialog::Accepted)
	{
		QString sDir = pDlg->selectedFiles()[0];
		ui->txtGameLocation->setText(sDir);
	}
}

void DlgNewProject::on_txtGameLocation_textChanged(const QString &arg1)
{
	UpdateProjectDir();
}

void DlgNewProject::on_txtTitleName_textChanged(const QString &arg1)
{
	QString sFixedForClass = arg1;
	HyGlobal::CodeNameValidator()->fixup(sFixedForClass);
	sFixedForClass.replace(" ", "");

	int iPos;
	if(QValidator::Invalid != HyGlobal::CodeNameValidator()->validate(sFixedForClass, iPos))
		ui->txtCodeName->setText(sFixedForClass);

	ui->lblAppendHint->setText("Appends \"/" % ui->txtCodeName->text() % "/\" to above");
	UpdateProjectDir();
}

void DlgNewProject::on_chkCreateGameDir_clicked()
{
	UpdateProjectDir();
}

void DlgNewProject::UpdateProjectDir()
{
	ui->wgtDataDir->Setup("Assets", "data", GetProjDirPath());
	ui->wgtMetaDir->Setup("Meta-Data", "meta", GetProjDirPath());
	ui->wgtSourceDir->Setup("Source Code", "Source", GetProjDirPath(), "meta");
	ui->wgtBuildDir->Setup("Build", "build", GetProjDirPath());
}

void DlgNewProject::ErrorCheck()
{
	bool bIsError = false;
	do
	{
		if(ui->txtTitleName->text().isEmpty())
		{
			ui->lblError->setText("Error: Game title cannot be blank.");
			bIsError = true;
			break;
		}
		if(ui->txtCodeName->text().isEmpty())
		{
			ui->lblError->setText("Error: Class name cannot be blank.");
			bIsError = true;
			break;
		}

		QFile projFile(GetProjFilePath());
		if(projFile.exists())
		{
			ui->lblError->setText("Error: Project with this name already exists at this location.");
			bIsError = true;
			break;
		}

		QDir rootDir(GetProjDirPath());

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		QString sError;
		sError = ui->wgtDataDir->GetError();
		if(sError.isEmpty() == false)
		{
			ui->lblError->setText(sError);
			bIsError = true;
			break;
		}

		sError = ui->wgtMetaDir->GetError();
		if(sError.isEmpty() == false)
		{
			ui->lblError->setText(sError);
			bIsError = true;
			break;
		}

		sError = ui->wgtSourceDir->GetError();
		if(sError.isEmpty() == false)
		{
			ui->lblError->setText(sError);
			bIsError = true;
			break;
		}

		sError = ui->wgtBuildDir->GetError();
		if(sError.isEmpty() == false)
		{
			ui->lblError->setText(sError);
			bIsError = true;
			break;
		}
	}while(false);

	ui->lblError->setVisible(bIsError);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!bIsError);
}
