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

    m_pHyEngine = new HyEngine(*GetHyApp());
}

/*virtual*/ void HyGuiRenderer::paintGL()
{
    if(m_pHyEngine->Update() == false)
        HYLOG("Harmony Gfx requested exit program.", LOGTYPE_Info);
}

/*virtual*/ void HyGuiRenderer::resizeGL(int w, int h)
{
    GetHyApp()->Window().SetResolution(glm::ivec2(w, h));
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

IHyApplication *HyGuiRenderer::GetHyApp()
{
    WidgetRenderer *pGameApp = reinterpret_cast<WidgetRenderer *>(parent());
    return static_cast<IHyApplication *>(pGameApp);
}
