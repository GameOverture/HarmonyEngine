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

#include "HyGlobal.h"
#include "ItemProject.h"

HyGuiRenderer::HyGuiRenderer(QWidget *parent) : QOpenGLWidget(parent),
                                                m_pProjOwner(NULL),
                                                m_pHyEngine(NULL)
{
    HyGuiLog("Wrong ctor HyGuiRenderer()", LOGTYPE_Error);
}

HyGuiRenderer::HyGuiRenderer(ItemProject *pProj, QWidget *parent /*= 0*/) : QOpenGLWidget(parent),
                                                                            m_pProjOwner(pProj),
                                                                            m_pHyEngine(NULL),
                                                                            m_bIsUpdating(false)
{
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);// QSurfaceFormat::CompatibilityProfile);
    format.setVersion(3,3);

    setFormat(format);

    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(update()));
    m_pTimer->start(10);
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

    if(m_pProjOwner)
        m_pHyEngine = new HyEngine(*m_pProjOwner);
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
