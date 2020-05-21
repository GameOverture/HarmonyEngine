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

#include <QDir>
#include <QFileDialog>
#include <QPushButton>

DlgNewBuild::DlgNewBuild(Project &projectRef, QWidget *parent) :
	QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
	m_ProjectRef(projectRef),
	ui(new Ui::DlgNewBuild)
{
	ui->setupUi(this);
	ui->wgtBuildDir->Setup("Build", "build", m_ProjectRef.GetDirPath());

	ui->lblError->setStyleSheet("QLabel { background-color : red; color : black; }");
	connect(ui->wgtBuildDir, &WgtMakeRelDir::OnDirty, this, &DlgNewBuild::ErrorCheck);
	ErrorCheck();
}

DlgNewBuild::~DlgNewBuild()
{
	delete ui;
}

void DlgNewBuild::on_buttonBox_accepted()
{
	
}

void DlgNewBuild::ErrorCheck()
{
	bool bIsError = false;
	do
	{
		QString sError = ui->wgtBuildDir->GetError();
		if(sError.isEmpty() == false)
		{
			ui->lblError->setText(sError);
			bIsError = true;
			break;
		}
	} while(false);

	ui->lblError->setVisible(bIsError);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!bIsError);
}
