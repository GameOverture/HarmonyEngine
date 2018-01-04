/**************************************************************************
*	EntSpriteFrame.h
*
*	Harmony Engine - Designer Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Designer Tool License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "EntSpriteFrame.h"
#include "ui_EntSpriteFrame.h"

EntSpriteFrame::EntSpriteFrame(QWidget *parent) :
	QFrame(parent),
	ui(new Ui::EntSpriteFrame)
{
	ui->setupUi(this);
}

EntSpriteFrame::~EntSpriteFrame()
{
	delete ui;
}
