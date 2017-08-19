/**************************************************************************
 *	WidgetLiveVariable.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetLiveVariable.h"
#include "ui_WidgetLiveVariable.h"

WidgetLiveVariable::WidgetLiveVariable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetLiveVariable)
{
    ui->setupUi(this);
}

WidgetLiveVariable::~WidgetLiveVariable()
{
    delete ui;
}
