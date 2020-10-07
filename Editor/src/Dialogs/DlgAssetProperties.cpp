/**************************************************************************
 *	DlgAssetProperties.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "DlgAssetProperties.h"
#include "ui_DlgAssetProperties.h"
#include "AtlasFrame.h"
#include "AudioAsset.h"

#include <QMessageBox>

DlgAssetProperties::DlgAssetProperties(AssetType eManagerType, QList<AssetItemData *> assetList, QWidget *parent) :
	QDialog(parent),
	m_SelectedAssets(assetList),
	ui(new Ui::DlgAssetProperties)
{
	ui->setupUi(this);
	ui->stackedAssetType->setCurrentIndex(eManagerType);

	if(m_SelectedAssets.count() > 1)
		ui->lblNumSelected->setText(QString::number(m_SelectedAssets.count()) % " assets selected");
	else
	{
		ui->txtName->setText(m_SelectedAssets[0]->GetName());
		ui->lblNumSelected->setVisible(false);
	}

	Qt::CheckState eCheckState;

	switch(eManagerType)
	{
	case ASSET_Atlas:
		// Texture Type
		for(int i = 0; i < HYNUM_TEXTUREFORMATS; ++i)
			ui->cmbTextureType->addItem(QString(HyAssets::GetTextureFormatName(static_cast<HyTextureFormat>(i)).c_str()));

		// Texture Filtering
		for(int i = 0; i < HYNUM_TEXTUREFILTERS; ++i)
			ui->cmbTextureFiltering->addItem(QString(HyAssets::GetTextureFilteringName(static_cast<HyTextureFiltering>(i)).c_str()));

		ui->cmbTextureType->setCurrentIndex(static_cast<AtlasFrame *>(m_SelectedAssets[0])->GetFormat());
		ui->cmbTextureFiltering->setCurrentIndex(static_cast<AtlasFrame *>(m_SelectedAssets[0])->GetFiltering());
		break;

	case ASSET_Audio:
		// Is Music ///////////////////////////////////////////////////////////////////////////////////////////
		eCheckState = static_cast<AudioAsset *>(m_SelectedAssets[0])->IsMusic() ? Qt::Checked : Qt::Unchecked;
		for(auto asset : m_SelectedAssets)
		{
			if((eCheckState == Qt::Unchecked && static_cast<AudioAsset *>(asset)->IsMusic()) ||
			   (eCheckState == Qt::Checked && static_cast<AudioAsset *>(asset)->IsMusic() == false))
			{
				eCheckState = Qt::PartiallyChecked;
				break;
			}
		}
		ui->chkIsMusic->setCheckState(eCheckState);

		// Export As Mono ///////////////////////////////////////////////////////////////////////////////////////////
		eCheckState = static_cast<AudioAsset *>(m_SelectedAssets[0])->IsExportMono() ? Qt::Checked : Qt::Unchecked;
		for(auto asset : m_SelectedAssets)
		{
			if((eCheckState == Qt::Unchecked && static_cast<AudioAsset *>(asset)->IsExportMono()) ||
			   (eCheckState == Qt::Checked && static_cast<AudioAsset *>(asset)->IsExportMono() == false))
			{
				eCheckState = Qt::PartiallyChecked;
				break;
			}
		}
		ui->chkExportAsMono->setCheckState(eCheckState);

		// Is Compressed ///////////////////////////////////////////////////////////////////////////////////////////
		eCheckState = static_cast<AudioAsset *>(m_SelectedAssets[0])->IsCompressed() ? Qt::Checked : Qt::Unchecked;
		for(auto asset : m_SelectedAssets)
		{
			if((eCheckState == Qt::Unchecked && static_cast<AudioAsset *>(asset)->IsCompressed()) ||
			   (eCheckState == Qt::Checked && static_cast<AudioAsset *>(asset)->IsCompressed() == false))
			{
				eCheckState = Qt::PartiallyChecked;
				break;
			}
		}
		ui->chkIsCompressed->setCheckState(eCheckState);

		// Use Global Limit ///////////////////////////////////////////////////////////////////////////////////////////
		eCheckState = static_cast<AudioAsset *>(m_SelectedAssets[0])->GetGlobalLimit() > 0 ? Qt::Checked : Qt::Unchecked;
		for(auto asset : m_SelectedAssets)
		{
			if((eCheckState == Qt::Unchecked && static_cast<AudioAsset *>(asset)->GetGlobalLimit() > 0) ||
			   (eCheckState == Qt::Checked && static_cast<AudioAsset *>(asset)->GetGlobalLimit() > 0 == false))
			{
				eCheckState = Qt::PartiallyChecked;
				break;
			}
		}
		ui->chkUseGlobalLimit->setCheckState(eCheckState);
		
		break;
	} // switch(eManagerType)

	Refresh();
}

DlgAssetProperties::~DlgAssetProperties()
{
	delete ui;
}

void DlgAssetProperties::on_chkIsCompressed_clicked()
{
	Refresh();
}

void DlgAssetProperties::on_chkUseGlobalLimit_clicked()
{
	Refresh();
}

/*virtual*/ void DlgAssetProperties::done(int r)
{
	if(r == QDialog::Accepted)
	{
		if(QMessageBox::Ok == QMessageBox::warning(nullptr, QString("Save asset properties?"), QString("Save asset properties? Changed assets will need to be repacked."), QMessageBox::Ok, QMessageBox::Cancel))
			QDialog::done(r);
		else
			QDialog::done(QDialog::Rejected);
	}
	
	QDialog::done(r);
}

void DlgAssetProperties::Refresh()
{
	switch(ui->stackedAssetType->currentIndex())
	{
	case ASSET_Atlas:
		break;

	case ASSET_Audio:
		ui->sbVbrQuality->setDisabled(ui->chkIsCompressed->checkState() == Qt::Unchecked);
		ui->lblVbrQuality->setDisabled(ui->chkIsCompressed->checkState() == Qt::Unchecked);

		ui->sbGlobalLimit->setDisabled(ui->chkUseGlobalLimit->checkState() == Qt::Unchecked);
		ui->lblInstances->setDisabled(ui->chkUseGlobalLimit->checkState() == Qt::Unchecked);
		break;
	}
}

//bool DlgAssetProperties::IsSettingsDirty()
//{
//	switch(ui->stackedAssetType->currentIndex())
//	{
//	case ASSET_Atlas:
//		break;
//
//	case ASSET_Audio:
//		if(ui->chkIsMusic->checkState() != Qt::PartiallyChecked)
//		{
//			if(ui->chkIsMusic->checkState() == Qt::Checked)
//			{
//				for(auto asset : m_SelectedAssets)
//				{
//					if(static_cast<AudioAsset *>(asset)->IsMusic() == false)
//						return true;
//				}
//			}
//		}
//
//		ui->sbVbrQuality->setDisabled(ui->chkIsCompressed->checkState() == Qt::Unchecked);
//		ui->lblVbrQuality->setDisabled(ui->chkIsCompressed->checkState() == Qt::Unchecked);
//
//		ui->sbGlobalLimit->setDisabled(ui->chkUseGlobalLimit->checkState() == Qt::Unchecked);
//		ui->lblInstances->setDisabled(ui->chkUseGlobalLimit->checkState() == Qt::Unchecked);
//		break;
//	}
//}
