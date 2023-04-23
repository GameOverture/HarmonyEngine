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

AuxAssetInspector::AuxAssetInspector(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AuxAssetInspector)
{
}

/*virtual*/ AuxAssetInspector::~AuxAssetInspector()
{
	delete ui;
}

void AuxAssetInspector::SetSelected(AssetManagerType eAssetType, QList<IAssetItemData *> selectedAssetsList)
{
	//selectedAssetsList
}
