/**************************************************************************
 *	WgtTileSetTerrain.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WgtTileSetTerrain_H
#define WgtTileSetTerrain_H

#include "Global.h"
#include "IWgtTileSetItem.h"

#include <QLineEdit>
#include <QLabel>
#include <QFrame>
#include <QToolButton>
#include <QPushButton>
#include <QColorDialog>

class WgtTileSetTerrainSet;

namespace Ui {
	class WgtTileSetTerrain;
}

class WgtTileSetTerrain : public IWgtTileSetItem
{
	Q_OBJECT

	Ui::WgtTileSetTerrain *		ui;
	WgtTileSetTerrainSet *		m_pParentTerrainSet;

public:
	WgtTileSetTerrain(AuxTileSet *pAuxTileSet, WgtTileSetTerrainSet *pParentTerrainSet, QJsonObject initDataObj, QWidget *pParent = nullptr);
	virtual ~WgtTileSetTerrain();

	WgtTileSetTerrainSet *GetParentTerrainSet();

	virtual void OnInit(QJsonObject serializedObj) override;
	virtual QJsonObject SerializeCurrentWidgets() override;

	void SetOrderBtns(bool bUpEnabled, bool bDownEnabled);

protected:
	virtual QFrame *GetBorderFrame() const override;

private Q_SLOTS:
	void on_actionDelete_triggered();
	void on_actionMoveUp_triggered();
	void on_actionMoveDown_triggered();
	void on_txtName_editingFinished();
	void on_btnColor_clicked();
};

#endif // WgtTileSetTerrain_H
