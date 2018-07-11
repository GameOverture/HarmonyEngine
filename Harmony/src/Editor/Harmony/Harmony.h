/**************************************************************************
*	Harmony.h
*
*	Harmony Engine - Designer Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Designer Tool License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HARMONY_H
#define HARMONY_H

#include "Project.h"
#include "HarmonyConnection.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class MainWindow;
class HarmonyWidget;

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

	static Project *GetProject();
	static void SetProject(Project *pProject);
	static void Reload(Project *pProject);

	static HarmonyWidget *GetWidget(Project *pProject);

private Q_SLOTS:
	void HarmonyWidgetReady(HarmonyWidget *pWidget);
};

#endif // HARMONY_H
