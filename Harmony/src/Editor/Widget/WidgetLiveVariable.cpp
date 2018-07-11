/**************************************************************************
 *	WidgetLiveVariable.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
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
