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
	buildDir.mkdir(HyGlobal::ItemName(ITEM_AtlasImage, true));
	buildDir.mkdir(HyGlobal::ItemName(ITEM_Audio, true));

	// META-DATA
	buildDir.setPath(GetProjDirPath());
	buildDir.mkdir(ui->wgtMetaDir->GetRelPath());
	buildDir.cd(ui->wgtMetaDir->GetRelPath());
	buildDir.mkdir(HyGlobal::ItemName(ITEM_Source, true));
	buildDir.mkdir(HyGlobal::ItemName(ITEM_AtlasImage, true));
	buildDir.mkdir(HyGlobal::ItemName(ITEM_Audio, true));

	// SOURCE
	buildDir.setPath(GetProjDirPath());
	buildDir.mkdir(ui->wgtSourceDir->GetRelPath());

	//// BUILD
	//buildDir.setPath(GetProjDirPath());
	//buildDir.mkdir(ui->wgtBuildDir->GetRelPath());

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Generate HyProj file. Insert the minimum required fields for it. 
	// The project's DlgProjectSettings will fill in the rest of the defaults
	QJsonObject jsonObj;
	jsonObj.insert("$fileVersion", HYGUI_FILE_VERSION);
	jsonObj.insert("Title", ui->txtTitleName->text());
	jsonObj.insert("DataPath", QString(ui->wgtDataDir->GetRelPath() + "/"));
	jsonObj.insert("MetaPath", QString(ui->wgtMetaDir->GetRelPath() + "/"));
	jsonObj.insert("SourcePath", QString(ui->wgtSourceDir->GetRelPath() + "/"));
	jsonObj.insert("BuildPath", QString(ui->wgtBuildDir->GetRelPath() + "/"));

	QJsonArray windowInfoArray;
	QJsonObject windowInfoObj;
	windowInfoObj.insert("Name", ui->txtTitleName->text());
	windowInfoObj.insert("Type", HYWINDOW_WindowedFixed);
	windowInfoObj.insert("ResolutionX", 1280);
	windowInfoObj.insert("ResolutionY", 720);
	windowInfoObj.insert("LocationX", 100);
	windowInfoObj.insert("LocationY", 100);
	windowInfoArray.append(windowInfoObj);
	jsonObj.insert("WindowInfo", windowInfoArray);

	jsonObj.insert("UseConsole", true);
	QJsonObject consoleInfoObj;
	consoleInfoObj.insert("LocationX", 0);
	consoleInfoObj.insert("LocationY", 0);
	consoleInfoObj.insert("Name", "Harmony Log Console");
	consoleInfoObj.insert("ResolutionX", 0);
	consoleInfoObj.insert("ResolutionY", 0);
	consoleInfoObj.insert("Type", 3);
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

	//// src files
	//QDir projGenSrcDir(MainWindow::EngineSrcLocation() % HYGUIPATH_ProjGenDir % "src");
	//fileInfoList = projGenSrcDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
	//for(int i = 0; i < fileInfoList.size(); ++i)
	//	QFile::copy(fileInfoList[i].absoluteFilePath(), srcDir.absoluteFilePath(fileInfoList[i].fileName()));

	///////////////////////////////////////////////////////////////////////////////////////////////////
	//// Rename the copied source files if needed
	//fileInfoList = QDir(GetProjDirPath()).entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
	//fileInfoList += srcDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
	//for(int i = 0; i < fileInfoList.size(); ++i)
	//{
	//	if(fileInfoList[i].fileName().contains("%HY_CLASS%"))
	//	{
	//		QFile file(fileInfoList[i].absoluteFilePath());
	//		QString sNewFileName = fileInfoList[i].fileName().replace("%HY_CLASS%", ui->txtCodeName->text());
	//		file.rename(fileInfoList[i].absoluteDir().absolutePath() % "/Game/" % sNewFileName);
	//		file.close();
	//	}
	//}
	//// Then replace the variable contents of the copied source files
	//fileInfoList = QDir(GetProjDirPath()).entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
	//fileInfoList += srcDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
	//fileInfoList += QDir(srcDir.absoluteFilePath("Game")).entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
	//QTextCodec *pCodec = QTextCodec::codecForLocale();
	//for(int i = 0; i < fileInfoList.size(); ++i)
	//{
	//	QFile file(fileInfoList[i].absoluteFilePath());
	//	if(!file.open(QFile::ReadOnly))
	//	{
	//		HyGuiLog("Error reading " % file.fileName() % " when generating source: " % file.errorString(), LOGTYPE_Error);
	//		return;
	//	}

	//	QString sContents = pCodec->toUnicode(file.readAll());
	//	file.close();

	//	sContents.replace("%HY_TITLE%", ui->txtTitleName->text());
	//	sContents.replace("%HY_CLASS%", ui->txtCodeName->text());
	//	sContents.replace("%HY_PROJDIR%", GetProjDirPath());
	//	sContents.replace("%HY_RELSRCDIR%", QDir(GetProjDirPath()).relativeFilePath(srcDir.absolutePath()));
	//	sContents.replace("%HY_RELHARMONYDIR%", QDir(GetProjDirPath()).relativeFilePath(MainWindow::EngineSrcLocation()));
	//	sContents.replace("%HY_RELDATADIR%", ui->wgtDataDir->GetRelPath());
	//	sContents.replace("%HY_DEPENDENCIES_ADD%", GetDependAdd());
	//	sContents.replace("%HY_DEPENDENCIES_LINK%", GetDependLink());

	//	if(!file.open(QFile::WriteOnly))
	//	{
	//		HyGuiLog("Error writing to " % file.fileName() % " when generating source: " % file.errorString(), LOGTYPE_Error);
	//		return;
	//	}
	//	file.write(pCodec->fromUnicode(sContents));
	//	file.close();
	//}
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
	ui->wgtSourceDir->Setup("Source Code", "src", GetProjDirPath(), "meta");
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
	}while(false);

	ui->lblError->setVisible(bIsError);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!bIsError);
}
