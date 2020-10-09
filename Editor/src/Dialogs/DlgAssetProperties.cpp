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
	{
		QString	sGroupName = m_SelectedAssets[0]->GetName();
		int iDiffIndex = sGroupName.length();
		for(auto pAsset : m_SelectedAssets)
		{
			for(int i = 0; i < pAsset->GetName().length() && i < iDiffIndex; ++i)
			{
				if(sGroupName[i] != pAsset->GetName()[i])
				{
					iDiffIndex = i;
					break;
				}
			}
		}
		sGroupName = sGroupName.left(iDiffIndex);
		ui->txtName->setText(sGroupName);
		ui->lblNumSelected->setText(QString::number(m_SelectedAssets.count()) % " assets selected");
	}
	else
	{
		ui->txtName->setText(m_SelectedAssets[0]->GetName());
		ui->lblNumSelected->setVisible(false);
	}

	Qt::CheckState eCheckState;

	switch(eManagerType)
	{
	case ASSET_Atlas: {
		// Texture Type ///////////////////////////////////////////////////////////////////////////////////////////
		HyTextureFormat eFormat = static_cast<AtlasFrame *>(m_SelectedAssets[0])->GetFormat();
		for(auto pAsset : m_SelectedAssets)
		{
			AtlasFrame *pFrame = static_cast<AtlasFrame *>(pAsset);
			if(pFrame->GetFormat() != eFormat)
			{
				ui->cmbTextureType->addItem("<different options>");
				eFormat = HYTEXTURE_Unknown;
				break;
			}
		}
		for(int i = 0; i < HYNUM_TEXTUREFORMATS; ++i)
			ui->cmbTextureType->addItem(QString(HyAssets::GetTextureFormatName(static_cast<HyTextureFormat>(i)).c_str()));

		ui->cmbTextureType->setCurrentIndex(eFormat == HYTEXTURE_Unknown ? 0 : eFormat);

		// Texture Filtering ///////////////////////////////////////////////////////////////////////////////////////////
		HyTextureFiltering eFiltering = static_cast<AtlasFrame *>(m_SelectedAssets[0])->GetFiltering();
		for(auto pAsset : m_SelectedAssets)
		{
			AtlasFrame *pFrame = static_cast<AtlasFrame *>(pAsset);
			if(pFrame->GetFiltering() != eFiltering)
			{
				ui->cmbTextureFiltering->addItem("<different options>");
				eFiltering = HYTEXFILTER_Unknown;
				break;
			}
		}
		for(int i = 0; i < HYNUM_TEXTUREFILTERS; ++i)
			ui->cmbTextureFiltering->addItem(QString(HyAssets::GetTextureFilteringName(static_cast<HyTextureFiltering>(i)).c_str()));

		ui->cmbTextureFiltering->setCurrentIndex(eFiltering == HYTEXFILTER_Unknown ? 0 : eFiltering);
		break; }

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

QList<AssetItemData *> DlgAssetProperties::GetChangedAssets()
{
	return m_ChangedAssets;
}

void DlgAssetProperties::on_chkIsCompressed_clicked()
{
	Refresh();
}

void DlgAssetProperties::on_sbVbrQuality_valueChanged(double dArg)
{
	ui->chkIsCompressed->setChecked(true);
	Refresh();
}

void DlgAssetProperties::on_chkUseGlobalLimit_clicked()
{
	Refresh();
}

void DlgAssetProperties::on_sbGlobalLimit_valueChanged(int iArg)
{
	ui->chkUseGlobalLimit->setChecked(true);
	Refresh();
}

/*virtual*/ void DlgAssetProperties::done(int r)
{
	bool bAssetsChanged = DetermineChangedAssets();

	if(r == QDialog::Accepted)
	{
		if(bAssetsChanged && QMessageBox::Ok == QMessageBox::warning(nullptr, QString("Save asset properties?"), QString("Save asset properties? Changed assets will need to be repacked."), QMessageBox::Ok, QMessageBox::Cancel))
		{
			ApplyChanges();
			QDialog::done(r);
		}
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

bool DlgAssetProperties::DetermineChangedAssets()
{
	m_ChangedAssets.clear();

	switch(ui->stackedAssetType->currentIndex())
	{
	case ASSET_Atlas: {
		bool bHasDiffOptions = ui->cmbTextureType->count() == (HYNUM_TEXTUREFORMATS + 1);
		bool bIsDiffOptions = bHasDiffOptions && ui->cmbTextureType->currentIndex() == 0;
		if(bIsDiffOptions == false)
		{
			for(auto pAsset : m_SelectedAssets)
			{
				AtlasFrame *pFrame = static_cast<AtlasFrame *>(pAsset);
				if(pFrame->GetFormat() != static_cast<HyTextureFormat>(ui->cmbTextureType->currentIndex() - (bHasDiffOptions ? 1 : 0)))
				{
					m_ChangedAssets.append(pAsset);
					continue;
				}
			}
		}

		bHasDiffOptions = ui->cmbTextureFiltering->count() == (HYNUM_TEXTUREFILTERS + 1);
		bIsDiffOptions = bHasDiffOptions && ui->cmbTextureFiltering->currentIndex() == 0;
		if(bIsDiffOptions == false)
		{
			for(auto pAsset : m_SelectedAssets)
			{
				AtlasFrame *pFrame = static_cast<AtlasFrame *>(pAsset);
				if(pFrame->GetFiltering() != static_cast<HyTextureFiltering>(ui->cmbTextureFiltering->currentIndex() - (bHasDiffOptions ? 1 : 0)))
				{
					m_ChangedAssets.append(pAsset);
					continue;
				}
			}
		}
		break; }

	case ASSET_Audio:
		for(auto pAsset : m_SelectedAssets)
		{
			AudioAsset *pAudio = static_cast<AudioAsset *>(pAsset);
			if((pAudio->IsMusic() && ui->chkIsMusic->checkState() == Qt::Unchecked) ||
			   (pAudio->IsMusic() == false && ui->chkIsMusic->checkState() == Qt::Checked))
			{
				m_ChangedAssets.append(pAsset);
				continue;
			}

			if((pAudio->IsExportMono() && ui->chkExportAsMono->checkState() == Qt::Unchecked) ||
			   (pAudio->IsExportMono() == false && ui->chkExportAsMono->checkState() == Qt::Checked))
			{
				m_ChangedAssets.append(pAsset);
				continue;
			}

			if((pAudio->IsCompressed() && ui->chkIsCompressed->checkState() == Qt::Unchecked) ||
			   (pAudio->IsCompressed() == false && ui->chkIsCompressed->checkState() == Qt::Checked) ||
			   (pAudio->IsCompressed() && pAudio->GetVbrQuality() != ui->sbVbrQuality->value()))
			{
				m_ChangedAssets.append(pAsset);
				continue;
			}

			if((pAudio->GetGlobalLimit() > 0 && ui->chkUseGlobalLimit->checkState() == Qt::Unchecked) ||
			   (pAudio->GetGlobalLimit() > 0 == false && ui->chkUseGlobalLimit->checkState() == Qt::Checked) ||
			   (pAudio->GetGlobalLimit() > 0 && pAudio->GetGlobalLimit() != ui->sbGlobalLimit->value()))
			{
				m_ChangedAssets.append(pAsset);
				continue;
			}
		}
		break;
	}

	return m_ChangedAssets.empty() == false;
}

void DlgAssetProperties::ApplyChanges()
{
	switch(ui->stackedAssetType->currentIndex())
	{
	case ASSET_Atlas: {
		bool bHasDiffOptions = ui->cmbTextureType->count() == (HYNUM_TEXTUREFORMATS + 1);
		bool bIsDiffOptions = bHasDiffOptions && ui->cmbTextureType->currentIndex() == 0;
		if(bIsDiffOptions == false)
		{
			HyTextureFormat eNewFormat = static_cast<HyTextureFormat>(ui->cmbTextureType->currentIndex() - (bHasDiffOptions ? 1 : 0));
			for(auto pAsset : m_ChangedAssets)
			{
				AtlasFrame *pFrame = static_cast<AtlasFrame *>(pAsset);
				pFrame->SetFormat(eNewFormat);
			}
		}

		bHasDiffOptions = ui->cmbTextureFiltering->count() == (HYNUM_TEXTUREFILTERS + 1);
		bIsDiffOptions = bHasDiffOptions && ui->cmbTextureFiltering->currentIndex() == 0;
		if(bIsDiffOptions == false)
		{
			HyTextureFiltering eNewFiltering = static_cast<HyTextureFiltering>(ui->cmbTextureFiltering->currentIndex() - (bHasDiffOptions ? 1 : 0));
			for(auto pAsset : m_ChangedAssets)
			{
				AtlasFrame *pFrame = static_cast<AtlasFrame *>(pAsset);
				pFrame->SetFiltering(eNewFiltering);
			}
		}
		break; }

	case ASSET_Audio:
		for(auto pAsset : m_ChangedAssets)
		{
			AudioAsset *pAudio = static_cast<AudioAsset *>(pAsset);
			if(ui->chkIsMusic->checkState() != Qt::PartiallyChecked)
				pAudio->SetIsMusic(ui->chkIsMusic->checkState() == Qt::Checked);

			if(ui->chkExportAsMono->checkState() != Qt::PartiallyChecked)
				pAudio->SetIsExportMono(ui->chkExportAsMono->checkState() == Qt::Checked);
				

			if(ui->chkIsCompressed->checkState() != Qt::PartiallyChecked)
			{
				pAudio->SetIsCompressed(ui->chkIsCompressed->checkState() == Qt::Checked);
				if(ui->chkIsCompressed->checkState() == Qt::Checked)
					pAudio->SetVbrQuality(ui->sbVbrQuality->value());
			}

			if(ui->chkUseGlobalLimit->checkState() != Qt::PartiallyChecked)
			{
				if(ui->chkUseGlobalLimit->checkState() == Qt::Checked)
					pAudio->SetGlobalLimit(ui->sbGlobalLimit->value());
				else
					pAudio->SetGlobalLimit(-1);
			}
		}
		break;
	}
}
