/**************************************************************************
 *	WgtTileSetTerrain.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "WgtTileSetTerrain.h"
#include "ui_WgtTileSetTerrain.h"
#include "TileSetUndoCmds.h"
#include "WgtTileSetTerrainSet.h"

WgtTileSetTerrain::WgtTileSetTerrain(AuxTileSet *pAuxTileSet, WgtTileSetTerrainSet *pParentTerrainSet, QJsonObject initDataObj, QWidget *pParent /*= nullptr*/) :
	IWgtTileSetItem(TILESETWGT_Terrain, initDataObj, pAuxTileSet, pParent),
	m_pParentTerrainSet(pParentTerrainSet),
	ui(new Ui::WgtTileSetTerrain)
{
	ui->setupUi(this);

	ui->btnUpward->setDefaultAction(ui->actionUpward);
	ui->btnDownward->setDefaultAction(ui->actionDownward);
	ui->btnDelete->setDefaultAction(ui->actionDelete);

	Init(initDataObj);
}

/*virtual*/ WgtTileSetTerrain::~WgtTileSetTerrain()
{
	delete ui;
}

WgtTileSetTerrainSet *WgtTileSetTerrain::GetParentTerrainSet()
{
	return m_pParentTerrainSet;
}

/*virtual*/ void WgtTileSetTerrain::OnInit(QJsonObject serializedObj) /*override*/
{
	ui->txtName->setText(serializedObj["name"].toString());
	SetButtonColor(ui->btnColor, HyColor(serializedObj["color"].toVariant().toLongLong()));

	m_SerializedJsonObj = serializedObj;
}

/*virtual*/ QJsonObject WgtTileSetTerrain::SerializeCurrentWidgets() /*override*/
{
	m_SerializedJsonObj["UUID"] = m_Uuid.toString();
	m_SerializedJsonObj["terrainSetUUID"] = m_pParentTerrainSet->GetUuid().toString();
	m_SerializedJsonObj["name"] = ui->txtName->text();
	HyColor btnColor(ui->btnColor->palette().button().color().red(),
					 ui->btnColor->palette().button().color().green(),
					 ui->btnColor->palette().button().color().blue());
	m_SerializedJsonObj["color"] = static_cast<qint64>(btnColor.GetAsHexCode());

	return m_SerializedJsonObj;
}

void WgtTileSetTerrain::SetOrderBtns(bool bUpEnabled, bool bDownEnabled)
{
	ui->btnUpward->setEnabled(bUpEnabled);
	ui->btnDownward->setEnabled(bDownEnabled);
}

/*virtual*/ QFrame *WgtTileSetTerrain::GetBorderFrame() const /*override*/
{
	return ui->frmBorder;
}

void WgtTileSetTerrain::on_actionDelete_triggered()
{
	TileSetUndoCmd_RemoveWgtItem *pNewCmd = new TileSetUndoCmd_RemoveWgtItem(*m_pAuxTileSet, m_Uuid);
	m_pAuxTileSet->GetTileSet()->GetUndoStack()->push(pNewCmd);
}

void WgtTileSetTerrain::on_actionMoveUp_triggered()
{
	int iCurrentIndex = m_pAuxTileSet->GetWgtItemIndex(m_Uuid);
	TileSetUndoCmd_OrderWgtItem *pNewCmd = new TileSetUndoCmd_OrderWgtItem(*m_pAuxTileSet, m_Uuid, iCurrentIndex, iCurrentIndex - 1);
	m_pAuxTileSet->GetTileSet()->GetUndoStack()->push(pNewCmd);
}

void WgtTileSetTerrain::on_actionMoveDown_triggered()
{
	int iCurrentIndex = m_pAuxTileSet->GetWgtItemIndex(m_Uuid);
	TileSetUndoCmd_OrderWgtItem *pNewCmd = new TileSetUndoCmd_OrderWgtItem(*m_pAuxTileSet, m_Uuid, iCurrentIndex, iCurrentIndex + 1);
	m_pAuxTileSet->GetTileSet()->GetUndoStack()->push(pNewCmd);
}

void WgtTileSetTerrain::on_txtName_editingFinished()
{
	OnModifyWidget("Terrain Name", -1);
}

void WgtTileSetTerrain::on_btnColor_clicked()
{
	QColor initialColor = ui->btnColor->palette().button().color();
	QColor selectedColor = QColorDialog::getColor(initialColor, this, tr("Select Terrain Color"));
	if (selectedColor.isValid() && selectedColor != initialColor)
	{
		ui->btnColor->setPalette(QPalette(selectedColor));
		OnModifyWidget("Terrain Color", -1);
	}
}
