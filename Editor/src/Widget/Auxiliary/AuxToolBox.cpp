/**************************************************************************
 *	AuxToolBox.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AuxToolBox.h"
#include "ui_AuxToolBox.h"

AuxToolBox::AuxToolBox(QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::AuxToolBox)
{
}

/*virtual*/ AuxToolBox::~AuxToolBox()
{
	delete ui;
}
