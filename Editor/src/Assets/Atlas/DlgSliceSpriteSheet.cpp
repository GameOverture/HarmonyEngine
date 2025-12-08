/**************************************************************************
 *	DlgSliceSpriteSheet.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "DlgSliceSpriteSheet.h"
#include "ui_DlgSliceSpriteSheet.h"
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

const int g_iDEFAULT_SLICE_SIZE = 64;

DlgSliceSpriteSheet::DlgSliceSpriteSheet(QWidget *parent) :
	QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
	m_iCurrentSheetIndex(0),
	m_pPreviewScene(nullptr),
	m_pPreviewPixmap(nullptr),
	m_pPreviewPixmapItem(nullptr),
	ui(new Ui::DlgSliceSpriteSheet)
{
	ui->setupUi(this);
	ui->lblError->setStyleSheet("QLabel { background-color : red; color : black; }");

	ui->txtMainFilter->setEnabled(false);

	ui->actionPreviewBack->setEnabled(false);
	ui->btnPreviewBack->setDefaultAction(ui->actionPreviewBack);
	ui->actionPreviewNext->setEnabled(false);
	ui->btnPreviewNext->setDefaultAction(ui->actionPreviewNext);

	ui->sbTileSizeX->setValue(g_iDEFAULT_SLICE_SIZE);
	ui->sbTileSizeY->setValue(g_iDEFAULT_SLICE_SIZE);

	m_pPreviewScene = new QGraphicsScene(this);
	ui->graphicsView->setScene(m_pPreviewScene);

	ErrorCheck();
}

DlgSliceSpriteSheet::~DlgSliceSpriteSheet()
{
	delete ui;
	delete m_pPreviewPixmap;
	delete m_pPreviewScene;
	delete m_pPreviewPixmap;
}

void DlgSliceSpriteSheet::Clear()
{
	m_SheetList.clear();
	m_iCurrentSheetIndex = 0;
}

QDir DlgSliceSpriteSheet::DumpToTempDir()
{
	if(Harmony::GetProject() == nullptr)
	{
		HyGuiLog("DlgSliceSpriteSheet::DumpToTempDir() - No project loaded", LOGTYPE_Error);
		return QDir();
	}

	QDir rootTempDir = HyGlobal::PrepTempDir(*Harmony::GetProject(), HYGUIPATH_TEMPSUBDIR_SpriteSheets);
	if(false == rootTempDir.mkdir(ui->txtMainFilter->text()))
		HyGuiLog("DlgSliceSpriteSheet::DumpToTempDir - Could not make root temp directory", LOGTYPE_Error);
	if(false == rootTempDir.cd(ui->txtMainFilter->text()))
		HyGuiLog("DlgSliceSpriteSheet::DumpToTempDir - Could not change to root temp directory", LOGTYPE_Error);

	for(int iSheetIndex = 0; iSheetIndex < m_SheetList.size(); ++iSheetIndex)
	{
		QDir tempDir = rootTempDir;
		if(ui->chkCreateSubFilters->isChecked())
		{
			if(false == tempDir.mkdir(m_SheetList[iSheetIndex].sBaseAssetName))
				HyGuiLog("DlgSliceSpriteSheet::DumpToTempDir - Could not make sheet sub-directory", LOGTYPE_Error);
			if(false == tempDir.cd(m_SheetList[iSheetIndex].sBaseAssetName))
				HyGuiLog("DlgSliceSpriteSheet::DumpToTempDir - Could not change to sheet sub-directory", LOGTYPE_Error);
		}

		Sheet &sheetRef = m_SheetList[iSheetIndex];
		for(int iFrameIndex = 0; iFrameIndex < sheetRef.framePixmapList.size(); ++iFrameIndex)
		{
			QString sMetaName = sheetRef.sBaseAssetName;
			sMetaName += QString("_%1.png").arg(iFrameIndex, 3, 10, QChar('0'));
			sheetRef.framePixmapList[iFrameIndex].save(tempDir.absoluteFilePath(sMetaName));
		}
	}

	return rootTempDir;
}

void DlgSliceSpriteSheet::on_btnImageBrowse_clicked()
{
	QFileDialog dlg(this);
	QStringList sFilterList;
	sFilterList += "*.png";
	dlg.setNameFilters(sFilterList);
	dlg.setViewMode(QFileDialog::Detail);
	dlg.setWindowModality(Qt::ApplicationModal);
	dlg.setFileMode(QFileDialog::ExistingFiles);
	dlg.setModal(true);

	if(dlg.exec() == QDialog::Rejected)
		return;

	QStringList sImportImgList = dlg.selectedFiles();
	if(sImportImgList.empty())
		return;

	ui->txtMainFilter->setEnabled(true);
	ui->txtMainFilter->setText(dlg.directory().dirName());

	Clear();
	ui->txtImportPaths->setText(sImportImgList.join("; "));
	for(QString sImportImgPath : sImportImgList)
		m_SheetList.emplace(m_SheetList.end(), sImportImgPath);
		
	if(m_SheetList.size() >= 1)
	{
		ui->lblPreview->show();
		ui->chkCreateSubFilters->show();
	}
	else
	{
		ui->lblPreview->hide();
		ui->chkCreateSubFilters->hide();
	}

	AssemblePixmaps();
	SetPreviewIndex(0);
	ErrorCheck();
}

void DlgSliceSpriteSheet::on_txtMainFilter_textChanged(const QString &arg1)
{
	ErrorCheck();
}

void DlgSliceSpriteSheet::on_sbTileSizeX_valueChanged(int iArg)
{
	AssemblePixmaps();
	SetPreviewIndex(m_iCurrentSheetIndex);
	ErrorCheck();
}

void DlgSliceSpriteSheet::on_sbTileSizeY_valueChanged(int iArg)
{
	AssemblePixmaps();
	SetPreviewIndex(m_iCurrentSheetIndex);
	ErrorCheck();
}

void DlgSliceSpriteSheet::on_sbOffsetX_valueChanged(int iArg)
{
	AssemblePixmaps();
	SetPreviewIndex(m_iCurrentSheetIndex);
	ErrorCheck();
}

void DlgSliceSpriteSheet::on_sbOffsetY_valueChanged(int iArg)
{
	AssemblePixmaps();
	SetPreviewIndex(m_iCurrentSheetIndex);
	ErrorCheck();
}

void DlgSliceSpriteSheet::on_sbPaddingX_valueChanged(int iArg)
{
	AssemblePixmaps();
	SetPreviewIndex(m_iCurrentSheetIndex);
	ErrorCheck();
}

void DlgSliceSpriteSheet::on_sbPaddingY_valueChanged(int iArg)
{
	AssemblePixmaps();
	SetPreviewIndex(m_iCurrentSheetIndex);
	ErrorCheck();
}

void DlgSliceSpriteSheet::on_txtAssetsBaseName_textChanged(const QString &arg1)
{
	m_SheetList[m_iCurrentSheetIndex].sBaseAssetName = arg1;
	ErrorCheck();
}

void DlgSliceSpriteSheet::on_actionPreviewNext_triggered()
{
	if(m_iCurrentSheetIndex >= m_SheetList.size() - 1)
	{
		HyGuiLog("DlgSliceSpriteSheet::on_actionPreviewNext_triggered() - Already at last sprite sheet", LOGTYPE_Error);
		return;
	}
	SetPreviewIndex(m_iCurrentSheetIndex + 1);
	ErrorCheck();
}

void DlgSliceSpriteSheet::on_actionPreviewBack_triggered()
{
	if(m_iCurrentSheetIndex <= 0)
	{
		HyGuiLog("DlgSliceSpriteSheet::on_actionPreviewBack_triggered() - Already at first sprite sheet", LOGTYPE_Error);
		return;
	}
	SetPreviewIndex(m_iCurrentSheetIndex - 1);
	ErrorCheck();
}

void DlgSliceSpriteSheet::on_buttonBox_accepted()
{
	int iNumSheets = m_SheetList.size();
	int iNumImages = 0;
	for(int i = 0; i < iNumSheets; ++i)
		iNumImages += m_SheetList[i].framePixmapList.size();
	if(iNumImages == 0)
	{
		HyGuiLog("DlgSliceSpriteSheet::on_buttonBox_accepted - No images to import", LOGTYPE_Error);
		return;
	}

	if(iNumSheets == 1)
	{
		if(QMessageBox::Yes != QMessageBox::question(MainWindow::GetInstance(), "Confirm Import", "Do you want to import " % QString::number(iNumImages) % " images?", QMessageBox::Yes, QMessageBox::No))
			return;
	}
	else
	{
		if(QMessageBox::Yes != QMessageBox::question(MainWindow::GetInstance(), "Confirm Import", "Do you want to import " % QString::number(iNumImages) % " images from " % QString::number(iNumSheets) % " sprite sheets?", QMessageBox::Yes, QMessageBox::No))
			return;
	}

	// Proceed with import
	accept();
}

void DlgSliceSpriteSheet::AssemblePixmaps()
{
	if(m_SheetList.empty())
	{
		ui->grpSlicingOptions->setEnabled(false);
		ui->txtAssetsBaseName->setText("");
		ui->txtAssetsBaseName->setEnabled(false);
		ui->lblPreview->hide();
		ui->lblNumFrames->hide();
		return;
	}

	for(int iSheetIndex = 0; iSheetIndex < m_SheetList.size(); ++iSheetIndex)
	{
		Sheet &curSheet = m_SheetList[iSheetIndex];
		curSheet.framePixmapList.clear();

		if(ui->sbTileSizeX->value() == 0 || ui->sbTileSizeY->value() == 0)
			continue;

		QPoint ptCurPos(ui->sbOffsetX->value() + ui->sbPaddingX->value(),
						ui->sbOffsetY->value() + ui->sbPaddingY->value());

		QPainter painter;
		painter.setOpacity(0.0f);
		bool bCountColumns = true;
		int iNumColumns = 0;
		int iNumRows = 0;
		while(ui->sbTileSizeY->value() <= curSheet.sheetPixmap.height() - ptCurPos.y() + 1)
		{
			iNumRows++;

			while(ui->sbTileSizeX->value() <= curSheet.sheetPixmap.width() - ptCurPos.x() + 1)
			{
				curSheet.framePixmapList.emplace(curSheet.framePixmapList.end(), ui->sbTileSizeX->value(), ui->sbTileSizeY->value());
				QPixmap &pixmapRef = curSheet.framePixmapList.begin()[curSheet.framePixmapList.size() - 1];

				pixmapRef.fill(QColor(Qt::transparent));
				painter.begin(&pixmapRef);
				{
					QRect curRect(ptCurPos.x(), ptCurPos.y(), ui->sbTileSizeX->value(), ui->sbTileSizeY->value());
					painter.drawPixmap(QPoint(0, 0), curSheet.sheetPixmap, curRect);
				}
				painter.end();

				ptCurPos.setX(ptCurPos.x() + ui->sbTileSizeX->value() + ui->sbPaddingX->value());

				if(bCountColumns)
					iNumColumns++;
			}
			bCountColumns = false;

			ptCurPos.setX(ui->sbOffsetX->value() + ui->sbPaddingX->value());
			ptCurPos.setY(ptCurPos.y() + ui->sbTileSizeY->value() + ui->sbPaddingY->value());
		}
	}

	int iTotalFrames = 0;
	for(const Sheet &sheetRef : m_SheetList)
		iTotalFrames += sheetRef.framePixmapList.size();
	ui->lblNumFrames->show();
	ui->lblNumFrames->setText(QString("Total Frames: %1").arg(iTotalFrames));
}

void DlgSliceSpriteSheet::SetPreviewIndex(int iSheetIndex)
{
	if(iSheetIndex >= m_SheetList.count() || iSheetIndex < 0)
		return;

	m_iCurrentSheetIndex = iSheetIndex;
	ui->lblPreview->show();
	ui->lblPreview->setText(QString("Sprite Sheet %1 of %2").arg(m_iCurrentSheetIndex + 1).arg(m_SheetList.size()));

	ui->actionPreviewBack->setEnabled(m_iCurrentSheetIndex > 0);
	ui->actionPreviewNext->setEnabled(m_iCurrentSheetIndex < m_SheetList.size() - 1);


	delete m_pPreviewPixmapItem;
	m_pPreviewPixmapItem = nullptr;
	delete m_pPreviewPixmap;
	m_pPreviewPixmap = nullptr;

	QPoint ptCurPos(ui->sbOffsetX->value() + ui->sbPaddingX->value(),
		ui->sbOffsetY->value() + ui->sbPaddingY->value());
	bool bCountColumns = true;
	int iNumColumns = 0;
	int iNumRows = 0;
	Sheet &curSheet = m_SheetList[m_iCurrentSheetIndex];
	while(ui->sbTileSizeY->value() <= curSheet.sheetPixmap.height() - ptCurPos.y() + 1)
	{
		iNumRows++;

		while(ui->sbTileSizeX->value() <= curSheet.sheetPixmap.width() - ptCurPos.x() + 1)
		{
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

	ui->grpSlicingOptions->setEnabled(true);
	ui->txtAssetsBaseName->setEnabled(true);
	ui->txtAssetsBaseName->setText(curSheet.sBaseAssetName);

	int iSpacingAmt = 2;
	m_pPreviewPixmap = new QPixmap((ui->sbTileSizeX->value() * iNumColumns) + (iSpacingAmt * (iNumColumns + 1)),
		(ui->sbTileSizeY->value() * iNumRows) + (iSpacingAmt * (iNumRows + 1)));
	m_pPreviewPixmap->fill(QColor("black"));

	ptCurPos.setX(iSpacingAmt);
	ptCurPos.setY(iSpacingAmt);

	int iPixmapIndex = 0;
	QPainter painter;
	painter.begin(m_pPreviewPixmap);
	{
		for(int i = 0; i < iNumRows; ++i)
		{
			for(int j = 0; j < iNumColumns; ++j)
			{
				painter.eraseRect(ptCurPos.x(), ptCurPos.y(), ui->sbTileSizeX->value(), ui->sbTileSizeY->value());
				painter.drawPixmap(ptCurPos, curSheet.framePixmapList[iPixmapIndex]);
				iPixmapIndex++;

				ptCurPos.setX(ptCurPos.x() + ui->sbTileSizeX->value() + iSpacingAmt);
			}

			ptCurPos.setX(iSpacingAmt);
			ptCurPos.setY(ptCurPos.y() + ui->sbTileSizeY->value() + iSpacingAmt);
		}
	}
	painter.end();

	m_pPreviewPixmapItem = new QGraphicsPixmapItem(*m_pPreviewPixmap);
	m_pPreviewScene->addItem(m_pPreviewPixmapItem);
	ui->graphicsView->update();
}

void DlgSliceSpriteSheet::ErrorCheck()
{
	bool bIsError = false;
	do
	{
		if(m_SheetList.size() == 0)
		{
			ui->lblError->setText("Error: No sprite sheets selected");
			bIsError = true;
			break;
		}

		if(ui->txtMainFilter->text().isEmpty())
		{
			ui->lblError->setText("Error: Main Filter cannot be empty");
			bIsError = true;
			break;
		}

		int iNumFrames = 0;
		for(int i = 0; i < m_SheetList.size(); ++i)
		{
			if(m_SheetList[i].sBaseAssetName.isEmpty())
			{
				ui->lblError->setText("Error: Base Asset Name for sheet " % QString::number(i+1) % " cannot be empty");
				bIsError = true;
				break;
			}
			iNumFrames += m_SheetList[i].framePixmapList.size();
		}
		if(bIsError)
			break;

		if(iNumFrames == 0)
		{
			ui->lblError->setText("Error: No frames to import");
			bIsError = true;
			break;
		}

	} while(false);

	ui->lblError->setVisible(bIsError);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!bIsError);
}
