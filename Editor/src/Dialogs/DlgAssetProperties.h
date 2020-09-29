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

public:
	explicit DlgAssetProperties(AssetType eManagerType, QList<AssetItemData *> assetList, QWidget *parent = 0);
	~DlgAssetProperties();

//private Q_SLOTS:
//	virtual void done(int r);

private:
	Ui::DlgAssetProperties *ui;
};

#endif // DLGASSETPROPERTIES_H
