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

	m_pTabBar = new QTabBar(ui->tabFrame);
	m_pTabBar->addTab(QIcon(QString::fromUtf8(":/icons16x16/items/TileSet.png")), tr("Tiles"));
	m_pTabBar->addTab(QIcon(QString::fromUtf8(":/icons16x16/Tools-ImportTileSheet.png")), tr("Patterns"));
	m_pTabBar->addTab(QIcon(QString::fromUtf8(":/icons16x16/tileset-autotile.png")), tr("Terrains"));

	m_pToolBar = new QToolBar(ui->toolBarFrame);

	ui->tileSetsTableView->verticalHeader()->hide();
	ui->tileSetsTableView->horizontalHeader()->hide();
}

/*virtual*/ AuxTileMap::~AuxTileMap()
{
	delete ui;
}

void AuxTileMap::Init(Project &projectRef, TileMapModel &tileMapModelRef)
{
	//ui->tileSetsTableView
}
