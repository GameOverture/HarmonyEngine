#include "HyGfx.h"

#include <QGLFormat>
#include <QDir>

#include "HyGlobal.h"

HyGfx::HyGfx(QWidget *parent) : QGLWidget(parent)
{
    QGLFormat format;
    format.setVersion(4, 2);
    format.setProfile(QGLFormat::CoreProfile);
    format.setSampleBuffers(true);

    setFormat(format);
}

void HyGfx::initializeGL()
{
    QString test = QDir::currentPath();

    glewExperimental = GL_TRUE;
    HyOpenGL::Initialize();
}

void HyGfx::paintGL()
{
    if(m_pGfxComms == NULL)
        return;

    if(Update() == false)
        HYLOG("Harmony Gfx requested exit program.", LOGTYPE_Info);
}

void HyGfx::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    m_mtxProj = glm::ortho(w * -0.5f, w * 0.5f, h * -0.5f, h * 0.5f);
}
