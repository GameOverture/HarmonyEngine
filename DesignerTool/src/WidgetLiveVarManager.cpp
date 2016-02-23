/**************************************************************************
 *	WidgetLiveVarManager.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetLiveVarManager.h"
#include "ui_WidgetLiveVarManager.h"

WidgetLiveVarManager::WidgetLiveVarManager(QWidget *parent) :   QWidget(parent),
                                                                ui(new Ui::WidgetLiveVarManager)
{
    ui->setupUi(this);

    ui->tabWidget->clear();
}

WidgetLiveVarManager::~WidgetLiveVarManager()
{
    delete ui;
}

//void WidgetLiveVarManager::AddVar(int iTabIndex, QString sGroup, QString sVarName, eVarType eType)
//{
//    //ui->tabWidget->addTab
//}
