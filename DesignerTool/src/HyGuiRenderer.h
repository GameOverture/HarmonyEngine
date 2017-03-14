/**************************************************************************
 *	HyGuiRenderer.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HYGUIRENDERER_H
#define HYGUIRENDERER_H

#include "GL/glew.h"
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QThread>

#include "Harmony/HyEngine.h"
#include "Project.h"

////class ItemProject;

//class HyGuiRendererLoadThread : public QThread
//{
//    Q_OBJECT

//    ItemProject *       m_pProjOwner;
//    QOpenGLContext *    m_pGLContext;
//    QSurface *          m_pSurface;

//public:
//    HyGuiRendererLoadThread(ItemProject *pProjOwner, QOpenGLContext *pGLContext, QSurface *pSurface, QObject *pParent) :    QThread(pParent),
//                                                                                                                            m_pProjOwner(pProjOwner),
//                                                                                                                            m_pGLContext(pGLContext),
//                                                                                                                            m_pSurface(pSurface)
//    { }

//    void run() Q_DECL_OVERRIDE;

//Q_SIGNALS:
//    void EngineLoaded(HyEngine *pNewHyEngine, QOpenGLContext *pGLContext);
//};

class HyGuiRenderer : public QOpenGLWidget//, protected QOpenGLFunctions
{
    Q_OBJECT

    ItemProject *       m_pProjOwner;
    HyEngine *          m_pHyEngine;
    
    bool                m_bIsUpdating;

    QTimer *            m_pTimer;

public:
    HyGuiRenderer(QWidget *parent = 0);
    HyGuiRenderer(ItemProject *pProj, QWidget *parent = 0);
    ~HyGuiRenderer();
    
    HyRendererInterop *GetHarmonyRenderer();

    // QOpenGLWidget overrides
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);

private Q_SLOTS:
    void OnBootCheck();
};

#endif // HYGUIRENDERER_H
