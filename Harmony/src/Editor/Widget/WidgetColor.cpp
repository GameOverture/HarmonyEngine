/**************************************************************************
 *	WidgetColor.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetColor.h"
#include "ui_WidgetColor.h"

#include <QColorDialog>

WidgetColor::WidgetColor(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::WidgetColor)
{
	ui->setupUi(this);

	ApplyColorToSampleBtn();
}

WidgetColor::~WidgetColor()
{
	delete ui;
}

QColor WidgetColor::GetColor()
{
	return QColor(ui->sbR->value(), ui->sbG->value(), ui->sbB->value());
}

void WidgetColor::SetColor(QColor color)
{
	ui->sliderR->setValue(color.red());
	ui->sliderG->setValue(color.green());
	ui->sliderB->setValue(color.blue());
}

void WidgetColor::on_sbR_valueChanged(int arg1)
{
	ui->sliderR->setValue(ui->sbR->value());
	ApplyColorToSampleBtn();
}

void WidgetColor::on_sbG_valueChanged(int arg1)
{
	ui->sliderG->setValue(ui->sbG->value());
	ApplyColorToSampleBtn();
}

void WidgetColor::on_sbB_valueChanged(int arg1)
{
	ui->sliderB->setValue(ui->sbB->value());
	ApplyColorToSampleBtn();
}

void WidgetColor::on_btnSample_clicked()
{
	QColorDialog *pDlg = new QColorDialog(this);
	if(pDlg->exec() == QDialog::Accepted)
	{
		ui->sbR->setValue(pDlg->currentColor().red());
		ui->sbG->setValue(pDlg->currentColor().green());
		ui->sbB->setValue(pDlg->currentColor().blue());
	}
	
	delete pDlg;
}

void WidgetColor::ApplyColorToSampleBtn()
{
	QColor color = GetColor();
	if(color.isValid())
	{
		QPalette pal = ui->btnSample->palette();
		pal.setColor(QPalette::Button, color);

		ui->btnSample->setAutoFillBackground(true);
		ui->btnSample->setPalette(pal);
		ui->btnSample->update();
	}
}

void WidgetColor::on_sliderR_valueChanged(int value)
{
	ui->sbR->setValue(ui->sliderR->value());
	ApplyColorToSampleBtn();
}

void WidgetColor::on_sliderG_valueChanged(int value)
{
	ui->sbG->setValue(ui->sliderG->value());
	ApplyColorToSampleBtn();
}

void WidgetColor::on_sliderB_valueChanged(int value)
{
	ui->sbB->setValue(ui->sliderB->value());
	ApplyColorToSampleBtn();
}
