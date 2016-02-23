/**************************************************************************
 *	WidgetSprite.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetSprite.h"
#include "ui_WidgetSprite.h"

WidgetSprite::WidgetSprite(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetSprite)
{
    ui->setupUi(this);
}

WidgetSprite::~WidgetSprite()
{
    delete ui;
}
