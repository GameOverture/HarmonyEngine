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
#include "TileSetUndoCmds.h"

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

	ui->vsbTextureRegion->Init(SPINBOXTYPE_Int2d, 1, MAX_INT_RANGE);
	ui->vsbStartOffset->Init(SPINBOXTYPE_Int2d, 0, MAX_INT_RANGE);
	ui->vsbPadding->Init(SPINBOXTYPE_Int2d, 0, MAX_INT_RANGE);
	ui->grpSlicingOptions->setVisible(false);
	ui->grpImportSide->setVisible(false);

	QList<TileSetType> eTileSetTypeList = HyGlobal::GetTileSetTypeList();
	for(TileSetType eTileSetType : eTileSetTypeList)
		ui->cmbTileShape->addItem(HyGlobal::TileSetTypeName(eTileSetType), eTileSetType);

	ui->splitter->setSizes(QList<int>() << 140 << width() - 140);

	connect(ui->vsbTileSize, SIGNAL(ValueChanged(QVariant)), this, SLOT(OnTileSizeChanged(QVariant)));

	connect(ui->vsbTextureRegion, SIGNAL(ValueChanged(QVariant)), this, SLOT(OnTextureRegionChanged(QVariant)));
	connect(ui->vsbStartOffset, SIGNAL(ValueChanged(QVariant)), this, SLOT(OnStartOffsetChanged(QVariant)));
	connect(ui->vsbPadding, SIGNAL(ValueChanged(QVariant)), this, SLOT(OnPaddingChanged(QVariant)));
}

/*virtual*/ AuxTileSet::~AuxTileSet()
{
	delete ui;
}

void AuxTileSet::Init(AtlasTileSet *pTileSet)
{
	if(m_pTileSet == pTileSet)
		return;

	m_pTileSet = pTileSet;
	RefreshInfo();

	if(m_pTileSet->GetTileType() == TILESETTYPE_Unknown)
		ui->cmbTileShape->setCurrentIndex(0);
	else
		ui->cmbTileShape->setCurrentIndex(m_pTileSet->GetTileType());

	if(m_pTileSet->GetAtlasRegionSize().isValid() == false)
		m_pTileSet->SetAtlasRegionSize(QSize(g_iDefaultTileSize, g_iDefaultTileSize));
	if(m_pTileSet->GetTileSize().isValid() == false)
		m_pTileSet->SetTileSize(QSize(g_iDefaultTileSize, g_iDefaultTileSize));
	
	ui->vsbTileSize->SetValue(QPoint(m_pTileSet->GetTileSize().width(), m_pTileSet->GetTileSize().height()));
	ui->vsbTextureRegion->SetValue(QPoint(m_pTileSet->GetAtlasRegionSize().width(), m_pTileSet->GetAtlasRegionSize().height()));

	ui->btnSave->setDefaultAction(m_pTileSet->GetSaveAction());
	ui->btnUndo->setDefaultAction(m_pTileSet->GetUndoAction());
	ui->btnRedo->setDefaultAction(m_pTileSet->GetRedoAction());

	connect(m_pTileSet->GetUndoStack(), SIGNAL(cleanChanged(bool)), this, SLOT(on_undoStack_cleanChanged(bool)));
	connect(m_pTileSet->GetUndoStack(), SIGNAL(indexChanged(int)), this, SLOT(on_undoStack_indexChanged(int)));

	SetImportWidgets();

	if(m_pTileSet->GetNumTiles() == 0)
		ui->tabWidget->setCurrentIndex(TAB_AddTiles);
	else
		ui->tabWidget->setCurrentIndex(TAB_Properties);

	ui->graphicsView->setScene(m_pTileSet->GetGfxScene());
	
	// Allow the graphicsView take mouse input
}

AtlasTileSet *AuxTileSet::GetTileSet() const
{
	return m_pTileSet;
}

void AuxTileSet::RefreshInfo()
{
	ui->lblIcon->setPixmap( m_pTileSet->GetTileSetIcon().pixmap(QSize(16,16)));
	ui->lblName->setText(m_pTileSet->GetName());
	ui->lblInfo->setText(m_pTileSet->GetTileSetInfo());
}

void AuxTileSet::SetImportWidgets()
{
	bool bTileSheet = ui->radTileSheet->isChecked();
	if(m_bIsImportingTileSheet != bTileSheet)
	{
		bool bHasPendingInfo = ui->txtImagePath->text().isEmpty() == false ||
			ui->vsbTextureRegion->GetValue() != m_pTileSet->GetAtlasRegionSize() ||
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

			ui->txtImagePath->clear();
		}
	}

	ui->vsbTextureRegion->SetValue(m_pTileSet->GetAtlasRegionSize());
	ui->vsbStartOffset->SetValue(QPoint(0, 0));
	ui->vsbPadding->SetValue(QPoint(0, 0));

	// Switching widgets
	m_bIsImportingTileSheet = bTileSheet;
	ui->grpSlicingOptions->setVisible(m_bIsImportingTileSheet);

	m_pTileSet->GetGfxScene()->ClearImport();
	m_pTileSet->GetGfxScene()->SetDisplayMode(TILESETMODE_Importing);

	ErrorCheckImport();
}

void AuxTileSet::SliceSheetPixmaps()
{
	QPoint vRegionSize = ui->vsbTextureRegion->GetValue().toPoint();
	QPoint vStartOffset = ui->vsbStartOffset->GetValue().toPoint();
	QPoint vPadding = ui->vsbPadding->GetValue().toPoint();

	if(m_pImportTileSheetPixmap == nullptr || vRegionSize.x() == 0 || vRegionSize.y() == 0)
		return;

	ui->grpSlicingOptions->setVisible(true);
	ui->grpImportSide->setVisible(m_pTileSet->GetNumTiles() > 0);

	TileSetScene *pGfxScene = m_pTileSet->GetGfxScene();
	pGfxScene->ClearImport();

	QPoint ptCurPos(vStartOffset.x() + vPadding.x(),
					vStartOffset.y() + vPadding.y());

	QPoint ptGridPos(0, 0);
	while(vRegionSize.y() <= m_pImportTileSheetPixmap->height() - ptCurPos.y() + 1)
	{
		while(vRegionSize.x() <= m_pImportTileSheetPixmap->width() - ptCurPos.x() + 1)
		{
			QRect tileRect(ptCurPos.x(), ptCurPos.y(), vRegionSize.x(), vRegionSize.y());
			QPixmap tilePixmap = m_pImportTileSheetPixmap->copy(tileRect);

			if(IsPixmapAllTransparent(tilePixmap) == false)
			{
				(ptCurPos.x() / vRegionSize.x(), ptCurPos.y() / vRegionSize.y());
				pGfxScene->AddImport(m_pTileSet->GetTilePolygon(), ptGridPos, tilePixmap);
			}

			ptCurPos.setX(ptCurPos.x() + vRegionSize.x() + vPadding.x());
			ptGridPos.setX(ptGridPos.x() + 1);
		}

		ptCurPos.setX(vStartOffset.x() + vPadding.x());
		ptCurPos.setY(ptCurPos.y() + vRegionSize.y() + vPadding.y());
		ptGridPos.setX(0);
		ptGridPos.setY(ptGridPos.y() + 1);
	}

	pGfxScene->SyncImport();
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
	ui->btnConfirmAdd->setEnabled(!bIsError);
}

bool AuxTileSet::IsPixmapAllTransparent(const QPixmap &pixmap)
{
	QImage image = pixmap.toImage();
	if(image.isNull())
		return true; // Consider a null QPixmap as transparent

	for(int y = 0; y < image.height(); ++y)
	{
		for(int x = 0; x < image.width(); ++x)
		{
			QColor color = image.pixelColor(x, y);
			if(color.alpha() > 0)
				return false; // Found a non-transparent pixel
		}
	}

	return true; // All pixels are transparent
}

void AuxTileSet::on_undoStack_cleanChanged(bool bClean)
{
	RefreshInfo();
}

void AuxTileSet::on_undoStack_indexChanged(int iIndex)
{
	RefreshInfo();
}

void AuxTileSet::on_actionSave_triggered()
{


	RefreshInfo();
}

void AuxTileSet::on_tabWidget_currentChanged(int iIndex)
{
	if(m_pTileSet == nullptr)
		return;

	m_pTileSet->GetGfxScene()->SetDisplayMode(static_cast<TileSetMode>(iIndex));
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

	// Initializing Tile Type
	if(m_pTileSet->GetTileType() == TILESETTYPE_Unknown)
	{
		m_ImportPolygon;
	}

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
		pGfxScene->ClearImport();

		QVector<QImage *> vImportImages;

		QString sImgPaths;
		for(auto sImgPath : sImportImgList)
		{
			sImgPaths += sImgPath % ";";
			vImportImages.push_back(new QImage(sImgPath));
		}
		ui->txtImagePath->setText(sImgPaths);

		// Determine the largest image size
		QSize maxSize;
		for(auto pImg : vImportImages)
		{
			if(pImg->width() > maxSize.width())
				maxSize.setWidth(pImg->width());
			if(pImg->height() > maxSize.height())
				maxSize.setHeight(pImg->height());
		}

		int iNUM_COLS = NUM_COLS_TILESET(vImportImages.size());
		int iIndex = 0;
		for(QImage *pImg : vImportImages)
		{
			QPixmap pixmap(maxSize);
			pixmap.fill(Qt::transparent);
			QPainter painter(&pixmap);
			painter.drawImage((maxSize.width() - pImg->width()) / 2, (maxSize.height() - pImg->height()) / 2, *pImg);
			painter.end();

			pGfxScene->AddImport(m_pTileSet->GetTilePolygon(), QPoint(iIndex % iNUM_COLS, iIndex / iNUM_COLS), pixmap);
			iIndex++;
		}

		for(auto pImg : vImportImages)
			delete pImg;

		pGfxScene->SyncImport();
	}

	ErrorCheckImport();
}

void AuxTileSet::OnTileSizeChanged(QVariant newSize)
{
	SliceSheetPixmaps();
	ErrorCheckImport();
}

void AuxTileSet::OnTextureRegionChanged(QVariant newOffset)
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

void AuxTileSet::on_btnConfirmAdd_clicked()
{
	if(QMessageBox::Yes != QMessageBox::question(MainWindow::GetInstance(), "Confirm TileSet Modification", "Do you want to import '" % QString::number(m_pTileSet->GetGfxScene()->GetNumImportPixmaps()) % "' new tiles?", QMessageBox::Yes, QMessageBox::No))
		return;

	QMap<QPoint, QPixmap> importMap = m_pTileSet->GetGfxScene()->AssembleImportMap();
	QSize vImportTileSize = m_pTileSet->GetGfxScene()->GetImportTileSize();
	Qt::Edge eAppendEdge;
	if(ui->radImportTop->isChecked())
		eAppendEdge = Qt::TopEdge;
	else if(ui->radImportLeft->isChecked())
		eAppendEdge = Qt::LeftEdge;
	else if(ui->radImportRight->isChecked())
		eAppendEdge = Qt::RightEdge;
	else
		eAppendEdge = Qt::BottomEdge;

	if(m_pTileSet->GetTileType() == TILESETTYPE_Unknown)
		m_pTileSet->SetTileType(static_cast<TileSetType>(ui->cmbTileShape->currentIndex()));

	if(m_pTileSet->GetTilePolygon().isEmpty())
	{
		m_pTileSet->SetTilePolygon(m_ImportPolygon);
		if(m_pTileSet->GetTilePolygon().isEmpty())
			HyGuiLog("AuxTileSet::on_btnConfirmAdd_clicked() Set invalid tile polygon with import", LOGTYPE_Error);
	}

	TileSetUndoCmd_AppendTiles *pUndoCmd = new TileSetUndoCmd_AppendTiles(*m_pTileSet, importMap, vImportTileSize, eAppendEdge);
	m_pTileSet->GetUndoStack()->push(pUndoCmd);

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
