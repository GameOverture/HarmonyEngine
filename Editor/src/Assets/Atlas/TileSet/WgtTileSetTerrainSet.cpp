/**************************************************************************
 *	WgtTileSetTerrainSet.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "WgtTileSetTerrainSet.h"
#include "ui_WgtTileSetTerrainSet.h"
#include "AuxTileSet.h"
#include "TileSetUndoCmds.h"
#include "WgtTileSetTerrain.h"

#include <QMouseEvent>
#include <QPushButton>

WgtTileSetTerrainSet::WgtTileSetTerrainSet(AuxTileSet *pAuxTileSet, QJsonObject initDataObj, QWidget *pParent /*= nullptr*/) :
	IWgtTileSetItem(TILESETWGT_TerrainSet, initDataObj, pAuxTileSet, pParent),
	ui(new Ui::WgtTileSetTerrainSet)
{
	ui->setupUi(this);

	ui->btnDelete->setDefaultAction(ui->actionDelete);
	ui->btnUpward->setDefaultAction(ui->actionUpward);
	ui->btnDownward->setDefaultAction(ui->actionDownward);

	Init(initDataObj);
}

WgtTileSetTerrainSet::~WgtTileSetTerrainSet()
{
	delete ui;
}

/*virtual*/ void WgtTileSetTerrainSet::OnInit(QJsonObject serializedObj) /*override*/
{
	// Delete any existing terrains first
	for (WgtTileSetTerrain *pTerrain : m_TerrainList)
	{
		ui->lytTerrains->removeWidget(pTerrain);
		delete pTerrain;
	}
	m_TerrainList.clear();

	ui->cmbTerrainSetMode->setCurrentIndex(serializedObj["terrainSetMode"].toInt());
	QJsonArray terrainsArray = serializedObj["terrains"].toArray();
	for (const QJsonValue &terrainVal : terrainsArray)
	{
		QJsonObject terrainObj = terrainVal.toObject();
		CmdSet_AllocTerrain(terrainObj);
	}
}

/*virtual*/ QJsonObject WgtTileSetTerrainSet::SerializeCurrentWidgets() /*override*/
{
	QJsonObject serializedJsonObj;

	serializedJsonObj["UUID"] = m_Uuid.toString();
	serializedJsonObj["terrainSetMode"] = ui->cmbTerrainSetMode->currentIndex();
	QJsonArray terrainsArray;
	for (WgtTileSetTerrain *pTerrain : m_TerrainList)
		terrainsArray.append(pTerrain->SerializeCurrentWidgets());
	serializedJsonObj["terrains"] = terrainsArray;

	return serializedJsonObj;
}

void WgtTileSetTerrainSet::CmdSet_AllocTerrain(QJsonObject initDataObj)
{
	if(initDataObj.contains("UUID") == false)
	{
		HyGuiLog("Error: WgtTileSetTerrainSet::CmdSet_AllocTerrain - initDataObj is missing 'UUID' field.", LOGTYPE_Error);
		return;
	}

	WgtTileSetTerrain *pNewTerrain = new WgtTileSetTerrain(m_pAuxTileSet, this, initDataObj);
	ui->lytTerrains->addWidget(pNewTerrain);
	m_TerrainList.append(pNewTerrain);
	m_pAuxTileSet->MakeSelectionChange(pNewTerrain);
}

void WgtTileSetTerrainSet::CmdSet_DeleteTerrain(QUuid uuid)
{
	for (WgtTileSetTerrain *pTerrain : m_TerrainList)
	{
		if (pTerrain->GetUuid() == uuid)
		{
			ui->lytTerrains->removeWidget(pTerrain);
			m_TerrainList.removeOne(pTerrain);
			delete pTerrain;
			break;
		}
	}
}

void WgtTileSetTerrainSet::CmdSet_OrderTerrain(QUuid uuid, int iNewIndex)
{
	for (int i = 0; i < m_TerrainList.size(); ++i)
	{
		WgtTileSetTerrain *pTerrain = m_TerrainList[i];
		if (pTerrain->GetUuid() == uuid)
		{
			ui->lytTerrains->removeWidget(pTerrain);
			m_TerrainList.removeAt(i);
			ui->lytTerrains->insertWidget(iNewIndex, pTerrain);
			m_TerrainList.insert(iNewIndex, pTerrain);
			break;
		}
	}
}

QList<WgtTileSetTerrain *> WgtTileSetTerrainSet::GetTerrains() const
{
	return m_TerrainList;
}

void WgtTileSetTerrainSet::SetOrderBtns(bool bUpEnabled, bool bDownEnabled)
{
	ui->btnUpward->setEnabled(bUpEnabled);
	ui->btnDownward->setEnabled(bDownEnabled);
}

/*virtual*/ bool WgtTileSetTerrainSet::eventFilter(QObject *pWatched, QEvent *pEvent) /*override*/
{
	if (pEvent->type() == QEvent::MouseButtonPress)
	{
		QMouseEvent *pMouseEvent = static_cast<QMouseEvent *>(pEvent);
		if (pMouseEvent->button() == Qt::LeftButton || pMouseEvent->button() == Qt::RightButton)
		{
			for (WgtTileSetTerrain *pTerrain : m_TerrainList)
			{
				// Convert mouse pos to terrain widget coords
				QPoint posInTerrain = mapTo(ui->frmBorder, pMouseEvent->pos());
				posInTerrain = ui->frmBorder->mapTo(pTerrain, posInTerrain);
				if(pTerrain->rect().contains(posInTerrain) && pTerrain->IsSelected() == false)
					m_pAuxTileSet->MakeSelectionChange(pTerrain);
			}
		}
	}

	return IWgtTileSetItem::eventFilter(pWatched, pEvent);
}

/*virtual*/ QFrame *WgtTileSetTerrainSet::GetBorderFrame() const /*override*/
{
	return ui->frmBorder;
}

void WgtTileSetTerrainSet::on_actionDelete_triggered()
{
	TileSetUndoCmd_RemoveWgtItem *pNewCmd = new TileSetUndoCmd_RemoveWgtItem(*m_pAuxTileSet, m_Uuid);
	m_pAuxTileSet->GetTileSet()->GetUndoStack()->push(pNewCmd);
}

void WgtTileSetTerrainSet::on_actionUpward_triggered()
{
	int iCurrentIndex = m_pAuxTileSet->GetWgtItemIndex(m_Uuid);
	TileSetUndoCmd_OrderWgtItem *pNewCmd = new TileSetUndoCmd_OrderWgtItem(*m_pAuxTileSet, m_Uuid, iCurrentIndex, iCurrentIndex - 1);
	m_pAuxTileSet->GetTileSet()->GetUndoStack()->push(pNewCmd);
}

void WgtTileSetTerrainSet::on_actionDownward_triggered()
{
	int iCurrentIndex = m_pAuxTileSet->GetWgtItemIndex(m_Uuid);
	TileSetUndoCmd_OrderWgtItem *pNewCmd = new TileSetUndoCmd_OrderWgtItem(*m_pAuxTileSet, m_Uuid, iCurrentIndex, iCurrentIndex + 1);
	m_pAuxTileSet->GetTileSet()->GetUndoStack()->push(pNewCmd);
}

void WgtTileSetTerrainSet::on_cmbTerrainSetMode_currentIndexChanged(int iNewIndex)
{
	OnModifyWidget("Terrain Set Mode Change", -1);
}

void WgtTileSetTerrainSet::on_btnAddTerrain_clicked()
{
	QString sTerrainName = "Terrain ";
	sTerrainName += QString::number(m_TerrainList.count());
	HyColor defaultColor = HyColor::Brown;
	QJsonObject initObj = AtlasTileSet::GenerateNewTerrainJsonObject(m_Uuid, sTerrainName, defaultColor);

	TileSetUndoCmd_AddWgtItem *pNewCmd = new TileSetUndoCmd_AddWgtItem(*m_pAuxTileSet, TILESETWGT_Terrain, initObj);
	m_pAuxTileSet->GetTileSet()->GetUndoStack()->push(pNewCmd);
}
