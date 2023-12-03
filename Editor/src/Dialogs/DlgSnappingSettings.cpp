/**************************************************************************
 *	DlgSnappingSettings.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "DlgSnappingSettings.h"
#include "ui_DlgSnappingSettings.h"

#include <QMessageBox>

DlgSnappingSettings::DlgSnappingSettings(uint32 uiSnappingSettings, QWidget *pParent /*= nullptr*/) :
	QDialog(pParent),
	ui(new Ui::DlgSnappingSettings)
{
	ui->setupUi(this);

	ui->sbSnappingTolerance->setValue(uiSnappingSettings & SNAPSETTING_ToleranceMask);

	ui->chkEnableSnapping->setChecked(uiSnappingSettings & SNAPSETTING_Enabled);
	ui->chkSnapToGrid->setChecked(uiSnappingSettings & SNAPSETTING_Grid);
	ui->chkSnapToGuides->setChecked(uiSnappingSettings & SNAPSETTING_Guides);
	ui->chkSnapToOrigin->setChecked(uiSnappingSettings & SNAPSETTING_Origin);
	ui->grpSnapToItems->setChecked(uiSnappingSettings & SNAPSETTING_Items);
	ui->chkSnapToItemMidPoints->setChecked(uiSnappingSettings & SNAPSETTING_ItemMidPoints);

	on_chkEnableSnapping_clicked();
}

DlgSnappingSettings::~DlgSnappingSettings()
{
	delete ui;
}

uint32 DlgSnappingSettings::GetSnappingSettings() const
{
	uint32 uiSnappingSettings = ui->sbSnappingTolerance->value();

	if(ui->chkEnableSnapping->isChecked())
		uiSnappingSettings |= SNAPSETTING_Enabled;
	if(ui->chkSnapToGrid->isChecked())
		uiSnappingSettings |= SNAPSETTING_Grid;
	if(ui->chkSnapToGuides->isChecked())
		uiSnappingSettings |= SNAPSETTING_Guides;
	if(ui->chkSnapToOrigin->isChecked())
		uiSnappingSettings |= SNAPSETTING_Origin;
	if(ui->grpSnapToItems->isChecked())
		uiSnappingSettings |= SNAPSETTING_Items;
	if(ui->chkSnapToItemMidPoints->isChecked())
		uiSnappingSettings |= SNAPSETTING_ItemMidPoints;

	return uiSnappingSettings;
}

/*virtual*/ void DlgSnappingSettings::done(int r) /*override*/
{
	QDialog::done(r);
}

void DlgSnappingSettings::on_chkEnableSnapping_clicked()
{
	bool bEnable = ui->chkEnableSnapping->isChecked();

	ui->sbSnappingTolerance->setEnabled(bEnable);
	ui->chkSnapToGrid->setEnabled(bEnable);
	ui->chkSnapToGuides->setEnabled(bEnable);
	ui->chkSnapToOrigin->setEnabled(bEnable);
	ui->grpSnapToItems->setEnabled(bEnable);
	ui->chkSnapToItemMidPoints->setEnabled(bEnable);
}
