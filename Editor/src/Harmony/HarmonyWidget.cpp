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

#include <QColorDialog>

HarmonyWidget::HarmonyWidget(Project *pProject, QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::HarmonyWidget),
	m_HorzScene(this),
	m_pHorzLine(nullptr),
	m_VertScene(this),
	m_pVertLine(nullptr)
{
	ui->setupUi(this);
	ui->wgtHarmony->InitProject(pProject);

	//m_pHorzLine = m_HorzScene.addLine(QLineF(0, 0, 0, RULER_WIDTH), QPen(HyGlobal::ConvertHyColor(HyColor::Orange)));
	ui->rulerHorz->setScene(&m_HorzScene);
	ui->rulerHorz->Init(HYORIENT_Horizontal, HyColor::ContainerPanel);

	//m_pVertLine = m_VertScene.addLine(QLineF(0, 0, RULER_WIDTH, 0), QPen(HyGlobal::ConvertHyColor(HyColor::Orange)));
	ui->rulerVert->setScene(&m_VertScene);
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

void HarmonyWidget::RefreshMousePos()
{
	//ui->rulerHorz->UpdateMouseLine(m_pHorzLine);
	//ui->rulerVert->UpdateMouseLine(m_pVertLine);
}
