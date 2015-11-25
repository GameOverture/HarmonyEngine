#include "HyGuiRenderer.h"

#include <QTimer>
//#include <QGLFormat>
#include <QDir>

#include "HyGlobal.h"

#include "WidgetRenderer.h"

HyGuiRenderer::HyGuiRenderer(QWidget *parent /*= 0*/) : QOpenGLWidget(parent),
                                                        m_pHyApp(NULL),
                                                        m_pHyEngine(NULL)
{
}

HyGuiRenderer::HyGuiRenderer(IHyApplication *pHyApp, QWidget *parent) : QOpenGLWidget(parent),
                                                                        m_pHyApp(pHyApp),
                                                                        m_pHyEngine(NULL)
{
    // Todo: test to see if QWidget parent works for HyEngine()'s ctor

//    QTimer *timer = new QTimer(this);
//    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
//    timer->start(17);
}

HyGuiRenderer::~HyGuiRenderer()
{
}

/*virtual*/ void HyGuiRenderer::initializeGL()
{
    //    QGLFormat format;
    //    format.setVersion(4, 2);
    //    format.setProfile(QGLFormat::CoreProfile);
    //    format.setSampleBuffers(true);
    //    setFormat(format);

    //WidgetRenderer *pGameApp = reinterpret_cast<WidgetRenderer *>(parent());
    //IHyApplication *pTest = static_cast<IHyApplication *>(pGameApp);

    if(m_pHyApp)
        m_pHyEngine = new HyEngine(*m_pHyApp);
}

/*virtual*/ void HyGuiRenderer::paintGL()
{
    if(m_pHyEngine == NULL)
        return;
    
    if(m_pHyEngine->Update() == false)
        HYLOG("Harmony Gfx requested exit program.", LOGTYPE_Info);
}

/*virtual*/ void HyGuiRenderer::resizeGL(int w, int h)
{
    if(m_pHyEngine == NULL)
        return;
    
    glViewport(0, 0, w, h);
   // m_mtxProj = glm::ortho(w * -0.5f, w * 0.5f, h * -0.5f, h * 0.5f);
}
