/**************************************************************************
 *	DlgImportTileSheet.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "DlgImportTileSheet.h"
#include "ui_DlgImportTileSheet.h"
#include "Project.h"
#include "MainWindow.h"
#include "AtlasModel.h"
#include "IManagerModel.h"
#include "ManagerWidget.h"

#include <QDir>
#include <QFileDialog>
#include <QPixmap>
#include <QPainter>
#include <QMessageBox>

const int g_iDEFAULT_TILE_SIZE = 32;

DlgImportTileSheet::DlgImportTileSheet(AtlasTileSet &tileSetRef, QWidget *parent) :
	QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
	m_DefaultTileSize(tileSetRef.GetAtlasRegionSize()),
	m_TileSetRef(tileSetRef),
	m_pOrigPixmap(nullptr),
	ui(new Ui::DlgImportTileSheet)
{
	ui->setupUi(this);
	ui->lblError->setStyleSheet("QLabel { background-color : red; color : black; }");

	if(m_DefaultTileSize.isValid() == false)
	{
		m_DefaultTileSize.setWidth(g_iDEFAULT_TILE_SIZE);
		m_DefaultTileSize.setHeight(g_iDEFAULT_TILE_SIZE);
	}
	ui->sbTileSizeX->setValue(m_DefaultTileSize.width());
	ui->sbTileSizeY->setValue(m_DefaultTileSize.height());

	ErrorCheck();

	setWindowTitle(tileSetRef.GetName() % " - Import/Replace Tiles"); // NOTE: This needs to be after ui->setupUi
	SetWidgets(ui->radTileSheet->isChecked());
}

DlgImportTileSheet::~DlgImportTileSheet()
{
	delete ui;
}

void DlgImportTileSheet::on_radTileSheet_toggled(bool bChecked)
{
	SetWidgets(bChecked);
}

void DlgImportTileSheet::on_radTileImages_toggled(bool bChecked)
{
	SetWidgets(!bChecked);
}

void DlgImportTileSheet::on_btnImageBrowse_clicked()
{
	QFileDialog dlg(this);
	QStringList sFilterList;
	sFilterList += "*.png";
	dlg.setNameFilters(sFilterList);
	dlg.setViewMode(QFileDialog::Detail);
	dlg.setWindowModality(Qt::ApplicationModal);
	dlg.setModal(true);

	if(m_bIsShowingTileSheet)
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

	if(m_bIsShowingTileSheet)
	{
		ui->txtImagePath->setText(sImportImgList[0]);

		delete m_pOrigPixmap;
		m_pOrigPixmap = new QPixmap(sImportImgList[0]);
	}
	else // Individual Tiles
	{
		QString sImgPaths;
		for(auto sImgPath : sImportImgList)
		{
			sImgPaths += sImgPath % ";";
			m_TilePixmaps.push_back(new QPixmap(sImgPath));
		}
		ui->txtImagePath->setText(sImgPaths);
	}

	AssemblePixmaps();
	ErrorCheck();
}

void DlgImportTileSheet::on_sbTileSizeX_valueChanged(int iArg)
{
	AssemblePixmaps();
	ErrorCheck();
}

void DlgImportTileSheet::on_sbTileSizeY_valueChanged(int iArg)
{
	AssemblePixmaps();
	ErrorCheck();
}

void DlgImportTileSheet::on_sbOffsetX_valueChanged(int iArg)
{
	AssemblePixmaps();
	ErrorCheck();
}

void DlgImportTileSheet::on_sbOffsetY_valueChanged(int iArg)
{
	AssemblePixmaps();
	ErrorCheck();
}

void DlgImportTileSheet::on_sbPaddingX_valueChanged(int iArg)
{
	AssemblePixmaps();
	ErrorCheck();
}

void DlgImportTileSheet::on_sbPaddingY_valueChanged(int iArg)
{
	AssemblePixmaps();
	ErrorCheck();
}

void DlgImportTileSheet::on_buttonBox_accepted()
{
	if(QMessageBox::Yes != QMessageBox::question(MainWindow::GetInstance(), "Confirm Batch Import", "Do you want to import '" % QString::number(m_TilePixmaps.size()) % "' images?", QMessageBox::Yes, QMessageBox::No))
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

void DlgImportTileSheet::SetWidgets(bool bTileSheet)
{
	if(m_bIsShowingTileSheet == bTileSheet)
		return;

	bool bHasPendingInfo = ui->txtImagePath->text().isEmpty() == false ||
						   ui->sbTileSizeX->value() != m_DefaultTileSize.width() ||
						   ui->sbTileSizeY->value() != m_DefaultTileSize.height() ||
						   ui->sbOffsetX->value() != 0 ||
						   ui->sbOffsetY->value() != 0 ||
						   ui->sbPaddingX->value() != 0 ||
						   ui->sbPaddingY->value() != 0;

	if(bHasPendingInfo)
	{
		if(QMessageBox::No == QMessageBox::question(MainWindow::GetInstance(), "Pending Changes", "Switching to " % QString(bTileSheet ? "'Import Tile Sheet'" : "'Import Individual Images'") % " will lose your changes. Do you want to continue?", QMessageBox::Yes, QMessageBox::No))
		{
			if(bTileSheet)
				ui->radTileSheet->setChecked(false);
			else
				ui->radTileImages->setChecked(false);
			return;
		}
	}

	// Switching widgets
	m_bIsShowingTileSheet = bTileSheet;

	ui->grpSlicingOptions->setVisible(m_bIsShowingTileSheet);
	ui->grpSlicingOptions->setEnabled(m_bIsShowingTileSheet);
	ui->lblInput->setText(bTileSheet ? "Tile Sheet Image Path" : "Individual Image Paths");

	ui->txtImagePath->clear();

	ErrorCheck();
}

void DlgImportTileSheet::AssemblePixmaps()
{
	if(m_pPreviewPixmap)
		delete m_pPreviewPixmap;

	if(m_pOrigPixmap == nullptr || ui->sbTileSizeX->value() == 0 || ui->sbTileSizeY->value() == 0)
		return;

	for(int i = 0; i < m_TilePixmaps.size(); ++i)
		delete m_TilePixmaps[i];
	m_TilePixmaps.clear();

	QPoint ptCurPos(ui->sbOffsetX->value() + ui->sbPaddingX->value(),
					ui->sbOffsetY->value() + ui->sbPaddingY->value());

	QPainter painter;
	painter.setOpacity(0.0f);
	bool bCountColumns = true;
	int iNumColumns = 0;
	int iNumRows = 0;
	while(ui->sbTileSizeY->value() <= m_pOrigPixmap->height() - ptCurPos.y() + 1)
	{
		iNumRows++;

		while(ui->sbTileSizeX->value() <= m_pOrigPixmap->width() - ptCurPos.x() + 1)
		{
			QPixmap *pNewPixmap = new QPixmap(ui->sbTileSizeX->value(), ui->sbTileSizeY->value());
			pNewPixmap->fill(QColor(Qt::transparent));
			painter.begin(pNewPixmap);
			{
				QRect curRect(ptCurPos.x(), ptCurPos.y(), ui->sbTileSizeX->value(), ui->sbTileSizeY->value());
				painter.drawPixmap(QPoint(0, 0), *m_pOrigPixmap, curRect);
			}
			painter.end();
			m_TilePixmaps.append(pNewPixmap);

			ptCurPos.setX(ptCurPos.x() + ui->sbTileSizeX->value() + ui->sbPaddingX->value());

			if(bCountColumns)
				iNumColumns++;
		}
		bCountColumns = false;

		ptCurPos.setX(ui->sbOffsetX->value() + ui->sbPaddingX->value());
		ptCurPos.setY(ptCurPos.y() + ui->sbTileSizeY->value() + ui->sbPaddingY->value());
	}

	if(iNumColumns == 0 || iNumRows == 0)
		return;

	int iSpacingAmt = 2;
	m_pPreviewPixmap = new QPixmap((ui->sbTileSizeX->value() * iNumColumns) + (iSpacingAmt * (iNumColumns+1)),
								   (ui->sbTileSizeY->value() * iNumRows) + (iSpacingAmt * (iNumRows+1)));
	m_pPreviewPixmap->fill(QColor("black"));

	ptCurPos.setX(iSpacingAmt);
	ptCurPos.setY(iSpacingAmt);

	int iPixmapIndex = 0;
	painter.begin(m_pPreviewPixmap);
	{
		for(int i = 0; i < iNumRows; ++i)
		{
			for(int j = 0; j < iNumColumns; ++j)
			{
				painter.eraseRect(ptCurPos.x(), ptCurPos.y(), ui->sbTileSizeX->value(), ui->sbTileSizeY->value());
				painter.drawPixmap(ptCurPos, *m_TilePixmaps[iPixmapIndex]);
				iPixmapIndex++;

				ptCurPos.setX(ptCurPos.x() + ui->sbTileSizeX->value() + iSpacingAmt);
			}

			ptCurPos.setX(iSpacingAmt);
			ptCurPos.setY(ptCurPos.y() + ui->sbTileSizeY->value() + iSpacingAmt);
		}
	}
	painter.end();
}

void DlgImportTileSheet::ErrorCheck()
{
	bool bIsError = false;
	do
	{
		if(QFile::exists(ui->txtImagePath->text()) == false)
		{
			ui->lblError->setText("Error: Invalid image selected");
			bIsError = true;
			break;
		}

		if(m_pPreviewPixmap == nullptr)
		{
			ui->lblError->setText("Error: Invalid slicing options");
			bIsError = true;
			break;
		}

	} while(false);

	ui->lblError->setVisible(bIsError);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!bIsError);
}
