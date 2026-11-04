/**************************************************************************
 *	DlgSetSpineAnims.cpp
 *
 *	Harmony Engine - HyEditor
 *	Copyright (c) 2025 Jason Knobler
 *
 *	GPL/LGPL License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
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
