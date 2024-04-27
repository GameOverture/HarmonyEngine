/**************************************************************************
 *	HarmonyWidget.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HARMONYWIDGET_H
#define HARMONYWIDGET_H

#include <QWidget>
#include "WgtHarmony.h"

#include <QGraphicsScene>

class Project;

namespace Ui {
class HarmonyWidget;
}

class HarmonyWidget : public QWidget
{
	Q_OBJECT

	//QGraphicsScene		m_HorzScene;
	//QGraphicsScene		m_VertScene;
	
public:
	explicit HarmonyWidget(Project *pProject, QWidget *pParent = 0);
	~HarmonyWidget();

	Project *GetProject();

	bool IsProject(Project *pProjectToTest);
	void CloseProject();

	Qt::CursorShape GetCursorShape() const;
	void SetCursorShape(Qt::CursorShape eShape);
	void RestoreCursorShape();

	WgtHarmony *GetWgtHarmony();
	HyRendererInterop *GetHarmonyRenderer();

	void RefreshRulers();
	
private Q_SLOTS:

private:
	Ui::HarmonyWidget *ui;
};

#endif // HARMONYWIDGET_H
