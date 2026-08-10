/**************************************************************************
 *	AuxTileMap.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AuxTileMap.h"
#include "ui_AuxTileMap.h"
#include "AtlasManager.h"

#include <QActionGroup>
#include <QResizeEvent>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TileSetTreeView::TileSetTreeView(QWidget *pParent /*= 0*/) :
//	QTreeView(pParent)
//{
//}
///*virtual*/ void TileSetTreeView::resizeEvent(QResizeEvent *pResizeEvent)
//{
//	// TODO: Use formula to account for device pixels and scaling using QWindow::devicePixelRatio()
//	int iWidth = pResizeEvent->size().width();
//
//	iWidth -= 50;
//	setColumnWidth(TileSetsTreeModel::COLUMN_Name, iWidth);
//	setColumnWidth(TileSetsTreeModel::COLUMN_Info, 50);
//
//	QTreeView::resizeEvent(pResizeEvent);
//}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AuxTileMap::AuxTileMap(QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::AuxTileMap),
	m_pTileMapGfxScene(nullptr)
{
	ui->setupUi(this);

	ui->actionSelectTool->setData(TILEMAPTOOL_Select);
	ui->actionPaintTool->setData(TILEMAPTOOL_Paint);
	ui->actionRectTool->setData(TILEMAPTOOL_Rect);
	ui->actionCircleTool->setData(TILEMAPTOOL_Circle);
	ui->actionFillTool->setData(TILEMAPTOOL_Fill);
	ui->actionPickerTool->setData(TILEMAPTOOL_Picker);
	ui->actionEraserTool->setData(TILEMAPTOOL_Eraser);

	m_pToolActionGroup = new QActionGroup(this);
	m_pToolActionGroup->setExclusive(true);
	m_pToolActionGroup->addAction(ui->actionSelectTool);
	m_pToolActionGroup->addAction(ui->actionPaintTool);
	m_pToolActionGroup->addAction(ui->actionRectTool);
	m_pToolActionGroup->addAction(ui->actionCircleTool);
	m_pToolActionGroup->addAction(ui->actionFillTool);
	m_pToolActionGroup->addAction(ui->actionPickerTool);
	m_pToolActionGroup->addAction(ui->actionEraserTool);

	m_pToolBar = new QToolBar(ui->toolBarFrame);
	m_pToolBar->addAction(ui->actionSelectTool);
	m_pToolBar->addSeparator();
	m_pToolBar->addAction(ui->actionPaintTool);
	m_pToolBar->addAction(ui->actionRectTool);
	m_pToolBar->addAction(ui->actionCircleTool);
	m_pToolBar->addAction(ui->actionFillTool);
	m_pToolBar->addSeparator();
	m_pToolBar->addAction(ui->actionPickerTool);
	m_pToolBar->addAction(ui->actionEraserTool);
	m_pToolBar->addSeparator();
	m_pToolBar->addAction(ui->actionRotateTileLeft);
	m_pToolBar->addAction(ui->actionRotateTileRight);
	m_pToolBar->addAction(ui->actionFlipTileH);
	m_pToolBar->addAction(ui->actionFlipTileV);

	ui->actionPaintTool->setChecked(true);

	//ui->tileSetsTreeView->setHeaderHidden(true);
	ui->tileSetsTreeView->setStyleSheet("QTreeView::item { height: 32px; }");

	m_pTileMapGfxScene = new TileMapGfxScene(this);
	ui->graphicsView->setScene(m_pTileMapGfxScene);
}

/*virtual*/ AuxTileMap::~AuxTileMap()
{
	delete ui;
}

void AuxTileMap::Init(AtlasManager &atlasManagerRef, TileMapModel &tileMapModelRef)
{
	ui->tileSetsTreeView->setModel(&atlasManagerRef.GetTileSetsModel());
	ui->tileSetsTreeView->expandAll();

	int iTotalWidth = width();
	int iTreeViewWidth = ui->tileSetsTreeView->sizeHint().width();
	ui->splitter->setSizes(QList<int>() << iTreeViewWidth << (iTotalWidth - iTreeViewWidth));
	//ui->tileSetsTreeView->resizeColumnToContents(0);
}

TileMapTool AuxTileMap::GetSelectedTool() const
{
	QAction *pCheckedTool = m_pToolActionGroup->checkedAction();
	if(pCheckedTool)
		return static_cast<TileMapTool>(pCheckedTool->data().toInt());

	return TILEMAPTOOL_Unknown;
}

void AuxTileMap::SetBrush(QMap<QPoint, TileData *> brushMap)
{
	m_BrushMap = brushMap;
}

void AuxTileMap::on_tileSetsTreeView_clicked(QModelIndex index)
{
	if(ui->tileSetsTreeView->model() == nullptr)
		return;
	
	TileSetsTreeModel *pModel = static_cast<TileSetsTreeModel *>(ui->tileSetsTreeView->model());
	AtlasTileSet *pTileSetPtr = pModel->GetTileSet(index);
	QUuid terrainUuid = pModel->GetTerrainUuid(index);
	
	m_pTileMapGfxScene->Initialize(pTileSetPtr, terrainUuid);
}
