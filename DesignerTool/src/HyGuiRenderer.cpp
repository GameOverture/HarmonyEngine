/**************************************************************************
 *	HyGuiRenderer.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "HyGuiRenderer.h"

#include <QTimer>
#include <QSurfaceFormat>
#include <QDir>
#include <QApplication>

#include "HyGuiGlobal.h"

//void HyGuiRendererLoadThread::run()
//{
//    m_pGLContext->makeCurrent(m_pSurface);
//    HyEngine *pNewHyEngine = nullptr;
//    if(m_pProjOwner)
//        pNewHyEngine = new HyEngine(*m_pProjOwner);

//    m_pGLContext->doneCurrent();

//    Q_EMIT EngineLoaded(pNewHyEngine, m_pGLContext);
//}

HyGuiRenderer::HyGuiRenderer(QWidget *parent) : QOpenGLWidget(parent),
                                                m_pProjOwner(NULL),
                                                m_pHyEngine(NULL)
{
    HyGuiLog("Wrong ctor HyGuiRenderer()", LOGTYPE_Error);
}

HyGuiRenderer::HyGuiRenderer(Project *pProj, QWidget *parent /*= 0*/) : QOpenGLWidget(parent),
                                                                            m_pProjOwner(pProj),
                                                                            m_pHyEngine(NULL),
                                                                            m_bIsUpdating(false)
{
//    QSurfaceFormat format;
//    format.setRenderableType(QSurfaceFormat::OpenGL);
//    format.setProfile(QSurfaceFormat::CoreProfile);// QSurfaceFormat::CompatibilityProfile);
//    format.setVersion(3,3);

//    setFormat(format);

    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(OnBootCheck()));
    m_pTimer->start(50);
    
    m_bIsUpdating = true;
}

HyGuiRenderer::~HyGuiRenderer()
{
    m_pTimer->stop();

    makeCurrent();
    m_bIsUpdating = true;
    
    if(m_pHyEngine)
        m_pHyEngine->Shutdown();
    
    delete m_pHyEngine;
    m_pHyEngine = NULL;
    m_bIsUpdating = false;
}

HyRendererInterop *HyGuiRenderer::GetHarmonyRenderer()
{
    if(m_pHyEngine)
        return &m_pHyEngine->GetRenderer();
    else
        return NULL;
}

/*virtual*/ void HyGuiRenderer::initializeGL()
{
    QString glType;
    QString glProfile;

    glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL";
    switch (format().profile())
    {
    case QSurfaceFormat::NoProfile: glProfile = "No Profile"; break;
    case QSurfaceFormat::CoreProfile: glProfile = "Core Profile"; break;
    case QSurfaceFormat::CompatibilityProfile: glProfile = "Compatibility Profile"; break;
    }

    HyGuiLog("Initializing OpenGL", LOGTYPE_Title);
    HyGuiLog(glType % "(" % glProfile % ")", LOGTYPE_Normal);
    HyGuiLog("Vendor: " % QString(reinterpret_cast<const char *>(glGetString(GL_VENDOR))), LOGTYPE_Normal);
    HyGuiLog("Renderer: " % QString(reinterpret_cast<const char *>(glGetString(GL_RENDERER))), LOGTYPE_Normal);
    HyGuiLog("Version: " % QString(reinterpret_cast<const char *>(glGetString(GL_VERSION))), LOGTYPE_Normal);
    HyGuiLog("GLSL: " % QString(reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION))), LOGTYPE_Normal);

    // TESTING ///
    if(m_pProjOwner)
    {
        m_pHyEngine = new HyEngine(*m_pProjOwner);
    }

//    QOpenGLContext *pThreadContext = new QOpenGLContext;
//    pThreadContext->setFormat(context()->format());
//    pThreadContext->setShareContext(context());
//    pThreadContext->create();

//    HyGuiRendererLoadThread *pNewLoadThread = new HyGuiRendererLoadThread(m_pProjOwner, pThreadContext, context()->surface(), this);
//    connect(pNewLoadThread, &HyGuiRendererLoadThread::EngineLoaded, this, &HyGuiRenderer::OnEngineLoaded);
//    connect(pNewLoadThread, &HyGuiRendererLoadThread::finished, pNewLoadThread, &QObject::deleteLater);

//    //MainWindow::StartLoading(MDI_Explorer);
//    pThreadContext->moveToThread(pNewLoadThread);
//    pNewLoadThread->start();
}

/*virtual*/ void HyGuiRenderer::paintGL()
{
    if(m_pHyEngine && m_bIsUpdating == false)
    {
        m_bIsUpdating = true;
        if(m_pHyEngine->Update() == false)
            HyGuiLog("Harmony Gfx requested exit program.", LOGTYPE_Info);

        m_bIsUpdating = false;
    }
}

/*virtual*/ void HyGuiRenderer::resizeGL(int w, int h)
{
    if(m_pProjOwner)
        m_pProjOwner->SetRenderSize(w, h);
}

void HyGuiRenderer::OnBootCheck()
{
    if(m_pHyEngine && m_pHyEngine->BootUpdate() == false)
    {
        m_pProjOwner->Reload();
        
        m_bIsUpdating = false;
        m_pTimer->stop();
        
        if(false == m_pTimer->disconnect())
            HyGuiLog("Harmony OnBootCheck could not disconnect its signal.", LOGTYPE_Error);
        
        connect(m_pTimer, SIGNAL(timeout()), this, SLOT(update()));
        m_pTimer->start(10);
    }
}

//void HyGuiRenderer::OnEngineLoaded(HyEngine *pNewHyEngine, QOpenGLContext *pGLContext)
//{
//    delete pGLContext;
//    //pGLContext->moveToThread(QApplication::instance()->thread());
//    //pGLContext->makeCurrent(context()->surface());
//    m_pHyEngine = pNewHyEngine;
//}
