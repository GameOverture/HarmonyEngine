/**************************************************************************
 *	AuxTileMap.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AuxTileMap.h"
#include "ui_AuxTileMap.h"

#include "TileMapModel.h"

AuxTileMap::AuxTileMap(QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::AuxTileMap)
{
	ui->setupUi(this);
}

/*virtual*/ AuxTileMap::~AuxTileMap()
{
	delete ui;
}

void AuxTileMap::Init(Project &projectRef, TileMapModel &tileMapModelRef)
{
	//ui->tileSetsTableView
}
