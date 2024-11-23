/**************************************************************************
 *	AuxTileSet.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AuxTileSet.h"
#include "MainWindow.h"
#include "ui_AuxTileSet.h"

#include "AtlasTileSet.h"
#include "DlgImportTileSheet.h"

#include <QMessageBox>
#include <QFileDialog>

const int g_iDefaultTileSize = 32;

AuxTileSet::AuxTileSet(QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::AuxTileSet),
	m_pTileSet(nullptr),
	m_bIsImportingTileSheet(true),
	m_pImportTileSheetPixmap(nullptr)
{
	ui->setupUi(this);
	ui->lblError->setStyleSheet("QLabel { background-color : red; color : black; }");

	ui->vsbTileSize->Init(SPINBOXTYPE_Int2d, 1, MAX_INT_RANGE);
	ui->vsbStartOffset->Init(SPINBOXTYPE_Int2d, 0, MAX_INT_RANGE);
	ui->vsbPadding->Init(SPINBOXTYPE_Int2d, 0, MAX_INT_RANGE);
	ui->grpSlicingOptions->setVisible(false);
	ui->grpImportSide->setVisible(false);

	ui->splitter->setSizes(QList<int>() << 140 << width() - 140);

	

	connect(ui->vsbTileSize, SIGNAL(ValueChanged(QVariant)), this, SLOT(OnTileSizeChanged(QVariant)));
	connect(ui->vsbStartOffset, SIGNAL(ValueChanged(QVariant)), this, SLOT(OnStartOffsetChanged(QVariant)));
	connect(ui->vsbPadding, SIGNAL(ValueChanged(QVariant)), this, SLOT(OnPaddingChanged(QVariant)));
}

/*virtual*/ AuxTileSet::~AuxTileSet()
{
	delete ui;
}

void AuxTileSet::Init(AtlasTileSet *pTileSet)
{
	m_pTileSet = pTileSet;
	ui->lblInfo->setText(m_pTileSet->GetTileSetInfo());

	if(m_pTileSet->GetTileSize().isValid() == false)
		m_pTileSet->SetTileSize(QSize(g_iDefaultTileSize, g_iDefaultTileSize));
	ui->vsbTileSize->SetValue(QPoint(m_pTileSet->GetTileSize().width(), m_pTileSet->GetTileSize().height()));

	SetImportWidgets();
	ui->graphicsView->setScene(m_pTileSet->GetGfxScene());
}

void AuxTileSet::SetImportWidgets()
{
	bool bTileSheet = ui->radTileSheet->isChecked();
	if(m_bIsImportingTileSheet == bTileSheet)
		return;

	bool bHasPendingInfo = ui->txtImagePath->text().isEmpty() == false ||
						   ui->vsbTileSize->GetValue() != m_pTileSet->GetTileSize() ||
						   ui->vsbStartOffset->GetValue() != QPoint(0, 0) ||
						   ui->vsbPadding->GetValue() != QPoint(0, 0);

	if(bHasPendingInfo)
	{
		if(QMessageBox::No == QMessageBox::question(MainWindow::GetInstance(), "Pending Changes", "Switching to " % QString(bTileSheet ? "'Tile Sheet Image'" : "'Individual Tile Images'") % " will lose your changes. Do you want to continue?", QMessageBox::Yes, QMessageBox::No))
		{
			if(bTileSheet)
				ui->radTileSheet->setChecked(false);
			else
				ui->radTileImages->setChecked(false);
			return;
		}
	}

	// Switching widgets
	m_bIsImportingTileSheet = bTileSheet;
	ui->grpSlicingOptions->setVisible(false);
	ui->grpImportSide->setVisible(false);

	ui->txtImagePath->clear();

	ErrorCheckImport();
}

void AuxTileSet::SliceSheetPixmaps()
{
	QPoint vTileSize = ui->vsbTileSize->GetValue().toPoint();
	QPoint vStartOffset = ui->vsbStartOffset->GetValue().toPoint();
	QPoint vPadding = ui->vsbPadding->GetValue().toPoint();

	if(m_pImportTileSheetPixmap == nullptr || vTileSize.x() == 0 || vTileSize.y() == 0)
		return;

	ui->grpSlicingOptions->setVisible(true);
	ui->grpImportSide->setVisible(true);

	TileSetScene *pGfxScene = m_pTileSet->GetGfxScene();
	pGfxScene->RemoveImportPixmaps();

	QPoint ptCurPos(vStartOffset.x() + vPadding.x(),
					vStartOffset.y() + vPadding.y());

	bool bCountColumns = true;
	int iNumColumns = 0;
	int iNumRows = 0;
	while(vTileSize.y() <= m_pImportTileSheetPixmap->height() - ptCurPos.y() + 1)
	{
		iNumRows++;

		while(vTileSize.x() <= m_pImportTileSheetPixmap->width() - ptCurPos.x() + 1)
		{
			QRect tileRect(ptCurPos.x(), ptCurPos.y(), vTileSize.x(), vTileSize.y());
			pGfxScene->AddImportPixmap(m_pImportTileSheetPixmap->copy(tileRect));

			ptCurPos.setX(ptCurPos.x() + vTileSize.x() + vPadding.x());

			if(bCountColumns)
				iNumColumns++;
		}
		bCountColumns = false;

		ptCurPos.setX(vStartOffset.x() + vPadding.x());
		ptCurPos.setY(ptCurPos.y() + vTileSize.y() + vPadding.y());
	}

	if(iNumColumns == 0 || iNumRows == 0)
		return;

	pGfxScene->ConstructImportScene(vTileSize, iNumColumns, iNumRows);
}

void AuxTileSet::ErrorCheckImport()
{
	bool bIsError = false;
	do
	{
		if(m_bIsImportingTileSheet)
		{
			if(QFile::exists(ui->txtImagePath->text()) == false)
			{
				ui->lblError->setText("Invalid file path");
				bIsError = true;
				break;
			}

			if(m_pImportTileSheetPixmap == nullptr)
			{
				ui->lblError->setText("Invalid tile sheet image");
				bIsError = true;
				break;
			}

			if(ui->vsbTileSize->GetValue().toPoint().x() == 0 || ui->vsbTileSize->GetValue().toPoint().y() == 0)
			{
				ui->lblError->setText("Invalid tile size");
				bIsError = true;
				break;
			}

			if(m_pTileSet->GetGfxScene()->GetNumImportPixmaps() == 0)
			{
				ui->lblError->setText("Invalid slicing options");
				bIsError = true;
				break;
			}
		}

	} while(false);

	ui->lblError->setVisible(bIsError);
	ui->btnConfirmAddRemove->setEnabled(!bIsError);
}


void AuxTileSet::on_radTileSheet_toggled(bool bChecked)
{
	SetImportWidgets();
}

void AuxTileSet::on_radTileImages_toggled(bool bChecked)
{
	SetImportWidgets();
}

void AuxTileSet::on_btnImageBrowse_clicked()
{
	QFileDialog dlg(this);
	QStringList sFilterList;
	sFilterList += "*.png";
	dlg.setNameFilters(sFilterList);
	dlg.setViewMode(QFileDialog::Detail);
	dlg.setWindowModality(Qt::ApplicationModal);
	dlg.setModal(true);

	if(m_bIsImportingTileSheet)
	{
		dlg.setWindowTitle("Select Tile Sheet Image");
		dlg.setFileMode(QFileDialog::ExistingFile);
	}
	else
	{
		dlg.setWindowTitle("Select Individual Tile Images");
		dlg.setFileMode(QFileDialog::ExistingFiles);
	}

	if(dlg.exec() == QDialog::Rejected)
		return;

	QStringList sImportImgList = dlg.selectedFiles();
	if(sImportImgList.empty())
		return;

	if(m_bIsImportingTileSheet)
	{
		ui->txtImagePath->setText(sImportImgList[0]);

		delete m_pImportTileSheetPixmap;
		m_pImportTileSheetPixmap = new QPixmap(sImportImgList[0]);

		SliceSheetPixmaps();
	}
	else // Individual Tiles
	{
		TileSetScene *pGfxScene = m_pTileSet->GetGfxScene();
		pGfxScene->RemoveImportPixmaps();

		QString sImgPaths;
		for(auto sImgPath : sImportImgList)
		{
			sImgPaths += sImgPath % ";";
			pGfxScene->AddImportPixmap(QPixmap(sImgPath));
		}
		ui->txtImagePath->setText(sImgPaths);
	}

	ErrorCheckImport();
}

void AuxTileSet::OnTileSizeChanged(QVariant newSize)
{
	SliceSheetPixmaps();
	ErrorCheckImport();
}

void AuxTileSet::OnStartOffsetChanged(QVariant newOffset)
{
	SliceSheetPixmaps();
	ErrorCheckImport();
}

void AuxTileSet::OnPaddingChanged(QVariant newPadding)
{
	SliceSheetPixmaps();
	ErrorCheckImport();
}

void AuxTileSet::on_btnConfirmAddRemove_clicked()
{
	if(QMessageBox::Yes != QMessageBox::question(MainWindow::GetInstance(), "Confirm Batch Import", "Do you want to import '" % QString::number(m_pTileSet->GetGfxScene()->GetNumImportPixmaps()) % "' images?", QMessageBox::Yes, QMessageBox::No))
		return;

	//QDir tempDir = HyGlobal::PrepTempDir(m_ProjectRef, HYGUIPATH_TEMPSUBDIR_ImportTileSheet);

	//char szBuffer[16];
	//uint uiPixmapIndex = 0;
	//for(auto pPixmap : m_TilePixmaps)
	//{
	//	QString sMetaName;// = ui->txtTilePrefix->text();
	//	sprintf(szBuffer, "%05u.png", uiPixmapIndex);
	//	sMetaName += szBuffer;

	//	pPixmap->save(tempDir.absoluteFilePath(sMetaName));
	//	uiPixmapIndex++;
	//}

	//QFileInfoList imageFileList = tempDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
	//QStringList sImageImportList;
	//for(auto fileInfo : imageFileList)
	//	sImageImportList << fileInfo.absoluteFilePath();

	//quint32 uiBankId = m_ProjectRef.GetAtlasWidget() ? m_ProjectRef.GetAtlasWidget()->GetSelectedBankId() : 0;

	//TreeModelItemData *pFirstSelected = nullptr;
	//if(m_ProjectRef.GetAtlasWidget())
	//	pFirstSelected = m_ProjectRef.GetAtlasWidget()->GetSelected();

	//TreeModelItemData *pParent = m_ProjectRef.GetAtlasModel().FindTreeItemFilter(pFirstSelected);

	//QVector<TreeModelItemData *> correspondingParentList;
	//QVector<QUuid> correspondingUuidList;
	//for(int i = 0; i < sImageImportList.size(); ++i)
	//{
	//	correspondingParentList.append(pParent);
	//	correspondingUuidList.append(QUuid::createUuid());
	//}

	//m_ProjectRef.GetAtlasModel().ImportNewAssets(sImageImportList, uiBankId, correspondingParentList, correspondingUuidList);
}
