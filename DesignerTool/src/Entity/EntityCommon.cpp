/**************************************************************************
*	EntityCommon.cpp
*
*	Harmony Engine - Designer Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Designer Tool License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "EntityCommon.h"
#include "ui_EntityCommon.h"

EntityCommon::EntityCommon(QWidget *parent) :
	QToolBox(parent),
	ui(new Ui::EntityCommon)
{
	ui->setupUi(this);
}

EntityCommon::~EntityCommon()
{
	delete ui;
}
