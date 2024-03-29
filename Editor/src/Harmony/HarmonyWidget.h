/**************************************************************************
*	HarmonyWidget.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HARMONYWIDGET_H
#define HARMONYWIDGET_H

#include "Project.h"

#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QTimer>

class HarmonyWidget : public QOpenGLWidget//, protected QOpenGLFunctions
{
	Q_OBJECT

	static bool					sm_bHarmonyLoaded;
	Project *					m_pProject;

	class GuiHyEngine : public HyEngine
	{
		Project *				m_pProject;

	public:
		GuiHyEngine(HarmonyInit &initStruct, Project *pProject) :
			HyEngine(initStruct),
			m_pProject(pProject)
		{
			HyEngine::Window().CreateCamera2d();
			//m_pProject->HarmonyInitialize();
		}

		~GuiHyEngine() {
			m_pProject->HarmonyShutdown();
		}

	protected:
		virtual bool OnUpdate() override {
			return m_pProject->HarmonyUpdate();
		}
	};
	GuiHyEngine *				m_pHyEngine;

	QTimer *					m_pTimer;

public:
	HarmonyWidget(Project *pProject);
	virtual ~HarmonyWidget();

	Project *GetProject();

	bool IsProject(Project *pProjectToTest);
	void CloseProject();

	Qt::CursorShape GetCursorShape() const;
	void SetCursorShape(Qt::CursorShape eShape);
	void RestoreCursorShape();

	HyRendererInterop *GetHarmonyRenderer();

protected:
	// QOpenGLWidget overrides
	virtual void initializeGL() override;
	virtual void paintGL() override;
	virtual void resizeGL(int w, int h) override;

	// QWidget overrides
	virtual void enterEvent(QEvent *pEvent) override;
	virtual void leaveEvent(QEvent *pEvent) override;
	virtual void dragEnterEvent(QDragEnterEvent *pEvent) override;
	virtual void dropEvent(QDropEvent *pEvent) override;
	virtual void keyPressEvent(QKeyEvent *pEvent) override;
	virtual void keyReleaseEvent(QKeyEvent *pEvent) override;
	virtual void mousePressEvent(QMouseEvent *pEvent) override;
	virtual void wheelEvent(QWheelEvent *pEvent) override;
	virtual void mouseMoveEvent(QMouseEvent *pEvent) override;
	virtual void mouseReleaseEvent(QMouseEvent *pEvent) override;

private Q_SLOTS:
	void OnBootCheck();

Q_SIGNALS:
	void HarmonyWidgetReady(HarmonyWidget *pWidget);
};

#endif // HARMONYWIDGET_H
