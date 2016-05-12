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

    m_pHyEngine = new HyEngine(*GetHyApp());
}

/*virtual*/ void HyGuiRenderer::paintGL()
{
    if(m_bIsUpdating == false)
    {
        m_bIsUpdating = true;
        if(m_pHyEngine->Update() == false)  // This will call WidgetRenderer::Update()
            HyGuiLog("Harmony Gfx requested exit program.", LOGTYPE_Info);

        m_bIsUpdating = false;
    }
}

/*virtual*/ void HyGuiRenderer::resizeGL(int w, int h)
{
    GetHyApp()->Window().SetResolution(glm::ivec2(w, h));
}

void HyGuiRenderer::Reload(ItemProject *pProj)
{
    m_pHyEngine->m_pAssetManager->Reload(sNewDataDir.toStdString());
}

IHyApplication *HyGuiRenderer::GetHyApp()
{
    WidgetRenderer *pGameApp = reinterpret_cast<WidgetRenderer *>(parent());
    return static_cast<IHyApplication *>(pGameApp);
}
