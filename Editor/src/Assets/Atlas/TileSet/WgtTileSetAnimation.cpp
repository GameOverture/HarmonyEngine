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

//#define TILEANIM_UNSET_TEXT "<Not Set>"

WgtTileSetAnimation::WgtTileSetAnimation(AuxTileSet *pAuxTileSet, QJsonObject initObj, QWidget *pParent /*= nullptr*/) :
	IWgtTileSetItem(TILESETWGT_Animation, initObj, pAuxTileSet, pParent),
	ui(new Ui::WgtTileSetAnimation),
	m_pPreviewTimer(nullptr)
{
	ui->setupUi(this);

	ui->btnDelete->setDefaultAction(ui->actionDelete);
	ui->btnUpward->setDefaultAction(ui->actionUpward);
	ui->btnDownward->setDefaultAction(ui->actionDownward);

	ui->btnAddFrame->setDefaultAction(ui->actionAddFrame);
	ui->btnRemoveFrame->setDefaultAction(ui->actionRemoveFrame);

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

	ui->sbWidth->setValue(serializedObj["width"].toInt());
	ui->sbHeight->setValue(serializedObj["height"].toInt());
	ui->sbFrameDuration->setValue(serializedObj["frameDuration"].toInt());
	ui->chkGlobalSync->setChecked(serializedObj["globalSync"].toBool());
	ui->chkBounce->setChecked(serializedObj["bounceAnim"].toBool());
	ui->chkReverse->setChecked(serializedObj["reverseAnim"].toBool());
	ui->chkLoop->setChecked(serializedObj["looping"].toBool());
	ui->chkEnabled->setChecked(serializedObj["enabled"].toBool());

	m_TileAnimList.clear();
	QJsonArray tileAnimsArray = serializedObj["tileAnims"].toArray();
	for(QJsonValue tileAnimVal : tileAnimsArray)
	{
		QJsonObject animObj = tileAnimVal.toObject();
		m_TileAnimList.push_back(TileAnimation(animObj));
	}

	int iMaxFrames = 0;
	for(TileAnimation &tileAnim : m_TileAnimList)
	{
		if(iMaxFrames < tileAnim.m_TileChecksumList.size())
			iMaxFrames = tileAnim.m_TileChecksumList.size();
	}
	for(int i = ui->framesList->count(); i < iMaxFrames; ++i)
		ui->framesList->addItem(new QListWidgetItem("Frame " % QString::number(i)));

	ErrorCheckFrames();
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

	serializedJsonObj["width"] = ui->sbWidth->value();
	serializedJsonObj["height"] = ui->sbHeight->value();

	QJsonArray tileAnimsArray;
	for(TileAnimation &tileAnim : m_TileAnimList)
		tileAnimsArray.append(tileAnim.ToJsonObject());
	serializedJsonObj["tileAnims"] = tileAnimsArray;

	serializedJsonObj["frameDuration"] = ui->sbFrameDuration->value();
	serializedJsonObj["globalSync"] = ui->chkGlobalSync->isChecked();
	serializedJsonObj["bounceAnim"] = ui->chkBounce->isChecked();
	serializedJsonObj["reverseAnim"] = ui->chkReverse->isChecked();
	serializedJsonObj["looping"] = ui->chkLoop->isChecked();
	serializedJsonObj["enabled"] = ui->chkEnabled->isEnabled();

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

int WgtTileSetAnimation::GetSelectedFrameIndex() const
{
	return ui->framesList->currentRow();
}

void WgtTileSetAnimation::SetSelectedFrameIndex(int iFrameIndex)
{
	ui->framesList->setCurrentRow(iFrameIndex);
}

/*virtual*/ QFrame *WgtTileSetAnimation::GetBorderFrame() const /*override*/
{
	return ui->frmBorder;
}

void WgtTileSetAnimation::ErrorCheckFrames()
{
	int iNumDesiredFrames = ui->framesList->count();
	int iRequiredNumTileAnims = ui->sbWidth->value() * ui->sbHeight->value();

	for(int iFrameIndex = 0; iFrameIndex < iNumDesiredFrames; ++iFrameIndex)
	{
		m_TileAnimList.size();
	}
	

	//m_FramesChecksumList.clear();
	//if(ui->txtStartingTile->text() == TILEANIM_UNSET_TEXT)
	//	return;

	//QUuid startTileUuid = QUuid(ui->txtStartingTile->text());
	//QVector<TileData *> tileDataList = m_pAuxTileSet->GetTileSet()->GetTileDataList();
	//TileData *pCurTile = nullptr;
	//int iMinGridX = INT_MAX, iMaxGridX = INT_MIN, iMinGridY = INT_MAX, iMaxGridY = INT_MIN;
	//for(TileData *pTile : tileDataList)
	//{
	//	if(pCurTile == nullptr && pTile->GetUuid() == startTileUuid)
	//		pCurTile = pTile;
	//	
	//	QPoint ptGridPos = pTile->GetMetaGridPos();
	//	iMinGridX = HyMath::Min(iMinGridX, ptGridPos.x());
	//	iMaxGridX = HyMath::Max(iMaxGridX, ptGridPos.x());
	//	iMinGridY = HyMath::Min(iMinGridY, ptGridPos.y());
	//	iMaxGridY = HyMath::Max(iMaxGridY, ptGridPos.y());
	//}
	//if(pCurTile == nullptr)
	//{
	//	HyGuiLog("WgtTileSetAnimation::GatherFrames - Starting tile not found", LOGTYPE_Error);
	//	ui->txtStartingTile->setText(TILEANIM_UNSET_TEXT);
	//	return;
	//}

	//int iNumFrames = ui->sbNumFrames->value();
	//int iSeparation = ui->sbSeparation->value();
	//QPoint ptCurGridPos = pCurTile->GetMetaGridPos();

	//for(int i = 0; i < iNumFrames; ++i)
	//{
	//	m_FramesChecksumList.append(pCurTile->GetTileChecksum());

	//	for(int j = 0; j < (1 + iSeparation); ++j)
	//	{
	//		ptCurGridPos.setX(ptCurGridPos.x() + 1);
	//		if(ptCurGridPos.x() > iMaxGridX)
	//		{
	//			ptCurGridPos.setX(iMinGridX);
	//			ptCurGridPos.setY(ptCurGridPos.y() + 1);

	//			if(ptCurGridPos.y() > iMaxGridY)
	//			{
	//				SetError("Animation frames out of bounds");
	//				break;
	//			}
	//		}
	//	}
	//	if(IsError())
	//	{
	//		m_FramesChecksumList.clear();
	//		break;
	//	}
	//	
	//	for(TileData *pTile : tileDataList)
	//	{
	//		if(pTile->GetMetaGridPos() == ptCurGridPos)
	//		{
	//			pCurTile = pTile;
	//			break;
	//		}
	//	}
	//}
}

void WgtTileSetAnimation::RefreshPreview()
{
	m_PreviewFrameList.clear();
	m_pPreviewTimer->stop();
	//for(quint32 uiChecksum : m_FramesChecksumList)
	//	m_PreviewFrameList.append(m_pAuxTileSet->GetTileSet()->GetTilePixmap(uiChecksum));
	//
	//if(m_PreviewFrameList.empty() == false)
	//{
	//	m_pPreviewTimer->setInterval(ui->sbFrameDuration->value());
	//	m_pPreviewTimer->start();

	//	m_iPreviewFrameIndex = 0;
	//}
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

void WgtTileSetAnimation::on_sbWidth_valueChanged(int iNewValue)
{
	OnModifyWidget("Animation Set Width", MERGABLEUNDOCMD_TileSetAnimWidth);
	m_pAuxTileSet->GetTileSet()->SetSubAtlasDirty();

	ErrorCheckFrames();
	RefreshPreview();
}

void WgtTileSetAnimation::on_sbHeight_valueChanged(int iNewValue)
{
	OnModifyWidget("Animation Set Height", MERGABLEUNDOCMD_TileSetAnimHeight);
	m_pAuxTileSet->GetTileSet()->SetSubAtlasDirty();

	ErrorCheckFrames();
	RefreshPreview();
}

void WgtTileSetAnimation::on_actionAddFrame_triggered()
{
	ui->framesList->addItem(new QListWidgetItem("Frame " % QString::number(ui->framesList->count())));
	m_pAuxTileSet->GetTileSet()->SetSubAtlasDirty();
	
	ErrorCheckFrames();
	RefreshPreview();
}

void WgtTileSetAnimation::on_actionRemoveFrame_triggered()
{
	ui->framesList->removeItemWidget(ui->framesList->currentItem());
	m_pAuxTileSet->GetTileSet()->SetSubAtlasDirty();

	ErrorCheckFrames();
	RefreshPreview();
}

void WgtTileSetAnimation::on_framesList_currentRowChanged(int iCurrentRow)
{
	m_pAuxTileSet->GetTileSet()->GetGfxScene()->RefreshTiles(*m_pAuxTileSet);
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
