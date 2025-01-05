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
#include "SourceSettingsDlg.h"
#include "ui_WgtSrcDependency.h"
#include "DlgNewProject.h"

#include <QFileDialog>

WgtSrcDependency::WgtSrcDependency(SourceSettingsDlg *pParentDlg, QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::WgtSrcDependency),
	m_pParentDlg(pParentDlg)
{
	ui->setupUi(this);
	ui->txtProjectName->setValidator(HyGlobal::FileNameValidator());
}

/*virtual*/ WgtSrcDependency::~WgtSrcDependency()
{
	delete ui;
}

QString WgtSrcDependency::GetProjectName() const
{
	return ui->txtProjectName->text();
}

QString WgtSrcDependency::GetAbsPath() const
{
	return m_sAbsSrcDepPath;
}

QString WgtSrcDependency::GetOptions() const
{
	return ui->txtOptions->toPlainText();
}

void WgtSrcDependency::Set(QString sProjectName, QString sDependAbsPath, QString sOptions)
{
	ui->txtProjectName->setText(sProjectName);
	m_sAbsSrcDepPath = sDependAbsPath;
	ui->txtOptions->setPlainText(sOptions);

	Refresh();
}

void WgtSrcDependency::Refresh()
{
	QDir projDir(m_pParentDlg->GetProjectDir());

	// Update ui->txtRelativePath to be the relative path from the HyProj location
	// NOTE: this relative location isn't useful in code because the CMakeLists.txt works from the meta/Source directory
	//       it is just here for the user's of the editor's sake
	ui->txtRelativePath->setText(projDir.relativeFilePath(m_sAbsSrcDepPath));
	if(ui->txtRelativePath->text().isEmpty())
		ui->txtRelativePath->setText(".");

	Q_EMIT OnDirty();
}

QString WgtSrcDependency::GetError()
{
	QString sError; // Empty string indicates no error

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

void WgtSrcDependency::on_btnRemoveDependency_clicked()
{
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
