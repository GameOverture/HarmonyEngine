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

DlgImportTileSheet::DlgImportTileSheet(Project &projectRef, QWidget *parent) :
	QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
	m_ProjectRef(projectRef),
	m_pOrigPixmap(nullptr),
	m_pPreviewPixmap(nullptr),
	ui(new Ui::DlgImportTileSheet)
{
	ui->setupUi(this);

	ui->txtTilePrefix->setValidator(HyGlobal::FreeFormValidator());
	ui->lblError->setStyleSheet("QLabel { background-color : red; color : black; }");

	ErrorCheck();
}

DlgImportTileSheet::~DlgImportTileSheet()
{
	delete ui;
}

void DlgImportTileSheet::on_buttonBox_accepted()
{
	if(QMessageBox::Yes != QMessageBox::question(MainWindow::GetInstance(), "Confirm Batch Import", "Do you want to import '" % QString::number(m_TilePixmaps.size()) % "' images?", QMessageBox::Yes, QMessageBox::No))
		return;

	QDir tempDir = HyGlobal::PrepTempDir(m_ProjectRef, HYGUIPATH_TEMPSUBDIR_ImportTileSheet);

	char szBuffer[16];
	uint uiPixmapIndex = 0;
	for(auto pPixmap : m_TilePixmaps)
	{
		QString sMetaName = ui->txtTilePrefix->text();
		sprintf(szBuffer, "%05u.png", uiPixmapIndex);
		sMetaName += szBuffer;
		
		pPixmap->save(tempDir.absoluteFilePath(sMetaName));
		uiPixmapIndex++;
	}

	QFileInfoList imageFileList = tempDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
	QStringList sImageImportList;
	for(auto fileInfo : imageFileList)
		sImageImportList << fileInfo.absoluteFilePath();

	quint32 uiBankId = m_ProjectRef.GetAtlasWidget() ? m_ProjectRef.GetAtlasWidget()->GetSelectedBankId() : 0;

	QList<IAssetItemData *> selectedAssetsList;
	QList<TreeModelItemData *> selectedFiltersList;
	TreeModelItemData *pFirstSelected = nullptr;
	if(m_ProjectRef.GetAtlasWidget())
		pFirstSelected = m_ProjectRef.GetAtlasWidget()->GetSelected(selectedAssetsList, selectedFiltersList);

	TreeModelItemData *pParent = m_ProjectRef.GetAtlasModel().FindTreeItemFilter(pFirstSelected);

	QList<TreeModelItemData *> correspondingParentList;
	QList<QUuid> correspondingUuidList;
	for(int i = 0; i < sImageImportList.size(); ++i)
	{
		correspondingParentList.append(pParent);
		correspondingUuidList.append(QUuid::createUuid());
	}

	m_ProjectRef.GetAtlasModel().ImportNewAssets(sImageImportList, uiBankId, ITEM_AtlasFrame, correspondingParentList, correspondingUuidList);
}

void DlgImportTileSheet::on_btnImageBrowse_clicked()
{
	QFileDialog dlg(this);
	dlg.setWindowTitle("Select Tile Sheet Image");
	dlg.setFileMode(QFileDialog::ExistingFile);
	dlg.setViewMode(QFileDialog::Detail);
	dlg.setWindowModality(Qt::ApplicationModal);
	dlg.setModal(true);

	QStringList sFilterList;
	sFilterList += "*.png";
	dlg.setNameFilters(sFilterList);

	if(dlg.exec() == QDialog::Rejected)
		return;

	QStringList sImportImgList = dlg.selectedFiles();
	if(sImportImgList.empty())
		return;

	ui->txtImagePath->setText(sImportImgList[0]);
	
	if(m_pOrigPixmap)
		delete m_pOrigPixmap;
	m_pOrigPixmap = new QPixmap(sImportImgList[0]);

	AssemblePixmaps();
	ErrorCheck();
}

void DlgImportTileSheet::on_sbGridX_valueChanged(int iArg)
{
	AssemblePixmaps();
	ErrorCheck();
}

void DlgImportTileSheet::on_sbGridY_valueChanged(int iArg)
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

void DlgImportTileSheet::AssemblePixmaps()
{
	if(m_pPreviewPixmap)
		delete m_pPreviewPixmap;

	ui->lblImage->setText("<Select Image>");

	if(m_pOrigPixmap == nullptr || ui->sbGridX->value() == 0 || ui->sbGridY->value() == 0)
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
	while(ui->sbGridY->value() <= m_pOrigPixmap->height() - ptCurPos.y() + 1)
	{
		iNumRows++;

		while(ui->sbGridX->value() <= m_pOrigPixmap->width() - ptCurPos.x() + 1)
		{
			QPixmap *pNewPixmap = new QPixmap(ui->sbGridX->value(), ui->sbGridY->value());
			pNewPixmap->fill(QColor(Qt::transparent));
			painter.begin(pNewPixmap);
			{
				QRect curRect(ptCurPos.x(), ptCurPos.y(), ui->sbGridX->value(), ui->sbGridY->value());
				painter.drawPixmap(QPoint(0, 0), *m_pOrigPixmap, curRect);
			}
			painter.end();
			m_TilePixmaps.append(pNewPixmap);

			ptCurPos.setX(ptCurPos.x() + ui->sbGridX->value() + ui->sbPaddingX->value());

			if(bCountColumns)
				iNumColumns++;
		}
		bCountColumns = false;

		ptCurPos.setX(ui->sbOffsetX->value() + ui->sbPaddingX->value());
		ptCurPos.setY(ptCurPos.y() + ui->sbGridY->value() + ui->sbPaddingY->value());
	}

	if(iNumColumns == 0 || iNumRows == 0)
		return;

	int iSpacingAmt = 2;
	m_pPreviewPixmap = new QPixmap((ui->sbGridX->value() * iNumColumns) + (iSpacingAmt * (iNumColumns+1)),
								   (ui->sbGridY->value() * iNumRows) + (iSpacingAmt * (iNumRows+1)));
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
				painter.eraseRect(ptCurPos.x(), ptCurPos.y(), ui->sbGridX->value(), ui->sbGridY->value());
				painter.drawPixmap(ptCurPos, *m_TilePixmaps[iPixmapIndex]);
				iPixmapIndex++;

				ptCurPos.setX(ptCurPos.x() + ui->sbGridX->value() + iSpacingAmt);
			}

			ptCurPos.setX(iSpacingAmt);
			ptCurPos.setY(ptCurPos.y() + ui->sbGridY->value() + iSpacingAmt);
		}
	}
	painter.end();

	ui->lblImage->clear();
	ui->lblImage->setPixmap(*m_pPreviewPixmap);
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
