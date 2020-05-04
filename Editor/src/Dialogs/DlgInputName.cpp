/**************************************************************************
 *	DlgInputName.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "DlgInputName.h"
#include "ui_DlgInputName.h"

#include <QPushButton>
#include <QStringBuilder>

void DlgInputName::CtorInit(QString sDlgTitle, QString sCurName)
{
	ui->setupUi(this);

	setWindowTitle(sDlgTitle);
	setWindowIcon(QIcon(":/icons16x16/generic-rename.png"));

	ui->txtName->setValidator(HyGlobal::FileNameValidator());
	ui->txtName->setText(sCurName);
	ui->txtName->selectAll();

	ui->lblName->setText("Name:");
}

DlgInputName::DlgInputName(const QString sDlgTitle, QString sCurName, QWidget *pParent /*= 0*/) :
	QDialog(pParent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
	ui(new Ui::DlgInputName)
{
	CtorInit(sDlgTitle, sCurName);
}

DlgInputName::DlgInputName(const QString sDlgTitle, ExplorerItemData *pItem, QWidget *parent /*= 0*/) :
	QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
	ui(new Ui::DlgInputName)
{
	CtorInit(sDlgTitle, pItem->GetName(false));

	if(pItem == nullptr)
	{
		HyGuiLog("DlgInputName recieved a nullptr Item pointer", LOGTYPE_Error);
		return;
	}
	
	setWindowIcon(pItem->GetIcon(SUBICON_Settings));
	ui->lblName->setText(HyGlobal::ItemName(pItem->GetType(), false) % " Name:");
	
	ErrorCheck();
}

DlgInputName::~DlgInputName()
{
	delete ui;
}

void DlgInputName::on_txtName_textChanged(const QString &arg1)
{
	ErrorCheck();
}

QString DlgInputName::GetName()
{
	return ui->txtName->text();
}

void DlgInputName::ErrorCheck()
{
	bool bIsError = false;
	do
	{
		if(ui->txtName->text().isEmpty())
		{
			ui->lblError->setText("Error: name cannot be blank");
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
	ui->buttonBox->button(QDialogButtonBox::Ok);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!bIsError);
}

