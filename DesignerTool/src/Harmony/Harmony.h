#ifndef HARMONY_H
#define HARMONY_H

#include "MainWindow.h"
#include "Project.h"
#include "HarmonyWidget.h"
#include "HarmonyConnection.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class Harmony : public QObject
{
    Q_OBJECT

    static Harmony *        sm_pInstance;

    MainWindow *            m_pMainWindow;
    HarmonyConnection       m_Connection;
    HarmonyWidget *         m_pLoadedWidget;

public:
    Harmony(MainWindow *pMainWindow);
    virtual ~Harmony();

    static Project *GetProject();
    static void SetProject(Project &projectRef);
    static void CloseProject();

    static HarmonyWidget *GetWidget(Project &projectRef);

private Q_SLOTS:
      void HarmonyWidgetReady(HarmonyWidget *pWidget);
};

#endif // HARMONY_H
