/**************************************************************************
 *	AuxShaderEditor.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
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
