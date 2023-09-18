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
#include "ui_AuxDopeSheet.h"

#include "EntityModel.h"

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
}

/*virtual*/ AuxDopeSheet::~AuxDopeSheet()
{
	delete ui;
}

void AuxDopeSheet::SetEntityStateModel(EntityStateData *pEntStateData)
{
	ui->graphicsView->setScene(&pEntStateData->GetDopeSheetScene());
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
