/**************************************************************************
 *	AuxShaderEditor.h
 *
 *	Harmony Engine - HyEditor
 *	Copyright (c) 2024 Jason Knobler
 *
 *	GPL/LGPL License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
 *************************************************************************/
#ifndef AuxShaderEditor_H
#define AuxShaderEditor_H

#include <QWidget>

class WgtCodeEditor;

namespace Ui {
class AuxShaderEditor;
}

class AuxShaderEditor : public QWidget
{
	Q_OBJECT

public:
	explicit AuxShaderEditor(QWidget *pParent = nullptr);
	virtual ~AuxShaderEditor();

	WgtCodeEditor &GetCodeEditor();

private:
	Ui::AuxShaderEditor *ui;

private Q_SLOTS:
};

#endif // AuxShaderEditor_H
