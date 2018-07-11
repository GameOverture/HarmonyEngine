/**************************************************************************
*	WidgetGradientEditor.cpp
*
*	Harmony Engine - Designer Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Designer Tool License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "WidgetGradientEditor.h"
#include "ui_WidgetGradientEditor.h"

WidgetGradientEditor::WidgetGradientEditor(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::WidgetGradientEditor)
{
	ui->setupUi(this);
}

WidgetGradientEditor::~WidgetGradientEditor()
{
	delete ui;
}
