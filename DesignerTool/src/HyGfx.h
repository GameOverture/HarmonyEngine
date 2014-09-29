#ifndef HYGFXWIDGET_H
#define HYGFXWIDGET_H

#include "GL/glew.h"

#include <QGLWidget>
#include "Harmony/HyEngine.h"

class HyGfx : public QGLWidget, public HyOpenGL
{
    Q_OBJECT

public:
    explicit HyGfx(QWidget *parent = 0);

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
};

#endif // HYGFXWIDGET_H
