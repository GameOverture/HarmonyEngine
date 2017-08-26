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
#include "HyGuiGlobal.h"
#include "ProjectItemMimeData.h"

#include <QTimer>
#include <QSurfaceFormat>
#include <QDir>
#include <QApplication>
#include <QMouseEvent>
#include <QMimeData>

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
                                                m_pProjOwner(nullptr),
                                                m_pHyEngine(nullptr)
{
    HyGuiLog("Wrong ctor HyGuiRenderer()", LOGTYPE_Error);
}

HyGuiRenderer::HyGuiRenderer(Project *pProj, QWidget *parent /*= 0*/) : QOpenGLWidget(parent),
                                                                            m_pProjOwner(pProj),
                                                                            m_pHyEngine(nullptr),
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

    RestoreCursor();
}

HyGuiRenderer::~HyGuiRenderer()
{
    m_pTimer->stop();

    makeCurrent();
    m_bIsUpdating = true;
    
    if(m_pHyEngine)
    {
        m_pProjOwner->Shutdown();
        m_pHyEngine->Shutdown();
    }
    
    delete m_pHyEngine;
    m_pHyEngine = nullptr;
    m_bIsUpdating = false;
}

HyRendererInterop *HyGuiRenderer::GetHarmonyRenderer()
{
    if(m_pHyEngine)
        return &m_pHyEngine->GetRenderer();
    else
        return nullptr;
}

void HyGuiRenderer::SetCursor(Qt::CursorShape eShape)
{
    setCursor(eShape);
}

void HyGuiRenderer::RestoreCursor()
{
    setCursor(Qt::CrossCursor);
}

/*virtual*/ void HyGuiRenderer::initializeGL() /*override*/
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
    {
        m_pHyEngine = new HyEngine(*m_pProjOwner);
        m_pHyEngine->sm_pInstance = m_pHyEngine;
    }
}

/*virtual*/ void HyGuiRenderer::paintGL() /*override*/
{
    if(m_pHyEngine && m_bIsUpdating == false)
    {
        m_bIsUpdating = true;
        if(m_pHyEngine->Update() == false)
            HyGuiLog("Harmony Gfx requested exit program.", LOGTYPE_Info);

        m_bIsUpdating = false;
    }
}

/*virtual*/ void HyGuiRenderer::resizeGL(int w, int h) /*override*/
{
    if(m_pProjOwner)
        m_pProjOwner->SetRenderSize(w, h);
}

/*virtual*/ void HyGuiRenderer::enterEvent(QEvent *pEvent) /*override*/
{
    this->setFocus();

}

/*virtual*/ void HyGuiRenderer::leaveEvent(QEvent *pEvent) /*override*/
{
}

/*virtual*/ void HyGuiRenderer::dragEnterEvent(QDragEnterEvent *event) /*override*/
{
    //if(event->mimeData()-
    //event->mimeData()->data(HYGUI_MIMETYPE)

    ProjectTabBar *pTabBar = m_pProjOwner->GetTabBar();
    if(pTabBar->tabData(pTabBar->currentIndex()).value<ProjectItem *>()->GetType() == ITEM_Entity)
    {

    }
}

/*virtual*/ void HyGuiRenderer::dropEvent(QDropEvent *event) /*override*/
{
}

void HyGuiRenderer::OnBootCheck()
{
    if(m_pHyEngine && m_pHyEngine->IsInitialized())
    {
        m_pProjOwner->OnHarmonyLoaded();
        
        m_bIsUpdating = false;
        m_pTimer->stop();
        
        if(false == m_pTimer->disconnect())
            HyGuiLog("Harmony OnBootCheck could not disconnect its signal.", LOGTYPE_Error);
        
        connect(m_pTimer, SIGNAL(timeout()), this, SLOT(update()));
        m_pTimer->start(10);
    }
}

/*virtual*/ void HyGuiRenderer::keyPressEvent(QKeyEvent *pEvent) /*override*/
{
    if(m_pProjOwner == nullptr)
        return;

    ProjectItem *pCurItem = m_pProjOwner->GetCurrentOpenItem();
    if(pCurItem == nullptr)
        return;

    pCurItem->GetDraw()->OnKeyPressEvent(pEvent);
}

/*virtual*/ void HyGuiRenderer::keyReleaseEvent(QKeyEvent *pEvent) /*override*/
{
    if(m_pProjOwner == nullptr)
        return;

    ProjectItem *pCurItem = m_pProjOwner->GetCurrentOpenItem();
    if(pCurItem == nullptr)
        return;

    pCurItem->GetDraw()->OnKeyReleaseEvent(pEvent);
}

/*virtual*/ void HyGuiRenderer::mousePressEvent(QMouseEvent *pEvent) /*override*/
{
    if(m_pProjOwner == nullptr)
        return;

    ProjectItem *pCurItem = m_pProjOwner->GetCurrentOpenItem();
    if(pCurItem == nullptr)
        return;

    pCurItem->GetDraw()->OnMousePressEvent(pEvent);
}

/*virtual*/ void HyGuiRenderer::wheelEvent(QWheelEvent *pEvent) /*override*/
{
    if(m_pProjOwner == nullptr)
        return;

    ProjectItem *pCurItem = m_pProjOwner->GetCurrentOpenItem();
    if(pCurItem == nullptr)
        return;

    pCurItem->GetDraw()->OnMouseWheelEvent(pEvent);
}

/*virtual*/ void HyGuiRenderer::mouseMoveEvent(QMouseEvent *pEvent) /*override*/
{
    if(m_pProjOwner == nullptr)
        return;

    ProjectItem *pCurItem = m_pProjOwner->GetCurrentOpenItem();
    if(pCurItem == nullptr)
        return;

    pCurItem->GetDraw()->OnMouseMoveEvent(pEvent);
}

/*virtual*/ void HyGuiRenderer::mouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
    if(m_pProjOwner == nullptr)
        return;

    ProjectItem *pCurItem = m_pProjOwner->GetCurrentOpenItem();
    if(pCurItem == nullptr)
        return;

    pCurItem->GetDraw()->OnMouseReleaseEvent(pEvent);
}
