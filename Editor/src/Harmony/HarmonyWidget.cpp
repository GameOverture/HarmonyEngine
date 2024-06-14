/**************************************************************************
 *	HarmonyWidget.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "HarmonyWidget.h"
#include "ui_HarmonyWidget.h"
#include "Project.h"
#include "MainWindow.h"

#include <QColorDialog>
#include <QMouseEvent>

HarmonyWidget::HarmonyWidget(Project *pProject, QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::HarmonyWidget),
	m_RulerHorzScene(this),
	m_RulerVertScene(this),
	m_eGuidePending(HYORIENT_Null)
{
	ui->setupUi(this);
	ui->wgtHarmony->InitProject(pProject);

	ui->rulerHorz->setScene(&m_RulerHorzScene);
	ui->rulerHorz->translate(0, 0);
	ui->rulerHorz->Init(HYORIENT_Horizontal, HyColor::ContainerPanel);

	ui->rulerVert->setScene(&m_RulerVertScene);
	ui->rulerVert->Init(HYORIENT_Vertical, HyColor::ContainerPanel);

	RestoreCursorShape();
}

HarmonyWidget::~HarmonyWidget()
{
	delete ui;
}

Project *HarmonyWidget::GetProject()
{
	return ui->wgtHarmony->GetProject();
}

bool HarmonyWidget::IsProject(Project *pProjectToTest)
{
	return ui->wgtHarmony->IsProject(pProjectToTest);
}

void HarmonyWidget::CloseProject()
{
	ui->wgtHarmony->CloseProject();
}

Qt::CursorShape HarmonyWidget::GetCursorShape() const
{
	return cursor().shape();
}

void HarmonyWidget::SetCursorShape(Qt::CursorShape eShape)
{
	setCursor(eShape);
}

void HarmonyWidget::RestoreCursorShape()
{
	setCursor(Qt::ArrowCursor);
}

WgtHarmony *HarmonyWidget::GetWgtHarmony()
{
	return ui->wgtHarmony;
}

HyRendererInterop *HarmonyWidget::GetHarmonyRenderer()
{
	return ui->wgtHarmony->GetHarmonyRenderer();
}

void HarmonyWidget::RefreshRulers()
{
	ui->rulerHorz->update();
	ui->rulerVert->update();
}

bool HarmonyWidget::IsShowRulersMouse() const
{
	return ui->rulerHorz->IsShowMouse();
}

void HarmonyWidget::ShowRulerMouse(bool bShow)
{
	ui->rulerHorz->ShowMouse(bShow);
	ui->rulerVert->ShowMouse(bShow);
	RefreshRulers();
}

void HarmonyWidget::OnRulerMouseMoveEvent(HyOrientation eOrientation, QMouseEvent *pEvent)
{
	if(m_eGuidePending != HYORIENT_Null)
	{
		if(m_eGuidePending != eOrientation)
			HyGuiLog("HarmonyWidget::OnRulerMouseMoveEvent - Wrong orientation set in m_eGuidePending", LOGTYPE_Error);

		QPointF ptCurMousePos = pEvent->localPos();
		if(eOrientation == HYORIENT_Horizontal)
			HyEngine::Input().SetWidgetMousePos(glm::vec2(ptCurMousePos.x(), ptCurMousePos.y() - RULER_WIDTH));
		else
			HyEngine::Input().SetWidgetMousePos(glm::vec2(ptCurMousePos.x() - RULER_WIDTH, ptCurMousePos.y()));

		if(GetWgtHarmony() &&
			GetWgtHarmony()->GetProject() &&
			GetWgtHarmony()->GetProject()->GetCurrentOpenItem())
		{
			ProjectItemData *pCurProjItem = GetWgtHarmony()->GetProject()->GetCurrentOpenItem();
			pCurProjItem->GetDraw()->SetPendingGuide(eOrientation);
		}
	}
}

void HarmonyWidget::OnWgtMouseMoveEvent(IDraw *pDrawItem, QMouseEvent *pEvent)
{
	if(MainWindow::GetCurrentLoading().empty() == false)
		SetCursorShape(Qt::WaitCursor);
	else if(GetCursorShape() == Qt::WaitCursor)
		RestoreCursorShape();

	QPointF ptCurMousePos = pEvent->localPos();
	HyEngine::Input().SetWidgetMousePos(glm::vec2(ptCurMousePos.x(), ptCurMousePos.y()));

	if(m_eGuidePending != HYORIENT_Null && pDrawItem)
		pDrawItem->SetPendingGuide(m_eGuidePending);
	else if(pDrawItem)
	{
		// Check if mouse is over an existing guide
		glm::vec2 ptWorldMousePos;
		if(pDrawItem->GetGuideMap().empty() == false && HyEngine::Input().GetWorldMousePos(ptWorldMousePos))
		{
			const int iSELECT_RADIUS = 2;
			bool bIsHovering = false;
			for(auto iter = pDrawItem->GetGuideMap().begin(); iter != pDrawItem->GetGuideMap().end(); ++iter)
			{
				int iWorldPos = iter.key().second;

				if(iter.key().first == HYORIENT_Horizontal &&
					ptWorldMousePos.y >= (iWorldPos - iSELECT_RADIUS) &&
					ptWorldMousePos.y <= (iWorldPos + iSELECT_RADIUS))
				{
					SetCursorShape(Qt::SplitVCursor);
					bIsHovering = true;
				}
				else if(iter.key().first == HYORIENT_Vertical &&
					ptWorldMousePos.x >= (iWorldPos - iSELECT_RADIUS) &&
					ptWorldMousePos.x <= (iWorldPos + iSELECT_RADIUS))
				{
					SetCursorShape(Qt::SplitHCursor);
					bIsHovering = true;
				}
			}

			if(bIsHovering == false)
				RestoreCursorShape();
		}
	}

	RefreshRulers();
}

void HarmonyWidget::OnRulerMousePressEvent(HyOrientation eOrientation, QMouseEvent *pEvent)
{
	if(pEvent->button() == Qt::LeftButton)
	{
		if(GetProject() == nullptr)
			return;

		m_eGuidePending = eOrientation;
		if(m_eGuidePending == HYORIENT_Horizontal)
			SetCursorShape(Qt::SplitVCursor);
		else
			SetCursorShape(Qt::SplitHCursor);
	}
}

void HarmonyWidget::OnWgtMousePressEvent(IDraw *pDrawItem, QMouseEvent *pEvent)
{
	// If hovering over an existing guide, then "select" it by removing it, and starting SetPendingGuide()
	Qt::CursorShape eCurCursorShape = GetCursorShape();
	glm::vec2 ptWorldMousePos;
	if(pEvent->button() == Qt::LeftButton &&
		pDrawItem &&
		pDrawItem->GetGuideMap().empty() == false &&
		HyEngine::Input().GetWorldMousePos(ptWorldMousePos) &&
		(eCurCursorShape == Qt::SplitHCursor || eCurCursorShape == Qt::SplitVCursor))
	{
		// Find closest existing guide
		QPair<HyOrientation, int> closestGuideKey;
		int iClosestDist = INT_MAX;
		for(auto iter = pDrawItem->GetGuideMap().begin(); iter != pDrawItem->GetGuideMap().end(); ++iter)
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
			m_eGuidePending = closestGuideKey.first;

			delete pDrawItem->GetGuideMap()[closestGuideKey];
			pDrawItem->GetGuideMap().remove(closestGuideKey);
		}
	}
}

void HarmonyWidget::OnRulerMouseReleaseEvent(HyOrientation eOrientation, QMouseEvent *pEvent)
{
	HyGuiLog("HarmonyWidget::OnRulerMouseReleaseEvent", LOGTYPE_Normal);

	if(m_eGuidePending != HYORIENT_Null)
	{
		if(m_eGuidePending != eOrientation)
			HyGuiLog("HarmonyWidget::OnRulerMouseReleaseEvent - Wrong orientation set in m_eGuidePending", LOGTYPE_Error);

		if(GetWgtHarmony() &&
			GetWgtHarmony()->GetProject() &&
			GetWgtHarmony()->GetProject()->GetCurrentOpenItem())
		{
			ProjectItemData *pCurProjItem = GetWgtHarmony()->GetProject()->GetCurrentOpenItem();

			glm::vec2 ptWorldPos;
			if(HyEngine::Input().GetWorldMousePos(ptWorldPos))
			{
				int iPos = eOrientation == HYORIENT_Horizontal ? static_cast<int>(ptWorldPos.y) : static_cast<int>(ptWorldPos.x);
				pCurProjItem->GetDraw()->TryAllocateGuide(eOrientation, iPos);
			}
			
			pCurProjItem->GetDraw()->SetPendingGuide(HYORIENT_Null);
		}

		RestoreCursorShape();
		m_eGuidePending = HYORIENT_Null;
	}
}

void HarmonyWidget::OnWgtMouseReleaseEvent(IDraw *pDrawItem, QMouseEvent *pEvent)
{
	HyGuiLog("HarmonyWidget::OnWgtMouseReleaseEvent", LOGTYPE_Normal);

	if(pDrawItem && m_eGuidePending != HYORIENT_Null)
	{
		glm::vec2 ptWorldPos;
		if(HyEngine::Input().GetWorldMousePos(ptWorldPos))
		{
			int iPos = m_eGuidePending == HYORIENT_Horizontal ? static_cast<int>(ptWorldPos.y) : static_cast<int>(ptWorldPos.x);
			pDrawItem->TryAllocateGuide(m_eGuidePending, iPos);
		}

		pDrawItem->SetPendingGuide(HYORIENT_Null);
		RestoreCursorShape();
		m_eGuidePending = HYORIENT_Null;
	}
}

/*virtual*/ void HarmonyWidget::resizeEvent(QResizeEvent *pEvent) /*override*/
{
	QWidget::resizeEvent(pEvent);

	m_RulerHorzScene.setSceneRect(0, 0, ui->rulerHorz->width(), RULER_WIDTH);
	m_RulerVertScene.setSceneRect(0, 0, RULER_WIDTH, ui->rulerVert->height());
}
