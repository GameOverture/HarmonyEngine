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
#include <QTextCodec>
#include <QUuid>

DlgNewProject::DlgNewProject(QString &sDefaultLocation, QWidget *parent) :
	QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
	ui(new Ui::DlgNewProject)
{
	ui->setupUi(this);

	ui->txtTitleName->blockSignals(true);
	ui->txtClassName->blockSignals(true);
	ui->txtGameLocation->blockSignals(true);
	{
		ui->txtTitleName->setText("New Game");
		ui->txtTitleName->setFocus();
		ui->txtTitleName->selectAll();
		ui->txtTitleName->setValidator(HyGlobal::FreeFormValidator());

		ui->txtClassName->setText("NewGame");
		ui->txtClassName->setValidator(HyGlobal::CodeNameValidator());

		ui->txtGameLocation->setText(sDefaultLocation);

		on_txtTitleName_textChanged("New Game");
	}
	ui->txtTitleName->blockSignals(false);
	ui->txtClassName->blockSignals(false);
	ui->txtGameLocation->blockSignals(false);
	
	UpdateProjectDir();
	UpdateSrcDependencies();

	ui->lblError->setStyleSheet("QLabel { background-color : red; color : black; }");
	connect(ui->wgtDataDir, &WgtMakeRelDir::OnDirty, this, &DlgNewProject::ErrorCheck);
	connect(ui->wgtMetaDir, &WgtMakeRelDir::OnDirty, this, &DlgNewProject::ErrorCheck);
	connect(ui->wgtSrcDir, &WgtMakeRelDir::OnDirty, this, &DlgNewProject::ErrorCheck);
	connect(ui->wgtBuildDir, &WgtMakeRelDir::OnDirty, this, &DlgNewProject::ErrorCheck);
	ErrorCheck();
}

DlgNewProject::~DlgNewProject()
{
	for(int i = 0; i < m_SrcDependencyList.count(); ++i)
		delete m_SrcDependencyList[i];

	delete ui;
}

void DlgNewProject::AddSrcDep()
{
	m_SrcDependencyList.append(new WgtSrcDependency(this, GetProjDirPath(), ui->grpAdvanced));
	ui->grpAdvanced->layout()->addWidget(m_SrcDependencyList[m_SrcDependencyList.count() - 1]);
	connect(m_SrcDependencyList[m_SrcDependencyList.count() - 1], &WgtSrcDependency::OnDirty, this, &DlgNewProject::ErrorCheck);

	UpdateSrcDependencies();
}

void DlgNewProject::RemoveSrcDep(WgtSrcDependency *pRemoved)
{
	for(int i = 0; i < m_SrcDependencyList.count(); ++i)
	{
		if(m_SrcDependencyList[i] == pRemoved)
		{
			delete m_SrcDependencyList.takeAt(i);
			break;
		}
	}
	UpdateSrcDependencies();
}

QString DlgNewProject::GetProjFilePath()
{
	return GetProjDirPath() % GetProjFileName();
}

QString DlgNewProject::GetProjFileName()
{
	return ui->txtClassName->text() % HyGlobal::ItemExt(ITEM_Project);
}

QString DlgNewProject::GetProjDirPath()
{
	if(ui->chkCreateGameDir->isChecked())
		return ui->txtGameLocation->text() + '/' + ui->txtClassName->text() + '/';
	else
		return ui->txtGameLocation->text() + '/';
}

bool DlgNewProject::IsCreatingGameDir()
{
	return ui->chkCreateGameDir->isChecked();
}

void DlgNewProject::on_buttonBox_accepted()
{
	QDir buildDir(GetProjDirPath());
	buildDir.mkpath(".");

	// Create workspace file tree
	//
	// DATA
	buildDir.mkdir(ui->wgtDataDir->GetRelPath());
	buildDir.cd(ui->wgtDataDir->GetRelPath());
	buildDir.mkdir(HyGlobal::ItemName(ITEM_AtlasImage, true));
	buildDir.mkdir(HyGlobal::ItemName(ITEM_Audio, true));

	// META-DATA
	buildDir.setPath(GetProjDirPath());
	buildDir.mkdir(ui->wgtMetaDir->GetRelPath());
	buildDir.cd(ui->wgtMetaDir->GetRelPath());
	buildDir.mkdir(HyGlobal::ItemName(ITEM_AtlasImage, true));

	// BUILD
	buildDir.setPath(GetProjDirPath());
	buildDir.mkdir(ui->wgtBuildDir->GetRelPath());

	// SOURCE
	buildDir.setPath(GetProjDirPath());
	buildDir.mkdir(ui->wgtSrcDir->GetRelPath());
	buildDir.cd(ui->wgtSrcDir->GetRelPath());
	buildDir.mkdir("Game");
	QDir srcDir(buildDir);

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Insert the minimum required fields for settings file. The project's DlgProjectSettings will fill in the rest of the defaults
	QJsonObject jsonObj;
	jsonObj.insert("$fileVersion", HYGUI_FILE_VERSION);
	jsonObj.insert("GameName", ui->txtTitleName->text());
	jsonObj.insert("ClassName", ui->txtClassName->text());
	jsonObj.insert("DataPath", QString(ui->wgtDataDir->GetRelPath() + "/"));
	jsonObj.insert("MetaDataPath", QString(ui->wgtMetaDir->GetRelPath() + "/"));
	jsonObj.insert("SourcePath", QString(ui->wgtSrcDir->GetRelPath() + "/"));
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
	// Copy files to new project location
	// root files
	QDir projGenDir(MainWindow::EngineSrcLocation() % HYGUIPATH_ProjGenDir);
	QFileInfoList fileInfoList = projGenDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
	for(int i = 0; i < fileInfoList.size(); ++i)
		QFile::copy(fileInfoList[i].absoluteFilePath(), QDir(GetProjDirPath()).absoluteFilePath(fileInfoList[i].fileName()));
	// src files
	QDir projGenSrcDir(MainWindow::EngineSrcLocation() % HYGUIPATH_ProjGenDir % "src");
	fileInfoList = projGenSrcDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
	for(int i = 0; i < fileInfoList.size(); ++i)
		QFile::copy(fileInfoList[i].absoluteFilePath(), srcDir.absoluteFilePath(fileInfoList[i].fileName()));

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// Rename the copied source files if needed
	fileInfoList = QDir(GetProjDirPath()).entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
	fileInfoList += srcDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
	for(int i = 0; i < fileInfoList.size(); ++i)
	{
		if(fileInfoList[i].fileName().contains("%HY_CLASS%"))
		{
			QFile file(fileInfoList[i].absoluteFilePath());
			QString sNewFileName = fileInfoList[i].fileName().replace("%HY_CLASS%", ui->txtClassName->text());
			file.rename(fileInfoList[i].absoluteDir().absolutePath() % "/Game/" % sNewFileName);
			file.close();
		}
	}
	// Then replace the variable contents of the copied source files
	fileInfoList = QDir(GetProjDirPath()).entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
	fileInfoList += srcDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
	fileInfoList += QDir(srcDir.absoluteFilePath("Game")).entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
	QTextCodec *pCodec = QTextCodec::codecForLocale();
	for(int i = 0; i < fileInfoList.size(); ++i)
	{
		QFile file(fileInfoList[i].absoluteFilePath());
		if(!file.open(QFile::ReadOnly))
		{
			HyGuiLog("Error reading " % file.fileName() % " when generating source: " % file.errorString(), LOGTYPE_Error);
			return;
		}

		QString sContents = pCodec->toUnicode(file.readAll());
		file.close();

		sContents.replace("%HY_TITLE%", ui->txtTitleName->text());
		sContents.replace("%HY_CLASS%", ui->txtClassName->text());
		sContents.replace("%HY_PROJDIR%", GetProjDirPath());
		sContents.replace("%HY_RELSRCDIR%", QDir(GetProjDirPath()).relativeFilePath(srcDir.absolutePath()));
		sContents.replace("%HY_RELHARMONYDIR%", QDir(GetProjDirPath()).relativeFilePath(MainWindow::EngineSrcLocation()));
		sContents.replace("%HY_RELDATADIR%", ui->wgtDataDir->GetRelPath());
		sContents.replace("%HY_DEPENDENCIES_ADD%", GetDependAdd());
		sContents.replace("%HY_DEPENDENCIES_LINK%", GetDependLink());

		if(!file.open(QFile::WriteOnly))
		{
			HyGuiLog("Error writing to " % file.fileName() % " when generating source: " % file.errorString(), LOGTYPE_Error);
			return;
		}
		file.write(pCodec->fromUnicode(sContents));
		file.close();
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
		ui->txtClassName->setText(sFixedForClass);

	ui->lblAppendHint->setText("Appends \"/" % ui->txtClassName->text() % "/\" to above");
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
	ui->wgtSrcDir->Setup("Source", "src", GetProjDirPath());
	ui->wgtBuildDir->Setup("Build", "build", GetProjDirPath());

	for(auto srcDep : m_SrcDependencyList)
		srcDep->ResetProjDir(GetProjDirPath());
}

void DlgNewProject::UpdateSrcDependencies()
{
	if(m_SrcDependencyList.empty())
	{
		m_SrcDependencyList.append(new WgtSrcDependency(this, GetProjDirPath(), ui->grpAdvanced));
		ui->grpAdvanced->layout()->addWidget(m_SrcDependencyList[m_SrcDependencyList.count() - 1]);
		connect(m_SrcDependencyList[m_SrcDependencyList.count() - 1], &WgtSrcDependency::OnDirty, this, &DlgNewProject::ErrorCheck);
		return;
	}

	if(ui->lytSrcDependencies->count() != m_SrcDependencyList.count())
	{
		if(ui->lytSrcDependencies->count() < m_SrcDependencyList.count())
		{
			for(int i = ui->lytSrcDependencies->count(); i < m_SrcDependencyList.count(); ++i)
				ui->lytSrcDependencies->addWidget(m_SrcDependencyList[i]);
		}
		else
		{
			for(int i = m_SrcDependencyList.count(); i < ui->lytSrcDependencies->count() - 1; ++i) // Keep at least '1'
				delete ui->lytSrcDependencies->takeAt(i);
		}

		// Remove and re-add the layout that holds SrcDependency widgets. Otherwise it jumbles them together.
		//ui->grpAdvanced->layout()->removeItem(ui->lytSrcDependencies);
		//ui->grpAdvanced->layout()->addItem(ui->lytSrcDependencies);
	}

	ErrorCheck();
}

QString DlgNewProject::GetDependAdd()
{
	QString sReturn = "";
	for(auto srcDep : m_SrcDependencyList)
	{
		if(srcDep->IsActivated() == false)
			continue;

		sReturn += "add_subdirectory(\"";
		sReturn += srcDep->GetRelPath();
		sReturn += "\" \"";
		sReturn += srcDep->GetProjectName();
		sReturn += "\")\n";
	}

	return sReturn;
}

QString DlgNewProject::GetDependLink()
{
	QString sReturn = "";
	for(auto srcDep : m_SrcDependencyList)
	{
		if(srcDep->IsActivated() == false)
			continue;

		sReturn += "\"";
		sReturn += srcDep->GetProjectName();
		sReturn += "\" ";
	}

	return sReturn;
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
		if(ui->txtClassName->text().isEmpty())
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

		sError = ui->wgtSrcDir->GetError();
		if(sError.isEmpty() == false)
		{
			ui->lblError->setText(sError);
			bIsError = true;
			break;
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		for(auto srcDep : m_SrcDependencyList)
		{
			sError = srcDep->GetError();
			if(sError.isEmpty() == false)
			{
				ui->lblError->setText(sError);
				bIsError = true;
				break;
			}
		}
		if(sError.isEmpty() == false)
			break;

	}while(false);

	ui->lblError->setVisible(bIsError);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!bIsError);
}
