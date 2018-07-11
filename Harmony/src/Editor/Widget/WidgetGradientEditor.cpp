/**************************************************************************
*	WidgetGradientEditor.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
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
