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

    static Harmony *        sm_pInstance;

    MainWindow &            m_MainWindowRef;
    HarmonyConnection       m_Connection;
    HarmonyWidget *         m_pLoadedWidget;

public:
    Harmony(MainWindow &mainWindowRef);
    virtual ~Harmony();

    static Project *GetProject();
    static void SetProject(Project &projectRef);
    static void CloseProject();

    static HarmonyWidget *GetWidget(Project &projectRef);

private Q_SLOTS:
      void HarmonyWidgetReady(HarmonyWidget *pWidget);
};

#endif // HARMONY_H
