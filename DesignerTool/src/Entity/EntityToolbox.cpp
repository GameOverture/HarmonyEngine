/**************************************************************************
*	EntityToolbox.h
*
*	Harmony Engine - Designer Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Designer Tool License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "EntityToolbox.h"
#include "ui_EntityToolbox.h"

EntityToolbox::EntityToolbox(QWidget *parent) : QWidget(parent),
												ui(new Ui::EntityToolbox)
{
	ui->setupUi(this);
}

EntityToolbox::~EntityToolbox()
{
	delete ui;
}
