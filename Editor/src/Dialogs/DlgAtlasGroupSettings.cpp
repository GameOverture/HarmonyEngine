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
}

void DlgAtlasGroupSettings::on_btnTexSize128_clicked()
{

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
