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
#include "ManagerWidget.h"

#include <QWidget>
#include <QGraphicsScene>

class WgtCodeEditor;

namespace Ui {
class AuxAssetInspector;
}

class AuxAssetInspector : public QWidget
{
	Q_OBJECT

	QGraphicsScene				m_AtlasesGfxScene;
	IAssetItemData *			m_pCurAtlasesAsset;

	QGraphicsScene				m_AudioGfxScene;

public:
	explicit AuxAssetInspector(QWidget *parent = 0);
	virtual ~AuxAssetInspector();

	WgtCodeEditor &GetCodeEditor();

	void SetAssetManager(AssetManagerType eAssetManager);
	void SetFocusedAssets(AssetManagerType eAssetManager, const QList<IAssetItemData *> &selectedAssetsList);

	void Clear(AssetManagerType eAssetManager);

private:
	Ui::AuxAssetInspector *ui;
};

#endif // AUXASSETINSPECTOR_H
