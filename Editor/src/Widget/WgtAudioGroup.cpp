/**************************************************************************
 *	WgtAudioGroup.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2022 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "WgtAudioGroup.h"
#include "ui_WgtAudioGroup.h"
#include "AudioManagerModel.h"

#include <QColorDialog>

WgtAudioGroup::WgtAudioGroup(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::WgtAudioGroup)
{
	ui->setupUi(this);
}

WgtAudioGroup::~WgtAudioGroup()
{
	delete ui;
}

void WgtAudioGroup::Init(AudioGroupsModel *pModel, quint32 uiId, bool bIncludeCheckBox)
{
	ui->cmbAudioGroup->setModel(pModel);
	ui->cmbAudioGroup->setCurrentIndex(pModel->GetIndex(uiId));

	ui->groupBox->setChecked(false);
	ui->groupBox->setCheckable(bIncludeCheckBox);
}

bool WgtAudioGroup::IsValid() const
{
	return (ui->groupBox->isCheckable() == false || (ui->groupBox->isCheckable() && ui->groupBox->isChecked()));
}

quint32 WgtAudioGroup::GetCurrentId() const
{
	return static_cast<AudioGroupsModel *>(ui->cmbAudioGroup->model())->GetId(ui->cmbAudioGroup->currentIndex());
}

void WgtAudioGroup::on_cmbAudioGroup_currentIndexChanged(int index)
{
}

void WgtAudioGroup::on_btnAddGroup_clicked()
{
}

void WgtAudioGroup::on_btnRemoveGroup_clicked()
{
}
