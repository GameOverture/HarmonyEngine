#include "HyGuiRenderer.h"

#include <QTimer>
//#include <QGLFormat>
#include <QDir>

#include "HyGlobal.h"

HyGuiRenderer::HyGuiRenderer(QWidget *parent) : QOpenGLWidget(parent)
{
    // Todo: test to see if QWidget parent works for HyEngine()'s ctor

//    QTimer *timer = new QTimer(this);
//    connect(timer, SIGNAL(timeout()), this, SLOT(Render()));
//    timer->start(17);
}

HyGuiRenderer::~HyGuiRenderer()
{
}

void HyGuiRenderer::initializeGL()
{
    //    QGLFormat format;
    //    format.setVersion(4, 2);
    //    format.setProfile(QGLFormat::CoreProfile);
    //    format.setSampleBuffers(true);
    //    setFormat(format);

    m_pHyEngine = new HyEngine(*reinterpret_cast<IHyApplication *>(parent()));
}

void HyGuiRenderer::paintGL()
{
    if(m_pHyEngine->Update() == false)
        HYLOG("Harmony Gfx requested exit program.", LOGTYPE_Info);
}

void HyGuiRenderer::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
   // m_mtxProj = glm::ortho(w * -0.5f, w * 0.5f, h * -0.5f, h * 0.5f);
}
