#ifndef HYGUIRENDERER_H
#define HYGUIRENDERER_H

#include "GL/glew.h"
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include "Harmony/HyEngine.h"

class HyGuiRenderer : public QOpenGLWidget//, protected QOpenGLFunctions
{
    Q_OBJECT

    HyEngine *          m_pHyEngine;

    HyPrimitive2d		m_primBox;
    HyCamera2d *        m_pCam;

public:
    HyGuiRenderer(QWidget *parent = 0);
    ~HyGuiRenderer();

    // QOpenGLWidget overrides
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);
    
    void Reload();
    void Reload(QStringList &sReloadPaths);
    void Reload(QString &sNewDataDir);

private:
    IHyApplication *GetHyApp();
};

#endif // HYGUIRENDERER_H
