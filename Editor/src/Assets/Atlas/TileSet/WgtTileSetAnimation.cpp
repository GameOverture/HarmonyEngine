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

#define TILEANIM_UNSET_TEXT "<Not Set>"

WgtTileSetAnimation::WgtTileSetAnimation(AuxTileSet *pAuxTileSet, QJsonObject initObj, QWidget *pParent /*= nullptr*/) :
	IWgtTileSetItem(TILESETWGT_Animation, initObj, pAuxTileSet, pParent),
	ui(new Ui::WgtTileSetAnimation),
	m_pPreviewTimer(nullptr)
{
	ui->setupUi(this);

	ui->btnDelete->setDefaultAction(ui->actionDelete);
	ui->btnUpward->setDefaultAction(ui->actionUpward);
	ui->btnDownward->setDefaultAction(ui->actionDownward);

	ui->txtStartingTile->setText(TILEANIM_UNSET_TEXT);

	ClearError();

	m_pPreviewTimer = new QTimer(this);
	connect(m_pPreviewTimer, SIGNAL(timeout()), this, SLOT(OnPreviewUpdate()));

	Init(initObj);
}

WgtTileSetAnimation::~WgtTileSetAnimation()
{
	m_pPreviewTimer->stop();
	delete m_pPreviewTimer;
	m_pPreviewTimer = nullptr; // This is required to avoid slot calls after destruction

	delete ui;
}

/*virtual*/ void WgtTileSetAnimation::OnInit(QJsonObject serializedObj) /*override*/
{
	ui->txtName->setText(serializedObj["name"].toString());
	SetButtonColor(ui->btnColor, HyColor(serializedObj["color"].toVariant().toLongLong()));

	ui->sbFrameDuration->setValue(serializedObj["frameDuration"].toInt());
	ui->chkGlobalSync->setChecked(serializedObj["globalSync"].toBool());
	ui->chkBounce->setChecked(serializedObj["bounceAnim"].toBool());
	ui->chkReverse->setChecked(serializedObj["reverseAnim"].toBool());
	ui->chkLoop->setChecked(serializedObj["looping"].toBool());
	ui->chkEnabled->setChecked(serializedObj["enabled"].toBool());

	ui->txtStartingTile->setText(serializedObj["startingTile"].toString());
	
	m_FramesChecksumList.clear();
	QJsonArray checksumFramesArray = serializedObj["checksumFrames"].toArray();
	for(const QJsonValue &checksumFrameVal : checksumFramesArray)
		m_FramesChecksumList.append(static_cast<quint32>(checksumFrameVal.toVariant().toLongLong()));

	RefreshPreview();
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
	serializedJsonObj["frameDuration"] = ui->sbFrameDuration->value();
	serializedJsonObj["globalSync"] = ui->chkGlobalSync->isChecked();
	serializedJsonObj["bounceAnim"] = ui->chkBounce->isChecked();
	serializedJsonObj["reverseAnim"] = ui->chkReverse->isChecked();
	serializedJsonObj["looping"] = ui->chkLoop->isChecked();
	serializedJsonObj["enabled"] = ui->chkEnabled->isEnabled();

	serializedJsonObj["startingTile"] = ui->txtStartingTile->text();

	QJsonArray checksumFramesArray;
	for(quint32 uiChecksum : m_FramesChecksumList)
		checksumFramesArray.append(static_cast<qint64>(uiChecksum));
	serializedJsonObj["checksumFrames"] = checksumFramesArray;

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

void WgtTileSetAnimation::GatherFrames()
{
	m_FramesChecksumList.clear();
	if(ui->txtStartingTile->text() == TILEANIM_UNSET_TEXT)
		return;

	QUuid startTileUuid = QUuid(ui->txtStartingTile->text());
	QVector<TileData *> tileDataList = m_pAuxTileSet->GetTileSet()->GetTileDataList();
	TileData *pCurTile = nullptr;
	int iMinGridX = INT_MAX, iMaxGridX = INT_MIN, iMinGridY = INT_MAX, iMaxGridY = INT_MIN;
	for(TileData *pTile : tileDataList)
	{
		if(pCurTile == nullptr && pTile->GetUuid() == startTileUuid)
			pCurTile = pTile;
		
		QPoint ptGridPos = pTile->GetMetaGridPos();
		iMinGridX = HyMath::Min(iMinGridX, ptGridPos.x());
		iMaxGridX = HyMath::Max(iMaxGridX, ptGridPos.x());
		iMinGridY = HyMath::Min(iMinGridY, ptGridPos.y());
		iMaxGridY = HyMath::Max(iMaxGridY, ptGridPos.y());
	}
	if(pCurTile == nullptr)
	{
		HyGuiLog("WgtTileSetAnimation::GatherFrames - Starting tile not found", LOGTYPE_Error);
		ui->txtStartingTile->setText(TILEANIM_UNSET_TEXT);
		return;
	}

	int iNumFrames = ui->sbNumFrames->value();
	int iSeparation = ui->sbSeparation->value();
	QPoint ptCurGridPos = pCurTile->GetMetaGridPos();

	for(int i = 0; i < iNumFrames; ++i)
	{
		m_FramesChecksumList.append(pCurTile->GetTileChecksum());

		for(int j = 0; j < (1 + iSeparation); ++j)
		{
			ptCurGridPos.setX(ptCurGridPos.x() + 1);
			if(ptCurGridPos.x() > iMaxGridX)
			{
				ptCurGridPos.setX(iMinGridX);
				ptCurGridPos.setY(ptCurGridPos.y() + 1);

				if(ptCurGridPos.y() > iMaxGridY)
				{
					SetError("Animation frames out of bounds");
					break;
				}
			}
		}
		if(IsError())
		{
			m_FramesChecksumList.clear();
			break;
		}
		
		for(TileData *pTile : tileDataList)
		{
			if(pTile->GetMetaGridPos() == ptCurGridPos)
			{
				pCurTile = pTile;
				break;
			}
		}
	}
}

void WgtTileSetAnimation::RefreshPreview()
{
	m_PreviewFrameList.clear();
	m_pPreviewTimer->stop();
	for(quint32 uiChecksum : m_FramesChecksumList)
		m_PreviewFrameList.append(m_pAuxTileSet->GetTileSet()->GetTilePixmap(uiChecksum));
	
	if(m_PreviewFrameList.empty() == false)
	{
		m_pPreviewTimer->setInterval(ui->sbFrameDuration->value());
		m_pPreviewTimer->start();

		m_iPreviewFrameIndex = 0;
	}
}

bool WgtTileSetAnimation::IsError()
{
	return ui->lblError->isVisible();
}

void WgtTileSetAnimation::SetError(QString sMessage)
{
	ui->lblError->setStyleSheet("QLabel { background-color : red; color : black; }");
	ui->lblError->setText(sMessage);
	ui->lblError->setVisible(true);
}

void WgtTileSetAnimation::ClearError()
{
	ui->lblError->setStyleSheet("QLabel { color : black; }");
	ui->lblError->setText("");
	ui->lblError->setVisible(false);
}

void WgtTileSetAnimation::OnPreviewUpdate()
{
	if(m_pPreviewTimer == nullptr) // This indicates destructor was called
		return;

	ui->lblFramePreview->setPixmap(m_PreviewFrameList[m_iPreviewFrameIndex]);
	
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

void WgtTileSetAnimation::on_sbNumFrames_valueChanged(int iNewValue)
{
	OnModifyWidget("Animation Frame Duration", MERGABLEUNDOCMD_TileSetAnimNumFrames);

	GatherFrames();
	RefreshPreview();
}

void WgtTileSetAnimation::on_sbSeparation_valueChanged(int iNewValue)
{
	OnModifyWidget("Animation Frame Separation", MERGABLEUNDOCMD_TileSetAnimSeparation);

	GatherFrames();
	RefreshPreview();
}

void WgtTileSetAnimation::on_btnHz1_clicked()
{
	ui->sbFrameDuration->setValue(1000);
}

void WgtTileSetAnimation::on_btnHz10_clicked()
{
	ui->sbFrameDuration->setValue(100);
}

void WgtTileSetAnimation::on_btnHz20_clicked()
{
	ui->sbFrameDuration->setValue(50);
}

void WgtTileSetAnimation::on_btnHz30_clicked()
{
	ui->sbFrameDuration->setValue(33);
}

void WgtTileSetAnimation::on_btnHz40_clicked()
{
	ui->sbFrameDuration->setValue(25);
}

void WgtTileSetAnimation::on_btnHz50_clicked()
{
	ui->sbFrameDuration->setValue(20);
}

void WgtTileSetAnimation::on_btnHz60_clicked()
{
	ui->sbFrameDuration->setValue(16);
}

void WgtTileSetAnimation::on_sbFrameDuration_valueChanged(int iNewValue)
{
	OnModifyWidget("Animation Frame Duration", MERGABLEUNDOCMD_TileSetAnimFrameRate);
	RefreshPreview();
}

void WgtTileSetAnimation::on_chkEnabled_toggled(bool bChecked)
{
	OnModifyWidget("Animation Enabled", -1);
	RefreshPreview();
}

void WgtTileSetAnimation::on_chkGlobalSync_toggled(bool bChecked)
{
	OnModifyWidget("Animation Global Sync", -1);
	RefreshPreview();
}

void WgtTileSetAnimation::on_chkLoop_toggled(bool bChecked)
{
	OnModifyWidget("Animation Loop", -1);
	RefreshPreview();
}

void WgtTileSetAnimation::on_chkReverse_toggled(bool bChecked)
{
	OnModifyWidget("Animation Reverse", -1);
	RefreshPreview();
}

void WgtTileSetAnimation::on_chkBounce_toggled(bool bChecked)
{
	OnModifyWidget("Animation Bounce", -1);
	RefreshPreview();
}
