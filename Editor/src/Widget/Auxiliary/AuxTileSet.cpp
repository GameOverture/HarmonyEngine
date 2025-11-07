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
	ui->vsbTileOffset->Init(SPINBOXTYPE_Int2d, -MAX_INT_RANGE, MAX_INT_RANGE);

	ui->vsbTextureRegion->Init(SPINBOXTYPE_Int2d, 1, MAX_INT_RANGE);
	ui->vsbStartOffset->Init(SPINBOXTYPE_Int2d, 0, MAX_INT_RANGE);
	ui->vsbPadding->Init(SPINBOXTYPE_Int2d, 0, MAX_INT_RANGE);
	ui->grpSlicingOptions->setVisible(false);
	ui->grpImportSide->setVisible(false);

	QList<TileSetShape> eTileSetTypeList = HyGlobal::GetTileSetTypeList();
	for(TileSetShape eTileSetType : eTileSetTypeList)
		ui->cmbTileShape->addItem(HyGlobal::TileSetShapeName(eTileSetType), eTileSetType);

	ui->splitter->setSizes(QList<int>() << 140 << width() - 140);

	connect(ui->vsbTileSize, SIGNAL(ValueChanged(QVariant)), this, SLOT(OnTileSizeChanged(QVariant)));
	connect(ui->vsbTileOffset, SIGNAL(ValueChanged(QVariant)), this, SLOT(OnTileOffsetChanged(QVariant)));

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
	
	if(m_pTileSet->GetAtlasRegionSize().isValid() == false)
		m_pTileSet->SetAtlasRegionSize(QSize(g_iDefaultTileSize, g_iDefaultTileSize));
	if(m_pTileSet->GetTileSize().isValid() == false)
		m_pTileSet->SetTileSize(QSize(g_iDefaultTileSize, g_iDefaultTileSize));
	
	CmdSet_TileShapeWidget(m_pTileSet->GetTileShape());
	CmdSet_TileSizeWidgets(m_pTileSet->GetTileSize());
	CmdSet_TileOffsetWidgets(m_pTileSet->GetTileOffset());

	ui->vsbTextureRegion->SetValue(QPoint(m_pTileSet->GetAtlasRegionSize().width(), m_pTileSet->GetAtlasRegionSize().height()));

	ui->btnSave->setDefaultAction(m_pTileSet->GetSaveAction());
	ui->btnUndo->setDefaultAction(m_pTileSet->GetUndoAction());
	ui->btnRedo->setDefaultAction(m_pTileSet->GetRedoAction());

	connect(m_pTileSet->GetUndoStack(), SIGNAL(cleanChanged(bool)), this, SLOT(on_undoStack_cleanChanged(bool)));
	connect(m_pTileSet->GetUndoStack(), SIGNAL(indexChanged(int)), this, SLOT(on_undoStack_indexChanged(int)));

	SetImportWidgets();

	if(m_pTileSet->GetNumTiles() == 0)
	{
		ui->tabWidget->setCurrentIndex(TAB_AddTiles);
		on_tabWidget_currentChanged(TAB_AddTiles);
	}
	else
	{
		ui->tabWidget->setCurrentIndex(TAB_Properties);
		on_tabWidget_currentChanged(TAB_Properties);
	}

	ui->graphicsView->SetScene(this, m_pTileSet->GetGfxScene());
	ui->graphicsView->centerOn(m_pTileSet->GetGfxScene()->GetFocusPt());
	
	RefreshInfo();
}

AtlasTileSet *AuxTileSet::GetTileSet() const
{
	return m_pTileSet;
}

void AuxTileSet::CmdSet_TileShapeWidget(TileSetShape eTileShape)
{
	ui->cmbTileShape->blockSignals(true);
	if(eTileShape == TILESETSHAPE_Unknown)
		ui->cmbTileShape->setCurrentIndex(0);
	else
		ui->cmbTileShape->setCurrentIndex(eTileShape);

	m_pTileSet->SetTileShape(eTileShape);
	m_pTileSet->GetGfxScene()->RefreshTiles();

	ui->cmbTileShape->blockSignals(false);
}

void AuxTileSet::CmdSet_TileSizeWidgets(QSize tileSize)
{
	ui->vsbTileSize->blockSignals(true);
	ui->vsbTileSize->SetValue(QPoint(tileSize.width(), tileSize.height()));

	m_pTileSet->SetTileSize(tileSize);
	m_pTileSet->GetGfxScene()->RefreshTiles();

	ui->vsbTileSize->blockSignals(false);
}

void AuxTileSet::CmdSet_TileOffsetWidgets(QPoint tileOffset)
{
	ui->vsbTileOffset->blockSignals(true);
	ui->vsbTileOffset->SetValue(tileOffset);

	m_pTileSet->SetTileOffset(tileOffset);
	m_pTileSet->GetGfxScene()->RefreshTiles();

	ui->vsbTileOffset->blockSignals(false);
}

void AuxTileSet::RefreshInfo()
{
	ui->lblIcon->setPixmap(m_pTileSet->GetTileSetIcon().pixmap(QSize(16,16)));
	ui->lblName->setText(m_pTileSet->GetName());
	ui->lblInfo->setText(m_pTileSet->GetTileSetInfo());
}

void AuxTileSet::UpdateImportSelection()
{
	ui->btnConfirmAdd->setText("Import " % QString::number(m_pTileSet->GetGfxScene()->GetNumImportPixmaps()) % " Tiles");
	ErrorCheckImport();
}

void AuxTileSet::SetImportWidgets()
{
	bool bTileSheet = ui->radTileSheet->isChecked();
	if(m_bIsImportingTileSheet != bTileSheet)
	{
		bool bHasPendingInfo = ui->txtImagePath->text().isEmpty() == false ||
			ui->vsbTextureRegion->GetValue() != m_pTileSet->GetAtlasRegionSize() ||
			ui->vsbStartOffset->GetValue() != m_pTileSet->GetTileOffset() ||
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

	m_pTileSet->GetGfxScene()->ClearImportTiles();
	m_pTileSet->GetGfxScene()->SetDisplayMode(TILESETMODE_Importing);
	ui->graphicsView->centerOn(m_pTileSet->GetGfxScene()->GetFocusPt());

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
	pGfxScene->ClearImportTiles();

	QPoint ptCurPos(vStartOffset.x() + vPadding.x(),
					vStartOffset.y() + vPadding.y());

	QPoint ptGridPos(0, 0);
	while(vRegionSize.y() <= m_pImportTileSheetPixmap->height() - ptCurPos.y() + 1)
	{
		while(vRegionSize.x() <= m_pImportTileSheetPixmap->width() - ptCurPos.x() + 1)
		{
			QRect tileRect(ptCurPos.x(), ptCurPos.y(), vRegionSize.x(), vRegionSize.y());
			QPixmap tilePixmap = m_pImportTileSheetPixmap->copy(tileRect);

			pGfxScene->AddTile(TILESETMODE_Importing, nullptr, m_pTileSet->GetTilePolygon(), ptGridPos, tilePixmap, IsPixmapAllTransparent(tilePixmap) == false);

			ptCurPos.setX(ptCurPos.x() + vRegionSize.x() + vPadding.x());
			ptGridPos.setX(ptGridPos.x() + 1);
		}

		ptCurPos.setX(vStartOffset.x() + vPadding.x());
		ptCurPos.setY(ptCurPos.y() + vRegionSize.y() + vPadding.y());
		ptGridPos.setX(0);
		ptGridPos.setY(ptGridPos.y() + 1);
	}

	pGfxScene->RefreshTiles();
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

			if(m_pTileSet->GetGfxScene()->GetNumImportPixmaps() <= 0)
			{
				ui->lblError->setText("Invalid slicing options");
				bIsError = true;
				break;
			}
		}
		else
		{
			if(m_pTileSet->GetGfxScene()->GetNumImportPixmaps() <= 0)
			{
				ui->lblError->setText("No tiles selected to import");
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

void AuxTileSet::on_tabWidget_currentChanged(int iIndex)
{
	if(m_pTileSet == nullptr)
		return;

	TileSetMode eTileSetMode = static_cast<TileSetMode>(iIndex);
	ui->graphicsView->SetStatusLabel(eTileSetMode == TILESETMODE_Importing ? "Import New Tiles" : "Setup");
	m_pTileSet->GetGfxScene()->SetDisplayMode(eTileSetMode);
	ui->graphicsView->centerOn(m_pTileSet->GetGfxScene()->GetFocusPt());
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
		pGfxScene->ClearImportTiles();

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

			pGfxScene->AddTile(TILESETMODE_Importing, nullptr, m_pTileSet->GetTilePolygon(), QPoint(iIndex % iNUM_COLS, iIndex / iNUM_COLS), pixmap, IsPixmapAllTransparent(pixmap) == false);
			iIndex++;
		}

		for(auto pImg : vImportImages)
			delete pImg;

		pGfxScene->RefreshTiles();
	}

	ErrorCheckImport();
}

void AuxTileSet::on_cmbTileShape_currentIndexChanged(int iIndex)
{
	if(m_pTileSet == nullptr || m_pTileSet->GetTileShape() == static_cast<TileSetShape>(iIndex))
		return;

	TileSetUndoCmd_TileShape *pCmd = new TileSetUndoCmd_TileShape(*this, m_pTileSet->GetTileShape(), static_cast<TileSetShape>(iIndex));
	m_pTileSet->GetUndoStack()->push(pCmd);

	ErrorCheckImport();
}

void AuxTileSet::OnTileSizeChanged(QVariant newSize)
{
	QSize vSize(newSize.toPoint().x(), newSize.toPoint().y());
	if(m_pTileSet->GetTileSize() == vSize)
		return;

	TileSetUndoCmd_TileSize *pCmd = new TileSetUndoCmd_TileSize(*m_pTileSet, *this, vSize);
	m_pTileSet->GetUndoStack()->push(pCmd); // This will attempt to merge the command
	
	ErrorCheckImport();
}

void AuxTileSet::OnTileOffsetChanged(QVariant newOffset)
{
	QPoint vOffset(newOffset.toPoint().x(), newOffset.toPoint().y());
	if(m_pTileSet->GetTileOffset() == vOffset)
		return;

	TileSetUndoCmd_TileOffset *pCmd = new TileSetUndoCmd_TileOffset(*m_pTileSet, *this, vOffset);
	m_pTileSet->GetUndoStack()->push(pCmd); // This will attempt to merge the command

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

void AuxTileSet::on_radImportBottom_toggled(bool bChecked)
{
	m_pTileSet->GetGfxScene()->SetImportAppendEdge(Qt::BottomEdge);
	m_pTileSet->GetGfxScene()->RefreshTiles();
}

void AuxTileSet::on_radImportTop_toggled(bool bChecked)
{
	m_pTileSet->GetGfxScene()->SetImportAppendEdge(Qt::TopEdge);
	m_pTileSet->GetGfxScene()->RefreshTiles();
}

void AuxTileSet::on_radImportLeft_toggled(bool bChecked)
{
	m_pTileSet->GetGfxScene()->SetImportAppendEdge(Qt::LeftEdge);
	m_pTileSet->GetGfxScene()->RefreshTiles();
}

void AuxTileSet::on_radImportRight_toggled(bool bChecked)
{
	m_pTileSet->GetGfxScene()->SetImportAppendEdge(Qt::RightEdge);
	m_pTileSet->GetGfxScene()->RefreshTiles();
}

void AuxTileSet::on_btnConfirmAdd_clicked()
{
	if(QMessageBox::Yes != QMessageBox::question(MainWindow::GetInstance(), "Confirm TileSet Modification", "Do you want to import '" % QString::number(m_pTileSet->GetGfxScene()->GetNumImportPixmaps()) % "' new tiles?", QMessageBox::Yes, QMessageBox::No))
		return;

	QSize vImportRegionSize = m_pTileSet->GetGfxScene()->GetImportRegionSize();
	if(m_pTileSet->GetAtlasRegionSize().isEmpty() == false &&
	   (m_pTileSet->GetAtlasRegionSize().width() < vImportRegionSize.width() || m_pTileSet->GetAtlasRegionSize().height() < vImportRegionSize.height()))
	{
		if(QMessageBox::Yes != QMessageBox::question(MainWindow::GetInstance(), "Atlas Region Size Increasing!", "New atlas region size is " % QString::number(vImportRegionSize.width()) % "x" % QString::number(vImportRegionSize.height()) % ". Increasing the atlas region size affects all existing tiles in this TileSet. Do you want to continue?", QMessageBox::Yes, QMessageBox::No))
			return;
	}

	QMap<QPoint, QPixmap> importMap = m_pTileSet->GetGfxScene()->AssembleImportMap();
	
	Qt::Edge eAppendEdge;
	if(ui->radImportTop->isChecked())
		eAppendEdge = Qt::TopEdge;
	else if(ui->radImportLeft->isChecked())
		eAppendEdge = Qt::LeftEdge;
	else if(ui->radImportRight->isChecked())
		eAppendEdge = Qt::RightEdge;
	else
		eAppendEdge = Qt::BottomEdge;

	TileSetUndoCmd_AppendTiles *pUndoCmd = new TileSetUndoCmd_AppendTiles(*m_pTileSet, importMap, vImportRegionSize, eAppendEdge);
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
