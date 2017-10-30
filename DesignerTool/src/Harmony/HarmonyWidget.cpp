#include "HarmonyWidget.h"

#include "ProjectItemMimeData.h"

#include <QDragEnterEvent>

HarmonyWidget::HarmonyWidget(Project &projectRef) : QOpenGLWidget(nullptr),
                                                    m_ProjectRef(projectRef),
                                                    m_pHyEngine(nullptr)
{
    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(OnBootCheck()));
    m_pTimer->start(50);

    setAcceptDrops(true);
    RestoreCursor();
}

/*virtual*/ HarmonyWidget::~HarmonyWidget()
{
    m_pTimer->stop();

    makeCurrent();

    if(m_pHyEngine)
    {
        m_ProjectRef.Shutdown();
        m_pHyEngine->Shutdown();
    }

    // TODO: FIX THIS SHIT
    delete m_pHyEngine;
    m_pHyEngine = nullptr;
}

Project &HarmonyWidget::GetProject()
{
    return m_ProjectRef;
}

bool HarmonyWidget::IsProject(Project &projectRef)
{
    return &m_ProjectRef == &projectRef;
}

void HarmonyWidget::SetCursor(Qt::CursorShape eShape)
{
    setCursor(eShape);
}

void HarmonyWidget::RestoreCursor()
{
    setCursor(Qt::CrossCursor);
}

HyRendererInterop *HarmonyWidget::GetHarmonyRenderer()
{
    if(m_pHyEngine)
        return &m_pHyEngine->GetRenderer();
    else
        return nullptr;
}

/*virtual*/ void HarmonyWidget::initializeGL() /*override*/
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

    m_pHyEngine = HyEngine::GuiCreate(m_ProjectRef);

    if(m_pHyEngine == nullptr)
        HyGuiLog("HyEngine::GuiCreate returned nullptr", LOGTYPE_Error);
}

/*virtual*/ void HarmonyWidget::paintGL() /*override*/
{
    if(m_pHyEngine && m_pHyEngine->IsInitialized())
    {
        if(m_pHyEngine->Update() == false)
            HyGuiLog("Harmony Gfx requested exit program.", LOGTYPE_Info);
    }
}

/*virtual*/ void HarmonyWidget::resizeGL(int w, int h) /*override*/
{
    m_ProjectRef.SetRenderSize(w, h);
}

/*virtual*/ void HarmonyWidget::enterEvent(QEvent *pEvent) /*override*/
{
    this->setFocus();
}

/*virtual*/ void HarmonyWidget::leaveEvent(QEvent *pEvent) /*override*/
{
}

/*virtual*/ void HarmonyWidget::dragEnterEvent(QDragEnterEvent *pEvent) /*override*/
{
    ProjectTabBar *pTabBar = m_ProjectRef.GetTabBar();
    ProjectItem *pCurProjItem = pTabBar->tabData(pTabBar->currentIndex()).value<ProjectItem *>();

    if(pCurProjItem &&
       pEvent->mimeData()->hasFormat(HYGUI_MIMETYPE) &&
       static_cast<ProjectItem *>(pEvent->source()) != pCurProjItem &&
       pCurProjItem->GetType() == ITEM_Entity)
    {
        pEvent->acceptProposedAction();
    }
}

/*virtual*/ void HarmonyWidget::dropEvent(QDropEvent *pEvent) /*override*/
{
    ProjectTabBar *pTabBar = m_ProjectRef.GetTabBar();
    ProjectItem *pCurProjItem = pTabBar->tabData(pTabBar->currentIndex()).value<ProjectItem *>();

    if(pCurProjItem &&
       pEvent->mimeData()->hasFormat(HYGUI_MIMETYPE) &&
       static_cast<ProjectItem *>(pEvent->source()) != pCurProjItem &&
       pCurProjItem->GetType() == ITEM_Entity &&
       pCurProjItem->GetWidget())
    {

    }
}

/*virtual*/ void HarmonyWidget::keyPressEvent(QKeyEvent *pEvent) /*override*/
{
    ProjectItem *pCurItem = m_ProjectRef.GetCurrentOpenItem();
    if(pCurItem == nullptr)
        return;

    pCurItem->GetDraw()->OnKeyPressEvent(pEvent);
}

/*virtual*/ void HarmonyWidget::keyReleaseEvent(QKeyEvent *pEvent) /*override*/
{
    ProjectItem *pCurItem = m_ProjectRef.GetCurrentOpenItem();
    if(pCurItem == nullptr)
        return;

    pCurItem->GetDraw()->OnKeyReleaseEvent(pEvent);
}

/*virtual*/ void HarmonyWidget::mousePressEvent(QMouseEvent *pEvent) /*override*/
{
    ProjectItem *pCurItem = m_ProjectRef.GetCurrentOpenItem();
    if(pCurItem == nullptr)
        return;

    pCurItem->GetDraw()->OnMousePressEvent(pEvent);
}

/*virtual*/ void HarmonyWidget::wheelEvent(QWheelEvent *pEvent) /*override*/
{
    ProjectItem *pCurItem = m_ProjectRef.GetCurrentOpenItem();
    if(pCurItem == nullptr)
        return;

    pCurItem->GetDraw()->OnMouseWheelEvent(pEvent);
}

/*virtual*/ void HarmonyWidget::mouseMoveEvent(QMouseEvent *pEvent) /*override*/
{
    ProjectItem *pCurItem = m_ProjectRef.GetCurrentOpenItem();
    if(pCurItem == nullptr)
        return;

    pCurItem->GetDraw()->OnMouseMoveEvent(pEvent);
}

/*virtual*/ void HarmonyWidget::mouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
    ProjectItem *pCurItem = m_ProjectRef.GetCurrentOpenItem();
    if(pCurItem == nullptr)
        return;

    pCurItem->GetDraw()->OnMouseReleaseEvent(pEvent);
}

void HarmonyWidget::OnBootCheck()
{
    if(m_pHyEngine && m_pHyEngine->IsInitialized())
    {
        Q_EMIT HarmonyWidgetReady(this);

        m_pTimer->stop();
        if(false == m_pTimer->disconnect())
            HyGuiLog("HarmonyWidget::OnBootCheck could not disconnect its signal.", LOGTYPE_Error);

        connect(m_pTimer, SIGNAL(timeout()), this, SLOT(update()));
        m_pTimer->start(10);
    }
}
