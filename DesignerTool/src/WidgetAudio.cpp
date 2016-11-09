/**************************************************************************
 *	WidgetAudio.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetAudio.h"
#include "ui_WidgetAudio.h"

WidgetAudio::WidgetAudio(ItemAudio *pOwner, QWidget *parent) :  QWidget(parent),
                                                                ui(new Ui::WidgetAudio),
                                                                m_pItemAudio(pOwner)
{
    ui->setupUi(this);
}

WidgetAudio::~WidgetAudio()
{
    delete ui;
}
