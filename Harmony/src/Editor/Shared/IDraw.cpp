/**************************************************************************
 *	IDraw.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Designer Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "IDraw.h"
#include "ProjectItem.h"
#include "IModel.h"
#include "MainWindow.h"
#include "Global.h"
#include "Harmony.h"
#include "HarmonyWidget.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QKeyEvent>
#include <QApplication>

#define KEY_PanCamera Qt::Key_Space

IDraw::IDraw(ProjectItem *pProjItem, IHyApplication &hyApp) :   m_pProjItem(pProjItem),
																m_HyAppRef(hyApp),
																m_pCamera(nullptr),
																m_primOriginHorz(this),
																m_primOriginVert(this),
																m_bPanCameraKeyDown(false),
																m_bIsCameraPanning(false)
{
	m_pCamera = m_HyAppRef.Window().CreateCamera2d();
	m_pCamera->SetEnabled(false);

	std::vector<glm::vec2> lineList(2, glm::vec2());

	lineList[0].x = -5000.0f;
	lineList[0].y = 0.0f;
	lineList[1].x = 5000.0f;
	lineList[1].y = 0.0f;
	m_primOriginHorz.SetLineThickness(2.0f);
	m_primOriginHorz.SetTint(1.0f, 1.0f, 1.0f);
	m_primOriginHorz.SetEnabled(false);
	m_primOriginHorz.GetShape().SetAsLineChain(&lineList[0], static_cast<uint32>(lineList.size()));

	lineList[0].x = 0.0f;
	lineList[0].y = -5000.0f;
	lineList[1].x = 0.0f;
	lineList[1].y = 5000.0f;
	m_primOriginVert.SetLineThickness(2.0f);
	m_primOriginVert.SetTint(1.0f, 1.0f, 1.0f);
	m_primOriginVert.SetEnabled(false);
	m_primOriginVert.GetShape().SetAsLineChain(&lineList[0], static_cast<uint32>(lineList.size()));
}

/*virtual*/ IDraw::~IDraw()
{
	m_HyAppRef.Window().RemoveCamera(m_pCamera);
}

void IDraw::ApplyJsonData()
{
	if(m_pProjItem == nullptr)
		return;

	QJsonValue valueData = m_pProjItem->GetModel()->GetJson();
	QByteArray src = JsonValueToSrc(valueData);

	jsonxx::Value newValue;
	newValue.parse(src.toStdString());

	OnApplyJsonData(newValue);
}

void IDraw::Show()
{
	m_pCamera->SetEnabled(true);
	OnResizeRenderer();

	m_primOriginHorz.SetEnabled(true);
	m_primOriginVert.SetEnabled(true);

	OnShow(m_HyAppRef);
}

void IDraw::Hide()
{
	m_pCamera->SetEnabled(false);

	m_primOriginHorz.SetEnabled(false);
	m_primOriginVert.SetEnabled(false);

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
		Harmony::GetWidget(&m_pProjItem->GetProject())->SetCursor(Qt::OpenHandCursor);
		m_bPanCameraKeyDown = true;
	}
}

/*virtual*/ void IDraw::OnKeyReleaseEvent(QKeyEvent *pEvent)
{
	if(pEvent->key() == KEY_PanCamera)
	{
		m_bPanCameraKeyDown = false;

		if(m_bIsCameraPanning == false)
			Harmony::GetWidget(&m_pProjItem->GetProject())->RestoreCursor();
	}
}

/*virtual*/ void IDraw::OnMousePressEvent(QMouseEvent *pEvent)
{
	if(pEvent->button() == Qt::LeftButton)
	{
		if(m_bPanCameraKeyDown && m_bIsCameraPanning == false)
		{
			m_bIsCameraPanning = true;
			m_ptOldMousePos = pEvent->localPos();
			Harmony::GetWidget(&m_pProjItem->GetProject())->SetCursor(Qt::ClosedHandCursor);
		}
	}
}

/*virtual*/ void IDraw::OnMouseReleaseEvent(QMouseEvent *pEvent)
{
	if(pEvent->button() == Qt::LeftButton)
	{
		if(m_bIsCameraPanning)
		{
			m_bIsCameraPanning = false;
			if(m_bPanCameraKeyDown)
				Harmony::GetWidget(&m_pProjItem->GetProject())->SetCursor(Qt::OpenHandCursor);
			else
				Harmony::GetWidget(&m_pProjItem->GetProject())->RestoreCursor();
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
