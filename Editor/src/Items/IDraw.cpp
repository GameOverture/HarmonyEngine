/**************************************************************************
 *	IDraw.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "IDraw.h"
#include "ProjectItemData.h"
#include "IModel.h"
#include "MainWindow.h"
#include "Harmony.h"
#include "HarmonyWidget.h"
#include "GlobalUndoCmds.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QKeyEvent>
#include <QApplication>
#include <QString>

#define KEY_PanCamera Qt::Key_Space

const int32 iNUM_ZOOM_LEVELS = 14;
QString g_sZoomLevels[iNUM_ZOOM_LEVELS] = { "1600%", "800%", "600%",   "500%", "400%", "300%", "200%", "100%", "75%", "50%", "33.3%", "25%", "12.5%", "6.25%" };
float g_fZoomLevels[iNUM_ZOOM_LEVELS] =   { 0.0625f, 0.125f, 0.166667f, 0.2f,  0.25f,  0.375f,  0.5f,   1.0f,   1.5f,  2.0f,  3.0f,   4.0f,   8.0f,    16.0f };

IDraw::IDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef) :
	m_pProjItem(pProjItem),
	m_pCamera(HyEngine::Window().GetCamera2d(0)),
	m_bPanCameraKeyDown(false),
	m_bIsCameraPanning(false),
	m_ptCamPos(0.0f, 0.0f),
	m_fCamZoom(1.0f),
	m_sZoomStatus("100%")
{
	if(HyGlobal::IsItemFileDataValid(initFileDataRef))
	{
		HySetVec(m_ptCamPos, initFileDataRef.m_Meta["CameraPos"].isArray() ? static_cast<float>(initFileDataRef.m_Meta["CameraPos"].toArray()[0].toDouble()) : 0.0f,
							 initFileDataRef.m_Meta["CameraPos"].isArray() ? static_cast<float>(initFileDataRef.m_Meta["CameraPos"].toArray()[1].toDouble()) : 0.0f);
		m_fCamZoom = static_cast<float>(initFileDataRef.m_Meta["CameraZoom"].toDouble());

		int32 iZoomLevel = 0;
		for(; iZoomLevel < iNUM_ZOOM_LEVELS; ++iZoomLevel)
		{
			if(m_fCamZoom <= g_fZoomLevels[iZoomLevel])
			{
				m_fCamZoom = g_fZoomLevels[iZoomLevel];
				break;
			}
		}
		iZoomLevel = HyClamp(iZoomLevel, 0, iNUM_ZOOM_LEVELS - 1);

		m_fCamZoom = g_fZoomLevels[iZoomLevel];
		m_sZoomStatus = g_sZoomLevels[iZoomLevel];

		m_pCamera->SetZoom(m_fCamZoom);
	}
	//m_pCamera->SetVisible(false);
}

/*virtual*/ IDraw::~IDraw()
{
	//if(HyEngine::IsInitialized())
	//	HyEngine::Window().RemoveCamera(m_pCamera);
}

void IDraw::GetCameraInfo(glm::vec2 &ptPosOut, float &fZoomOut)
{
	ptPosOut = m_ptCamPos;
	fZoomOut = m_fCamZoom;
}

void IDraw::SetCamera(glm::vec2 ptCamPos, float fZoom)
{
	m_ptCamPos = ptCamPos;
	m_fCamZoom = fZoom;

	m_pCamera->pos.Set(m_ptCamPos);
	m_pCamera->SetZoom(m_fCamZoom);
}

void IDraw::ApplyJsonData()
{
	if(m_pProjItem == nullptr)
		return;

	FileDataPair itemFileData;
	m_pProjItem->GetLatestFileData(itemFileData);

	QByteArray src = JsonValueToSrc(m_pProjItem->GetType() == ITEM_Entity ? itemFileData.m_Meta : itemFileData.m_Data);

	HyJsonDoc itemDataDoc;
	if(itemDataDoc.ParseInsitu(src.data()).HasParseError())
		HyGuiLog("IDraw::ApplyJsonData failed to parse", LOGTYPE_Error);

	OnApplyJsonData(itemDataDoc);
}

void IDraw::Show()
{
	m_pCamera->pos.Set(m_ptCamPos);
	m_pCamera->SetZoom(m_fCamZoom);

	//m_pCamera->SetVisible(true);
	OnResizeRenderer();

	OnShow();
	UpdateDrawStatus(m_sSizeStatus);
}

void IDraw::Hide()
{
	//m_pCamera->SetVisible(false);
	OnHide();
}

void IDraw::ResizeRenderer()
{
	OnResizeRenderer();
}

void IDraw::UpdateDrawStatus(QString sSizeDescription)
{
	m_sSizeStatus = sSizeDescription;
	glm::vec2 ptWorldMousePos;
	if(HyEngine::Input().GetWorldMousePos(ptWorldMousePos) == false)
		MainWindow::SetDrawStatus("", m_sSizeStatus, m_sZoomStatus);
	else
		MainWindow::SetDrawStatus(QString::number(floor(ptWorldMousePos.x)) % " " % QString::number(floor(ptWorldMousePos.y)), m_sSizeStatus, m_sZoomStatus);
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

			if(m_ptCamPos.x != m_pCamera->pos.X() || m_ptCamPos.y != m_pCamera->pos.Y())
			{
				QUndoCommand *pCmd = new UndoCmd_CameraUpdate("Camera Pan", *m_pProjItem, m_ptCamPos, m_fCamZoom, m_pCamera->pos.Get(), m_pCamera->GetZoom());
				m_pProjItem->GetUndoStack()->push(pCmd);
				
				m_ptCamPos = m_pCamera->pos.Get();
			}
		}
	}
}

/*virtual*/ void IDraw::OnMouseWheelEvent(QWheelEvent *pEvent)
{
	QPoint numPixels = pEvent->pixelDelta();
	QPoint numDegrees = pEvent->angleDelta() / 8;

	if(!numDegrees.isNull())
	{
		float fCurScale = m_pCamera->scale.X();
		int32 iZoomLevel = 0;
		for(; iZoomLevel < iNUM_ZOOM_LEVELS; ++iZoomLevel)
		{
			if(fCurScale <= g_fZoomLevels[iZoomLevel])
			{
				fCurScale = g_fZoomLevels[iZoomLevel];
				break;
			}
		}

		if(numDegrees.y() < 0.0f)
			iZoomLevel++;
		else 
			iZoomLevel--;
		iZoomLevel = HyClamp(iZoomLevel, 0, iNUM_ZOOM_LEVELS - 1);

		fCurScale = g_fZoomLevels[iZoomLevel];
		m_sZoomStatus = g_sZoomLevels[iZoomLevel];

		m_pCamera->SetZoom(fCurScale);
		//m_pCamera->scale.TweenOffset(fCurScale, fCurScale, 0.5f, HyTween::QuadInOut);

		UpdateDrawStatus(m_sSizeStatus);

		QUndoCommand *pCmd = new UndoCmd_CameraUpdate("Camera Zoom", *m_pProjItem, m_ptCamPos, m_fCamZoom, m_pCamera->pos.Get(), m_pCamera->GetZoom());
		m_pProjItem->GetUndoStack()->push(pCmd);

		m_fCamZoom = m_pCamera->GetZoom();
	}

	pEvent->accept();
}

/*virtual*/ void IDraw::OnMouseMoveEvent(QMouseEvent *pEvent)
{
	QPointF ptCurMousePos = pEvent->localPos();

	if(m_bIsCameraPanning)//0 != (pEvent->buttons() & Qt::MidButton))
	{
		if(ptCurMousePos != m_ptOldMousePos)
		{
			QPointF vDeltaMousePos = m_ptOldMousePos - ptCurMousePos;
			m_pCamera->pos.Offset(vDeltaMousePos.x(), vDeltaMousePos.y() * -1.0f);
		}

		m_ptOldMousePos = ptCurMousePos;
	}

	UpdateDrawStatus(m_sSizeStatus);
}
