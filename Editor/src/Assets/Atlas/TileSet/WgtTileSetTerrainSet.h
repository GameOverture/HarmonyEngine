/**************************************************************************
 *	WgtTileSetTerrainSet.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WgtTileSetTerrainSet_H
#define WgtTileSetTerrainSet_H

#include "Global.h"
#include "IWgtTileSetItem.h"

#include <QLineEdit>
#include <QLabel>
#include <QFrame>
#include <QToolButton>
#include <QPushButton>
#include <QColorDialog>

namespace Ui {
class WgtTileSetTerrainSet;
}

class WgtTileSetTerrain;

class WgtTileSetTerrainSet : public IWgtTileSetItem
{
	Q_OBJECT

	Ui::WgtTileSetTerrainSet *		ui;

	QList<WgtTileSetTerrain *>		m_TerrainList;

public:
	WgtTileSetTerrainSet(AuxTileSet *pAuxTileSet, QJsonObject initDataObj, QWidget* pParent = nullptr);
	virtual ~WgtTileSetTerrainSet();

	virtual void OnInit(QJsonObject serializedObj) override;
	virtual QJsonObject SerializeCurrentWidgets() override;

	void CmdSet_AllocTerrain(QJsonObject initDataObj);
	void CmdSet_DeleteTerrain(QUuid uuid);
	void CmdSet_OrderTerrain(QUuid uuid, int iNewIndex);

	QList<WgtTileSetTerrain *> GetTerrains() const;

	void SetOrderBtns(bool bUpEnabled, bool bDownEnabled);

protected:
	virtual bool eventFilter(QObject *pWatched, QEvent *pEvent) override;
	virtual QFrame *GetBorderFrame() const override;


private Q_SLOTS:
	void on_actionDelete_triggered();
	void on_actionUpward_triggered();
	void on_actionDownward_triggered();
	void on_cmbTerrainSetMode_currentIndexChanged(int iNewIndex);
	void on_btnAddTerrain_clicked();
};

#endif // WgtTileSetTerrainSet_H
