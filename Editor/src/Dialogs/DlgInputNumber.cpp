/**************************************************************************
 *	DlgInputNumber.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "DlgInputNumber.h"
#include "ui_DlgInputNumber.h"

#include <QPushButton>
#include <QStringBuilder>


DlgInputNumber::DlgInputNumber(const QString &sDlgTitle, const QIcon &icon, int iStartValue, int iMin, int iMax, std::function<QString(int)> fpErrorCheckFunc, QWidget *pParent /*= nullptr*/) :
	QDialog(pParent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
	m_iStartValue(iStartValue),
	m_fpErrorCheckFunc(fpErrorCheckFunc),
	ui(new Ui::DlgInputNumber)
{
	ui->setupUi(this);

	setWindowTitle(sDlgTitle);
	setWindowIcon(icon);

	ui->sbValue->setValue(m_iStartValue);
	ui->sbValue->setMinimum(iMin);
	ui->sbValue->setMaximum(iMax);

	ui->lblName->setText("Value");

	ErrorCheck();
}

/*virtual*/ DlgInputNumber::~DlgInputNumber()
{
	delete ui;
}

void DlgInputNumber::on_txtName_textChanged(const QString &arg1)
{
	ErrorCheck();
}

int DlgInputNumber::GetValue()
{
	return ui->sbValue->value();
}

void DlgInputNumber::ErrorCheck()
{
	bool bIsError = false;
	do
	{
		if(ui->sbValue->value() == m_iStartValue)
		{
			ui->lblError->setText(ui->lblName->text() % " already set to " % QString::number(m_iStartValue));
			bIsError = true;
			break;
		}

		if(m_fpErrorCheckFunc)
		{
			QString sError = m_fpErrorCheckFunc(ui->sbValue->value());
			if(sError.isEmpty() == false)
			{
				ui->lblError->setText(sError);
				bIsError = true;
				break;
			}
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

