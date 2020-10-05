/**************************************************************************
 *	WgtSrcDependency.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "WgtSrcDependency.h"
#include "ui_WgtSrcDependency.h"
#include "DlgNewProject.h"

#include <QFileDialog>

WgtSrcDependency::WgtSrcDependency(DlgNewProject *pParentDlg, QString sAbsProjectPath, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::WgtSrcDependency),
	m_pParentDlg(pParentDlg),
	m_sAbsProjPath(sAbsProjectPath),
	m_sAbsSrcDepPath(sAbsProjectPath)
{
	ui->setupUi(this);
	ui->stackedWidget->setCurrentIndex(0);

	ui->txtProjectName->setValidator(HyGlobal::FileNameValidator());
}

WgtSrcDependency::~WgtSrcDependency()
{
	delete ui;
}

bool WgtSrcDependency::IsActivated() const
{
	return ui->stackedWidget->currentIndex() != 0;
}

QString WgtSrcDependency::GetProjectName() const
{
	return ui->txtProjectName->text();
}

QString WgtSrcDependency::GetRelPath() const
{
	return ui->txtRelativePath->text();
}

QString WgtSrcDependency::GetAbsPath() const
{
	return m_sAbsSrcDepPath;
}

void WgtSrcDependency::ResetProjDir(QString sNewProjDirPath)
{
	if(m_sAbsSrcDepPath == m_sAbsProjPath)
		m_sAbsSrcDepPath = sNewProjDirPath;

	m_sAbsProjPath = sNewProjDirPath;

	Refresh();
}

void WgtSrcDependency::Refresh()
{
	QDir projDir(m_sAbsProjPath);

	// Update ui->txtRelativePath using 'm_sAbsSrcDepPath'
	ui->txtRelativePath->setText(projDir.relativeFilePath(m_sAbsSrcDepPath));
	if(ui->txtRelativePath->text().isEmpty())
		ui->txtRelativePath->setText(".");

	Q_EMIT OnDirty();
}

QString WgtSrcDependency::GetError()
{
	QString sError; // Empty string indicates no error

	if(ui->stackedWidget->currentIndex() == 0)
		return sError;

	do
	{
		if(ui->txtProjectName->text().isEmpty())
		{
			sError = "Error: CMake Project Name name cannot be blank.";
			break;
		}

		QDir srcDepDir(m_sAbsSrcDepPath);
		if(srcDepDir.exists() == false)
		{
			sError = "Error: Library directory does not exist.";
			break;
		}

		QFileInfoList filesInDir = srcDepDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
		bool bFoundCMakeLists = false;
		for(QFileInfo file : filesInDir)
		{
			if(file.baseName().compare("CMakeLists", Qt::CaseInsensitive) == 0)
			{
				bFoundCMakeLists = true;
				break;
			}
		}
		if(bFoundCMakeLists == false)
		{
			sError = "Error: Library directory does contain a CMakeLists.txt file.";
			break;
		}
		
	} while(false);

	return sError;
}

void WgtSrcDependency::on_btnAddDependency_clicked()
{
	ui->stackedWidget->setCurrentIndex(1);
	m_pParentDlg->AddSrcDep();
}

void WgtSrcDependency::on_btnRemoveDependency_clicked()
{
	ui->stackedWidget->setCurrentIndex(0);
	m_pParentDlg->RemoveSrcDep(this);
}

void WgtSrcDependency::on_btnBrowseDir_clicked()
{
	QFileDialog *pDlg = new QFileDialog(this, "Choose where the library directory is located");

	pDlg->setDirectory(m_sAbsSrcDepPath);
	pDlg->setFileMode(QFileDialog::Directory);
	pDlg->setOption(QFileDialog::ShowDirsOnly, true);
	pDlg->setViewMode(QFileDialog::Detail);
	pDlg->setWindowModality(Qt::ApplicationModal);
	pDlg->setModal(true);

	if(pDlg->exec() == QDialog::Accepted)
	{
		m_sAbsSrcDepPath = pDlg->selectedFiles()[0];
		Refresh();
	}
}

void WgtSrcDependency::on_txtProjectName_textChanged(const QString &arg1)
{
	Refresh();
}
