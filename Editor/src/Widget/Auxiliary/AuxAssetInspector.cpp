/**************************************************************************
 *	AuxAssetInspector.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AuxAssetInspector.h"
#include "ui_AuxAssetInspector.h"
#include "IManagerModel.h"

AuxAssetInspector::AuxAssetInspector(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AuxAssetInspector)
{
	ui->setupUi(this);
}

/*virtual*/ AuxAssetInspector::~AuxAssetInspector()
{
	delete ui;
}

void AuxAssetInspector::SetAssetManager(ManagerWidget *pManagerWidget)
{
	if(pManagerWidget)
		ui->graphicsView->setScene(pManagerWidget->GetModel().GetInspectorScene());
}
