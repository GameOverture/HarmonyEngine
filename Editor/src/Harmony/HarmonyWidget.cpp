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
#include "GlobalUndoCmds.h"

#include <QColorDialog>
#include <QMouseEvent>

HarmonyWidget::HarmonyWidget(Project *pProject, QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::HarmonyWidget),
	m_RulerHorzScene(this),
	m_RulerVertScene(this)
{
	ui->setupUi(this);
	ui->wgtHarmony->InitProject(pProject);

	ui->rulerHorz->setScene(&m_RulerHorzScene);
	ui->rulerHorz->translate(0, 0);
	ui->rulerHorz->Init(HYORIENT_Horizontal);

	ui->rulerVert->setScene(&m_RulerVertScene);
	ui->rulerVert->Init(HYORIENT_Vertical);
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
	if(HyEngine::IsInitialized() == false)
		return;

	QPointF ptCurMousePos = pEvent->position();
	if(eOrientation == HYORIENT_Horizontal)
		HyEngine::Input().SetWidgetMousePos(glm::vec2(ptCurMousePos.x(), ptCurMousePos.y() - RULER_WIDTH));
	else
		HyEngine::Input().SetWidgetMousePos(glm::vec2(ptCurMousePos.x() - RULER_WIDTH, ptCurMousePos.y()));
}

void HarmonyWidget::OnWgtMouseMoveEvent(IDraw *pDrawItem, QMouseEvent *pEvent)
{
	if(HyEngine::IsInitialized() == false)
		return;

	QPointF ptCurMousePos = pEvent->position();
	HyEngine::Input().SetWidgetMousePos(glm::vec2(ptCurMousePos.x(), ptCurMousePos.y()));

	RefreshRulers();
}

void HarmonyWidget::OnRulerMousePressEvent(HyOrientation eOrientation, QMouseEvent *pEvent)
{
	if(GetProject() == nullptr)
		return;

	if(pEvent->button() == Qt::LeftButton &&
		GetWgtHarmony() &&
		GetWgtHarmony()->GetProject() &&
		GetWgtHarmony()->GetProject()->GetCurrentOpenItem() &&
		GetWgtHarmony()->GetProject()->GetCurrentOpenItem()->GetDraw())
	{
		IDraw *pDraw = GetWgtHarmony()->GetProject()->GetCurrentOpenItem()->GetDraw();

		if(eOrientation == HYORIENT_Horizontal)
			pDraw->SetAction(HYACTION_ManipGuideHorz);
		else
			pDraw->SetAction(HYACTION_ManipGuideVert);
	}
}

void HarmonyWidget::OnRulerMouseReleaseEvent(HyOrientation eOrientation, QMouseEvent *pEvent)
{
	if(GetWgtHarmony() &&
		GetWgtHarmony()->GetProject() &&
		GetWgtHarmony()->GetProject()->GetCurrentOpenItem() &&
		GetWgtHarmony()->GetProject()->GetCurrentOpenItem()->GetDraw())
	{
		IDraw *pDraw = GetWgtHarmony()->GetProject()->GetCurrentOpenItem()->GetDraw();

		if(pDraw->GetCurAction() == HYACTION_ManipGuideHorz || pDraw->GetCurAction() == HYACTION_ManipGuideVert)
		{
			if((pDraw->GetCurAction() == HYACTION_ManipGuideHorz && HYORIENT_Horizontal != eOrientation) ||
				(pDraw->GetCurAction() == HYACTION_ManipGuideVert && HYORIENT_Vertical != eOrientation))
			{
				HyGuiLog("HarmonyWidget::OnRulerMouseReleaseEvent - Wrong orientation set in m_eGuidePending", LOGTYPE_Error);
			}

			glm::vec2 ptWorldPos;
			if(HyEngine::Input().GetWorldMousePos(ptWorldPos))
			{
				int iPos = eOrientation == HYORIENT_Horizontal ? static_cast<int>(ptWorldPos.y) : static_cast<int>(ptWorldPos.x);
				pDraw->TryAllocateGuide(eOrientation, iPos);
			}

			pDraw->ClearAction();
		}
	}
}

void HarmonyWidget::OnRefreshLoading()
{
	if(	GetWgtHarmony() &&
		GetWgtHarmony()->GetProject() &&
		GetWgtHarmony()->GetProject()->GetCurrentOpenItem() &&
		GetWgtHarmony()->GetProject()->GetCurrentOpenItem()->GetDraw())
	{
		IDraw *pDraw = GetWgtHarmony()->GetProject()->GetCurrentOpenItem()->GetDraw();

		QList<LoadingType> currentLoadingTypeList = MainWindow::GetCurrentLoading();
		if(currentLoadingTypeList.empty() == false)
		{
			if(currentLoadingTypeList.size() == 1 && currentLoadingTypeList.contains(LOADINGTYPE_HarmonyStreaming))
				pDraw->SetAction(HYACTION_Streaming);
			else
				pDraw->SetAction(HYACTION_Wait);
		}
		else if(pDraw->GetCurAction() == HYACTION_Wait || pDraw->GetCurAction() == HYACTION_Streaming)
			pDraw->ClearAction();
	}
}

/*virtual*/ void HarmonyWidget::resizeEvent(QResizeEvent *pEvent) /*override*/
{
	QWidget::resizeEvent(pEvent);

	m_RulerHorzScene.setSceneRect(0, 0, ui->rulerHorz->width(), RULER_WIDTH);
	m_RulerVertScene.setSceneRect(0, 0, RULER_WIDTH, ui->rulerVert->height());
}
