/**************************************************************************
 *	HyGuiRenderer.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HYGUIRENDERER_H
#define HYGUIRENDERER_H

#include "GL/glew.h"
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include "Harmony/HyEngine.h"
#include "Project.h"

class HyGuiRenderer : public QOpenGLWidget//, protected QOpenGLFunctions
{
    Q_OBJECT

    Project *           m_pProjOwner;
    HyEngine *          m_pHyEngine;
    
    bool                m_bIsUpdating;

    QTimer *            m_pTimer;

public:
    HyGuiRenderer(QWidget *parent = 0);
    HyGuiRenderer(Project *pProj, QWidget *parent = 0);
    ~HyGuiRenderer();
    
    HyRendererInterop *GetHarmonyRenderer();

    void SetCursor(Qt::CursorShape eShape);
    void RestoreCursor();

    // QOpenGLWidget overrides
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;


protected:
    // QWidget overrides
    virtual void enterEvent(QEvent *pEvent) override;
    virtual void leaveEvent(QEvent *pEvent) override;

private Q_SLOTS:
    void OnBootCheck();

protected:
    virtual void keyPressEvent(QKeyEvent *pEvent) override;
    virtual void keyReleaseEvent(QKeyEvent *pEvent) override;
    virtual void mousePressEvent(QMouseEvent *pEvent) override;
    virtual void wheelEvent(QWheelEvent *pEvent) override;
    virtual void mouseMoveEvent(QMouseEvent *pEvent) override;
    virtual void mouseReleaseEvent(QMouseEvent *pEvent) override;
};

#endif // HYGUIRENDERER_H
