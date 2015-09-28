#ifndef HYGUIRENDERER_H
#define HYGUIRENDERER_H

#include "GL/glew.h"
#include <QOpenGLWidget>

#include "Harmony/HyEngine.h"

class HyGuiRenderer : public QOpenGLWidget
{
    Q_OBJECT

    HyEngine *          m_pHyEngine;

    bool                m_bInitialized;

public:
    HyGuiRenderer(QWidget *parent = 0);
    ~HyGuiRenderer();

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
};

#endif // HYGUIRENDERER_H
