/**************************************************************************
 *	DlgSetSpineAnims.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "DlgSetSpineAnims.h"
#include "ui_DlgSetSpineAnims.h"
#include "Project.h"

DlgSetSpineAnims::DlgSetSpineAnims(Project &projectRef, QString sTitle, QWidget *parent /*= 0*/) :
	QDialog(parent),
	m_ProjectRef(projectRef),
	ui(new Ui::DlgSetSpineAnims)
{
	ui->setupUi(this);
	setWindowTitle(sTitle);
}

DlgSetSpineAnims::~DlgSetSpineAnims()
{
	delete ui;
}
