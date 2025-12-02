/**************************************************************************
 *	DlgNewBuild.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "DlgNewBuild.h"
#include "ui_DlgNewBuild.h"
#include "Project.h"
#include "MainWindow.h"

#include <QDir>
#include <QFileDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QProcess>

DlgNewBuild::DlgNewBuild(Project &projectRef, QWidget *parent) :
	QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
	m_ProjectRef(projectRef),
	ui(new Ui::DlgNewBuild)
{
	ui->setupUi(this);

	ui->txtBuildName->setValidator(HyGlobal::CodeNameValidator());
	
	// Acquire a list of valid CMake generators using the --help output
	QString sCMakeApp = "cmake";
	QStringList sArgList;
	sArgList << "--help";
	QProcess *pCMakeProcess = new QProcess(this);
	pCMakeProcess->start(sCMakeApp, sArgList);
	pCMakeProcess->waitForFinished();
	if(pCMakeProcess->error() != QProcess::UnknownError)
	{
		m_bCMakeFound = false;
	}
	else
	{
		m_bCMakeFound = true;

		m_sCMakeHelp = pCMakeProcess->readAllStandardOutput();
		
		// Isolate the generators in --help
		QString sCMakeGeneratorOutput = m_sCMakeHelp;
		sCMakeGeneratorOutput = sCMakeGeneratorOutput.remove(0, sCMakeGeneratorOutput.indexOf(QRegularExpression("^Generators", QRegularExpression::MultilineOption)));

		// Isolate the options in --help
		m_sCMakeHelp = m_sCMakeHelp.remove(0, m_sCMakeHelp.indexOf(QRegularExpression("^Options", QRegularExpression::MultilineOption)));
		m_sCMakeHelp.chop(m_sCMakeHelp.size() - m_sCMakeHelp.indexOf(QRegularExpression("^Generators", QRegularExpression::MultilineOption)));

		// Use regular expressions to find generators with the --help output
		QStringList sGeneratorList;
		QRegularExpression reGenerator("^  (\\w[^=]*)", QRegularExpression::MultilineOption);
		QRegularExpressionMatchIterator iter = reGenerator.globalMatch(sCMakeGeneratorOutput);
		while(iter.hasNext())
		{
			QRegularExpressionMatch match = iter.next();
			if(match.hasMatch())
				sGeneratorList << match.captured(1);
		}
		QRegularExpression reDefaultGen("^\\* (\\w[^=]*)", QRegularExpression::MultilineOption);
		QRegularExpressionMatch match = reDefaultGen.match(sCMakeGeneratorOutput);
		if(match.hasMatch())
			sGeneratorList.prepend(match.captured(1));

		for(int i = 0; i < sGeneratorList.size(); ++i)
		{
			int iArchTag = sGeneratorList[i].lastIndexOf("[arch]");
			if(iArchTag != -1)
				sGeneratorList[i].truncate(iArchTag);

			sGeneratorList[i] = sGeneratorList[i].trimmed();
		}

		ui->cmbCMake->insertItems(0, sGeneratorList);
	}

	ui->lblError->setStyleSheet("QLabel { background-color : red; color : black; }");

	ErrorCheck();
}

DlgNewBuild::~DlgNewBuild()
{
	delete ui;
}

QString DlgNewBuild::GetAbsBuildDir() const
{
	return m_ProjectRef.GetBuildAbsPath() % ui->txtBuildName->text();
}

QStringList DlgNewBuild::GetProcOptions() const
{
	return QStringList()	<< "-G"
							<< ui->cmbCMake->currentText()
							<< ui->txtCMakeOptions->text().split(' ', Qt::SkipEmptyParts)
							<< "-S"
							<< m_ProjectRef.GetSourceAbsPath()
							<< "-B"
							<< GetAbsBuildDir();
}

void DlgNewBuild::on_txtBuildName_textChanged(const QString &arg1)
{
	ErrorCheck();
}

void DlgNewBuild::on_buttonBox_accepted()
{
	
}

void DlgNewBuild::on_radCMake_clicked()
{
	ui->cmbCMake->setEnabled(true);
	ui->lblCMakeOptions->setEnabled(true);
	ui->txtCMakeOptions->setEnabled(true);
	ui->btnCMakeHelp->setEnabled(true);

	ErrorCheck();
}

void DlgNewBuild::on_radCMakeGui_clicked()
{
	ui->cmbCMake->setEnabled(false);
	ui->lblCMakeOptions->setEnabled(false);
	ui->txtCMakeOptions->setEnabled(false);
	ui->btnCMakeHelp->setEnabled(false);

	ErrorCheck();
}

void DlgNewBuild::on_cmbCMake_currentIndexChanged(int iIndex)
{
	if(ui->cmbCMake->currentText().contains("Visual"))
		ui->txtCMakeOptions->setText("-A x64");
	else
		ui->txtCMakeOptions->setText("");
}

void DlgNewBuild::on_btnCMakeHelp_clicked()
{
	QMessageBox::information(this, "CMake Help", m_sCMakeHelp, QMessageBox::Ok);
}

void DlgNewBuild::ErrorCheck()
{
	bool bIsError = false;
	do
	{
		if(m_bCMakeFound == false)
		{
			ui->lblError->setText("Error: CMake was not found");
			bIsError = true;
			break;
		}

		QDir buildDir(GetAbsBuildDir());
		if(buildDir.exists())
		{
			ui->lblError->setText("Build directory already exists");
			bIsError = true;
			break;
		}

	} while(false);

	ui->lblError->setVisible(bIsError);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!bIsError);
}
