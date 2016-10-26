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
//#include <QGLFormat>
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
    
    doneCurrent();
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
    //    QGLFormat format;
    //    format.setVersion(4, 2);
    //    format.setProfile(QGLFormat::CoreProfile);
    //    format.setSampleBuffers(true);
    //    setFormat(format);

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
