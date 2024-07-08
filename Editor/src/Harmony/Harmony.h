/**************************************************************************
*	Harmony.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HARMONY_H
#define HARMONY_H

#include "Project.h"
#include "HarmonyConnection.h"
#include "HarmonyWidget.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class MainWindow;

class Harmony : public QObject
{
	Q_OBJECT

	static Harmony *		sm_pInstance;

	MainWindow &			m_MainWindowRef;
	HarmonyWidget *			m_pWidget;
	HarmonyConnection		m_Connection;

public:
	Harmony(MainWindow &mainWindowRef);
	virtual ~Harmony();

	// TODO: REMOVE THESE STATIC FUNCTIONS IF THIS CLASS CAN BE DELETED
	static Project *GetProject();
	static void SetProject(Project *pProject);
	static void Reload(Project *pProject);

	static void OnProjectDestructor(Project *pProject);

	static HarmonyWidget *GetHarmonyWidget(Project *pProject);

private Q_SLOTS:
	void OnHarmonyWidgetReady(WgtHarmony *pWidget);
};

#endif // HARMONY_H
