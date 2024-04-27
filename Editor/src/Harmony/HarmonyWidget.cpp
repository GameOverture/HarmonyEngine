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
	ui(new Ui::HarmonyWidget)//,
	//m_HorzScene(this),
	//m_VertScene(this)
{
	ui->setupUi(this);
	ui->wgtHarmony->InitProject(pProject);
	//ui->rulerHorz->setScene(&m_HorzScene);
	ui->rulerHorz->setBackgroundBrush(HyGlobal::ConvertHyColor(HyColor::ContainerPanel));
	//ui->rulerVert->setScene(&m_VertScene);
	ui->rulerVert->setBackgroundBrush(HyGlobal::ConvertHyColor(HyColor::ContainerPanel));

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
	//m_HorzScene.clear();
	//m_VertScene.clear();
	//HyGuiRect rect = ui->wgtHarmony->GetHarmonyRenderer()->GetViewportRect();
	//QRectF qRect(0, 0, rect.width, rect.height);
	//QPen pen(Qt::black);
	//pen.setWidth(1);
	//for(int i = 0; i < rect.width; i += 10)
	//{
	//	m_HorzScene.addLine(i, 0, i, 10, pen);
	//	m_HorzScene.addText(i, 0, 0, QString::number(i));
	//}
	//for(int i = 0; i < rect.height; i += 10)
	//{
	//	m_VertScene.addLine(0, i, 10, i, pen);
	//	m_VertScene.addText(0, i, 0, QString::number(i));
	//}
}
