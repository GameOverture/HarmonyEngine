/**************************************************************************
 *	WidgetFontMap.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetFontMap.h"
#include "ui_WidgetFontMap.h"

WidgetFontMap::WidgetFontMap(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetFontMap)
{
    ui->setupUi(this);
}

WidgetFontMap::~WidgetFontMap()
{
    delete ui;
}
