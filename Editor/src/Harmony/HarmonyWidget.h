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

	QGraphicsScene		m_RulerHorzScene;
	QGraphicsScene		m_RulerVertScene;

	HyOrientation		m_eGuidePending; // Clicked ruler (or existing guide) and held mouse button down
	
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
	bool IsShowRulersMouse() const;
	void ShowRulerMouse(bool bShow);

	// Child widgets will pass the event upward to its parent HarmonyWidget
	void OnRulerMouseMoveEvent(HyOrientation eOrientation, QMouseEvent *pEvent);
	void OnWgtMouseMoveEvent(IDraw *pDrawItem, QMouseEvent *pEvent);
	
	void OnRulerMousePressEvent(HyOrientation eOrientation, QMouseEvent *pEvent);
	void OnWgtMousePressEvent(IDraw *pDrawItem, QMouseEvent *pEvent);
	
	void OnRulerMouseReleaseEvent(HyOrientation eOrientation, QMouseEvent *pEvent);
	void OnWgtMouseReleaseEvent(IDraw *pDrawItem, QMouseEvent *pEvent);

protected:
	void resizeEvent(QResizeEvent *pEvent) override;
	
private Q_SLOTS:

private:
	Ui::HarmonyWidget *ui;
};

#endif // HARMONYWIDGET_H
