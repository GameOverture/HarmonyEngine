/**************************************************************************
 *	AuxTileSet.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AuxTileSet.h"
#include "MainWindow.h"
#include "ui_AuxTileSet.h"

#include "DlgImportTileSheet.h"

AuxTileSet::AuxTileSet(QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::AuxTileSet)
{
	ui->setupUi(this);

	ui->btnImportTileSheet->setDefaultAction(ui->actionImportTileSheet);
	ui->btnImportTiles->setDefaultAction(ui->actionImportTiles);
}

/*virtual*/ AuxTileSet::~AuxTileSet()
{
	delete ui;
}

void AuxTileSet::on_actionImportTileSheet_triggered()
{
	DlgImportTileSheet dlgImportTileSheet(*Harmony::GetProject(), this);
	dlgImportTileSheet.exec();
}
