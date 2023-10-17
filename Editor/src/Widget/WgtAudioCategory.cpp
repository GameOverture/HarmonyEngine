/**************************************************************************
 *	WgtAudioCategory.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2022 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "WgtAudioCategory.h"
#include "ui_WgtAudioCategory.h"
#include "AudioManagerModel.h"

#include <QColorDialog>

WgtAudioCategory::WgtAudioCategory(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::WgtAudioCategory)
{
	ui->setupUi(this);
}

WgtAudioCategory::~WgtAudioCategory()
{
	delete ui;
}

void WgtAudioCategory::Init(AudioCategoriesModel *pModel, quint32 uiId, bool bIncludeCheckBox)
{
	ui->cmbAudioGroup->setModel(pModel);
	ui->cmbAudioGroup->setCurrentIndex(pModel->GetIndex(uiId));

	ui->groupBox->setChecked(false);
	ui->groupBox->setCheckable(bIncludeCheckBox);
}

bool WgtAudioCategory::IsValid() const
{
	return (ui->groupBox->isCheckable() == false || (ui->groupBox->isCheckable() && ui->groupBox->isChecked()));
}

quint32 WgtAudioCategory::GetCurrentId() const
{
	return static_cast<AudioCategoriesModel *>(ui->cmbAudioGroup->model())->GetId(ui->cmbAudioGroup->currentIndex());
}

void WgtAudioCategory::on_cmbAudioGroup_currentIndexChanged(int index)
{
}

void WgtAudioCategory::on_btnAddGroup_clicked()
{
}

void WgtAudioCategory::on_btnRemoveGroup_clicked()
{
}
