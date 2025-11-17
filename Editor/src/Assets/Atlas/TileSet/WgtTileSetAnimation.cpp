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
#include "TileSetUndoCmds.h"

#include <QPushButton>
#include <QColorDialog>

WgtTileSetAnimation::WgtTileSetAnimation(AuxTileSet *pAuxTileSet, QJsonObject initObj, QWidget *pParent /*= nullptr*/) :
	IWgtTileSetItem(TILESETWGT_Animation, initObj, pAuxTileSet, pParent),
	ui(new Ui::WgtTileSetAnimation)
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
	HyColor color(serializedObj["color"].toVariant().toLongLong());
	ui->btnColor->setPalette(QPalette(QColor(color.GetRed(), color.GetGreen(), color.GetBlue())));
	ui->sbColumns->setValue(serializedObj["numColumns"].toInt());
	ui->sbNumFrames->setValue(serializedObj["numFrames"].toInt());
	ui->sbFrameRate->setValue(1.0f / static_cast<float>(serializedObj["frameDuration"].toDouble(1.0)));
	ui->chkStartRandom->setChecked(serializedObj["startAtRandomFrame"].toBool());

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
	m_SerializedJsonObj["numColumns"] = ui->sbColumns->value();
	m_SerializedJsonObj["numFrames"] = ui->sbNumFrames->value();
	m_SerializedJsonObj["frameDuration"] = 1.0f / ui->sbFrameRate->value();
	m_SerializedJsonObj["startAtRandomFrame"] = ui->chkStartRandom->isChecked();

	return m_SerializedJsonObj;
}

void WgtTileSetAnimation::SetOrderBtns(bool bUpEnabled, bool bDownEnabled)
{
	ui->actionUpward->setEnabled(bUpEnabled);
	ui->actionDownward->setEnabled(bDownEnabled);
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

void WgtTileSetAnimation::on_txtName_textEdited(const QString &newText)
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

void WgtTileSetAnimation::on_sbColumns_valueChanged(int iNewValue)
{
	OnModifyWidget("Animation Columns", MERGABLEUNDOCMD_TileSetAnimColumn);
}

void WgtTileSetAnimation::on_sbNumFrames_valueChanged(int iNewValue)
{
	OnModifyWidget("Animation Frames", MERGABLEUNDOCMD_TileSetAnimNumFrames);
}

void WgtTileSetAnimation::on_sbFrameRate_valueChanged(double dNewValue)
{
	OnModifyWidget("Animation Frame Rate", MERGABLEUNDOCMD_TileSetAnimFrameRate);
}

void WgtTileSetAnimation::on_chkStartRandom_toggled(bool bChecked)
{
	OnModifyWidget("Animation Start At Random", -1);
}
