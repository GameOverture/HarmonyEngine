/**************************************************************************
 *	WgtMakeRelDir.cpp
 *
 *	Harmony Engine - HyEditor
 *	Copyright (c) 2020 Jason Knobler
 *
 *	GPL/LGPL License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
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

void WgtMakeRelDir::Setup(QString sTitle, QString sAbsBasePath, QString sDefaultRelativePath, bool bMustBeEmptyFolder)
{
	m_sTitle = sTitle;
	m_bMustBeEmptyFolder = bMustBeEmptyFolder;

	m_sAbsBasePath = sAbsBasePath;

	if(sDefaultRelativePath.isEmpty() || sDefaultRelativePath.compare('.') == 0)
		HyGuiLog("WgtMakeRelDir::Setup() - invalid sDefaultRelativePath passed in", LOGTYPE_Error);

	QDir tmp(sDefaultRelativePath);

	ui->txtDirName->blockSignals(true);
	{
		ui->txtDirName->setText(tmp.dirName());
		ui->txtDirName->setValidator(HyGlobal::FileNameValidator());
	}
	ui->txtDirName->blockSignals(false);

	ui->txtRelativePath->blockSignals(true);
	{
		// Go up one directory and set tmp to it, can't use cdUp() because the directory may not exist
		QString sNewPath = tmp.path();
		sNewPath = sNewPath.replace('\\', '/');
		if(sNewPath.endsWith('/'))
			sNewPath.chop(1);
		if(sNewPath.lastIndexOf('/') != -1)
			sNewPath = sNewPath.left(sNewPath.lastIndexOf('/'));
		else
			sNewPath = ".";

		ui->txtRelativePath->setText(sNewPath);
	}
	ui->txtRelativePath->blockSignals(false);

	ui->lblRelative->setText(m_sTitle % " relative location:");
	ui->lblDirName->setText(m_sTitle % " dir name:");

	Refresh();
}

QString WgtMakeRelDir::GetRelPath() const
{
	return ui->txtRelativePath->text() % "/" % ui->txtDirName->text();
}

QString WgtMakeRelDir::GetAbsPath() const
{
	return m_sAbsBasePath % "/" % GetRelPath();// ui->txtDirName->text();
}

void WgtMakeRelDir::Refresh()
{
	if(ui->txtRelativePath->text().isEmpty())
		ui->txtRelativePath->setText(".");
	else
	{
		QDir baseDir(m_sAbsBasePath);
		ui->txtRelativePath->setText(baseDir.relativeFilePath(baseDir.absoluteFilePath(ui->txtRelativePath->text())));
	}

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

		if(m_bMustBeEmptyFolder)
		{
			QDir parentDir(m_sAbsBasePath);
			QDir newDir(parentDir.absolutePath() + "/" + ui->txtDirName->text());
			if(parentDir.exists() && newDir.exists() && newDir.isEmpty() == false)
			{
				sError = "Error: " % m_sTitle % " directory does not point to an empty folder.";
				break;
			}
		}
		
	} while(false);

	return sError;
}

void WgtMakeRelDir::on_btnBrowseDir_clicked()
{
	QFileDialog *pDlg = new QFileDialog(this, "Choose where to place the new " % m_sTitle % " directory");

	QDir dir(m_sAbsBasePath);
	dir.cdUp();
	
	pDlg->setDirectory(dir.absolutePath());
	pDlg->setFileMode(QFileDialog::Directory);
	pDlg->setOption(QFileDialog::ShowDirsOnly, true);
	pDlg->setViewMode(QFileDialog::Detail);
	pDlg->setWindowModality(Qt::ApplicationModal);
	pDlg->setModal(true);

	if(pDlg->exec() == QDialog::Accepted)
	{
		ui->txtRelativePath->setText(QDir(m_sAbsBasePath).relativeFilePath(pDlg->selectedFiles()[0]));
		Refresh();
	}
}

void WgtMakeRelDir::on_txtDirName_textChanged(const QString &arg1)
{
	Refresh();
}
