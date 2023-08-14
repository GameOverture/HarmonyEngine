/**************************************************************************
 *	DlgAtlasGroupSettings.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "DlgAtlasGroupSettings.h"
#include "ui_DlgAtlasGroupSettings.h"

#include <QMessageBox>

DlgAtlasGroupSettings::DlgAtlasGroupSettings(bool bAtlasGrpHasImages, QJsonObject packerSettingsObj, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DlgAtlasGroupSettings),
	m_bAtlasGrpHasImages(bAtlasGrpHasImages),
	m_InitialPackerSettingsObj(packerSettingsObj)
{
	ui->setupUi(this);

	ui->txtName->setText(m_InitialPackerSettingsObj["bankName"].toString());

	ui->cmbSortOrder->setCurrentIndex(m_InitialPackerSettingsObj["cmbSortOrder"].toInt());
	ui->sbFrameMarginTop->setValue(m_InitialPackerSettingsObj["sbFrameMarginTop"].toInt());
	ui->sbFrameMarginLeft->setValue(m_InitialPackerSettingsObj["sbFrameMarginLeft"].toInt());
	ui->sbFrameMarginRight->setValue(m_InitialPackerSettingsObj["sbFrameMarginRight"].toInt());
	ui->sbFrameMarginBottom->setValue(m_InitialPackerSettingsObj["sbFrameMarginBottom"].toInt());

	ui->sbTextureWidth->setValue(m_InitialPackerSettingsObj["maxWidth"].toInt());
	ui->sbTextureHeight->setValue(m_InitialPackerSettingsObj["maxHeight"].toInt());
	ui->cmbHeuristic->setCurrentIndex(m_InitialPackerSettingsObj["cmbHeuristic"].toInt());

	ui->chkSquareTexturesOnly->setChecked(m_InitialPackerSettingsObj["squareTexturesOnly"].toBool());
	ui->grpCropUnusedSpace->setChecked(m_InitialPackerSettingsObj["cropUnusedSpace"].toBool());
	ui->chkAggressiveResize->setChecked(m_InitialPackerSettingsObj["aggressiveResizing"].toBool());
	ui->sbResizingThreshold->setValue(m_InitialPackerSettingsObj["minimumFillRate"].toInt());
	
	on_chkSquareTexturesOnly_clicked();
	on_chkAggressiveResize_clicked();
}

DlgAtlasGroupSettings::~DlgAtlasGroupSettings()
{
	delete ui;
}

bool DlgAtlasGroupSettings::IsSettingsDirty()
{
	if(ui->chkForceRegenBank->isChecked())
		return true;
	if(ui->cmbSortOrder->currentIndex() != m_InitialPackerSettingsObj["cmbSortOrder"].toInt())
		return true;
	if(ui->sbFrameMarginTop->value() != m_InitialPackerSettingsObj["sbFrameMarginTop"].toInt())
		return true;
	if(ui->sbFrameMarginLeft->value() != m_InitialPackerSettingsObj["sbFrameMarginLeft"].toInt())
		return true;
	if(ui->sbFrameMarginRight->value() != m_InitialPackerSettingsObj["sbFrameMarginRight"].toInt())
		return true;
	if(ui->sbFrameMarginBottom->value() != m_InitialPackerSettingsObj["sbFrameMarginBottom"].toInt())
		return true;
	if(ui->sbTextureWidth->value() != m_InitialPackerSettingsObj["maxWidth"].toInt())
		return true;
	if(ui->sbTextureHeight->value() != m_InitialPackerSettingsObj["maxHeight"].toInt())
		return true;
	if(ui->cmbHeuristic->currentIndex() != m_InitialPackerSettingsObj["cmbHeuristic"].toInt())
		return true;
	if(ui->chkSquareTexturesOnly->isChecked() != m_InitialPackerSettingsObj["squareTexturesOnly"].toBool())
		return true;
	if(ui->grpCropUnusedSpace->isChecked() != m_InitialPackerSettingsObj["cropUnusedSpace"].toBool())
		return true;
	if(ui->grpCropUnusedSpace->isChecked())
	{
		if(ui->chkAggressiveResize->isChecked() != m_InitialPackerSettingsObj["aggressiveResizing"].toBool())
			return true;
		if(ui->chkAggressiveResize->isChecked() &&
			ui->sbResizingThreshold->value() != m_InitialPackerSettingsObj["minimumFillRate"].toInt())
		{
			return true;
		}
	}

	return false;
}

void DlgAtlasGroupSettings::ApplyCurrentSettingsToObj(QJsonObject &settingsObjOut)
{
	settingsObjOut.insert("bankName", ui->txtName->text());
	settingsObjOut.insert("cmbSortOrder", ui->cmbSortOrder->currentIndex());
	settingsObjOut.insert("sbFrameMarginTop", ui->sbFrameMarginTop->value());
	settingsObjOut.insert("sbFrameMarginLeft", ui->sbFrameMarginLeft->value());
	settingsObjOut.insert("sbFrameMarginRight", ui->sbFrameMarginRight->value());
	settingsObjOut.insert("sbFrameMarginBottom", ui->sbFrameMarginBottom->value());
	settingsObjOut.insert("maxWidth", ui->sbTextureWidth->value());
	settingsObjOut.insert("maxHeight", ui->sbTextureHeight->value());
	settingsObjOut.insert("cmbHeuristic", ui->cmbHeuristic->currentIndex());
	settingsObjOut.insert("squareTexturesOnly", ui->chkSquareTexturesOnly->isChecked());
	settingsObjOut.insert("cropUnusedSpace", ui->grpCropUnusedSpace->isChecked());
	settingsObjOut.insert("aggressiveResizing", ui->chkAggressiveResize->isChecked());
	settingsObjOut.insert("minimumFillRate", ui->sbResizingThreshold->value());
}

void DlgAtlasGroupSettings::on_chkSquareTexturesOnly_clicked()
{
	if(ui->chkSquareTexturesOnly->isChecked())
	{
		ui->sbTextureHeight->setValue(ui->sbTextureWidth->value());
		ui->sbTextureHeight->setEnabled(false);
	}
	else
		ui->sbTextureHeight->setEnabled(true);
}

void DlgAtlasGroupSettings::on_sbTextureWidth_valueChanged(int iNewValue)
{
	if(ui->chkSquareTexturesOnly->isChecked())
		ui->sbTextureHeight->setValue(iNewValue);
}

void DlgAtlasGroupSettings::on_btnTexSize128_clicked()
{
	ui->sbTextureWidth->setValue(128);
	ui->sbTextureHeight->setValue(128);
}

void DlgAtlasGroupSettings::on_btnTexSize256_clicked()
{
	ui->sbTextureWidth->setValue(256);
	ui->sbTextureHeight->setValue(256);
}

void DlgAtlasGroupSettings::on_btnTexSize512_clicked()
{
	ui->sbTextureWidth->setValue(512);
	ui->sbTextureHeight->setValue(512);
}

void DlgAtlasGroupSettings::on_btnTexSize1024_clicked()
{
	ui->sbTextureWidth->setValue(1024);
	ui->sbTextureHeight->setValue(1024);
}

void DlgAtlasGroupSettings::on_btnTexSize2048_clicked()
{
	ui->sbTextureWidth->setValue(2048);
	ui->sbTextureHeight->setValue(2048);
}

void DlgAtlasGroupSettings::on_btnTexSize4096_clicked()
{
	ui->sbTextureWidth->setValue(4096);
	ui->sbTextureHeight->setValue(4096);
}

void DlgAtlasGroupSettings::on_btnTexSize8192_clicked()
{
	ui->sbTextureWidth->setValue(8192);
	ui->sbTextureHeight->setValue(8192);
}

void DlgAtlasGroupSettings::on_btnTexSize16384_clicked()
{
	ui->sbTextureWidth->setValue(16384);
	ui->sbTextureHeight->setValue(16384);
}

void DlgAtlasGroupSettings::on_btnMatchTextureWidthHeight_clicked()
{
	ui->sbTextureHeight->setValue(ui->sbTextureWidth->value());
}

void DlgAtlasGroupSettings::on_chkAggressiveResize_clicked()
{
	ui->sbResizingThreshold->setEnabled(ui->chkAggressiveResize->isChecked());
	ui->lblResizingThreshold->setEnabled(ui->chkAggressiveResize->isChecked());
}

/*virtual*/ void DlgAtlasGroupSettings::done(int r)
{
	if(r == QDialog::Accepted)
	{
		// check if it is ok or not
		if(IsSettingsDirty() && m_bAtlasGrpHasImages)
		{
			if(QMessageBox::Ok == QMessageBox::warning(nullptr, QString("Save Atlas Group Settings?"), QString("By modifying the Atlas Group settings, it is required to regenerate the entire Atlas Group."), QMessageBox::Ok, QMessageBox::Cancel))
				QDialog::done(r);
			else
				return;
		}
	}
	
	QDialog::done(r);
}
