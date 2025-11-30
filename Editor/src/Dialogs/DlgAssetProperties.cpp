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
#include "SoundClip.h"
#include "AudioManagerModel.h"

#include <QMessageBox>

DlgAssetProperties::DlgAssetProperties(IManagerModel *pManagerModel, QList<IAssetItemData *> assetList, QWidget *parent) :
	QDialog(parent),
	m_SelectedAssets(assetList),
	ui(new Ui::DlgAssetProperties)
{
	ui->setupUi(this);

	setWindowIcon(HyGlobal::AssetIcon(pManagerModel->GetAssetType(), SUBICON_Settings));
	ui->stackedAssetType->setCurrentIndex(pManagerModel->GetAssetType());

	// Assign values to each combobox entry
	// Uncompressed
	ui->cmbUncompressedColorChannels->setItemData(0, 4); // RGBA
	ui->cmbUncompressedColorChannels->setItemData(1, 3); // RGB
	ui->cmbUncompressedFileType->setItemData(0, HyTextureInfo::UNCOMPRESSEDFILE_PNG);
	// DXT
	ui->cmbDxtType->setItemData(0, 5); // DXT 5 (RGBA)
	ui->cmbDxtType->setItemData(1, 1); // DXT 1 (RGB)
	// ASTC
	ui->cmbAstcColorProfile->setItemData(0, 0); // LDR Linear
	ui->cmbAstcColorProfile->setItemData(1, 1); // LDR sRGBA
	ui->cmbAstcColorProfile->setItemData(2, 2); // HDR RGB
	ui->cmbAstcColorProfile->setItemData(3, 3); // HDR RGBA
	ui->cmbAstcBlockSize->setItemData(0, 0); // 4x4
	ui->cmbAstcBlockSize->setItemData(1, 1); // 5x4
	ui->cmbAstcBlockSize->setItemData(2, 2); // 5x5
	ui->cmbAstcBlockSize->setItemData(3, 3); // 6x5
	ui->cmbAstcBlockSize->setItemData(4, 4); // 6x6
	ui->cmbAstcBlockSize->setItemData(5, 5); // 8x5
	ui->cmbAstcBlockSize->setItemData(6, 6); // 8x6
	ui->cmbAstcBlockSize->setItemData(7, 7); // 10x5
	ui->cmbAstcBlockSize->setItemData(8, 8); // 10x6
	ui->cmbAstcBlockSize->setItemData(9, 9); // 8x8
	ui->cmbAstcBlockSize->setItemData(10, 10); // 10x8
	ui->cmbAstcBlockSize->setItemData(11, 11); // 10x10
	ui->cmbAstcBlockSize->setItemData(12, 12); // 12x10
	ui->cmbAstcBlockSize->setItemData(13, 13); // 12x12

	// Set 'name' and 'num selected'
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

	switch(pManagerModel->GetAssetType())
	{
	case ASSETMAN_Atlases: {
		// Texture Format ///////////////////////////////////////////////////////////////////////////////////////////
		HyTextureInfo texInfo = static_cast<AtlasFrame *>(m_SelectedAssets[0])->GetTextureInfo();
		bool bIsDiffOptions = false;
		for(auto pAsset : m_SelectedAssets)
		{
			AtlasFrame *pFrame = static_cast<AtlasFrame *>(pAsset);
			if(pFrame->GetFormat() != texInfo.GetFormat())
			{
				ui->cmbTextureType->addItem("<different options>");
				ui->cmbTextureType->setCurrentIndex(0);
				bIsDiffOptions = true;
				break;
			}
		}
		for(int i = 0; i < HYNUM_TEXTUREFORMATS; ++i)
			ui->cmbTextureType->addItem(QString(HyAssets::GetTextureFormatName(static_cast<HyTextureFormat>(i)).c_str()));

		if(bIsDiffOptions == false)
		{
			ui->cmbTextureType->setCurrentIndex(texInfo.GetFormat());

			switch(texInfo.GetFormat())
			{
			case HYTEXTURE_Uncompressed:
				if(texInfo.m_uiFormatParam1 == 4)
					ui->cmbUncompressedColorChannels->setCurrentIndex(0);
				else
					ui->cmbUncompressedColorChannels->setCurrentIndex(1);
				ui->cmbUncompressedFileType->setCurrentIndex(ui->cmbUncompressedFileType->findData(QVariant(texInfo.m_uiFormatParam2)));
				break;

			case HYTEXTURE_DXT:
				if(texInfo.m_uiFormatParam2 == 5)
					ui->cmbDxtType->setCurrentIndex(0);
				else
					ui->cmbDxtType->setCurrentIndex(1);
				break;

			case HYTEXTURE_ASTC:
				ui->cmbAstcBlockSize->setCurrentIndex(ui->cmbAstcBlockSize->findData(QVariant(texInfo.m_uiFormatParam1)));
				ui->cmbAstcColorProfile->setCurrentIndex(ui->cmbAstcColorProfile->findData(QVariant(texInfo.m_uiFormatParam2)));
				break;
			}
			ui->grpFormatOptions->setVisible(true);
		}

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

	case ASSETMAN_Audio:
		// Audio Category /////////////////////////////////////////////////////////////////////////////////////////
		int32 iCategoryId = static_cast<SoundClip *>(m_SelectedAssets[0])->GetCategoryId();
		bool bCheckable = false;
		for(auto asset : m_SelectedAssets)
		{
			if(static_cast<SoundClip *>(asset)->GetCategoryId() != iCategoryId)
			{
				bCheckable = true;
				break;
			}
		}
		ui->audioCategory->Init(&static_cast<AudioManagerModel *>(pManagerModel)->GetCategoriesModel(), iCategoryId, bCheckable);

		// Is Streaming ///////////////////////////////////////////////////////////////////////////////////////////
		eCheckState = static_cast<SoundClip *>(m_SelectedAssets[0])->IsStreaming() ? Qt::Checked : Qt::Unchecked;
		for(auto asset : m_SelectedAssets)
		{
			if((eCheckState == Qt::Unchecked && static_cast<SoundClip *>(asset)->IsStreaming()) ||
			   (eCheckState == Qt::Checked && static_cast<SoundClip *>(asset)->IsStreaming() == false))
			{
				eCheckState = Qt::PartiallyChecked;
				break;
			}
		}
		ui->chkIsStreaming->setCheckState(eCheckState);

		// Export As Mono ///////////////////////////////////////////////////////////////////////////////////////////
		eCheckState = static_cast<SoundClip *>(m_SelectedAssets[0])->IsExportMono() ? Qt::Checked : Qt::Unchecked;
		for(auto asset : m_SelectedAssets)
		{
			if((eCheckState == Qt::Unchecked && static_cast<SoundClip *>(asset)->IsExportMono()) ||
			   (eCheckState == Qt::Checked && static_cast<SoundClip *>(asset)->IsExportMono() == false))
			{
				eCheckState = Qt::PartiallyChecked;
				break;
			}
		}
		ui->chkExportAsMono->setCheckState(eCheckState);

		// Is Compressed ///////////////////////////////////////////////////////////////////////////////////////////
		eCheckState = static_cast<SoundClip *>(m_SelectedAssets[0])->IsCompressed() ? Qt::Checked : Qt::Unchecked;
		for(auto asset : m_SelectedAssets)
		{
			if((eCheckState == Qt::Unchecked && static_cast<SoundClip *>(asset)->IsCompressed()) ||
			   (eCheckState == Qt::Checked && static_cast<SoundClip *>(asset)->IsCompressed() == false))
			{
				eCheckState = Qt::PartiallyChecked;
				break;
			}
		}
		ui->chkIsCompressed->setCheckState(eCheckState);

		// Use Instance Limit ///////////////////////////////////////////////////////////////////////////////////////////
		int iInstLimit = static_cast<SoundClip *>(m_SelectedAssets[0])->GetInstanceLimit(); // 0 == no limit; -1 == different settings among assets
		for(auto asset : m_SelectedAssets)
		{
			if(static_cast<SoundClip *>(asset)->GetInstanceLimit() != iInstLimit)
			{
				iInstLimit = -1;
				break;
			}
		}

		ui->grpMaxInstances->setChecked(iInstLimit == 0 ? false : true);
		if(iInstLimit == -1 || iInstLimit == 0)
			ui->sbInstanceLimit->clear();
		else
			ui->sbInstanceLimit->setValue(iInstLimit);
		break;
	} // switch(eManagerType)

	Refresh();
}

DlgAssetProperties::~DlgAssetProperties()
{
	delete ui;
}

QList<IAssetItemData *> DlgAssetProperties::GetChangedAssets()
{
	return m_ChangedAssets;
}

void DlgAssetProperties::ApplyChanges()
{
	switch(ui->stackedAssetType->currentIndex())
	{
	case ASSETMAN_Atlases: {
		uint8 uiParam1 = 0, uiParam2 = 0;
		HyTextureFormat eFormat = GetSelectedAtlasFormat(uiParam1, uiParam2);
		HyTextureFiltering eFiltering = GetSelectedAtlasFiltering();

		for(auto pAsset : m_ChangedAssets)
		{
			AtlasFrame *pFrame = static_cast<AtlasFrame *>(pAsset);
			HyTextureInfo assetTexInfo = pFrame->GetTextureInfo();

			if(eFiltering != HYTEXFILTER_Unknown)
				pFrame->SetFiltering(eFiltering);

			if(eFormat != HYTEXTURE_Unknown)
				pFrame->SetFormat(eFormat, uiParam1, uiParam2);
		}
		break; }

	case ASSETMAN_Audio:
		for(auto pAsset : m_ChangedAssets)
		{
			SoundClip *pAudio = static_cast<SoundClip *>(pAsset);

			if(ui->audioCategory->IsValid() && ui->audioCategory->GetCurrentId() != pAudio->GetCategoryId())
				pAudio->SetCategoryId(ui->audioCategory->GetCurrentId());

			if(ui->chkIsStreaming->checkState() != Qt::PartiallyChecked)
				pAudio->SetIsStreaming(ui->chkIsStreaming->checkState() == Qt::Checked);

			if(ui->chkExportAsMono->checkState() != Qt::PartiallyChecked)
				pAudio->SetIsExportMono(ui->chkExportAsMono->checkState() == Qt::Checked);

			if(ui->chkIsCompressed->checkState() != Qt::PartiallyChecked)
			{
				pAudio->SetIsCompressed(ui->chkIsCompressed->checkState() == Qt::Checked);
				if(ui->chkIsCompressed->checkState() == Qt::Checked)
					pAudio->SetVbrQuality(ui->sbVbrQuality->value());
			}

			// TODO: Add instance action
			if(ui->sbInstanceLimit->text().isEmpty() == false)
			{
				if(ui->grpMaxInstances->isChecked())
					pAudio->SetInstanceLimit(ui->sbInstanceLimit->value());
				else
					pAudio->SetInstanceLimit(0);
			}
			else
				pAudio->SetInstanceLimit(0);
		}
		break;
	}
}

void DlgAssetProperties::on_cmbTextureType_currentIndexChanged(int iIndex)
{
	Refresh();
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

void DlgAssetProperties::on_sbInstanceLimit_valueChanged(int iArg)
{
	Refresh();
}

/*virtual*/ void DlgAssetProperties::done(int r)
{
	bool bAssetsChanged = DetermineChangedAssets();

	if(r == QDialog::Accepted)
	{
		if(bAssetsChanged && QMessageBox::Ok == QMessageBox::warning(nullptr, QString("Save asset properties?"), QString("Save asset properties? Changed assets will need to be repacked."), QMessageBox::Ok, QMessageBox::Cancel))
		{
			QDialog::done(r);
		}
		else
			QDialog::done(QDialog::Rejected);
	}
	
	QDialog::done(r);
}

HyTextureFormat DlgAssetProperties::GetSelectedAtlasFormat(uint8 &uiParam1Out, uint8 &uiParam2Out) const
{
	bool bHasDiffOptions = ui->cmbTextureType->count() == (HYNUM_TEXTUREFORMATS + 1);
	if(bHasDiffOptions && ui->cmbTextureType->currentIndex() == 0)
	{
		uiParam1Out = uiParam2Out = 0;
		return HYTEXTURE_Unknown; // This means "<different options>" is selected
	}

	HyTextureFormat eFormat = static_cast<HyTextureFormat>(ui->cmbTextureType->currentIndex() - (bHasDiffOptions ? 1 : 0));
	switch(eFormat)
	{
	case HYTEXTURE_Uncompressed:
		uiParam1Out = static_cast<uint8>(ui->cmbUncompressedColorChannels->currentData().toUInt());
		uiParam2Out = static_cast<uint8>(ui->cmbUncompressedFileType->currentData().toUInt());
		break;

	case HYTEXTURE_DXT:
		if(ui->cmbDxtType->currentData().toUInt() == 1)
		{
			uiParam1Out = 3; // RGB
			uiParam2Out = 1; // DXT 1
		}
		else
		{
			uiParam1Out = 4; // RGBA
			uiParam2Out = 5; // DXT 5
		}
		break;

	case HYTEXTURE_ASTC:
		uiParam1Out = static_cast<uint8>(ui->cmbAstcBlockSize->currentData().toUInt());
		uiParam2Out = static_cast<uint8>(ui->cmbAstcColorProfile->currentData().toUInt());
		break;
	}

	return eFormat;
}

HyTextureFiltering DlgAssetProperties::GetSelectedAtlasFiltering() const
{
	bool bHasDiffOptions = ui->cmbTextureFiltering->count() == (HYNUM_TEXTUREFILTERS + 1);
	if(bHasDiffOptions && ui->cmbTextureFiltering->currentIndex() == 0)
		return HYTEXFILTER_Unknown; // This means "<different options>" is selected

	return static_cast<HyTextureFiltering>(ui->cmbTextureFiltering->currentIndex() - (bHasDiffOptions ? 1 : 0));
}

void DlgAssetProperties::Refresh()
{
	switch(ui->stackedAssetType->currentIndex())
	{
	case ASSETMAN_Atlases: {
		uint8 uiParam1 = 0, uiParam2 = 0;
		HyTextureFormat eFormat = GetSelectedAtlasFormat(uiParam1, uiParam2);
		if(eFormat == HYTEXTURE_Unknown)
			ui->grpFormatOptions->setVisible(false);
		else
		{
			ui->grpFormatOptions->setVisible(true);
			ui->stackedFormatOptions->setCurrentIndex(eFormat);
		}
		break; }

	case ASSETMAN_Audio:
		ui->sbVbrQuality->setDisabled(ui->chkIsCompressed->checkState() == Qt::Unchecked);
		ui->lblVbrQuality->setDisabled(ui->chkIsCompressed->checkState() == Qt::Unchecked);
		break;
	}
}

bool DlgAssetProperties::DetermineChangedAssets()
{
	m_ChangedAssets.clear();

	switch(ui->stackedAssetType->currentIndex())
	{
	case ASSETMAN_Atlases: {
		uint8 uiParam1 = 0, uiParam2 = 0;
		HyTextureFormat eFormat = GetSelectedAtlasFormat(uiParam1, uiParam2);
		HyTextureFiltering eFiltering = GetSelectedAtlasFiltering();

		for(auto pAsset : m_SelectedAssets)
		{
			HyTextureInfo assetTexInfo = static_cast<AtlasFrame *>(pAsset)->GetTextureInfo();

			if(eFiltering != HYTEXFILTER_Unknown && eFiltering != assetTexInfo.GetFiltering())
				m_ChangedAssets.append(pAsset);
			else if(eFormat != HYTEXTURE_Unknown)
			{
				HyTextureInfo selectedTexInfo(eFiltering, eFormat, uiParam1, uiParam2);
				if(selectedTexInfo.GetBucketId() != assetTexInfo.GetBucketId())
					m_ChangedAssets.append(pAsset);
			}
		}
		break; }

	case ASSETMAN_Audio:
		for(auto pAsset : m_SelectedAssets)
		{
			SoundClip *pAudio = static_cast<SoundClip *>(pAsset);

			if(ui->audioCategory->IsValid() && pAudio->GetCategoryId() != ui->audioCategory->GetCurrentId())
			{
				m_ChangedAssets.append(pAsset);
				continue;
			}

			if((pAudio->IsStreaming() && ui->chkIsStreaming->checkState() == Qt::Unchecked) ||
			   (pAudio->IsStreaming() == false && ui->chkIsStreaming->checkState() == Qt::Checked))
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

			if(ui->grpMaxInstances->isChecked() == false && pAudio->GetInstanceLimit() != 0)
			{
				m_ChangedAssets.append(pAsset);
				continue;
			}
			else if(ui->grpMaxInstances->isChecked() && ui->sbInstanceLimit->value() != pAudio->GetInstanceLimit())
			{
				m_ChangedAssets.append(pAsset);
				continue;
			}
		}
		break;
	}

	return m_ChangedAssets.empty() == false;
}

