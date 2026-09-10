/**************************************************************************
 *	DlgAssetProperties.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DLGASSETPROPERTIES_H
#define DLGASSETPROPERTIES_H

#include "IAssetItemData.h"

#include <QDialog>
#include <QJsonObject>

class IManagerModel;

namespace Ui {
class DlgAssetProperties;
}

class DlgAssetProperties : public QDialog
{
	Q_OBJECT

	QList<IAssetItemData *>		m_SelectedAssets;
	QList<IAssetItemData *>		m_ChangedAssets; // Order is preserved from 'm_SelectedAssets'

public:
	explicit DlgAssetProperties(IManagerModel *pManagerModel, QList<IAssetItemData *> assetList, QWidget *parent = 0);
	~DlgAssetProperties();

	QList<IAssetItemData *> GetChangedAssets();
	void ApplyChanges();

private Q_SLOTS:
	void on_cmbImageType_currentIndexChanged(int iIndex);
	void on_cmbDdsCompression_currentIndexChanged(int iIndex);
	void on_chkIsCompressed_clicked();
	void on_sbVbrQuality_valueChanged(double dArg);

	virtual void done(int r);

private:
	Ui::DlgAssetProperties *ui;

	HyImageInfo GetSelectedImageInfo(bool &bIsVerticalFlipSetOut) const;
	HyTextureInfo GetSelectedTextureInfo() const;

	bool DetermineChangedAssets();
};

static_assert(0 == ASSETMAN_Source, "DlgAssetProperties - stackedAssetType page does not align with AssetManagerType");
static_assert(1 == ASSETMAN_Atlases, "DlgAssetProperties - stackedAssetType page does not align with AssetManagerType");
static_assert(2 == ASSETMAN_Prefabs, "DlgAssetProperties - stackedAssetType page does not align with AssetManagerType");
static_assert(3 == ASSETMAN_Audio, "DlgAssetProperties - stackedAssetType page does not align with AssetManagerType");
static_assert(4 == NUM_ASSETMANTYPES, "DlgAssetProperties - stackedAssetType missing an 'AssetManagerType'");

static_assert(0 == HYIMAGE_PNG, "DlgAssetProperties - stackedFormatOptions page does not align with HyImageType");
static_assert(1 == HYIMAGE_HYTX, "DlgAssetProperties - stackedFormatOptions page does not align with HyImageType");
static_assert(2 == HYIMAGE_DDS, "DlgAssetProperties - stackedFormatOptions page does not align with HyImageType");
static_assert(3 == HYIMAGE_ASTC, "DlgAssetProperties - stackedFormatOptions page does not align with HyImageType");
static_assert(4 == HYNUM_IMAGETYPES, "DlgAssetProperties - stackedFormatOptions missing an 'HyImageType'");

#endif // DLGASSETPROPERTIES_H
