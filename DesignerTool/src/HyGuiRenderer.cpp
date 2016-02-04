#include "HyGuiRenderer.h"

#include <QTimer>
//#include <QGLFormat>
#include <QDir>

#include "HyGlobal.h"

#include "WidgetRenderer.h"

HyGuiRenderer::HyGuiRenderer(QWidget *parent) : QOpenGLWidget(parent),
                                                m_pHyEngine(NULL)
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(17);
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

    WidgetRenderer *pGameApp = reinterpret_cast<WidgetRenderer *>(parent());
    IHyApplication *pTest = static_cast<IHyApplication *>(pGameApp);

    // TODO: test to see if QWidget parent works for HyEngine()'s ctor
    m_pHyEngine = new HyEngine(*pTest);
}

/*virtual*/ void HyGuiRenderer::paintGL()
{
    if(m_pHyEngine->Update() == false)
        HYLOG("Harmony Gfx requested exit program.", LOGTYPE_Info);

    if(m_primBox.GetLoadState() == HYLOADSTATE_Inactive)
    {
        WidgetRenderer *pGameApp = reinterpret_cast<WidgetRenderer *>(parent());
        IHyApplication *pTest = static_cast<IHyApplication *>(pGameApp);

        m_pCam = pTest->Window().CreateCamera2d();

        m_primBox.Load();
        m_primBox.Color().Set(0.0f, 0.0f, 1.0f, 1.0f);
        m_primBox.SetAsQuad(1500.0f, 150.0f, false);
        m_primBox.Pos().Set(-100.0f, -100.0f);
        m_primBox.SetDisplayOrder(100);
    }
}

/*virtual*/ void HyGuiRenderer::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
   // m_mtxProj = glm::ortho(w * -0.5f, w * 0.5f, h * -0.5f, h * 0.5f);
}

void HyGuiRenderer::Reload()
{
    if(m_pHyEngine)
        m_pHyEngine->m_pAssetManager->Reload();
}

void HyGuiRenderer::Reload(QStringList &sReloadPaths)
{
    if(!m_pHyEngine)
        return;
    
    vector<std::string> vReloadPaths;
    foreach(QString sPath, sReloadPaths)
        vReloadPaths.push_back(sPath.toStdString());
    
    m_pHyEngine->m_pAssetManager->Reload(vReloadPaths);
}

void HyGuiRenderer::Reload(QString &sNewDataDir)
{
    m_pHyEngine->m_pAssetManager->Reload(sNewDataDir.toStdString());
}
