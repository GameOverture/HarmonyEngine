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
#include "WidgetRenderer.h"
#include "ItemProject.h"

HyGuiRenderer::HyGuiRenderer(QWidget *parent) : QOpenGLWidget(parent),
                                                m_pHyApp(NULL),
                                                m_pHyEngine(NULL),
                                                m_bIsUpdating(false)
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(20);
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

    //m_pHyEngine = new HyEngine(*GetHyApp());
}

/*virtual*/ void HyGuiRenderer::paintGL()
{
    if(m_pHyEngine && m_bIsUpdating == false)
    {
        m_bIsUpdating = true;
        if(m_pHyEngine->Update() == false)  // This will call WidgetRenderer::Update()
            HyGuiLog("Harmony Gfx requested exit program.", LOGTYPE_Info);

        m_bIsUpdating = false;
    }
}

/*virtual*/ void HyGuiRenderer::resizeGL(int w, int h)
{
    if(m_pHyApp)
        m_pHyApp->Window().SetResolution(glm::ivec2(w, h));
}

void HyGuiRenderer::LoadItemProject(ItemProject *pProj)
{
    if(m_pHyEngine)
        m_pHyEngine->Shutdown();
    
    delete m_pHyEngine;
    m_pHyEngine = NULL;
    
    if(pProj)
        m_pHyEngine = new HyEngine(*pProj->GetTabsManager());   // WidgetTabsManager is a IHyApplication
}
