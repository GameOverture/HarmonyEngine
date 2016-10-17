/**************************************************************************
 *	DlgColorPicker.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "DlgColorPicker.h"
#include "ui_DlgColorPicker.h"

DlgColorPicker::DlgColorPicker(QString sTitle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgColorPicker)
{
    ui->setupUi(this);
    
    setWindowTitle(sTitle);
}

DlgColorPicker::~DlgColorPicker()
{
    delete ui;
}

bool DlgColorPicker::IsSolidColor()
{
    return ui->tabWidget->currentIndex() == 0;
}

QColor DlgColorPicker::GetSolidColor()
{
    return ui->solidColor->GetColor();
}

QColor DlgColorPicker::GetVgTopColor()
{
    return ui->vgTopColor->GetColor();
}

QColor DlgColorPicker::GetVgBotColor()
{
    return ui->vgBotColor->GetColor();
}
