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
	ui->extrude->setValue(m_InitialPackerSettingsObj["extrude"].toInt());
	ui->chkMerge->setChecked(m_InitialPackerSettingsObj["chkMerge"].toBool());
	ui->chkSquare->setChecked(m_InitialPackerSettingsObj["chkSquare"].toBool());
	ui->chkAutosize->setChecked(m_InitialPackerSettingsObj["chkAutosize"].toBool());
	ui->minFillRate->setValue(m_InitialPackerSettingsObj["minFillRate"].toInt());

	ui->sbTextureWidth->setValue(m_InitialPackerSettingsObj["maxWidth"].toInt());
	ui->sbTextureHeight->setValue(m_InitialPackerSettingsObj["maxHeight"].toInt());
	ui->cmbHeuristic->setCurrentIndex(m_InitialPackerSettingsObj["cmbHeuristic"].toInt());
	
	// Texture Type
	for(int i = 0; i < HYNUM_TEXTUREFORMATS; ++i)
		ui->cmbTextureType->addItem(QString(HyAssets::GetTextureFormatName(static_cast<HyTextureFormat>(i)).c_str()));
	
	QString sTextureFormat = m_InitialPackerSettingsObj["textureFormat"].toString();
	std::vector<std::string> stdStringList = HyAssets::GetTextureFormatNameList();
	QStringList sTextureFormatList;
	for(auto str : stdStringList)
		sTextureFormatList.push_back(str.c_str());
	for(int i = 0; i < sTextureFormatList.size(); ++i)
	{
		if(sTextureFormatList[i].compare(sTextureFormat, Qt::CaseInsensitive) == 0)
		{
			ui->cmbTextureType->setCurrentIndex(i);
			break;
		}
	}

	// Texture Filtering
	for(int i = 0; i < HYNUM_TEXTUREFILTERS; ++i)
		ui->cmbTextureFiltering->addItem(QString(HyAssets::GetTextureFilteringName(static_cast<HyTextureFiltering>(i)).c_str()));

	QString sTextureFiltering = m_InitialPackerSettingsObj["textureFiltering"].toString();
	stdStringList = HyAssets::GetTextureFilteringNameList();
	QStringList sTextureFilterList;
	for(auto str : stdStringList)
		sTextureFilterList.push_back(str.c_str());
	for(int i = 0; i < sTextureFilterList.size(); ++i)
	{
		if(sTextureFilterList[i].compare(sTextureFiltering, Qt::CaseInsensitive) == 0)
		{
			ui->cmbTextureFiltering->setCurrentIndex(i);
			break;
		}
	}
}

DlgAtlasGroupSettings::~DlgAtlasGroupSettings()
{
	delete ui;
}

bool DlgAtlasGroupSettings::IsSettingsDirty()
{
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
	if(ui->extrude->value() != m_InitialPackerSettingsObj["extrude"].toInt())
		return true;
	if(ui->chkMerge->isChecked() != m_InitialPackerSettingsObj["chkMerge"].toBool())
		return true;
	if(ui->chkSquare->isChecked() != m_InitialPackerSettingsObj["chkSquare"].toBool())
		return true;
	if(ui->chkAutosize->isChecked() != m_InitialPackerSettingsObj["chkAutosize"].toBool())
		return true;
	if(ui->minFillRate->value() != m_InitialPackerSettingsObj["minFillRate"].toInt())
		return true;
	if(ui->sbTextureWidth->value() != m_InitialPackerSettingsObj["maxWidth"].toInt())
		return true;
	if(ui->sbTextureHeight->value() != m_InitialPackerSettingsObj["maxHeight"].toInt())
		return true;
	if(ui->cmbHeuristic->currentIndex() != m_InitialPackerSettingsObj["cmbHeuristic"].toInt())
		return true;
	if(ui->cmbTextureType->currentIndex() != static_cast<int>(HyAssets::GetTextureFormatFromString(m_InitialPackerSettingsObj["textureFormat"].toString().toStdString())))
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
	settingsObjOut.insert("extrude", ui->extrude->value());
	settingsObjOut.insert("chkMerge", ui->chkMerge->isChecked());
	settingsObjOut.insert("chkSquare", ui->chkSquare->isChecked());
	settingsObjOut.insert("chkAutosize", ui->chkAutosize->isChecked());
	settingsObjOut.insert("minFillRate", ui->minFillRate->value());
	settingsObjOut.insert("maxWidth", ui->sbTextureWidth->value());
	settingsObjOut.insert("maxHeight", ui->sbTextureHeight->value());
	settingsObjOut.insert("cmbHeuristic", ui->cmbHeuristic->currentIndex());
	settingsObjOut.insert("textureFormat", QString(HyAssets::GetTextureFormatName(static_cast<HyTextureFormat>(ui->cmbTextureType->currentIndex())).c_str()));
	settingsObjOut.insert("textureFiltering", QString(HyAssets::GetTextureFilteringName(static_cast<HyTextureFiltering>(ui->cmbTextureFiltering->currentIndex())).c_str()));
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
