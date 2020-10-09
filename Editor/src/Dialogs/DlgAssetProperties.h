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

namespace Ui {
class DlgAssetProperties;
}

class DlgAssetProperties : public QDialog
{
	Q_OBJECT

	QList<AssetItemData *>		m_SelectedAssets;
	QList<AssetItemData *>		m_ChangedAssets; // Order is preserved from 'm_SelectedAssets'

public:
	explicit DlgAssetProperties(AssetType eManagerType, QList<AssetItemData *> assetList, QWidget *parent = 0);
	~DlgAssetProperties();

	QList<AssetItemData *> GetChangedAssets();

private Q_SLOTS:
	void on_chkIsCompressed_clicked();
	void on_sbVbrQuality_valueChanged(double dArg);
	void on_chkUseGlobalLimit_clicked();
	void on_sbGlobalLimit_valueChanged(int iArg);

	virtual void done(int r);

private:
	Ui::DlgAssetProperties *ui;

	void Refresh();
	bool DetermineChangedAssets();
	void ApplyChanges();
};

#endif // DLGASSETPROPERTIES_H
