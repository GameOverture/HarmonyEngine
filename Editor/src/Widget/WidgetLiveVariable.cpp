/**************************************************************************
 *	WidgetLiveVariable.cpp
 *
 *	Harmony Engine - HyEditor
 *	Copyright (c) 2016 Jason Knobler
 *
 *	GPL/LGPL License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
 *************************************************************************/
#include "Global.h"
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
