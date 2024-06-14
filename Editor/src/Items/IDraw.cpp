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
#include "GlobalUndoCmds.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QKeyEvent>
#include <QApplication>
#include <QString>

#define KEY_PanUp Qt::Key_W
#define KEY_PanDown Qt::Key_S
#define KEY_PanLeft Qt::Key_A
#define KEY_PanRight Qt::Key_D

const QString g_sZoomLevels[HYNUM_ZOOMLEVELS] = { "6.25%","12.5%", "25%",  "33.33%","50%", "75%","100%","200%","300%","400%","500%","600%","800%","1200%","1600%" };

IDraw::IDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef) :
	m_pProjItem(pProjItem),
	m_pCamera(HyEngine::Window().GetCamera2d(0)),
	m_uiPanFlags(0),
	m_bIsMiddleMouseDown(false),
	m_ptCamPos(0.0f, 0.0f),
	m_fCamZoom(1.0f),
	m_eModifyingGuidePending(HYORIENT_Null),
	m_sZoomStatus("100%")
{
	if(HyGlobal::IsItemFileDataValid(initFileDataRef))
	{
		HySetVec(m_ptCamPos, initFileDataRef.m_Meta["CameraPos"].isArray() ? static_cast<float>(initFileDataRef.m_Meta["CameraPos"].toArray()[0].toDouble()) : 0.0f,
							 initFileDataRef.m_Meta["CameraPos"].isArray() ? static_cast<float>(initFileDataRef.m_Meta["CameraPos"].toArray()[1].toDouble()) : 0.0f);
		m_fCamZoom = static_cast<float>(initFileDataRef.m_Meta["CameraZoom"].toDouble());

		m_pCamera->SetZoom(m_fCamZoom);
		HyZoomLevel eZoomLevel = m_pCamera->SetZoomLevel();

		m_fCamZoom = m_pCamera->GetZoom();
		m_sZoomStatus = g_sZoomLevels[eZoomLevel];

		QJsonArray guideHorzArray = initFileDataRef.m_Meta["guideHorzArray"].toArray();
		for(int i = 0; i < guideHorzArray.size(); ++i)
			AllocateGuide(HYORIENT_Horizontal, guideHorzArray[i].toInt());
		
		QJsonArray guideVertArray = initFileDataRef.m_Meta["guideVertArray"].toArray();
		for(int i = 0; i < guideVertArray.size(); ++i)
			AllocateGuide(HYORIENT_Vertical, guideVertArray[i].toInt());
	}

	m_PendingGuide.SetTint(HyColor::DarkCyan);
	m_PendingGuide.SetVisible(false);
	m_PendingGuide.UseWindowCoordinates();
	ChildAppend(m_PendingGuide);
}

/*virtual*/ IDraw::~IDraw()
{
	for(HyPrimitive2d *pGuide : m_GuideMap.values())
		delete pGuide;
}

void IDraw::GetCameraInfo(glm::vec2 &ptPosOut, float &fZoomOut)
{
	ptPosOut = m_ptCamPos;
	fZoomOut = m_fCamZoom;
}

void IDraw::SetCamera(glm::vec2 ptCamPos, float fZoom)
{
	m_ptCamPos = HyMath::RoundVec(ptCamPos);
	m_fCamZoom = fZoom;

	m_pCamera->pos.Set(m_ptCamPos);
	m_pCamera->SetZoom(m_fCamZoom);

	CameraUpdated();
}

void IDraw::StopCameraPanning()
{
	m_uiPanFlags = 0;
}

void IDraw::ApplyJsonData()
{
	if(m_pProjItem == nullptr)
		return;

	FileDataPair itemFileData;
	m_pProjItem->GetLatestFileData(itemFileData);

	if(m_pProjItem->GetType() == ITEM_Entity)
		OnApplyJsonMeta(itemFileData.m_Meta);
	else
	{
		QByteArray src = JsonValueToSrc(itemFileData.m_Data);
		HyJsonDoc itemDataDoc;
		if(itemDataDoc.ParseInsitu(src.data()).HasParseError())
			HyGuiLog("IDraw::ApplyJsonData failed to parse", LOGTYPE_Error);

		OnApplyJsonData(itemDataDoc);
	}
}

void IDraw::Show()
{
	m_pCamera->pos.Set(m_ptCamPos);
	m_pCamera->SetZoom(m_fCamZoom);
	CameraUpdated();

	OnResizeRenderer();

	OnShow();
	UpdateDrawStatus(m_sSizeStatus);
}

void IDraw::Hide()
{
	OnHide();
}

void IDraw::ResizeRenderer()
{
	glm::vec2 vWindowSize = HyEngine::Window().GetWindowSize();
	for(auto iter = m_GuideMap.begin(); iter != m_GuideMap.end(); ++iter)
	{
		if(iter.key().first == HYORIENT_Horizontal)
			iter.value()->SetAsLineSegment(glm::vec2(0.0f, 0.0f), glm::vec2(vWindowSize.x, 0.0f));
		else
			iter.value()->SetAsLineSegment(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, vWindowSize.y));
	}

	OnResizeRenderer();
	CameraUpdated();
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
	if(pEvent->key() == KEY_PanUp)
		m_uiPanFlags |= PAN_UP;
	else if(pEvent->key() == KEY_PanDown)
		m_uiPanFlags |= PAN_DOWN;
	else if(pEvent->key() == KEY_PanLeft)
		m_uiPanFlags |= PAN_LEFT;
	else if(pEvent->key() == KEY_PanRight)
		m_uiPanFlags |= PAN_RIGHT;
}

/*virtual*/ void IDraw::OnKeyReleaseEvent(QKeyEvent *pEvent)
{
	if(pEvent->key() == KEY_PanUp)
		m_uiPanFlags &= ~PAN_UP;
	else if(pEvent->key() == KEY_PanDown)
		m_uiPanFlags &= ~PAN_DOWN;
	else if(pEvent->key() == KEY_PanLeft)
		m_uiPanFlags &= ~PAN_LEFT;
	else if(pEvent->key() == KEY_PanRight)
		m_uiPanFlags &= ~PAN_RIGHT;
}

/*virtual*/ void IDraw::OnMousePressEvent(QMouseEvent *pEvent)
{
	if(pEvent->button() == Qt::MiddleButton)
	{
		if(m_bIsMiddleMouseDown == false)
		{
			m_bIsMiddleMouseDown = true;
			m_ptOldMousePos = pEvent->localPos();
			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->SetCursorShape(Qt::ClosedHandCursor);
		}
	}

	// If hovering over an existing guide, then "select" it by removing it, and starting SetPendingGuide()
	Qt::CursorShape eCurCursorShape = Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->GetCursorShape();
	glm::vec2 ptWorldMousePos;
	if(pEvent->button() == Qt::LeftButton &&
	   m_GuideMap.empty() == false &&
	   HyEngine::Input().GetWorldMousePos(ptWorldMousePos) &&
	   (eCurCursorShape == Qt::SplitHCursor || eCurCursorShape == Qt::SplitVCursor))
	{
		// Find closest existing guide
		QPair<HyOrientation, int> closestGuideKey;
		int iClosestDist = INT_MAX;
		for(auto iter = m_GuideMap.begin(); iter != m_GuideMap.end(); ++iter)
		{
			if(eCurCursorShape == Qt::SplitVCursor && iter.key().first == HYORIENT_Horizontal)
			{
				int iDist = abs((int)ptWorldMousePos.y - iter.key().second);
				if(iDist < iClosestDist)
				{
					iClosestDist = iDist;
					closestGuideKey = iter.key();
				}
			}
			else if(eCurCursorShape == Qt::SplitHCursor && iter.key().first == HYORIENT_Vertical)
			{
				int iDist = abs((int)ptWorldMousePos.x - iter.key().second);
				if(iDist < iClosestDist)
				{
					iClosestDist = iDist;
					closestGuideKey = iter.key();
				}
			}
		}
		if(iClosestDist == INT_MAX)
			HyGuiLog("IDraw::OnMousePressEvent failed to find closest guide", LOGTYPE_Error);
		else
		{
			delete m_GuideMap[closestGuideKey];
			m_GuideMap.remove(closestGuideKey);

			m_eModifyingGuidePending = closestGuideKey.first;
			SetPendingGuide(m_eModifyingGuidePending);
		}
	}
}

/*virtual*/ void IDraw::OnMouseReleaseEvent(QMouseEvent *pEvent)
{
	if(pEvent->button() == Qt::MiddleButton)
	{
		if(m_bIsMiddleMouseDown)
		{
			m_bIsMiddleMouseDown = false;
			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->RestoreCursorShape();
		}
	}

	if(m_eModifyingGuidePending != HYORIENT_Null)
	{
		glm::vec2 ptWorldPos;
		if(HyEngine::Input().GetWorldMousePos(ptWorldPos))
		{
			int iPos = m_eModifyingGuidePending == HYORIENT_Horizontal ? static_cast<int>(ptWorldPos.y) : static_cast<int>(ptWorldPos.x);
			TryAllocateGuide(m_eModifyingGuidePending, iPos);
			m_eModifyingGuidePending = HYORIENT_Null;
			SetPendingGuide(HYORIENT_Null);
		}
	}
}

/*virtual*/ void IDraw::OnMouseWheelEvent(QWheelEvent *pEvent)
{
	QPoint numPixels = pEvent->pixelDelta();
	QPoint numDegrees = pEvent->angleDelta() / 8;

	if(!numDegrees.isNull())
	{
		int iZoomLevel = m_pCamera->SetZoomLevel();
		if(numDegrees.y() < 0.0f)
			iZoomLevel--;
		else 
			iZoomLevel++;

		iZoomLevel = HyMath::Clamp(iZoomLevel, 0, HYNUM_ZOOMLEVELS - 1);
		m_pCamera->SetZoomLevel(static_cast<HyZoomLevel>(iZoomLevel));

		m_sZoomStatus = g_sZoomLevels[iZoomLevel];
		UpdateDrawStatus(m_sSizeStatus);

		CameraUpdated();
	}

	pEvent->accept();
}

/*virtual*/ void IDraw::OnMouseMoveEvent(QMouseEvent *pEvent)
{
	if(MainWindow::GetCurrentLoading().empty() == false)
		Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->SetCursorShape(Qt::WaitCursor);
	else if(Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->GetCursorShape() == Qt::WaitCursor)
		Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->RestoreCursorShape();

	QPointF ptCurMousePos = pEvent->localPos();

	if(m_eModifyingGuidePending != HYORIENT_Null)
	{
		SetPendingGuide(m_eModifyingGuidePending);
	}
	else
	{
		// Check if mouse is over an existing guide
		glm::vec2 ptWorldMousePos;
		if(m_GuideMap.empty() == false && HyEngine::Input().GetWorldMousePos(ptWorldMousePos))
		{
			const int iSELECT_RADIUS = 2;
			bool bIsHovering = false;
			for(auto iter = m_GuideMap.begin(); iter != m_GuideMap.end(); ++iter)
			{
				int iWorldPos = iter.key().second;

				if(iter.key().first == HYORIENT_Horizontal &&
					ptWorldMousePos.y >= (iWorldPos - iSELECT_RADIUS) &&
					ptWorldMousePos.y <= (iWorldPos + iSELECT_RADIUS))
				{
					Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->SetCursorShape(Qt::SplitVCursor);
					bIsHovering = true;
				}
				else if(iter.key().first == HYORIENT_Vertical &&
					ptWorldMousePos.x >= (iWorldPos - iSELECT_RADIUS) &&
					ptWorldMousePos.x <= (iWorldPos + iSELECT_RADIUS))
				{
					Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->SetCursorShape(Qt::SplitHCursor);
					bIsHovering = true;
				}
			}

			if(bIsHovering == false)
				Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->RestoreCursorShape();
		}
	}

	if(m_bIsMiddleMouseDown)
	{
		if(ptCurMousePos != m_ptOldMousePos)
		{
			QPointF vDeltaMousePos = m_ptOldMousePos - ptCurMousePos;
			m_pCamera->pos.Offset(static_cast<float>(vDeltaMousePos.x()), vDeltaMousePos.y() * -1.0f);

			CameraUpdated();
		}

		m_ptOldMousePos = ptCurMousePos;
	}

	UpdateDrawStatus(m_sSizeStatus);
}

void IDraw::SetPendingGuide(HyOrientation eOrientation)
{
	glm::vec2 ptMousePos = HyEngine::Input().GetMousePos();
	glm::ivec2 vRendererSize = HyEngine::Window().GetWindowSize();

	if(eOrientation == HYORIENT_Horizontal)
	{
		m_PendingGuide.SetAsLineSegment(glm::vec2(0.0f, ptMousePos.y),
										glm::vec2(static_cast<float>(vRendererSize.x), ptMousePos.y));
		m_PendingGuide.SetVisible(true);
	}
	else if(eOrientation == HYORIENT_Vertical)
	{
		m_PendingGuide.SetAsLineSegment(glm::vec2(ptMousePos.x, 0.0f),
										glm::vec2(ptMousePos.x, static_cast<float>(vRendererSize.y)));
		m_PendingGuide.SetVisible(true);
	}
	else
		m_PendingGuide.SetVisible(false);
}

bool IDraw::TryAllocateGuide(HyOrientation eOrientation, int iWorldPos)
{
	b2AABB aabb;
	m_pCamera->CalcWorldViewBounds(aabb);
	if((eOrientation == HYORIENT_Horizontal && (iWorldPos < aabb.lowerBound.y || iWorldPos > aabb.upperBound.y)) ||
		(eOrientation == HYORIENT_Vertical && (iWorldPos < aabb.lowerBound.x || iWorldPos > aabb.upperBound.x)))
	{
		return false;
	}

	AllocateGuide(eOrientation, iWorldPos);
	return true;
}

void IDraw::AllocateGuide(HyOrientation eOrientation, int iWorldPos)
{
	if(m_GuideMap.contains(QPair<HyOrientation, int>(eOrientation, iWorldPos)))
		return;

	HyPrimitive2d *pNewGuide = new HyPrimitive2d();
	pNewGuide->SetTint(HyColor::Cyan);
	pNewGuide->UseWindowCoordinates();

	glm::vec2 vWindowSize = HyEngine::Window().GetWindowSize();
	if(eOrientation == HYORIENT_Horizontal)
		pNewGuide->SetAsLineSegment(glm::vec2(0.0f, 0.0f), glm::vec2(vWindowSize.x, 0.0f));
	else
		pNewGuide->SetAsLineSegment(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, vWindowSize.y));

	ChildAppend(*pNewGuide);
	m_GuideMap.insert(QPair<HyOrientation, int>(eOrientation, iWorldPos), pNewGuide);

	CameraUpdated();
}

/*virtual*/ void IDraw::OnUpdate() /*override*/
{
	if(m_pProjItem == nullptr)
		return;

	HarmonyWidget *pHarmonyWidget = Harmony::GetHarmonyWidget(&m_pProjItem->GetProject());
	if(pHarmonyWidget->IsShowRulersMouse() && m_uiPanFlags || IsCameraPanning())
	{
		if(m_uiPanFlags & PAN_UP)
			m_pCamera->PanUp();
		if(m_uiPanFlags & PAN_DOWN)
			m_pCamera->PanDown();
		if(m_uiPanFlags & PAN_LEFT)
			m_pCamera->PanLeft();
		if(m_uiPanFlags & PAN_RIGHT)
			m_pCamera->PanRight();

		CameraUpdated();
	}
}

bool IDraw::IsCameraPanning() const
{
	return m_pCamera->IsPanning() || m_bIsMiddleMouseDown;
}

void IDraw::CameraUpdated()
{
	m_ptCamPos = m_pCamera->pos.Get();
	m_fCamZoom = m_pCamera->GetZoom();

	if(m_pProjItem)
		Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->RefreshRulers();

	if(m_GuideMap.empty() == false)
	{
		for(auto iter = m_GuideMap.begin(); iter != m_GuideMap.end(); ++iter)
		{
			QPair<HyOrientation, int> keyPair = iter.key();
			HyPrimitive2d *pGuide = iter.value();

			if(keyPair.first == HYORIENT_Horizontal)
			{
				glm::vec2 ptCamProjPos;
				m_pCamera->ProjectToCamera(glm::vec2(0.0f, keyPair.second), ptCamProjPos);
				pGuide->pos.Set(0.0f, ptCamProjPos.y);
			}
			else // HYORIENT_Vertical
			{
				glm::vec2 ptCamProjPos;
				m_pCamera->ProjectToCamera(glm::vec2(keyPair.second, 0.0f), ptCamProjPos);
				pGuide->pos.Set(ptCamProjPos.x, 0.0f);
			}
		}
	}

	OnCameraUpdated();
}

float IDraw::GetLineThickness(HyZoomLevel eZoomLevel)
{
	switch(eZoomLevel)
	{
	case HYZOOM_6:
		return 8.0f;
	
	case HYZOOM_12:
		return 6.0f;
	
	case HYZOOM_25:
	case HYZOOM_33:
		return 4.0f;
	
	case HYZOOM_50:
	case HYZOOM_75:
		return 2.0f;

	case HYZOOM_100:
	default:
		return 1.0f;
			
	case HYZOOM_200:
		return 0.5f;
			
	case HYZOOM_300:
		return 0.333333f;
		
	case HYZOOM_400:
		return 0.25f;

	case HYZOOM_500:
		return 0.2f;

	case HYZOOM_600:
		return 0.166666f;

	case HYZOOM_800:
		return 0.125f;

	case HYZOOM_1200:
		return 0.083333f;

	case HYZOOM_1600:
		return 0.0625f;
	}
}
