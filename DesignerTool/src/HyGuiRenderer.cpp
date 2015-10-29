#include "HyGuiRenderer.h"

#include <QTimer>
//#include <QGLFormat>
#include <QDir>

#include "HyGlobal.h"

#include "WidgetRenderer.h"

HyGuiRenderer::HyGuiRenderer(QWidget *parent) : QOpenGLWidget(parent)
{
    // Todo: test to see if QWidget parent works for HyEngine()'s ctor

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(17);
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

    WidgetRenderer *pGameApp = reinterpret_cast<WidgetRenderer *>(parent());
    IHyApplication *pTest = static_cast<IHyApplication *>(pGameApp);

    m_pHyEngine = new HyEngine(*pTest);
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
