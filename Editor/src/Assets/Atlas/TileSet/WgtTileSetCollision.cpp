/**************************************************************************
 *	WgtTileSetCollision.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "WgtTileSetCollision.h"
#include "ui_WgtTileSetCollision.h"
#include "AuxTileSet.h"
#include "TileData.h"
#include "TileSetUndoCmds.h"
#include "DlgCollisionFilter.h"
#include "DlgSurfaceMaterials.h"

#include <QPushButton>
#include <QColorDialog>

WgtTileSetCollision::WgtTileSetCollision(AuxTileSet *pAuxTileSet, QJsonObject initObj, QWidget *pParent /*= nullptr*/) :
	IWgtTileSetItem(TILESETWGT_Animation, initObj, pAuxTileSet, pParent),
	ui(new Ui::WgtTileSetCollision),
	m_CachedB2Filter(b2DefaultFilter())
{
	ui->setupUi(this);

	ui->btnDelete->setDefaultAction(ui->actionDelete);
	ui->btnUpward->setDefaultAction(ui->actionUpward);
	ui->btnDownward->setDefaultAction(ui->actionDownward);

	Init(initObj);
}

WgtTileSetCollision::~WgtTileSetCollision()
{
	delete ui;
}

/*virtual*/ void WgtTileSetCollision::OnInit(QJsonObject serializedObj) /*override*/
{
	QJsonObject filterObj = serializedObj["filter"].toObject();

	m_CachedB2Filter.categoryBits = static_cast<uint64_t>(filterObj["categoryBits"].toVariant().toLongLong());
	m_CachedB2Filter.maskBits = static_cast<uint64_t>(filterObj["maskBits"].toVariant().toLongLong());
	m_CachedB2Filter.groupIndex = static_cast<int32>(filterObj["groupIndex"].toInt());

	QString sFilterText = "Category: " + QString::number(m_CachedB2Filter.categoryBits) +
						  " | Mask: " + QString::number(m_CachedB2Filter.maskBits) +
						  " | Group: " + QString::number(m_CachedB2Filter.groupIndex);
	ui->txtFilter->setText(sFilterText);

	m_SurfaceMaterialUuid = QUuid(serializedObj["surfaceMaterialUUID"].toString());
	QString sSurfaceMatName;
	HyColor surfaceMatColor;
	m_pAuxTileSet->GetTileSet()->GetProject().GetSurfaceMaterialInfo(m_SurfaceMaterialUuid, sSurfaceMatName, surfaceMatColor);

	ui->txtMatName->setText(sSurfaceMatName);
	SetButtonColor(ui->btnMatColor, surfaceMatColor);
	
	ui->chkIsSensor->setChecked(serializedObj["isSensor"].toBool());
}

/*virtual*/ QJsonObject WgtTileSetCollision::SerializeCurrentWidgets() /*override*/
{
	QJsonObject serializedJsonObj;

	serializedJsonObj["UUID"] = m_Uuid.toString(QUuid::WithoutBraces);

	QJsonObject filterObj;
	filterObj["categoryBits"] = static_cast<qint64>(m_CachedB2Filter.categoryBits);
	filterObj["maskBits"] = static_cast<qint64>(m_CachedB2Filter.maskBits);
	filterObj["groupIndex"] = static_cast<int>(m_CachedB2Filter.groupIndex);
	serializedJsonObj["filter"] = filterObj;
	
	serializedJsonObj["surfaceMaterialUUID"] = m_SurfaceMaterialUuid.toString(QUuid::WithoutBraces);

	serializedJsonObj["isSensor"] = ui->chkIsSensor->isChecked();

	return serializedJsonObj;
}

void WgtTileSetCollision::SetOrderBtns(bool bUpEnabled, bool bDownEnabled)
{
	ui->actionUpward->setEnabled(bUpEnabled);
	ui->actionDownward->setEnabled(bDownEnabled);
}

/*virtual*/ QFrame *WgtTileSetCollision::GetBorderFrame() const /*override*/
{
	return ui->frmBorder;
}

void WgtTileSetCollision::on_actionDelete_triggered()
{
	TileSetUndoCmd_RemoveWgtItem *pNewCmd = new TileSetUndoCmd_RemoveWgtItem(*m_pAuxTileSet, m_Uuid);
	m_pAuxTileSet->GetTileSet()->GetUndoStack()->push(pNewCmd);
}

void WgtTileSetCollision::on_actionUpward_triggered()
{
	int iCurrentIndex = m_pAuxTileSet->GetWgtItemIndex(m_Uuid);
	TileSetUndoCmd_OrderWgtItem *pNewCmd = new TileSetUndoCmd_OrderWgtItem(*m_pAuxTileSet, m_Uuid, iCurrentIndex, iCurrentIndex - 1);
	m_pAuxTileSet->GetTileSet()->GetUndoStack()->push(pNewCmd);
}

void WgtTileSetCollision::on_actionDownward_triggered()
{
	int iCurrentIndex = m_pAuxTileSet->GetWgtItemIndex(m_Uuid);
	TileSetUndoCmd_OrderWgtItem *pNewCmd = new TileSetUndoCmd_OrderWgtItem(*m_pAuxTileSet, m_Uuid, iCurrentIndex, iCurrentIndex + 1);
	m_pAuxTileSet->GetTileSet()->GetUndoStack()->push(pNewCmd);
}

void WgtTileSetCollision::on_txtName_editingFinished()
{
	
}

void WgtTileSetCollision::on_btnSetFilter_clicked()
{
	DlgCollisionFilter dlg(m_pAuxTileSet->GetTileSet()->GetProject(), m_CachedB2Filter);
	if(dlg.exec() == QDialog::Accepted)
	{
		m_CachedB2Filter = dlg.GetB2Filter();
		QString sFilterText = "Category: " + QString::number(m_CachedB2Filter.categoryBits) +
							  " | Mask: " + QString::number(m_CachedB2Filter.maskBits) +
							  " | Group: " + QString::number(m_CachedB2Filter.groupIndex);
		ui->txtFilter->setText(sFilterText);
		OnModifyWidget("Collision Layer Filter", -1);
	}
}

void WgtTileSetCollision::on_btnSetMat_clicked()
{
	DlgSurfaceMaterials dlg(*m_pAuxTileSet->GetTileSet()->GetProject().GetSurfaceMaterialsModel());
	if(dlg.exec() == QDialog::Accepted)
	{
		m_SurfaceMaterialUuid = dlg.GetSelectedMaterialUuid();
		QString sSurfaceMatName;
		HyColor surfaceMatColor;
		m_pAuxTileSet->GetTileSet()->GetProject().GetSurfaceMaterialInfo(m_SurfaceMaterialUuid, sSurfaceMatName, surfaceMatColor);
		ui->txtMatName->setText(sSurfaceMatName);
		SetButtonColor(ui->btnMatColor, surfaceMatColor);
		OnModifyWidget("Collision Layer Surface Material", -1);
	}
}

void WgtTileSetCollision::on_chkIsSensor_toggled(bool bChecked)
{
	OnModifyWidget("Collision Layer Is Sensor", -1);
}
