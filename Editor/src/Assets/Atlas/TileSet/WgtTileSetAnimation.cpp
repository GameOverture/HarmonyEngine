/**************************************************************************
 *	WgtTileSetAnimation.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "WgtTileSetAnimation.h"
#include "ui_WgtTileSetAnimation.h"
#include "AuxTileSet.h"
#include "TileData.h"
#include "TileSetUndoCmds.h"

#include <QPushButton>
#include <QColorDialog>

WgtTileSetAnimation::WgtTileSetAnimation(AuxTileSet *pAuxTileSet, QJsonObject initObj, QWidget *pParent /*= nullptr*/) :
	IWgtTileSetItem(TILESETWGT_Animation, initObj, pAuxTileSet, pParent),
	ui(new Ui::WgtTileSetAnimation),
	m_bPaintingTiles(false)
{
	ui->setupUi(this);

	ui->btnDelete->setDefaultAction(ui->actionDelete);
	ui->btnUpward->setDefaultAction(ui->actionUpward);
	ui->btnDownward->setDefaultAction(ui->actionDownward);

	Init(initObj);
}

WgtTileSetAnimation::~WgtTileSetAnimation()
{
	delete ui;
}

/*virtual*/ void WgtTileSetAnimation::OnInit(QJsonObject serializedObj) /*override*/
{
	ui->txtName->setText(serializedObj["name"].toString());
	SetButtonColor(ui->btnColor, HyColor(serializedObj["color"].toVariant().toLongLong()));
	QJsonArray framesArray = serializedObj["frames"].toArray();
	for(QJsonValue frameVal : framesArray)
	{
		QString sTileDataUuid = frameVal.toString();
		TileData *pTileData = m_pAuxTileSet->GetTileSet()->FindTileData(QUuid(sTileDataUuid));
		if(pTileData)
			m_FrameList.append(pTileData);
		else
			HyGuiLog("WgtTileSetAnimation::OnInit: Could not find TileData for frame UUID " + sTileDataUuid, LOGTYPE_Error);
	}
	
	ui->sbFrameRate->setValue(static_cast<float>(serializedObj["frameDuration"].toDouble(0.0333)));
	ui->chkStartRandom->setChecked(serializedObj["startAtRandomFrame"].toBool());

	ui->lblNumFrames->setEnabled(m_FrameList.size() != 0);
	ui->lblNumFrames->setText("Num Frames: " + QString::number(m_FrameList.size()));

	m_SerializedJsonObj = serializedObj;
}

/*virtual*/ QJsonObject WgtTileSetAnimation::SerializeCurrentWidgets() /*override*/
{
	m_SerializedJsonObj["UUID"] = m_Uuid.toString(QUuid::WithoutBraces);
	m_SerializedJsonObj["name"] = ui->txtName->text();
	HyColor btnColor(ui->btnColor->palette().button().color().red(),
					 ui->btnColor->palette().button().color().green(),
					 ui->btnColor->palette().button().color().blue());
	m_SerializedJsonObj["color"] = static_cast<qint64>(btnColor.GetAsHexCode());
	QJsonArray framesArray;
	for(TileData *pFrameTileData : m_FrameList)
		framesArray.append(pFrameTileData->GetUuid().toString(QUuid::WithoutBraces));
	m_SerializedJsonObj["frames"] = framesArray;
	m_SerializedJsonObj["frameDuration"] = ui->sbFrameRate->value();
	m_SerializedJsonObj["startAtRandomFrame"] = ui->chkStartRandom->isChecked();

	return m_SerializedJsonObj;
}

void WgtTileSetAnimation::SetOrderBtns(bool bUpEnabled, bool bDownEnabled)
{
	ui->actionUpward->setEnabled(bUpEnabled);
	ui->actionDownward->setEnabled(bDownEnabled);
}

QString WgtTileSetAnimation::GetName() const
{
	return m_SerializedJsonObj["name"].toString();
}

bool WgtTileSetAnimation::IsPaintingTiles() const
{
	return m_bPaintingTiles;
}

/*virtual*/ QFrame *WgtTileSetAnimation::GetBorderFrame() const /*override*/
{
	return ui->frmBorder;
}

void WgtTileSetAnimation::on_actionDelete_triggered()
{
	TileSetUndoCmd_RemoveWgtItem *pNewCmd = new TileSetUndoCmd_RemoveWgtItem(*m_pAuxTileSet, m_Uuid);
	m_pAuxTileSet->GetTileSet()->GetUndoStack()->push(pNewCmd);
}

void WgtTileSetAnimation::on_actionUpward_triggered()
{
	int iCurrentIndex = m_pAuxTileSet->GetWgtItemIndex(m_Uuid);
	TileSetUndoCmd_OrderWgtItem *pNewCmd = new TileSetUndoCmd_OrderWgtItem(*m_pAuxTileSet, m_Uuid, iCurrentIndex, iCurrentIndex - 1);
	m_pAuxTileSet->GetTileSet()->GetUndoStack()->push(pNewCmd);
}

void WgtTileSetAnimation::on_actionDownward_triggered()
{
	int iCurrentIndex = m_pAuxTileSet->GetWgtItemIndex(m_Uuid);
	TileSetUndoCmd_OrderWgtItem *pNewCmd = new TileSetUndoCmd_OrderWgtItem(*m_pAuxTileSet, m_Uuid, iCurrentIndex, iCurrentIndex + 1);
	m_pAuxTileSet->GetTileSet()->GetUndoStack()->push(pNewCmd);
}

void WgtTileSetAnimation::on_txtName_editingFinished()
{
	OnModifyWidget("Animation Name", -1);
}

void WgtTileSetAnimation::on_btnColor_clicked()
{
	QColorDialog colorDlg;
	colorDlg.setCurrentColor(ui->btnColor->palette().button().color());
	if (colorDlg.exec() == QDialog::Accepted)
	{
		QColor newColor = colorDlg.selectedColor();
		ui->btnColor->setPalette(QPalette(newColor));
		OnModifyWidget("Animation Color", -1);
	}
}

void  WgtTileSetAnimation::on_btnFramePreview_clicked()
{
	if(m_bPaintingTiles == false)
	{
		ui->btnFramePreview->setText("Select Tiles...");
		m_bPaintingTiles = true;

		m_pAuxTileSet->SetPainting_Animation(m_Uuid);
	}
	else
	{
		QMap<TileData *, TileSetGfxItem *> selectedTilesMap = m_pAuxTileSet->GetTileSet()->GetGfxScene()->GetSelectedSetupTiles();


		//start_here;

		for(TileData *pTileData : selectedTilesMap.keys())
		{

		}

		m_bPaintingTiles = false;
	}
}

void WgtTileSetAnimation::on_sbColumns_valueChanged(int iNewValue)
{
	OnModifyWidget("Animation Columns", MERGABLEUNDOCMD_TileSetAnimColumn);
}

void WgtTileSetAnimation::on_sbNumFrames_valueChanged(int iNewValue)
{
	OnModifyWidget("Animation Frames", MERGABLEUNDOCMD_TileSetAnimNumFrames);
}

void WgtTileSetAnimation::on_btnHz10_clicked()
{
	ui->sbFrameRate->setValue(1.0 / 10.0);
}

void WgtTileSetAnimation::on_btnHz20_clicked()
{
	ui->sbFrameRate->setValue(1.0 / 20.0);
}

void WgtTileSetAnimation::on_btnHz30_clicked()
{
	ui->sbFrameRate->setValue(1.0 / 30.0);
}

void WgtTileSetAnimation::on_btnHz40_clicked()
{
	ui->sbFrameRate->setValue(1.0 / 40.0);
}

void WgtTileSetAnimation::on_btnHz60_clicked()
{
	ui->sbFrameRate->setValue(1.0 / 60.0);
}

void WgtTileSetAnimation::on_sbFrameRate_valueChanged(double dNewValue)
{
	OnModifyWidget("Animation Frame Rate", MERGABLEUNDOCMD_TileSetAnimFrameRate);
}

void WgtTileSetAnimation::on_chkStartRandom_toggled(bool bChecked)
{
	OnModifyWidget("Animation Start At Random", -1);
}
