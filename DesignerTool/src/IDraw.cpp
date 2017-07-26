/**************************************************************************
 *	IDraw.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "IDraw.h"
#include "ProjectItem.h"
#include "IModel.h"
#include "MainWindow.h"
#include "HyGuiRenderer.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QKeyEvent>
#include <QApplication>

#define KEY_PanCamera Qt::Key_Space

IDraw::IDraw(ProjectItem *pProjItem, IHyApplication &hyApp) :   m_pProjItem(pProjItem),
                                                                m_HyAppRef(hyApp),
                                                                m_pCamera(nullptr),
                                                                m_bPanCameraKeyDown(false),
                                                                m_bIsCameraPanning(false)
{
    m_pCamera = m_HyAppRef.Window().CreateCamera2d();
    m_pCamera->SetEnabled(false);
}

/*virtual*/ IDraw::~IDraw()
{
    m_HyAppRef.Window().RemoveCamera(m_pCamera);
}

void IDraw::ApplyJsonData(bool bReloadInAssetManager)
{
    if(m_pProjItem == nullptr)
        return;

    QJsonValue valueData = m_pProjItem->GetModel()->GetSaveInfo(false);
    QByteArray src;
    if(valueData.isArray())
    {
        QJsonDocument tmpDoc(valueData.toArray());
        src = tmpDoc.toJson();
    }
    else
    {
        QJsonDocument tmpDoc(valueData.toObject());
        src = tmpDoc.toJson();
    }

    jsonxx::Value newValue;
    newValue.parse(src.toStdString());

    OnApplyJsonData(newValue, bReloadInAssetManager);
}

void IDraw::Show()
{
    m_pCamera->SetEnabled(true);
    OnResizeRenderer();
    OnShow(m_HyAppRef);
}

void IDraw::Hide()
{
    m_pCamera->SetEnabled(false);
    OnHide(m_HyAppRef);
}

void IDraw::ResizeRenderer()
{
    OnResizeRenderer();
}

/*virtual*/ void IDraw::OnKeyPressEvent(QKeyEvent *pEvent)
{
    if(pEvent->key() == KEY_PanCamera)
    {
        MainWindow::GetCurrentRenderer()->SetCursor(Qt::OpenHandCursor);
        m_bPanCameraKeyDown = true;
    }
}

/*virtual*/ void IDraw::OnKeyReleaseEvent(QKeyEvent *pEvent)
{
    if(pEvent->key() == KEY_PanCamera)
    {
        m_bPanCameraKeyDown = false;

        if(m_bIsCameraPanning == false)
            MainWindow::GetCurrentRenderer()->RestoreCursor();
    }
}

/*virtual*/ void IDraw::OnMousePressEvent(QMouseEvent *pEvent)
{
    if(pEvent->button() == Qt::LeftButton)
    {
        if(m_bPanCameraKeyDown && m_bIsCameraPanning == false);
        {
            m_bIsCameraPanning = true;\
            m_ptOldMousePos = pEvent->localPos();
            MainWindow::GetCurrentRenderer()->SetCursor(Qt::ClosedHandCursor);
        }
    }
}

/*virtual*/ void IDraw::OnMouseReleaseEvent(QMouseEvent *pEvent)
{
    if(pEvent->button() == Qt::LeftButton)
    {
        if(m_bIsCameraPanning);
        {
            m_bIsCameraPanning = false;
            if(m_bPanCameraKeyDown)
                MainWindow::GetCurrentRenderer()->SetCursor(Qt::OpenHandCursor);
            else
                MainWindow::GetCurrentRenderer()->RestoreCursor();
        }
    }
}

/*virtual*/ void IDraw::OnMouseWheelEvent(QWheelEvent *pEvent)
{
    QPoint numPixels = pEvent->pixelDelta();
    QPoint numDegrees = pEvent->angleDelta() / 8;

    /*if(!numPixels.isNull())
    {
        //scrollWithPixels(numPixels);
    }
    else */if(!numDegrees.isNull())
    {
        QPoint numSteps = numDegrees / 15;
        m_pCamera->scale.TweenOffset(numSteps.y() * 0.2f, numSteps.y() * 0.2f, 0.5f, HyTween::QuadInOut);
        //scrollWithDegrees(numSteps);
    }

    pEvent->accept();
}

/*virtual*/ void IDraw::OnMouseMoveEvent(QMouseEvent *pEvent)
{
    if(m_bIsCameraPanning)//0 != (pEvent->buttons() & Qt::MidButton))
    {
        QPointF ptCurMousePos = pEvent->localPos();
        if(ptCurMousePos != m_ptOldMousePos)
        {
            QPointF vDeltaMousePos = m_ptOldMousePos - ptCurMousePos;
            m_pCamera->pos.Offset(vDeltaMousePos.x(), vDeltaMousePos.y() * -1.0f);
        }

        m_ptOldMousePos = ptCurMousePos;
    }
}
