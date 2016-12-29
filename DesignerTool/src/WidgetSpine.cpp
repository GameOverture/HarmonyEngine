/**************************************************************************
 *	WidgetSpine.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetSpine.h"
#include "ui_WidgetSpine.h"

WidgetSpine::WidgetSpine(QWidget *parent) : QWidget(parent),
                                            ui(new Ui::WidgetSpine)
{
    ui->setupUi(this);
}

WidgetSpine::~WidgetSpine()
{
    delete ui;
}
