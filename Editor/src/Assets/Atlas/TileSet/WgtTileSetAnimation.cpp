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
	m_pPreviewTimer(nullptr)
{
	ui->setupUi(this);

	ui->btnDelete->setDefaultAction(ui->actionDelete);
	ui->btnUpward->setDefaultAction(ui->actionUpward);
	ui->btnDownward->setDefaultAction(ui->actionDownward);

	m_pPreviewTimer = new QTimer(this);
	connect(m_pPreviewTimer, SIGNAL(timeout()), this, SLOT(OnPreviewUpdate()));

	Init(initObj);
}

WgtTileSetAnimation::~WgtTileSetAnimation()
{
	m_pPreviewTimer->stop();
	delete m_pPreviewTimer;

	delete ui;
}

/*virtual*/ void WgtTileSetAnimation::OnInit(QJsonObject serializedObj) /*override*/
{
	ui->txtName->setText(serializedObj["name"].toString());
	SetButtonColor(ui->btnColor, HyColor(serializedObj["color"].toVariant().toLongLong()));
	
	ui->sbFrameRate->setValue(static_cast<float>(serializedObj["frameDuration"].toDouble(0.0333)));
	ui->chkStartRandom->setChecked(serializedObj["startAtRandomFrame"].toBool());

	RefreshPreview();

	ui->lblNumFrames->setEnabled(m_PreviewFrameList.size() != 0);
	ui->lblNumFrames->setText("Num Frames: " + QString::number(m_PreviewFrameList.size()));
}

/*virtual*/ QJsonObject WgtTileSetAnimation::SerializeCurrentWidgets() /*override*/
{
	QJsonObject serializedJsonObj;

	serializedJsonObj["UUID"] = m_Uuid.toString(QUuid::WithoutBraces);
	serializedJsonObj["name"] = ui->txtName->text();
	HyColor btnColor(ui->btnColor->palette().button().color().red(),
					 ui->btnColor->palette().button().color().green(),
					 ui->btnColor->palette().button().color().blue());
	serializedJsonObj["color"] = static_cast<qint64>(btnColor.GetAsHexCode());
	serializedJsonObj["frameDuration"] = ui->sbFrameRate->value();
	serializedJsonObj["startAtRandomFrame"] = ui->chkStartRandom->isChecked();

	return serializedJsonObj;
}

void WgtTileSetAnimation::SetOrderBtns(bool bUpEnabled, bool bDownEnabled)
{
	ui->actionUpward->setEnabled(bUpEnabled);
	ui->actionDownward->setEnabled(bDownEnabled);
}

QString WgtTileSetAnimation::GetName() const
{
	return ui->txtName->text();
}

/*virtual*/ QFrame *WgtTileSetAnimation::GetBorderFrame() const /*override*/
{
	return ui->frmBorder;
}

void WgtTileSetAnimation::RefreshPreview()
{
	m_PreviewFrameList.clear();
	m_pPreviewTimer->stop();
	QVector<TileData *> tileDataList = m_pAuxTileSet->GetTileSet()->GetTileDataList();
	for(TileData *pTileData : tileDataList)
	{
		if(pTileData->GetAnimation() == m_Uuid)
			m_PreviewFrameList.append(pTileData);
	}
	if(m_PreviewFrameList.empty() == false)
	{
		m_pPreviewTimer->setInterval(static_cast<int>(ui->sbFrameRate->value() * 1000));
		m_pPreviewTimer->start();

		m_iPreviewFrameIndex = 0;
	}
}

void WgtTileSetAnimation::OnPreviewUpdate()
{
	ui->lblFramePreview->setPixmap(m_PreviewFrameList[m_iPreviewFrameIndex]->GetPixmap());
	
	m_iPreviewFrameIndex++;
	if(m_iPreviewFrameIndex >= m_PreviewFrameList.size())
		m_iPreviewFrameIndex = 0;
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
