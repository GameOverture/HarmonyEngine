/**************************************************************************
 *	DlgSetEngineLocation.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "DlgSetEngineLocation.h"
#include "ui_DlgSetEngineLocation.h"

#include <QFileDialog>

DlgSetEngineLocation::DlgSetEngineLocation(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DlgSetEngineLocation)
{
	ui->setupUi(this);
	
	m_pFileModel = new QFileSystemModel(this);
	m_pFileModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
	m_pFileModel->setRootPath(QDir::currentPath());
	
	ui->listView->setModel(m_pFileModel);

	ErrorCheck();
}

DlgSetEngineLocation::~DlgSetEngineLocation()
{
	delete ui;
}

QString DlgSetEngineLocation::SelectedDir()
{
	return ui->txtCurDirectory->text();
}

void DlgSetEngineLocation::on_listView_doubleClicked(const QModelIndex &index)
{
	if(ui->buttonBox->button(QDialogButtonBox::Ok)->isEnabled() == false)
		ui->listView->setRootIndex(index);
	
	m_DirPathUndoStack.push(new DlgSetEngineLocationUndoCmd(m_sDirPath, m_pFileModel->fileInfo(index).absoluteFilePath()));

	ui->txtCurDirectory->setText(m_sDirPath);

	ErrorCheck();
}

void DlgSetEngineLocation::on_txtCurDirectory_editingFinished()
{
	m_DirPathUndoStack.push(new DlgSetEngineLocationUndoCmd(m_sDirPath, ui->txtCurDirectory->text()));

	ui->listView->setRootIndex(m_pFileModel->setRootPath(m_sDirPath));

	ErrorCheck();
}

void DlgSetEngineLocation::ErrorCheck()
{
	QString sProjDir = ui->txtCurDirectory->text();

	bool bIsError = false;
	do
	{
		if(sProjDir.isEmpty())
		{
			ui->lblError->setText("Error: The directory path cannot be blank");
			bIsError = true;
			break;
		}

		QDir projDir(sProjDir);
		if(projDir.exists() == false)
		{
			ui->lblError->setText("Error: This directory does not exist");
			bIsError = true;
			break;
		}

		if(HyGlobal::IsEngineDirValid(projDir) == false)
		{
			ui->lblError->setText("Error: This is not the Harmony Engine project directory");
			bIsError = true;
			break;
		}
	}while(false);

	if(bIsError)
		ui->lblError->setStyleSheet("QLabel { background-color : red; color : black; }");
	else
	{
		ui->lblError->setStyleSheet("QLabel { color : black; }");
		ui->lblError->setText("");
	}
	ui->lblError->setVisible(bIsError);
	//ui->buttonBox->button(QDialogButtonBox::Ok);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!bIsError);
}

//void DlgSetEngineLocation::on_listView_clicked(const QModelIndex &index)
//{
//	QString sPath = m_pFileModel->fileInfo(index).absoluteFilePath();
//	ui->txtCurDirectory->setText(sPath);
//
//	ErrorCheck();
//}

void DlgSetEngineLocation::on_btnBack_clicked()
{
	if(m_DirPathUndoStack.canUndo() == false)
		return;

	m_DirPathUndoStack.undo();

	ui->txtCurDirectory->setText(m_sDirPath);
	ui->listView->setRootIndex(m_pFileModel->setRootPath(m_sDirPath));

	ErrorCheck();
}

void DlgSetEngineLocation::on_btnForward_clicked()
{
	if(m_DirPathUndoStack.canRedo() == false)
		return;

	m_DirPathUndoStack.redo();

	ui->txtCurDirectory->setText(m_sDirPath);
	ui->listView->setRootIndex(m_pFileModel->setRootPath(m_sDirPath));

	ErrorCheck();
}

void DlgSetEngineLocation::on_btnUp_clicked()
{
	QDir dir(m_sDirPath);
	if(dir.cdUp() == false)
		return;

	m_DirPathUndoStack.push(new DlgSetEngineLocationUndoCmd(m_sDirPath, dir.absolutePath()));
	ui->listView->setRootIndex(m_pFileModel->setRootPath(m_sDirPath));
	ui->txtCurDirectory->setText(m_sDirPath);

	ErrorCheck();
}
