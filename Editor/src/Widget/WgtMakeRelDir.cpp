/**************************************************************************
 *	WgtMakeRelDir.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "WgtMakeRelDir.h"
#include "ui_WgtMakeRelDir.h"

#include <QFileDialog>

WgtMakeRelDir::WgtMakeRelDir(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::WgtMakeRelDir)
{
	ui->setupUi(this);
}

WgtMakeRelDir::~WgtMakeRelDir()
{
	delete ui;
}

void WgtMakeRelDir::Setup(QString sTitle, QString sDefaultName, QString sAbsProjectPath)
{
	m_sTitle = sTitle;

	ui->txtDirName->blockSignals(true);
	{
		ui->txtDirName->setText(sDefaultName);
		ui->txtDirName->setValidator(HyGlobal::FileNameValidator());
	}
	ui->txtDirName->blockSignals(false);

	m_sAbsProjPath = sAbsProjectPath;
	m_sAbsParentDirPath = m_sAbsProjPath;

	ui->lblRelative->setText(sTitle % " relative location:");
	ui->lblDirName->setText(sTitle % " dir name:");

	Q_EMIT OnDirty();
}

QString WgtMakeRelDir::GetRelPath() const
{
	return ui->txtRelativePath->text() % "/" % ui->txtDirName->text();
}

QString WgtMakeRelDir::GetAbsPath() const
{
	return m_sAbsParentDirPath % "/" % ui->txtDirName->text();
}

void WgtMakeRelDir::Refresh()
{
	QDir projDir(m_sAbsProjPath);

	// Update ui->txtRelativePath using 'm_sAbsParentDirPath'
	ui->txtRelativePath->setText(projDir.relativeFilePath(m_sAbsParentDirPath));
	if(ui->txtRelativePath->text().isEmpty())
		ui->txtRelativePath->setText(".");

	Q_EMIT OnDirty();
}

QString WgtMakeRelDir::GetError()
{
	QString sError;
	do
	{
		if(ui->txtDirName->text().isEmpty())
		{
			sError = "Error: " % m_sTitle % " directory name cannot be blank.";
			break;
		}

		QDir parentDir(m_sAbsParentDirPath);
		QDir newDir(parentDir.absolutePath() + "/" + ui->txtDirName->text());
		if(parentDir.exists() && newDir.exists() && newDir.isEmpty() == false)
		{
			sError = "Error: " % m_sTitle % " directory does not point to an empty folder.";
			break;
		}
		
	} while(false);

	return sError;
}

void WgtMakeRelDir::on_btnBrowseDir_clicked()
{
	QFileDialog *pDlg = new QFileDialog(this, "Choose where to place the new " % m_sTitle % " directory");

	pDlg->setDirectory(m_sAbsParentDirPath);
	pDlg->setFileMode(QFileDialog::Directory);
	pDlg->setOption(QFileDialog::ShowDirsOnly, true);
	pDlg->setViewMode(QFileDialog::Detail);
	pDlg->setWindowModality(Qt::ApplicationModal);
	pDlg->setModal(true);

	if(pDlg->exec() == QDialog::Accepted)
	{
		m_sAbsParentDirPath = pDlg->selectedFiles()[0];
		Refresh();
	}
}

void WgtMakeRelDir::on_txtDirName_textChanged(const QString &arg1)
{
	Refresh();
}
