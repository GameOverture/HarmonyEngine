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
#include "AudioManager.h"

#include <QMessageBox>

DlgAssetProperties::DlgAssetProperties(IManagerModel *pManagerModel, QList<IAssetItemData *> assetList, QWidget *parent) :
	QDialog(parent),
	m_SelectedAssets(assetList),
	ui(new Ui::DlgAssetProperties)
{
	ui->setupUi(this);

	setWindowIcon(HyGlobal::AssetIcon(pManagerModel->GetAssetType(), SUBICON_Settings));
	ui->stackedAssetType->setCurrentIndex(pManagerModel->GetAssetType());

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

	// Assign values (and map data) to each combobox entry
	std::vector<HyImageType> imageTypeList = HyAssets::GetImageTypeList();
	for(HyImageType eType : imageTypeList)
		ui->cmbImageType->addItem(QString(HyAssets::GetImageTypeName(eType).c_str()), eType);

	ui->cmbPngChannels->addItem("32bit - RGBA", 4);
	ui->cmbPngChannels->addItem("24bit - RGB", 3);
	ui->cmbPngChannels->setCurrentIndex(0);

	ui->cmbHytxChannels->addItem("32bit - RGBA", 4);
	ui->cmbHytxChannels->addItem("24bit - RGB", 3);
	ui->cmbHytxChannels->setCurrentIndex(0);
	ui->cmbHytxDataType->addItem("UINT8", HYTEXFORMAT_UINT8);
	ui->cmbHytxDataType->addItem("INT8", HYTEXFORMAT_INT8);
	ui->cmbHytxDataType->setCurrentIndex(0);

	ui->cmbDdsCompression->addItem("DXT5", HYTEXFORMAT_BC3_DXT5);
	ui->cmbDdsCompression->addItem("DXT1", HYTEXFORMAT_BC1_DXT1);
	ui->cmbDdsCompression->setCurrentIndex(0);
	ui->cmbDdsDxt1Channels->addItem("RGB", 3);
	ui->cmbDdsDxt1Channels->addItem("RGBA (1 bit alpha)", 4);
	ui->cmbDdsDxt1Channels->setCurrentIndex(0);
	// cmbDdsDxt1Channels visibility gets set by 'on_cmbDdsCompression_currentIndexChanged' signal callback
	
	ui->cmbAstcColorProfile->addItem("LDR - Linear", HYASTC_Linear);
	ui->cmbAstcColorProfile->addItem("LDR - Standard", HYASTC_Standard);
	ui->cmbAstcColorProfile->addItem("HDR - RGB (with linear alpha)", HYASTC_HDR_LinearA);
	ui->cmbAstcColorProfile->addItem("HDR - RGBA", HYASTC_HDR_RGBA);
	ui->cmbAstcColorProfile->setCurrentIndex(1);
	// NOTE: Using HYTEXFORMAT_ASTC_LINEAR_* as the item data (even if LDR - Standard is selected)
	ui->cmbAstcBlockSize->addItem("4x4 (8.00bpp)", HYTEXFORMAT_ASTC_LINEAR_4x4);
	ui->cmbAstcBlockSize->addItem("5x4 (6.40bpp)", HYTEXFORMAT_ASTC_LINEAR_5x4);
	ui->cmbAstcBlockSize->addItem("5x5 (5.12bpp)", HYTEXFORMAT_ASTC_LINEAR_5x5);
	ui->cmbAstcBlockSize->addItem("6x5 (4.27bpp)", HYTEXFORMAT_ASTC_LINEAR_6x5);
	ui->cmbAstcBlockSize->addItem("6x6 (3.56bpp)", HYTEXFORMAT_ASTC_LINEAR_6x6);
	ui->cmbAstcBlockSize->addItem("8x5 (3.20bpp)", HYTEXFORMAT_ASTC_LINEAR_8x5);
	ui->cmbAstcBlockSize->addItem("8x6 (2.67bpp)", HYTEXFORMAT_ASTC_LINEAR_8x6);
	ui->cmbAstcBlockSize->addItem("10x5 (2.56bpp)", HYTEXFORMAT_ASTC_LINEAR_10x5);
	ui->cmbAstcBlockSize->addItem("10x6 (2.13bpp)", HYTEXFORMAT_ASTC_LINEAR_10x6);
	ui->cmbAstcBlockSize->addItem("8x8 (2.00bpp)", HYTEXFORMAT_ASTC_LINEAR_8x8);
	ui->cmbAstcBlockSize->addItem("10x8 (1.60bpp)", HYTEXFORMAT_ASTC_LINEAR_10x8);
	ui->cmbAstcBlockSize->addItem("10x10 (1.28bpp)", HYTEXFORMAT_ASTC_LINEAR_10x10);
	ui->cmbAstcBlockSize->addItem("12x10 (1.07bpp)", HYTEXFORMAT_ASTC_LINEAR_12x10);
	ui->cmbAstcBlockSize->addItem("12x12 (0.89bpp)", HYTEXFORMAT_ASTC_LINEAR_12x12);
	ui->cmbAstcBlockSize->setCurrentIndex(4);

	std::vector<HyTextureFilter> filterList = HyAssets::GetTextureFilterList();
	for(HyTextureFilter eFilter : filterList)
		ui->cmbTextureFilter->addItem(QString(HyAssets::GetTextureFilterName(eFilter).c_str()), eFilter);

	switch(pManagerModel->GetAssetType())
	{
	case ASSETMAN_Atlases: {
		// Image Type and Format ///////////////////////////////////////////////////////////////////////////////////////////
		HyImageInfo compareImageInfo = static_cast<AtlasFrame *>(m_SelectedAssets[0])->GetImageInfo();
		HyTextureInfo compareTextureInfo = static_cast<AtlasFrame *>(m_SelectedAssets[0])->GetTextureInfo();
		bool bIsDiffImage = false;
		for(auto pAsset : m_SelectedAssets)
		{
			AtlasFrame *pFrame = static_cast<AtlasFrame *>(pAsset);
			if(pFrame->GetImageInfo().GetType() != compareImageInfo.GetType())
			{
				ui->cmbImageType->insertItem(0, "<different options>", HYIMAGE_Unknown);
				ui->cmbImageType->setCurrentIndex(0);
				bIsDiffImage = true;
				break;
			}
		}
		if(bIsDiffImage)
			ui->stackedFormatOptions->setCurrentIndex(ui->stackedFormatOptions->count() - 1);
		else
		{
			ui->cmbImageType->setCurrentIndex(compareImageInfo.GetType());

			bool bIsDiffFormatOption1 = false;
			bool bIsDiffFormatOption2 = false;
			QVariant imageTypeVar = ui->cmbImageType->itemData(ui->cmbImageType->currentIndex());
			switch(static_cast<HyImageType>(imageTypeVar.toInt()))
			{
			case HYIMAGE_PNG:
				for(auto pAsset : m_SelectedAssets)
				{
					AtlasFrame *pFrame = static_cast<AtlasFrame *>(pAsset);
					if(pFrame->GetImageInfo().GetNumChannels() != compareImageInfo.GetNumChannels())
					{
						ui->cmbPngChannels->insertItem(0, "<different options>", 0);
						ui->cmbPngChannels->setCurrentIndex(0);
						bIsDiffFormatOption1 = true;
						break;
					}
				}
				if(bIsDiffFormatOption1 == false)
					ui->cmbPngChannels->setCurrentIndex(ui->cmbPngChannels->findData(compareImageInfo.GetNumChannels()));
				break;
			case HYIMAGE_HYTX:
				for(auto pAsset : m_SelectedAssets)
				{
					AtlasFrame *pFrame = static_cast<AtlasFrame *>(pAsset);
					if(bIsDiffFormatOption1 == false && pFrame->GetImageInfo().GetNumChannels() != compareImageInfo.GetNumChannels())
					{
						ui->cmbHytxChannels->insertItem(0, "<different options>", 0);
						ui->cmbHytxChannels->setCurrentIndex(0);
						bIsDiffFormatOption1 = true;
					}
					if(bIsDiffFormatOption2 == false && pFrame->GetImageInfo().GetFormat() != compareImageInfo.GetFormat())
					{
						ui->cmbHytxDataType->insertItem(0, "<different options>", HYTEXFORMAT_Unknown);
						ui->cmbHytxDataType->setCurrentIndex(0);
						bIsDiffFormatOption2 = true;
					}
					if(bIsDiffFormatOption1 && bIsDiffFormatOption2)
						break;
				}
				if(bIsDiffFormatOption1 == false)
					ui->cmbHytxChannels->setCurrentIndex(ui->cmbHytxChannels->findData(compareImageInfo.GetNumChannels()));
				if(bIsDiffFormatOption2 == false)
					ui->cmbHytxDataType->setCurrentIndex(ui->cmbHytxDataType->findData(compareImageInfo.GetFormat()));
				break;

			case HYIMAGE_DDS:
				for(auto pAsset : m_SelectedAssets)
				{
					AtlasFrame *pFrame = static_cast<AtlasFrame *>(pAsset);
					if(pFrame->GetImageInfo().GetFormat() != compareImageInfo.GetFormat())
					{
						ui->cmbDdsCompression->insertItem(0, "<different options>", HYTEXFORMAT_Unknown);
						ui->cmbDdsCompression->setCurrentIndex(0);
						bIsDiffFormatOption1 = true;
						break;
					}
				}
				if(bIsDiffFormatOption1 == false && compareImageInfo.GetFormat() == HYTEXFORMAT_BC1_DXT1)
				{
					for(auto pAsset : m_SelectedAssets)
					{
						AtlasFrame *pFrame = static_cast<AtlasFrame *>(pAsset);
						if(pFrame->GetImageInfo().GetNumChannels() != compareImageInfo.GetNumChannels())
						{
							ui->cmbDdsDxt1Channels->insertItem(0, "<different options>", 0);
							ui->cmbDdsDxt1Channels->setCurrentIndex(0);
							bIsDiffFormatOption2 = true;
							break;
						}
					}
				}
				if(bIsDiffFormatOption1 == false)
					ui->cmbDdsCompression->setCurrentIndex(ui->cmbDdsCompression->findData(compareImageInfo.GetFormat()));
				if(bIsDiffFormatOption2 == false)
					ui->cmbDdsDxt1Channels->setCurrentIndex(ui->cmbDdsDxt1Channels->findData(compareImageInfo.GetNumChannels()));
				break;

			case HYIMAGE_ASTC:
				for(auto pAsset : m_SelectedAssets)
				{
					AtlasFrame *pFrame = static_cast<AtlasFrame *>(pAsset);
					if(bIsDiffFormatOption1 == false && pFrame->GetImageInfo().GetFormatParam() != compareImageInfo.GetFormatParam())
					{
						ui->cmbAstcColorProfile->insertItem(0, "<different options>", HYASTC_Unknown);
						ui->cmbAstcColorProfile->setCurrentIndex(0);
						bIsDiffFormatOption1 = true;
					}
					if(bIsDiffFormatOption2 == false && pFrame->GetImageInfo().GetFormat() != compareImageInfo.GetFormat())
					{
						ui->cmbAstcBlockSize->insertItem(0, "<different options>", HYTEXFORMAT_Unknown);
						ui->cmbAstcBlockSize->setCurrentIndex(0);
						bIsDiffFormatOption2 = true;
					}
				}
				if(bIsDiffFormatOption1 == false)
					ui->cmbAstcBlockSize->setCurrentIndex(ui->cmbAstcBlockSize->findData(compareImageInfo.GetFormatParam()));
				if(bIsDiffFormatOption2 == false)
					ui->cmbAstcColorProfile->setCurrentIndex(ui->cmbAstcColorProfile->findData(compareImageInfo.GetFormat()));
				break;
			}
		}
		// Flip Image ///////////////////////////////////////////////////////////////////////////////////////////
		bool bIsDiffFlip = false;
		for(auto pAsset : m_SelectedAssets)
		{
			AtlasFrame *pFrame = static_cast<AtlasFrame *>(pAsset);
			if(pFrame->GetImageInfo().IsVerticalFlip() != compareImageInfo.IsVerticalFlip())
			{
				ui->chkVerticalFlip->setCheckState(Qt::PartiallyChecked);
				bIsDiffFlip = true;
				break;
			}
		}
		if(bIsDiffFlip == false)
			ui->chkVerticalFlip->setChecked(compareImageInfo.IsVerticalFlip());

		// Texture Filter //////////////////////////////////////////////////////////////////////////////////////////
		bool bIsDiffFilter = false;
		for(auto pAsset : m_SelectedAssets)
		{
			AtlasFrame *pFrame = static_cast<AtlasFrame *>(pAsset);
			if(pFrame->GetTextureInfo().GetFilter() != compareTextureInfo.GetFilter())
			{
				ui->cmbTextureFilter->insertItem(0, "<different options>", HYTEXFILTER_Unknown);
				ui->cmbTextureFilter->setCurrentIndex(0);
				break;
			}
		}
		break; }

	case ASSETMAN_Audio: {
		Qt::CheckState eCheckState;
		// Audio Category /////////////////////////////////////////////////////////////////////////////////////////
		int32 iCategoryId = static_cast<SoundClip *>(m_SelectedAssets[0])->GetCategoryId();
		bool bCheckable = false;
		for(auto pAsset : m_SelectedAssets)
		{
			if(static_cast<SoundClip *>(pAsset)->GetCategoryId() != iCategoryId)
			{
				bCheckable = true;
				break;
			}
		}
		ui->audioCategory->Init(&static_cast<AudioManager *>(pManagerModel)->GetCategoriesModel(), iCategoryId, bCheckable);

		// Is Streaming ///////////////////////////////////////////////////////////////////////////////////////////
		eCheckState = static_cast<SoundClip *>(m_SelectedAssets[0])->IsStreaming() ? Qt::Checked : Qt::Unchecked;
		for(auto pAsset : m_SelectedAssets)
		{
			if((eCheckState == Qt::Unchecked && static_cast<SoundClip *>(pAsset)->IsStreaming()) ||
			   (eCheckState == Qt::Checked && static_cast<SoundClip *>(pAsset)->IsStreaming() == false))
			{
				eCheckState = Qt::PartiallyChecked;
				break;
			}
		}
		ui->chkIsStreaming->setCheckState(eCheckState);

		// Export As Mono ///////////////////////////////////////////////////////////////////////////////////////////
		eCheckState = static_cast<SoundClip *>(m_SelectedAssets[0])->IsExportMono() ? Qt::Checked : Qt::Unchecked;
		for(auto pAsset : m_SelectedAssets)
		{
			if((eCheckState == Qt::Unchecked && static_cast<SoundClip *>(pAsset)->IsExportMono()) ||
			   (eCheckState == Qt::Checked && static_cast<SoundClip *>(pAsset)->IsExportMono() == false))
			{
				eCheckState = Qt::PartiallyChecked;
				break;
			}
		}
		ui->chkExportAsMono->setCheckState(eCheckState);

		// Is Compressed ///////////////////////////////////////////////////////////////////////////////////////////
		eCheckState = static_cast<SoundClip *>(m_SelectedAssets[0])->IsCompressed() ? Qt::Checked : Qt::Unchecked;
		for(auto pAsset : m_SelectedAssets)
		{
			if((eCheckState == Qt::Unchecked && static_cast<SoundClip *>(pAsset)->IsCompressed()) ||
			   (eCheckState == Qt::Checked && static_cast<SoundClip *>(pAsset)->IsCompressed() == false))
			{
				eCheckState = Qt::PartiallyChecked;
				break;
			}
		}
		ui->chkIsCompressed->setCheckState(eCheckState);

		// Use Instance Limit ///////////////////////////////////////////////////////////////////////////////////////////
		int iInstLimit = static_cast<SoundClip *>(m_SelectedAssets[0])->GetInstanceLimit(); // 0 == no limit; -1 == different settings among assets
		for(auto pAsset : m_SelectedAssets)
		{
			if(static_cast<SoundClip *>(pAsset)->GetInstanceLimit() != iInstLimit)
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

		ui->sbVbrQuality->setDisabled(ui->chkIsCompressed->checkState() == Qt::Unchecked);
		ui->lblVbrQuality->setDisabled(ui->chkIsCompressed->checkState() == Qt::Unchecked);
		break; }
	} // switch(eManagerType)
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
		bool bIsVerticalFlipSet;
		HyImageInfo curImageInfo = GetSelectedImageInfo(bIsVerticalFlipSet);
		HyTextureInfo curTextureInfo = GetSelectedTextureInfo();

		for(auto pAsset : m_ChangedAssets)
		{
			AtlasFrame *pFrame = static_cast<AtlasFrame *>(pAsset);
			HyImageInfo frameImageInfo = pFrame->GetImageInfo();
			HyTextureInfo frameTextureInfo = pFrame->GetTextureInfo();

			if(curImageInfo.GetType() != HYIMAGE_Unknown && frameImageInfo.GetType() != curImageInfo.GetType())
				frameImageInfo.SetType(curImageInfo.GetType());

			if(curImageInfo.GetNumChannels() != 0 && frameImageInfo.GetNumChannels() != curImageInfo.GetNumChannels())
				frameImageInfo.SetNumChannels(curImageInfo.GetNumChannels());

			if(curImageInfo.GetFormat() != HYTEXFORMAT_Unknown && frameImageInfo.GetFormat() != curImageInfo.GetFormat())
				frameImageInfo.SetFormat(curImageInfo.GetFormat());

			if(curImageInfo.GetFormatParam() != 0 && frameImageInfo.GetFormatParam() != curImageInfo.GetFormatParam())
				frameImageInfo.SetFormatParam(curImageInfo.GetFormatParam());

			if(bIsVerticalFlipSet && frameImageInfo.IsVerticalFlip() != curImageInfo.IsVerticalFlip())
				frameImageInfo.SetVerticalFlip(curImageInfo.IsVerticalFlip());

			if(curTextureInfo.GetFilter() != HYTEXFILTER_Unknown && frameTextureInfo.GetFilter() != curTextureInfo.GetFilter())
				frameTextureInfo.SetFilter(curTextureInfo.GetFilter());
			
			frameTextureInfo.SetNumChannels(frameImageInfo.GetNumChannels());
			frameTextureInfo.SetFormat(frameImageInfo.GetFormat());
			frameTextureInfo.SetFormatParam(frameImageInfo.GetFormatParam());

			pFrame->SetImageInfo(frameImageInfo);
			pFrame->SetTextureInfo(frameTextureInfo);
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

void DlgAssetProperties::on_cmbImageType_currentIndexChanged(int iIndex)
{
	ui->stackedFormatOptions->setCurrentIndex(ui->cmbImageType->itemData(ui->cmbImageType->currentIndex()).toInt());
}

void DlgAssetProperties::on_cmbDdsCompression_currentIndexChanged(int iIndex)
{
	if(ui->cmbDdsCompression->itemData(iIndex).toInt() == HYTEXFORMAT_BC1_DXT1)
	{
		ui->cmbDdsDxt1Channels->setVisible(true);
		ui->lblDdsDxt1Channels->setVisible(true);
	}
	else
	{
		ui->cmbDdsDxt1Channels->setVisible(false);
		ui->lblDdsDxt1Channels->setVisible(false);
	}
}

void DlgAssetProperties::on_chkIsCompressed_clicked()
{
	ui->sbVbrQuality->setDisabled(ui->chkIsCompressed->checkState() == Qt::Unchecked);
	ui->lblVbrQuality->setDisabled(ui->chkIsCompressed->checkState() == Qt::Unchecked);
}

void DlgAssetProperties::on_sbVbrQuality_valueChanged(double dArg)
{
	ui->chkIsCompressed->setChecked(true);
	
	ui->sbVbrQuality->setDisabled(ui->chkIsCompressed->checkState() == Qt::Unchecked);
	ui->lblVbrQuality->setDisabled(ui->chkIsCompressed->checkState() == Qt::Unchecked);
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

HyImageInfo DlgAssetProperties::GetSelectedImageInfo(bool &bIsVerticalFlipSetOut) const
{
	HyImageInfo imageInfo;

	HyImageType eImageType = static_cast<HyImageType>(ui->cmbImageType->itemData(ui->cmbImageType->currentIndex()).toInt());
	imageInfo.SetType(eImageType);

	switch(eImageType)
	{
	case HYIMAGE_PNG:
		imageInfo.SetNumChannels(ui->cmbPngChannels->itemData(ui->cmbPngChannels->currentIndex()).toInt());
		break;

	case HYIMAGE_HYTX:
		imageInfo.SetNumChannels(ui->cmbHytxChannels->itemData(ui->cmbHytxChannels->currentIndex()).toInt());
		imageInfo.SetFormat(static_cast<HyTextureFormat>(ui->cmbHytxDataType->itemData(ui->cmbHytxDataType->currentIndex()).toInt()));
		break;

	case HYIMAGE_DDS:
		imageInfo.SetFormat(static_cast<HyTextureFormat>(ui->cmbDdsCompression->itemData(ui->cmbDdsCompression->currentIndex()).toInt()));
		if(imageInfo.GetFormat() == HYTEXFORMAT_BC1_DXT1)
			imageInfo.SetNumChannels(ui->cmbDdsDxt1Channels->itemData(ui->cmbDdsDxt1Channels->currentIndex()).toInt());
		break;

	case HYIMAGE_ASTC:
		imageInfo.SetFormatParam(static_cast<HyAstcColorProfile>(ui->cmbAstcColorProfile->itemData(ui->cmbAstcColorProfile->currentIndex()).toInt()));
		imageInfo.SetFormat(static_cast<HyTextureFormat>(ui->cmbAstcBlockSize->itemData(ui->cmbAstcBlockSize->currentIndex()).toInt()));
		break;
	}

	if(ui->chkVerticalFlip->checkState() != Qt::PartiallyChecked)
	{
		bIsVerticalFlipSetOut = true;
		imageInfo.SetVerticalFlip(ui->chkVerticalFlip->isChecked());
	}
	else
		bIsVerticalFlipSetOut = false;

	return imageInfo;
}

HyTextureInfo DlgAssetProperties::GetSelectedTextureInfo() const
{
	HyTextureInfo textureInfo;
	textureInfo.SetFilter(static_cast<HyTextureFilter>(ui->cmbTextureFilter->itemData(ui->cmbTextureFilter->currentIndex()).toInt()));
	
	return textureInfo;
}

bool DlgAssetProperties::DetermineChangedAssets()
{
	m_ChangedAssets.clear();

	switch(ui->stackedAssetType->currentIndex())
	{
	case ASSETMAN_Atlases: {
		bool bIsVerticalFlipSet;
		HyImageInfo curImageInfo = GetSelectedImageInfo(bIsVerticalFlipSet);
		HyTextureInfo curTextureInfo = GetSelectedTextureInfo();

		for(auto pAsset : m_SelectedAssets)
		{
			AtlasFrame *pFrame = static_cast<AtlasFrame *>(pAsset);
			HyImageInfo frameImageInfo = pFrame->GetImageInfo();
			HyTextureInfo frameTextureInfo = pFrame->GetTextureInfo();

			if((curImageInfo.GetType() != HYIMAGE_Unknown && frameImageInfo.GetType() != curImageInfo.GetType()) ||
			   (curImageInfo.GetNumChannels() != 0 && frameImageInfo.GetNumChannels() != curImageInfo.GetNumChannels()) ||
			   (curImageInfo.GetFormat() != HYTEXFORMAT_Unknown && frameImageInfo.GetFormat() != curImageInfo.GetFormat()) ||
			   (curImageInfo.GetFormatParam() != 0 && frameImageInfo.GetFormatParam() != curImageInfo.GetFormatParam()) ||
			   (bIsVerticalFlipSet && frameImageInfo.IsVerticalFlip() != curImageInfo.IsVerticalFlip()) ||
			   (curTextureInfo.GetFilter() != HYTEXFILTER_Unknown && frameTextureInfo.GetFilter() != curTextureInfo.GetFilter()))
			{
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

