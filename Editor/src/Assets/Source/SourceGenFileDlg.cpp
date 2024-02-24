/**************************************************************************
 *	SourceGenFileDlg.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2021 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "SourceGenFileDlg.h"
#include "ui_SourceGenFileDlg.h"

#include <QPushButton>

SourceGenFileDlg::SourceGenFileDlg(QStringList sEditorEntityList, QWidget *pParent /*= nullptr*/) :
	QDialog(pParent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
	ui(new Ui::SourceGenFileDlg)
{
	ui->setupUi(this);

	ui->txtClassName->setValidator(HyGlobal::CodeNameValidator());
	ui->txtHFile->setValidator(HyGlobal::FileNameValidator());
	ui->txtCppFile->setValidator(HyGlobal::FileNameValidator());
	ui->txtBaseClass->setValidator(HyGlobal::CodeNameValidator());

	ui->lblError->setStyleSheet("QLabel { background-color : red; color : black; }");

	for(QString sEditorEntity : sEditorEntityList)
		ui->editorEntitiesList->addItem(sEditorEntity);

	ErrorCheck();
}

SourceGenFileDlg::~SourceGenFileDlg()
{
	delete ui;
}

QString SourceGenFileDlg::GetCodeClassName() const
{
	return ui->txtClassName->text();
}

QString SourceGenFileDlg::GetHeaderFileName() const
{
	return ui->txtHFile->text();
}

QString SourceGenFileDlg::GetCppFileName() const
{
	return ui->txtCppFile->text();
}

QString SourceGenFileDlg::GetBaseClassName() const
{
	if(ui->radBaseSpecify->isChecked())
		return ui->txtBaseClass->text();
	else
		return "hy::" + ui->editorEntitiesList->currentItem()->text();
}

bool SourceGenFileDlg::IsEntityBaseClass() const
{
	return ui->chkHyEntityBaseClass->isChecked();
}

void SourceGenFileDlg::on_txtClassName_textChanged(const QString &arg1)
{
	QString sFixedForClass = arg1;
	HyGlobal::FileNameValidator()->fixup(sFixedForClass);

	int iPos;
	if(QValidator::Invalid != HyGlobal::FileNameValidator()->validate(sFixedForClass, iPos))
	{
		ui->txtHFile->setText(sFixedForClass);
		ui->txtCppFile->setText(sFixedForClass);
	}

	ErrorCheck();
}

void SourceGenFileDlg::on_txtHFile_textChanged(const QString &arg1)
{
	ErrorCheck();
}

void SourceGenFileDlg::on_txtCppFile_textChanged(const QString &arg1)
{
	ErrorCheck();
}

void SourceGenFileDlg::on_txtBaseClass_textChanged(const QString &arg1)
{
	if(ui->txtBaseClass->text().isEmpty())
		ui->chkHyEntityBaseClass->setEnabled(false);
	else
		ui->chkHyEntityBaseClass->setEnabled(true);

	ErrorCheck();
}

void SourceGenFileDlg::on_radBaseSpecify_toggled(bool bChecked)
{
	ui->txtBaseClass->setEnabled(bChecked);
	ui->chkHyEntityBaseClass->setEnabled(bChecked);
	ui->editorEntitiesList->setEnabled(!bChecked);
	ErrorCheck();
}

void SourceGenFileDlg::on_radBaseEditor_toggled(bool bChecked)
{
	ui->txtBaseClass->setEnabled(!bChecked);
	ui->chkHyEntityBaseClass->setEnabled(!bChecked);
	ui->editorEntitiesList->setEnabled(bChecked);
	ErrorCheck();
}

void SourceGenFileDlg::on_editorEntitiesList_itemSelectionChanged()
{
	ErrorCheck();
}

void SourceGenFileDlg::ErrorCheck()
{
	bool bIsError = false;
	do
	{
		if(ui->txtClassName->text().isEmpty())
		{
			ui->lblError->setText("'Class Name' cannot be blank");
			bIsError = true;
			break;
		}

		if(ui->txtHFile->text().isEmpty())
		{
			ui->lblError->setText(".h File name cannot be blank");
			bIsError = true;
			break;
		}

		if(ui->txtCppFile->text().isEmpty())
		{
			ui->lblError->setText(".cpp File name cannot be blank");
			bIsError = true;
			break;
		}

		if(ui->radBaseEditor->isChecked())
		{
			if(ui->editorEntitiesList->currentItem() == nullptr || ui->editorEntitiesList->currentItem()->text().isEmpty())
			{
				ui->lblError->setText("Invalid editor entity selected");
				bIsError = true;
				break;
			}
		}

	}while(false);

	ui->lblError->setVisible(bIsError);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!bIsError);
}
