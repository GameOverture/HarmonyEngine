#ifndef HYGUIRENDERER_H
#define HYGUIRENDERER_H

#include "GL/glew.h"
#include <QOpenGLWidget>

#include "Harmony/HyEngine.h"

class HyGuiRenderer : public QOpenGLWidget
{
    Q_OBJECT

    HyEngine *          m_pHyEngine;

    IHyApplication *    m_pHyApp;
    bool                m_bInitialized;

public:
    HyGuiRenderer(IHyApplication *pHyApp, QWidget *parent = 0);
    ~HyGuiRenderer();

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
};

#endif // HYGUIRENDERER_H
