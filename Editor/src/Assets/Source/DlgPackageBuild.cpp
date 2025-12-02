/**************************************************************************
 *	DlgPackageBuild.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "DlgPackageBuild.h"
#include "ui_DlgPackageBuild.h"
#include "Project.h"
#include "MainWindow.h"

#include <QDir>
#include <QFileDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QProcess>

DlgPackageBuild::DlgPackageBuild(Project &projectRef, QWidget *parent) :
	QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
	m_ProjectRef(projectRef),
	ui(new Ui::DlgPackageBuild)
{
	ui->setupUi(this);

	ui->lblError->setStyleSheet("QLabel { background-color : red; color : black; }");
	ui->stackedWidget->setCurrentIndex(PACKAGE_Desktop);

	ErrorCheck();
}

DlgPackageBuild::~DlgPackageBuild()
{
	delete ui;
}

PackageType DlgPackageBuild::GetPackageType() const
{
	if(ui->radDesktop->isChecked())
		return PACKAGE_Desktop;
	else if(ui->radBrowser->isChecked())
		return PACKAGE_Browser;

	return PACKAGE_Unknown;
}

QString DlgPackageBuild::GetAbsPackageDir() const
{
	return m_ProjectRef.GetDirPath() % "bin/" % ui->txtDirName->text() % "/";
}

QString DlgPackageBuild::GetProc() const
{
	if(ui->radDesktop->isChecked())
		return ""; // TODO: Implement desktop packaging process
	else if(ui->radBrowser->isChecked())
	{
#ifdef Q_OS_WIN
		return MainWindow::EngineSrcLocation() % HYGUIPATH_EditorDataDir % "embuild.bat";
#else
		HyGuiLog("DlgPackageBuild::GetProc() not implemented on this platform", LOGTYPE_Error);
#endif
	}

	return QString();
}

QStringList DlgPackageBuild::GetProcOptions() const
{
	if(ui->radDesktop->isChecked())
	{
		// TODO: Implement desktop packaging options
		return QStringList();
	}
	else if(ui->radBrowser->isChecked())
	{
		QDir packageDir(GetAbsPackageDir());
		QString sFormattedPath = packageDir.absolutePath();
#ifdef Q_OS_WIN
		sFormattedPath.replace("/", "\\");	// batch file requires Windows native directory separators
#endif

		QString sCMakeCmds = "";
		if(ui->chkBrowserDebug->isChecked())
			sCMakeCmds += "-DHYBUILD_DebugEmscripten"; // Batch files replace '=' with space if you don't use quotes

		return QStringList()	<< sFormattedPath
								<< QDir::cleanPath(ui->txtEmscriptenSdk->text())
								<< QDir::cleanPath(packageDir.relativeFilePath(m_ProjectRef.GetSourceAbsPath()))
								<< sCMakeCmds;
	}

	return QStringList();
}

void DlgPackageBuild::on_buttonBox_accepted()
{
	
}

void DlgPackageBuild::on_radDesktop_clicked()
{
	ui->stackedWidget->setCurrentIndex(PACKAGE_Desktop);
	ErrorCheck();
}

void DlgPackageBuild::on_radBrowser_clicked()
{
	ui->stackedWidget->setCurrentIndex(PACKAGE_Browser);
	ErrorCheck();
}

void DlgPackageBuild::on_txtEmscriptenSdk_textChanged(const QString &arg1)
{
	ErrorCheck();
}

void DlgPackageBuild::on_btnEmscriptenSdkBrowse_clicked()
{
	QFileDialog *pDlg = new QFileDialog(this, "Choose the \"root\" location of the Emscripten SDK");
	pDlg->setFileMode(QFileDialog::Directory);
	pDlg->setOption(QFileDialog::ShowDirsOnly, true);
	pDlg->setViewMode(QFileDialog::Detail);
	pDlg->setWindowModality(Qt::ApplicationModal);
	pDlg->setModal(true);

	if(pDlg->exec() == QDialog::Accepted)
	{
		QString sDir = pDlg->selectedFiles()[0];
		ui->txtEmscriptenSdk->setText(sDir);
	}

	ErrorCheck();
}

void DlgPackageBuild::ErrorCheck()
{
	bool bIsError = false;
	do
	{
		if(ui->txtDirName->text().isEmpty())
		{
			ui->lblError->setText("Package output directory name cannot be blank");
			bIsError = true;
			break;
		}

		QDir packageDir(GetAbsPackageDir());
		if(packageDir.exists())
		{
			ui->lblError->setText("Package output directory already exists");
			bIsError = true;
			break;
		}

		if(ui->stackedWidget->currentIndex() == PACKAGE_Browser)
		{
			QDir sdkDir(ui->txtEmscriptenSdk->text());
			if(sdkDir.exists() == false)
			{
				ui->lblError->setText("Error: Emscripten SDK location doesn't exist");
				bIsError = true;
				break;
			}

			if(!sdkDir.exists("emsdk.py") ||
				!sdkDir.exists("emsdk_env.sh") ||
				!sdkDir.exists("emcmdprompt.bat"))
			{
				ui->lblError->setText("Error: Emscripten SDK location is not valid");
				bIsError = true;
				break;
			}
		}

	} while(false);

	ui->lblError->setVisible(bIsError);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!bIsError);
}
