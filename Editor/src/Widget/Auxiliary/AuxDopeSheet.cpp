/**************************************************************************
 *	AuxDopeSheet.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AuxDopeSheet.h"
#include "EntityModel.h"
#include "GlobalUndoCmds.h"
#include "ui_AuxDopeSheet.h"


AuxDopeSheet::AuxDopeSheet(QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::AuxDopeSheet)
{
	ui->setupUi(this);

	ui->btnRewind->setDefaultAction(ui->actionRewind);
	ui->btnPreviousKeyFrame->setDefaultAction(ui->actionPreviousKeyFrame);
	ui->btnPlayAnimations->setDefaultAction(ui->actionPlayAnimations);
	ui->btnNextKeyFrame->setDefaultAction(ui->actionNextKeyFrame);
	ui->btnLastKeyFrame->setDefaultAction(ui->actionLastKeyFrame);

	m_WidgetMapper.setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
	UpdateWidgets();
}

/*virtual*/ AuxDopeSheet::~AuxDopeSheet()
{
	delete ui;
}

EntityStateData *AuxDopeSheet::GetEntityStateModel() const
{
	if(ui->graphicsView->GetScene() == nullptr)
		return nullptr;

	return ui->graphicsView->GetScene()->GetStateData();
}

void AuxDopeSheet::SetEntityStateModel(EntityStateData *pEntStateData)
{
	ui->graphicsView->SetScene(pEntStateData);

	m_WidgetMapper.clearMapping();

	if(pEntStateData)
	{
		m_WidgetMapper.setModel(pEntStateData->GetDopeSheetScene().GetAuxWidgetsModel());
		m_WidgetMapper.addMapping(ui->sbFramesPerSecond, AUXDOPEWIDGETSECTION_FramesPerSecond);
		m_WidgetMapper.addMapping(ui->chkAutoInitialize, AUXDOPEWIDGETSECTION_AutoInitialize);

		m_WidgetMapper.toFirst();
		m_WidgetMapper.revert();
	}
	else
		m_WidgetMapper.setModel(nullptr);

	UpdateWidgets();
}

void AuxDopeSheet::UpdateWidgets()
{
	if(ui->graphicsView->scene())
	{
		ui->sbFramesPerSecond->setEnabled(true);
		ui->chkAutoInitialize->setEnabled(true);
	}
	else
	{
		ui->sbFramesPerSecond->setEnabled(false);
		ui->chkAutoInitialize->setEnabled(false);
	}
}

void AuxDopeSheet::on_actionRewind_triggered()
{
}

void AuxDopeSheet::on_actionPreviousKeyFrame_triggered()
{
}

void AuxDopeSheet::on_actionPlayAnimations_triggered()
{
}

void AuxDopeSheet::on_actionNextKeyFrame_triggered()
{
}

void AuxDopeSheet::on_actionLastKeyFrame_triggered()
{
}
