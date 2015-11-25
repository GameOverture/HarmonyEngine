#ifndef HYGUIRENDERER_H
#define HYGUIRENDERER_H

#include "GL/glew.h"
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include "Harmony/HyEngine.h"

class HyGuiRenderer : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

    IHyApplication *    m_pHyApp;
    HyEngine *          m_pHyEngine;

public:
    HyGuiRenderer(IHyApplication *pHyApp, QWidget *parent = 0);
    HyGuiRenderer(QWidget *parent = 0);
    ~HyGuiRenderer();

    // QOpenGLWidget overrides
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);
};

#endif // HYGUIRENDERER_H
