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

#include <QMessageBox>

DlgAssetProperties::DlgAssetProperties(AssetType eManagerType, QList<AssetItemData *> assetList, QWidget *parent) :
	QDialog(parent),
	m_SelectedAssets(assetList),
	ui(new Ui::DlgAssetProperties)
{
	ui->setupUi(this);
	ui->stackedAssetType->setCurrentIndex(eManagerType);

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
	} // switch(eManagerType)
}

DlgAssetProperties::~DlgAssetProperties()
{
	delete ui;
}

///*virtual*/ void DlgAssetProperties::done(int r)
//{
//	if(r == QDialog::Accepted)
//	{
//		// check if it is ok or not
//		if(true)//IsSettingsDirty() && m_bAtlasGrpHasImages)
//		{
//			if(QMessageBox::Ok == QMessageBox::warning(nullptr, QString("Save asset properties?"), QString("Save asset properties?"), QMessageBox::Ok, QMessageBox::Cancel))
//				QDialog::done(r);
//			else
//				QDialog::done(QDialog::Rejected);
//		}
//	}
//	
//	QDialog::done(r);
//}
