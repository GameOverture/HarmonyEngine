#ifndef HARMONY_H
#define HARMONY_H

#include "MainWindow.h"
#include "Project.h"
#include "HarmonyWidget.h"
#include "HarmonyConnection.h"
#include "_Dependencies/QtWaitingSpinner/waitingspinnerwidget.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class Harmony : public QObject
{
    Q_OBJECT

    static Harmony *        sm_pInstance;

    MainWindow *            m_pMainWindow;

    WaitingSpinnerWidget    m_LoadingSpinner;
    HarmonyConnection       m_Connection;

    HarmonyWidget *         m_pCurrentWidget;

public:
    Harmony(MainWindow *pMainWindow);
    virtual ~Harmony();

    static void SetProject(Project *pProject);
    static HarmonyWidget *GetWidget(Project *pProject);

Q_SIGNALS:
      void SetProjectFinished(Project *pProject);

private Q_SLOTS:
      void HarmonyWidgetReady();
};

#endif // HARMONY_H
