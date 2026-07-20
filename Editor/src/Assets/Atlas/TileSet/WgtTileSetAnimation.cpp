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
#include <QPainter>

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

	int iNumFrames = serializedObj["numFrames"].toInt();
	// Add or remove rows to ui->framesList so it equals iNumFrames
	while(ui->framesList->count() < iNumFrames)
		ui->framesList->addItem(new QListWidgetItem("Frame " % QString::number(ui->framesList->count())));
	while(ui->framesList->count() > iNumFrames)
	{
		QListWidgetItem *pItem = ui->framesList->takeItem(ui->framesList->count() - 1);
		delete pItem;
	}

	//m_TileAnimList.clear();
	//QJsonArray tileAnimsArray = serializedObj["tileAnims"].toArray();
	//for(QJsonValue tileAnimVal : tileAnimsArray)
	//{
	//	QJsonObject animObj = tileAnimVal.toObject();
	//	m_TileAnimList.push_back(TileAnimation(animObj));
	//}

	ErrorCheckFrames();
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

	serializedJsonObj["numFrames"] = ui->framesList->count();

	//QJsonArray tileAnimsArray;
	//for(TileAnimation &tileAnim : m_TileAnimList)
	//	tileAnimsArray.append(tileAnim.ToJsonObject());
	//serializedJsonObj["tileAnims"] = tileAnimsArray;

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

//void WgtTileSetAnimation::CmdSet_AddAnimationFrames(int iFrameIndex, QList<TileData *> tileList)
//{
//	// Sort tileList by their meta-grid locations (left to right, top to bottom)
//	std::sort(tileList.begin(), tileList.end(),
//		[](TileData *pTileA, TileData *pTileB)
//		{
//			QPoint ptTileA = pTileA->GetMetaGridPos();
//			QPoint ptTileB = pTileB->GetMetaGridPos();
//			if(ptTileA.y() == ptTileB.y())
//				return ptTileA.x() < ptTileB.x();
//			else
//				return ptTileA.y() < ptTileB.y();
//		});
//	
//	if(m_TileAnimList.size() < tileList.size())
//		m_TileAnimList.resize(tileList.size());
//
//	for(int i = 0; i < tileList.size(); ++i)
//	{
//		if(m_TileAnimList[i].m_TileChecksumList.size() < iFrameIndex+1)
//			m_TileAnimList[i].m_TileChecksumList.resize(iFrameIndex+1, -1);
//
//		m_TileAnimList[i].m_TileChecksumList[iFrameIndex] = tileList[i]->GetTileChecksum();
//
//		if(iFrameIndex == 0)
//			m_TileAnimList[i].m_StartingTileUuid = tileList[i]->GetUuid();
//	}
//}
//
//void WgtTileSetAnimation::CmdSet_RemoveAnimationFrames(int iFrameIndex, QList<TileData *> tileList)
//{
//	for(int i = 0; i < m_TileAnimList.size(); ++i)
//	{
//		if(m_TileAnimList[i].m_TileChecksumList.size() > iFrameIndex)
//		{
//			for(int j = 0; j < tileList.size(); ++j)
//			{
//				if(m_TileAnimList[i].m_TileChecksumList[iFrameIndex] == tileList[j]->GetTileChecksum())
//					m_TileAnimList[i].m_TileChecksumList[iFrameIndex] = -1;
//			}
//		}
//
//		while(m_TileAnimList[i].m_TileChecksumList.back() == -1)
//			m_TileAnimList[i].m_TileChecksumList.pop_back();
//	}
//
//	while(m_TileAnimList.back().m_TileChecksumList.empty())
//		m_TileAnimList.pop_back();
//}

/*virtual*/ QFrame *WgtTileSetAnimation::GetBorderFrame() const /*override*/
{
	return ui->frmBorder;
}

void WgtTileSetAnimation::ErrorCheckFrames()
{
	m_PreviewFrameList.clear();
	m_pPreviewTimer->stop();

	int iNumDesiredFrames = ui->framesList->count();
	int iRequiredNumTileAnims = ui->sbWidth->value() * ui->sbHeight->value();

	QVector<TileData *> tileDataList = m_pAuxTileSet->GetTileSet()->GetTileDataList();
	QString sErrorMsg;

	for(int iFrameIndex = 0; iFrameIndex < iNumDesiredFrames; ++iFrameIndex)
	{
		QList<TileData *> tileFrameList;

		for(TileData *pTile : tileDataList)
		{
			const QMap<QUuid, QList<int>> &tileAnimMapRef = pTile->GetAnimationMap();
			
			for(QUuid animUuid : tileAnimMapRef.keys())
			{
				if(GetUuid() == animUuid)
				{
					for(int iFrame : tileAnimMapRef[animUuid])
					{
						if(iFrame == iFrameIndex)
							tileFrameList.push_back(pTile);
					}
				}
			}
		}

		// Sort 'tileFrameList' by their meta-grid positions
		std::sort(tileFrameList.begin(), tileFrameList.end(),
			[](TileData *pTileDataA, TileData *pTileDataB)
			{
				QPoint ptTileA = pTileDataA->GetMetaGridPos();
				QPoint ptTileB = pTileDataB->GetMetaGridPos();
				if(ptTileA.y() == ptTileB.y())
					return ptTileA.x() < ptTileB.x();
				else
					return ptTileA.y() < ptTileB.y();
			});
		QImage frameImg(ui->sbWidth->value() * m_pAuxTileSet->GetTileSet()->GetTileSize().width(), ui->sbHeight->value() * m_pAuxTileSet->GetTileSet()->GetTileSize().height(), QImage::Format_ARGB32);
		frameImg.fill(Qt::transparent);
		QPainter p(&frameImg);
		for(int iCol = 0; iCol < ui->sbWidth->value(); ++iCol)
		{
			for(int iRow = 0; iRow < ui->sbHeight->value(); ++iRow)
			{
				int iTileIndex = (iRow * ui->sbWidth->value()) + iCol;
				if(iTileIndex < tileFrameList.size())
				{
					QPixmap tilePixmap = m_pAuxTileSet->GetTileSet()->GetTilePixmap(tileFrameList[iTileIndex]->GetTileChecksum());
					p.drawPixmap(iCol * m_pAuxTileSet->GetTileSet()->GetTileSize().width(), iRow * m_pAuxTileSet->GetTileSet()->GetTileSize().height(), tilePixmap);
				}
			}
		}
		m_PreviewFrameList.push_back(QPixmap::fromImage(frameImg));

		int iTileCountThisFrame = tileFrameList.size();
		QString sFrameText = "Frame " % QString::number(iFrameIndex);
		if(iRequiredNumTileAnims == 1 && iTileCountThisFrame == 0)
			sFrameText += " - Select a tile";
		else if(iTileCountThisFrame < iRequiredNumTileAnims)
			sFrameText += " - Select " % QString::number(iRequiredNumTileAnims - iTileCountThisFrame) % " Tiles";
		else if(iTileCountThisFrame > iRequiredNumTileAnims)
			sFrameText += " - Too many tiles selected";

		ui->framesList->item(iFrameIndex)->setText(sFrameText);
		
		if(iTileCountThisFrame != iRequiredNumTileAnims)
			sErrorMsg = "Animation not setup properly";
	}

	if(m_PreviewFrameList.empty() == false)
	{
		m_pPreviewTimer->setInterval(ui->sbFrameDuration->value());
		m_pPreviewTimer->start();

		m_iPreviewFrameIndex = 0;
	}

	if(sErrorMsg.isEmpty())
		ClearError();
	else
		SetError(sErrorMsg);
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
}

void WgtTileSetAnimation::on_sbHeight_valueChanged(int iNewValue)
{
	OnModifyWidget("Animation Set Height", MERGABLEUNDOCMD_TileSetAnimHeight);
	m_pAuxTileSet->GetTileSet()->SetSubAtlasDirty();

	ErrorCheckFrames();
}

void WgtTileSetAnimation::on_actionAddFrame_triggered()
{
	ui->framesList->addItem(new QListWidgetItem("Frame " % QString::number(ui->framesList->count())));
	OnModifyWidget("Animation Frame Added", -1);
	m_pAuxTileSet->GetTileSet()->SetSubAtlasDirty();
	
	ErrorCheckFrames();
}

void WgtTileSetAnimation::on_actionRemoveFrame_triggered()
{
	int itest = ui->framesList->count();
	QListWidgetItem *pItem = ui->framesList->takeItem(ui->framesList->currentRow());
	if(pItem == nullptr)
		return;
	delete pItem;

	itest = ui->framesList->count();
	OnModifyWidget("Animation Frame Removed", -1);
	m_pAuxTileSet->GetTileSet()->SetSubAtlasDirty();

	ErrorCheckFrames();
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
	ErrorCheckFrames();
}

void WgtTileSetAnimation::on_chkEnabled_toggled(bool bChecked)
{
	OnModifyWidget("Animation Enabled", -1);
	ErrorCheckFrames();
}

void WgtTileSetAnimation::on_chkGlobalSync_toggled(bool bChecked)
{
	OnModifyWidget("Animation Global Sync", -1);
	ErrorCheckFrames();
}

void WgtTileSetAnimation::on_chkLoop_toggled(bool bChecked)
{
	OnModifyWidget("Animation Loop", -1);
	ErrorCheckFrames();
}

void WgtTileSetAnimation::on_chkReverse_toggled(bool bChecked)
{
	OnModifyWidget("Animation Reverse", -1);
	ErrorCheckFrames();
}

void WgtTileSetAnimation::on_chkBounce_toggled(bool bChecked)
{
	OnModifyWidget("Animation Bounce", -1);
	ErrorCheckFrames();
}
