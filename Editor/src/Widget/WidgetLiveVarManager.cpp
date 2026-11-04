/**************************************************************************
 *	WidgetLiveVarManager.cpp
 *
 *	Harmony Engine - HyEditor
 *	Copyright (c) 2016 Jason Knobler
 *
 *	GPL/LGPL License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
 *************************************************************************/
#include "Global.h"
#include "WidgetLiveVarManager.h"
#include "ui_WidgetLiveVarManager.h"

WidgetLiveVarManager::WidgetLiveVarManager(QWidget *parent) :
	QWidget(parent),
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
