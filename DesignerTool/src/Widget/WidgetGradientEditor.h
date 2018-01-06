/**************************************************************************
*	WidgetGradientEditor.h
*
*	Harmony Engine - Designer Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Designer Tool License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef WIDGETGRADIENTEDITOR_H
#define WIDGETGRADIENTEDITOR_H

#include <QWidget>

namespace Ui {
class WidgetGradientEditor;
}

class WidgetGradientEditor : public QWidget
{
	Q_OBJECT
	
public:
	explicit WidgetGradientEditor(QWidget *parent = 0);
	~WidgetGradientEditor();
	
private:
	Ui::WidgetGradientEditor *ui;
};

#endif // WIDGETGRADIENTEDITOR_H
