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
	ui(new Ui::HarmonyWidget)
{
	ui->setupUi(this);
	ui->wgtHarmony->InitProject(pProject);

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
