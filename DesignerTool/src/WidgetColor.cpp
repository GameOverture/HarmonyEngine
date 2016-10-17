/**************************************************************************
 *	WidgetColor.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetColor.h"
#include "ui_WidgetColor.h"

#include <QColorDialog>

WidgetColor::WidgetColor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetColor)
{
    ui->setupUi(this);
}

WidgetColor::~WidgetColor()
{
    delete ui;
}

QColor WidgetColor::GetColor()
{
    return QColor(ui->sbR->value(), ui->sbG->value(), ui->sbB->value());
}

void WidgetColor::on_sliderR_sliderMoved(int position)
{
    ui->sbR->setValue(ui->sliderR->value());
    ApplyColorToSampleBtn();
}

void WidgetColor::on_sliderG_sliderMoved(int position)
{
    ui->sbG->setValue(ui->sliderG->value());
    ApplyColorToSampleBtn();
}

void WidgetColor::on_sliderB_sliderMoved(int position)
{
    ui->sbB->setValue(ui->sliderB->value());
    ApplyColorToSampleBtn();
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
        QString sStyle = QString("background-color: %1").arg(color.name());
        ui->btnSample->setStyleSheet(sStyle);
        
        ui->btnSample->repaint();
    }
}
