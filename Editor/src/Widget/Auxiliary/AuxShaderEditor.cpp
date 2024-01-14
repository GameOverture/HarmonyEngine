/**************************************************************************
 *	AuxShaderEditor.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AuxShaderEditor.h"
#include "ui_AuxShaderEditor.h"

AuxShaderEditor::AuxShaderEditor(QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::AuxShaderEditor)
{
	ui->setupUi(this);
}

/*virtual*/ AuxShaderEditor::~AuxShaderEditor()
{
	delete ui;
}

WgtCodeEditor &AuxShaderEditor::GetCodeEditor()
{
	return *ui->wgtCodeEditor;
}
