/**************************************************************************
 *	AuxShaderEditor.cpp
 *
 *	Harmony Engine - HyEditor
 *	Copyright (c) 2024 Jason Knobler
 *
 *	GPL/LGPL License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
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
