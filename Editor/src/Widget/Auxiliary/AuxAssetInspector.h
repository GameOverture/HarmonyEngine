/**************************************************************************
 *	AuxAssetInspector.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AUXASSETINSPECTOR_H
#define AUXASSETINSPECTOR_H

#include "Global.h"
#include "IAssetItemData.h"

#include <QWidget>

namespace Ui {
class AuxAssetInspector;
}

class AuxAssetInspector : public QWidget
{
	Q_OBJECT

public:
	explicit AuxAssetInspector(QWidget *parent = 0);
	virtual ~AuxAssetInspector();

	void SetSelected(AssetManagerType eAssetType, QList<IAssetItemData *> selectedAssetsList);

private:
	Ui::AuxAssetInspector *ui;
};

#endif // AUXASSETINSPECTOR_H
