#ifndef HARMONYWIDGET_H
#define HARMONYWIDGET_H

#include "Harmony/HyEngine.h"
#include "Project.h"

#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QTimer>

class HarmonyWidget : public QOpenGLWidget//, protected QOpenGLFunctions
{
    Q_OBJECT

    Project *                   m_pProject;

    HyEngine *                  m_pHyEngine;
    bool                        m_bHarmonyLoaded;

    QTimer *                    m_pTimer;

public:
    HarmonyWidget(Project *pProject);
    virtual ~HarmonyWidget();

    Project *GetProject();

    bool IsProject(Project *pProjectToTest);

    void SetCursor(Qt::CursorShape eShape);
    void RestoreCursor();

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
