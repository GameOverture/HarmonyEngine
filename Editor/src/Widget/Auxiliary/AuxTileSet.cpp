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
#include "WgtTileSetAnimation.h"
#include "WgtTileSetTerrainSet.h"
#include "WgtTileSetTerrain.h"
#include "TileData.h"

#include <QMessageBox>
#include <QFileDialog>

const int g_iDefaultTileSize = 32;

AuxTileSet::AuxTileSet(QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::AuxTileSet),
	m_pTileSet(nullptr),
	m_pSelectedAnimationWgt(nullptr),
	m_pSelectedTerrainSetWgt(nullptr),
	m_pSelectedTerrainWgt(nullptr),
	m_bIsImportingTileSheet(true),
	m_pImportTileSheetPixmap(nullptr)
{
	ui->setupUi(this);
	
	m_pTabBar = new QTabBar(ui->tabFrame);
	m_pTabBar->addTab(QIcon(QString::fromUtf8(":/icons16x16/generic-add.png")), tr("Import"));
	m_pTabBar->addTab(QIcon(QString::fromUtf8(":/icons16x16/tileset-arrange.png")), tr("Arrange"));
	m_pTabBar->addTab(QIcon(QString::fromUtf8(":/icons16x16/media-play.png")), tr("Animation"));
	m_pTabBar->addTab(QIcon(QString::fromUtf8(":/icons16x16/tileset-autotile.png")), tr("Autotile"));
	m_pTabBar->addTab(QIcon(QString::fromUtf8(":/icons16x16/collision.png")), tr("Collision"));
	m_pTabBar->addTab(QIcon(QString::fromUtf8(":/icons16x16/generic-rename.png")), tr("Custom Data"));
	connect(m_pTabBar, SIGNAL(currentChanged(int)), this, SLOT(OnTabBarChanged(int)));
	ui->tabFrame->resize(m_pTabBar->sizeHint().width(), ui->tabFrame->height());

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

	ui->splitter->setSizes(QList<int>() << 100 << width() - 100);
	ui->splitter->setCollapsible(0, false);
	ui->splitter->setCollapsible(1, false);

	connect(ui->vsbTileSize, SIGNAL(ValueChanged(QVariant)), this, SLOT(OnTileSizeChanged(QVariant)));
	connect(ui->vsbTileOffset, SIGNAL(ValueChanged(QVariant)), this, SLOT(OnTileOffsetChanged(QVariant)));

	connect(ui->vsbTextureRegion, SIGNAL(ValueChanged(QVariant)), this, SLOT(OnTextureRegionChanged(QVariant)));
	connect(ui->vsbStartOffset, SIGNAL(ValueChanged(QVariant)), this, SLOT(OnStartOffsetChanged(QVariant)));
	connect(ui->vsbPadding, SIGNAL(ValueChanged(QVariant)), this, SLOT(OnPaddingChanged(QVariant)));

	ui->btnReplaceTiles->setDefaultAction(ui->actionReplaceTiles);
	ui->btnDeleteTiles->setDefaultAction(ui->actionRemoveTiles);
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
	m_pSelectedAnimationWgt = nullptr;
	m_pSelectedTerrainSetWgt = nullptr;
	m_pSelectedTerrainWgt = nullptr;

	if(m_pTileSet == nullptr)
		return;
	
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

	for(WgtTileSetAnimation *pAnim : m_AnimationList)
		delete pAnim;
	m_AnimationList.clear();
	QVector<QJsonObject> animObjList = m_pTileSet->GetAnimations();
	for(QJsonObject animObj : animObjList)
		CmdSet_CreateWgtItem(TILESETWGT_Animation, animObj);

	for (WgtTileSetTerrainSet *pTerrain : m_TerrainSetList)
		delete pTerrain;
	m_TerrainSetList.clear();
	QVector<QJsonObject> terrainSetObjList = m_pTileSet->GetTerrainSets();
	for (QJsonObject terrainSetObj : terrainSetObjList)
		CmdSet_CreateWgtItem(TILESETWGT_TerrainSet, terrainSetObj);

	// TODO: physics here
	

	if(m_pTileSet->GetNumTiles() == 0)
		m_pTabBar->setCurrentIndex(TILESETPAGE_Import);
	else
		m_pTabBar->setCurrentIndex(TILESETPAGE_Arrange);
	OnTabBarChanged(m_pTabBar->currentIndex());
	
	ui->graphicsView->SetScene(this, m_pTileSet->GetGfxScene());
	ui->graphicsView->centerOn(m_pTileSet->GetGfxScene()->GetFocusPt(GetCurrentPage()));
	
	RefreshInfo();
	UpdateGfxItemSelection();
}

AtlasTileSet *AuxTileSet::GetTileSet() const
{
	return m_pTileSet;
}

TileSetPage AuxTileSet::GetCurrentPage() const
{
	return static_cast<TileSetPage>(ui->setupStackedWidget->currentIndex());
}

//m_pTileSet->GetGfxScene()->GetFocusPt(ePage)

void AuxTileSet::SetCurrentPage(TileSetPage ePage)
{
	ui->setupStackedWidget->setCurrentIndex(static_cast<int>(ePage));
	m_pTileSet->GetGfxScene()->OnTileSetPageChange(ePage);
	ui->graphicsView->centerOn(m_pTileSet->GetGfxScene()->GetFocusPt(ePage));

	switch (ePage)
	{
	case TILESETPAGE_Animation:
		for(WgtTileSetAnimation *pAnimWgt : m_AnimationList)
		{
			if(pAnimWgt->IsSelected())
			{
				MakeSelectionChange(pAnimWgt);
				break;
			}
		}
		break;

	case TILESETPAGE_Autotile:
		for (WgtTileSetTerrainSet *pTerrainSetWgt : m_TerrainSetList)
		{
			if (pTerrainSetWgt->IsSelected())
			{
				bool bTerrainSelected = false;
				for (WgtTileSetTerrain *pTerrain : pTerrainSetWgt->GetTerrains())
				{
					if (pTerrain->IsSelected())
					{
						MakeSelectionChange(pTerrain);
						bTerrainSelected = true;
						break;
					}
				}
				if(bTerrainSelected == false)
					MakeSelectionChange(pTerrainSetWgt);
				break;
			}
		}
		break;
	}
}

QUuid AuxTileSet::GetSelectedAnimation() const
{
	if(m_pSelectedAnimationWgt)
		return m_pSelectedAnimationWgt->GetUuid();
	return QUuid();
}

QUuid AuxTileSet::GetSelectedTerrainSet() const
{
	if(m_pSelectedTerrainSetWgt)
		return m_pSelectedTerrainSetWgt->GetUuid();
	return QUuid();
}

QUuid AuxTileSet::GetSelectedTerrain() const
{
	if(m_pSelectedTerrainWgt)
		return m_pSelectedTerrainWgt->GetUuid();
	return QUuid();
}

void AuxTileSet::CmdSet_TileShapeWidget(TileSetShape eTileShape)
{
	ui->cmbTileShape->blockSignals(true);
	if(eTileShape == TILESETSHAPE_Unknown)
		ui->cmbTileShape->setCurrentIndex(0);
	else
		ui->cmbTileShape->setCurrentIndex(eTileShape);

	m_pTileSet->SetTileShape(eTileShape);
	m_pTileSet->GetGfxScene()->RefreshImportTiles();
	m_pTileSet->GetGfxScene()->RefreshSetupTiles(GetCurrentPage());

	ui->cmbTileShape->blockSignals(false);
}

void AuxTileSet::CmdSet_TileSizeWidgets(QSize tileSize)
{
	ui->vsbTileSize->blockSignals(true);
	ui->vsbTileSize->SetValue(QPoint(tileSize.width(), tileSize.height()));

	m_pTileSet->SetTileSize(tileSize);
	m_pTileSet->GetGfxScene()->RefreshImportTiles();
	m_pTileSet->GetGfxScene()->RefreshSetupTiles(GetCurrentPage());

	ui->vsbTileSize->blockSignals(false);
}

void AuxTileSet::CmdSet_TileOffsetWidgets(QPoint tileOffset)
{
	ui->vsbTileOffset->blockSignals(true);
	ui->vsbTileOffset->SetValue(tileOffset);

	m_pTileSet->SetTileOffset(tileOffset);
	m_pTileSet->GetGfxScene()->RefreshImportTiles();
	m_pTileSet->GetGfxScene()->RefreshSetupTiles(GetCurrentPage());

	ui->vsbTileOffset->blockSignals(false);
}

void AuxTileSet::RefreshInfo()
{
	ui->lblIcon->setPixmap(m_pTileSet->GetTileSetIcon().pixmap(QSize(16,16)));
	ui->lblName->setText(m_pTileSet->GetName());
	ui->lblInfo->setText(m_pTileSet->GetTileSetInfo());

	m_pTileSet->GetSaveAction()->setEnabled(m_pTileSet->IsSaveClean() == false);
}

void AuxTileSet::UpdateGfxItemSelection()
{
	ui->btnConfirmAdd->setText("Import " % QString::number(m_pTileSet->GetGfxScene()->GetNumImportPixmaps()) % " Tiles");
	ErrorCheckImport();

	int iNumSetupSelected = m_pTileSet->GetGfxScene()->GetNumSetupSelected();
	if(iNumSetupSelected == 1)
		ui->lblArrangeSelectedTiles->setText(QString::number(iNumSetupSelected) % " Selected Tile");
	else
		ui->lblArrangeSelectedTiles->setText(QString::number(iNumSetupSelected) % " Selected Tiles");
	ui->actionReplaceTiles->setEnabled(iNumSetupSelected > 0);
	ui->actionRemoveTiles->setEnabled(iNumSetupSelected > 0);
}

IWgtTileSetItem *AuxTileSet::FindWgtItem(QUuid uuid) const
{
	for (WgtTileSetAnimation *pAnimationWidget : m_AnimationList)
	{
		if (pAnimationWidget->GetUuid() == uuid)
			return pAnimationWidget;
	}
	for (WgtTileSetTerrainSet *pTerrainSetWidget : m_TerrainSetList)
	{
		if (pTerrainSetWidget->GetUuid() == uuid)
			return pTerrainSetWidget;

		QList<WgtTileSetTerrain *> terrains = pTerrainSetWidget->GetTerrains();
		for (WgtTileSetTerrain *pTerrainWidget : terrains)
		{
			if (pTerrainWidget->GetUuid() == uuid)
				return pTerrainWidget;
		}
	}
	return nullptr;
}

int AuxTileSet::GetWgtItemIndex(QUuid uuid) const
{
	for (int i = 0; i < m_AnimationList.size(); ++i)
	{
		WgtTileSetAnimation *pAnimationWidget = m_AnimationList[i];
		if (pAnimationWidget->GetUuid() == uuid)
			return i;
	}
	for (int i = 0; i < m_TerrainSetList.size(); ++i)
	{
		WgtTileSetTerrainSet *pTerrainWidget = m_TerrainSetList[i];
		if (pTerrainWidget->GetUuid() == uuid)
			return i;
		QList<WgtTileSetTerrain *> terrains = pTerrainWidget->GetTerrains();
		for (int j = 0; j < terrains.size(); ++j)
		{
			WgtTileSetTerrain *pTerrainSubWidget = terrains[j];
			if (pTerrainSubWidget->GetUuid() == uuid)
				return j;
		}
	}

	HyGuiLog("AuxTileSet::GetWgtItemIndex: Widget with specified UUID not found!", LOGTYPE_Error);
	return -1;
}

void AuxTileSet::CmdSet_CreateWgtItem(TileSetWgtType eType, QJsonObject data)
{
	if(data.contains("UUID") == false)
	{
		HyGuiLog("AuxTileSet::CmdSet_AddWgtItem: Widget data missing UUID!", LOGTYPE_Error);
		return;
	}
	if(FindWgtItem(QUuid(data.value("UUID").toString())) != nullptr)
	{
		HyGuiLog("AuxTileSet::CmdSet_AddWgtItem: Widget with same UUID already exists!", LOGTYPE_Error);
		return;
	}

	switch (eType)
	{
	case TILESETWGT_Animation: {
		WgtTileSetAnimation *pNewAnim = new WgtTileSetAnimation(this, data);
		ui->lytAnimations->addWidget(pNewAnim);
		m_AnimationList.append(pNewAnim);
		pNewAnim->SetOrderBtns(m_AnimationList.size() > 1, false);
		
		MakeSelectionChange(pNewAnim);
		break; }

	case TILESETWGT_TerrainSet: {
		WgtTileSetTerrainSet *pNewTerrainSet = new WgtTileSetTerrainSet(this, data);
		ui->lytTerrainSets->addWidget(pNewTerrainSet);
		m_TerrainSetList.append(pNewTerrainSet);
		pNewTerrainSet->SetOrderBtns(m_TerrainSetList.size() > 1, false);
		
		MakeSelectionChange(pNewTerrainSet);
		break; }

	case TILESETWGT_Terrain: {
		WgtTileSetTerrainSet *pParentTerrain = static_cast<WgtTileSetTerrainSet *>(FindWgtItem(QUuid(data.value("terrainSetUUID").toString())));
		if (pParentTerrain == nullptr)
		{
			HyGuiLog("AuxTileSet::CmdSet_AddWgtItem: Terrain's parent TerrainSet widget not found!", LOGTYPE_Error);
			return;
		}

		pParentTerrain->CmdSet_AllocTerrain(data);
		break; }

	default:
		HyGuiLog("AuxTileSet::CmdSet_AddWgtItem: Unknown TileSetWgtType!", LOGTYPE_Error);
		return;
	}
}

void AuxTileSet::CmdSet_DeleteWgtItem(QUuid uuid)
{
	for (int i = 0; i < m_AnimationList.size(); ++i)
	{
		WgtTileSetAnimation *pAnimationWidget = m_AnimationList[i];
		if (pAnimationWidget->GetUuid() == uuid)
		{
			ui->lytAnimations->removeWidget(pAnimationWidget);
			m_AnimationList.removeAt(i);
			delete pAnimationWidget;
			m_pTileSet->Cmd_RemoveJsonItem(uuid);
			return;
		}
	}
	for (int i = 0; i < m_TerrainSetList.size(); ++i)
	{
		WgtTileSetTerrainSet *pTerrainWidget = m_TerrainSetList[i];
		if (pTerrainWidget->GetUuid() == uuid)
		{
			ui->lytTerrainSets->removeWidget(pTerrainWidget);
			m_TerrainSetList.removeAt(i);
			delete pTerrainWidget;
			m_pTileSet->Cmd_RemoveJsonItem(uuid);
			return;
		}

		QList<WgtTileSetTerrain *> terrains = pTerrainWidget->GetTerrains();
		for (int j = 0; j < terrains.size(); ++j)
		{
			WgtTileSetTerrain *pTerrainSubWidget = terrains[j];
			if (pTerrainSubWidget->GetUuid() == uuid)
			{
				pTerrainWidget->CmdSet_DeleteTerrain(uuid);
				m_pTileSet->Cmd_RemoveJsonItem(uuid);
				return;
			}
		}
	}

	HyGuiLog("AuxTileSet::CmdSet_DeleteWgtItem: Widget with specified UUID not found!", LOGTYPE_Error);
}

void AuxTileSet::CmdSet_OrderWgtItem(QUuid uuid, int newIndex)
{
	for (int i = 0; i < m_AnimationList.size(); ++i)
	{
		WgtTileSetAnimation *pAnimationWidget = m_AnimationList[i];
		if (pAnimationWidget->GetUuid() == uuid)
		{
			ui->lytAnimations->removeWidget(pAnimationWidget);
			m_AnimationList.removeAt(i);
			ui->lytAnimations->insertWidget(newIndex, pAnimationWidget);
			m_AnimationList.insert(newIndex, pAnimationWidget);

			pAnimationWidget->SetOrderBtns(newIndex > 0, newIndex < m_AnimationList.size() - 1);
			return;
		}
	}
	for (int i = 0; i < m_TerrainSetList.size(); ++i)
	{
		WgtTileSetTerrainSet *pTerrainWidget = m_TerrainSetList[i];
		if (pTerrainWidget->GetUuid() == uuid)
		{
			ui->lytTerrainSets->removeWidget(pTerrainWidget);
			m_TerrainSetList.removeAt(i);
			ui->lytTerrainSets->insertWidget(newIndex, pTerrainWidget);
			m_TerrainSetList.insert(newIndex, pTerrainWidget);

			pTerrainWidget->SetOrderBtns(newIndex > 0, newIndex < m_TerrainSetList.size() - 1);
			return;
		}

		QList<WgtTileSetTerrain *> terrains = pTerrainWidget->GetTerrains();
		for (WgtTileSetTerrain *pTerrainSubWidget : terrains)
		{
			if (pTerrainSubWidget->GetUuid() == uuid)
			{
				pTerrainWidget->CmdSet_OrderTerrain(uuid, newIndex);
				return;
			}
		}
	}
	HyGuiLog("AuxTileSet::CmdSet_OrderWgtItem: Widget with specified UUID not found!", LOGTYPE_Error);
}

void AuxTileSet::CmdSet_ModifyWgtItem(QUuid uuid, QJsonObject newData)
{
	GetTileSet()->Cmd_SetJsonItem(uuid, newData);
	
	IWgtTileSetItem *pItem = FindWgtItem(uuid);
	pItem->Init(newData);
}

void AuxTileSet::MakeSelectionChange(IWgtTileSetItem *pItem)
{
	switch (pItem->GetWgtType())
	{
	case TILESETWGT_Animation:
		m_pSelectedAnimationWgt = static_cast<WgtTileSetAnimation *>(pItem);

		for (WgtTileSetAnimation *pAnimationWidget : m_AnimationList)
			pAnimationWidget->SetSelected(m_pSelectedAnimationWgt == pAnimationWidget);
		break;
	case TILESETWGT_TerrainSet:
		m_pSelectedTerrainSetWgt = static_cast<WgtTileSetTerrainSet *>(pItem);

		for (WgtTileSetTerrainSet *pTerrainSetWidget : m_TerrainSetList)
			pTerrainSetWidget->SetSelected(m_pSelectedTerrainSetWgt == pTerrainSetWidget);
		break;
	case TILESETWGT_Terrain: {
		m_pSelectedTerrainWgt = static_cast<WgtTileSetTerrain *>(pItem);

		WgtTileSetTerrainSet *pTerrainSetWidget = static_cast<WgtTileSetTerrain *>(m_pSelectedTerrainWgt)->GetParentTerrainSet();
		QList<WgtTileSetTerrain *> terrainList = pTerrainSetWidget->GetTerrains();
		for (WgtTileSetTerrain *pTerrainWidget : terrainList)
			pTerrainWidget->SetSelected(m_pSelectedTerrainWgt == pTerrainWidget);
		break; }

	default:
		HyGuiLog("AuxTileSet::MakeSelectionChange: Unknown GetWgtType!", LOGTYPE_Error);
		break;
	}
}

void AuxTileSet::CmdSet_ApplyTerrainSet(QList<TileData *> tileDataList, QList<QUuid> terrainSetUuidList)
{
	for(int i = 0; i < tileDataList.size(); ++i)
	{
		tileDataList[i]->SetTerrainSet(terrainSetUuidList[i]);
		m_pTileSet->GetGfxScene()->OnTerrainSetApplied(tileDataList[i]);
	}

	m_pTileSet->GetGfxScene()->RefreshSetupTiles(GetCurrentPage());
}

void AuxTileSet::SetPainting_Animation(QUuid animUuid)
{
	IWgtTileSetItem *pWgtItem = FindWgtItem(animUuid);
	if(pWgtItem == nullptr)
	{
		HyGuiLog("AuxTileSet::SetPainting_Animation() - TileSet Item not found", LOGTYPE_Error);
		return;
	}
	if(pWgtItem->GetWgtType() != TILESETWGT_Animation)
	{
		HyGuiLog("AuxTileSet::SetPainting_Animation() - Wrong TileSet Item", LOGTYPE_Error);
		return;
	}

	WgtTileSetAnimation *pAnimItem = static_cast<WgtTileSetAnimation *>(pWgtItem);
	pAnimItem->GetUuid();

	ui->graphicsView->SetStatusLabel(pAnimItem->GetName() + " - Select Animation Frames");

}

void AuxTileSet::SetImportWidgets()
{
	bool bTileSheet = ui->radTileSheet->isChecked();
	if(m_bIsImportingTileSheet != bTileSheet)
	{
		bool bHasPendingInfo = ui->txtImagePath->text().isEmpty() == false;/* ||
			ui->vsbTextureRegion->GetValue().toSize() != m_pTileSet->GetAtlasRegionSize() ||
			ui->vsbStartOffset->GetValue().toPoint() != m_pTileSet->GetTileOffset() ||
			ui->vsbPadding->GetValue().toPoint() != QPoint(0, 0);*/

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
	m_pTileSet->GetGfxScene()->OnTileSetPageChange(GetCurrentPage());
	ui->graphicsView->centerOn(m_pTileSet->GetGfxScene()->GetFocusPt(GetCurrentPage()));

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

			pGfxScene->AddTile(true, nullptr, m_pTileSet->GetTilePolygon(), ptGridPos, tilePixmap, IsPixmapAllTransparent(tilePixmap) == false);

			ptCurPos.setX(ptCurPos.x() + vRegionSize.x() + vPadding.x());
			ptGridPos.setX(ptGridPos.x() + 1);
		}

		ptCurPos.setX(vStartOffset.x() + vPadding.x());
		ptCurPos.setY(ptCurPos.y() + vRegionSize.y() + vPadding.y());
		ptGridPos.setX(0);
		ptGridPos.setY(ptGridPos.y() + 1);
	}

	pGfxScene->RefreshImportTiles();
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

			pGfxScene->AddTile(true, nullptr, m_pTileSet->GetTilePolygon(), QPoint(iIndex % iNUM_COLS, iIndex / iNUM_COLS), pixmap, IsPixmapAllTransparent(pixmap) == false);
			iIndex++;
		}

		for(auto pImg : vImportImages)
			delete pImg;

		pGfxScene->RefreshImportTiles();
	}

	ui->graphicsView->centerOn(m_pTileSet->GetGfxScene()->GetFocusPt(GetCurrentPage()));

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
	m_pTileSet->GetGfxScene()->RefreshImportTiles();
}

void AuxTileSet::on_radImportTop_toggled(bool bChecked)
{
	m_pTileSet->GetGfxScene()->SetImportAppendEdge(Qt::TopEdge);
	m_pTileSet->GetGfxScene()->RefreshImportTiles();
}

void AuxTileSet::on_radImportLeft_toggled(bool bChecked)
{
	m_pTileSet->GetGfxScene()->SetImportAppendEdge(Qt::LeftEdge);
	m_pTileSet->GetGfxScene()->RefreshImportTiles();
}

void AuxTileSet::on_radImportRight_toggled(bool bChecked)
{
	m_pTileSet->GetGfxScene()->SetImportAppendEdge(Qt::RightEdge);
	m_pTileSet->GetGfxScene()->RefreshImportTiles();
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

	// TODO: Do a check to see if the resulting sub-atlas is larger than all Atlas manager banks can handle, and ask to proceed or cancel

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

	TileSetUndoCmd_AppendTiles *pUndoCmd = new TileSetUndoCmd_AppendTiles(*this, importMap, vImportRegionSize, eAppendEdge);
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

void AuxTileSet::on_actionRemoveTiles_triggered()
{
	TileSetUndoCmd_RemoveTiles *pNewCmd = new TileSetUndoCmd_RemoveTiles(*this);
	m_pTileSet->GetUndoStack()->push(pNewCmd);
}

void AuxTileSet::on_actionReplaceTiles_triggered()
{
}

void AuxTileSet::on_btnAddAnimation_clicked()
{
	QString sAnimName = "Animation ";
	sAnimName += QString::number(m_AnimationList.size() + 1);
	HyColor color = HyColor::Blue;
	QJsonObject animObj = AtlasTileSet::GenerateNewAnimationJsonObject(sAnimName, color);

	TileSetUndoCmd_AddWgtItem *pNewCmd = new TileSetUndoCmd_AddWgtItem(*this, TILESETWGT_Animation, animObj);
	m_pTileSet->GetUndoStack()->push(pNewCmd);
}

void AuxTileSet::on_btnAddTerrainSet_clicked()
{
	QJsonObject initObj = AtlasTileSet::GenerateNewTerrainSetJsonObject();

	TileSetUndoCmd_AddWgtItem *pNewCmd = new TileSetUndoCmd_AddWgtItem(*this, TILESETWGT_TerrainSet, initObj);
	m_pTileSet->GetUndoStack()->push(pNewCmd);
}

void AuxTileSet::OnTabBarChanged(int iIndex)
{
	if(m_pTileSet == nullptr)
		return;

	ui->setupStackedWidget->setCurrentIndex(iIndex);

	m_pTileSet->GetGfxScene()->ClearSetupSelection();

	switch (iIndex)
	{
	case TILESETPAGE_Import:
		ui->graphicsView->SetStatusLabel("Importing New Tiles...");
		ui->grpImportSide->setVisible(m_pTileSet->GetNumTiles() > 0);
		break;
	case TILESETPAGE_Arrange:
		ui->graphicsView->SetStatusLabel("Arrange, Replace, Delete", 2000);
		break;
	case TILESETPAGE_Animation:
		ui->graphicsView->SetStatusLabel("Animation Setup", 2000);
		break;
	case TILESETPAGE_Autotile:
		ui->graphicsView->SetStatusLabel("Autotile Setup", 2000);
		break;
	case TILESETPAGE_Collision:
		ui->graphicsView->SetStatusLabel("Collision Setup", 2000);
		break;
	case TILESETPAGE_CustomData:
		ui->graphicsView->SetStatusLabel("Custom Data Setup", 2000);
		break;
	default:
		HyGuiLog("AuxTileSet::on_setupToolBox_currentChanged - Unknown TileSetPage index: " % QString::number(iIndex), LOGTYPE_Error);
		break;
	}

	m_pTileSet->GetGfxScene()->OnTileSetPageChange(static_cast<TileSetPage>(iIndex));
	ui->graphicsView->centerOn(m_pTileSet->GetGfxScene()->GetFocusPt(static_cast<TileSetPage>(iIndex)));
}
