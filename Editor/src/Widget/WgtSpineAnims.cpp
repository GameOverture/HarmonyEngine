/**************************************************************************
 *	WgtSpineAnims.cpp
 *
 *	Harmony Engine - HyEditor
 *	Copyright (c) 2025 Jason Knobler
 *
 *	GPL/LGPL License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
 *************************************************************************/
#include "Global.h"
#include "WgtSpineAnims.h"
#include "ui_WgtSpineAnims.h"
#include "DlgNewProject.h"

#include <QFileDialog>

WgtSpineAnims::WgtSpineAnims(QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::WgtSpineAnims)
{
	ui->setupUi(this);
}

/*virtual*/ WgtSpineAnims::~WgtSpineAnims()
{
	delete ui;
}
