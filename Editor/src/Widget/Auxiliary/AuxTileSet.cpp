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

#include "AtlasTileSet.h"
#include "DlgImportTileSheet.h"

AuxTileSet::AuxTileSet(QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::AuxTileSet),
	m_pTileSet(nullptr)
{
	ui->setupUi(this);

	ui->btnImportTileSheet->setDefaultAction(ui->actionImportTileSheet);

	ui->splitter->setSizes(QList<int>() << 140 << width() - 140);
}

/*virtual*/ AuxTileSet::~AuxTileSet()
{
	delete ui;
}

void AuxTileSet::Init(AtlasTileSet *pTileSet)
{
	m_pTileSet = pTileSet;
	ui->lblName->setText(m_pTileSet->GetName());
	ui->lblInfo->setText(m_pTileSet->GetTileSetInfo());
}

void AuxTileSet::on_actionImportTileSheet_triggered()
{
	if(m_pTileSet == nullptr)
		return;

	DlgImportTileSheet dlgImportTileSheet(*m_pTileSet, this);
	dlgImportTileSheet.exec();
}
