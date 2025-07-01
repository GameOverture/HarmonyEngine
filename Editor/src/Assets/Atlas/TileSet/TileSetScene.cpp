/**************************************************************************
 *	TileSetScene.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "TileSetScene.h"
#include "AtlasTileSet.h"
#include "TileData.h"
#include "TileGfxItem.h"

TileSetScene::TileSetScene() :
	QGraphicsScene(),
	m_pTileSet(nullptr),
	m_eMode(TILESETMODE_Importing),
	m_pModeImportGroup(new TileSetGroupItem()),
	m_pModeTileSetGroup(new TileSetGroupItem()),
	m_vImportRegionSize(0, 0)
{
	addItem(m_pModeImportGroup);
	addItem(m_pModeTileSetGroup);

	m_BoundsRect.setPen(QPen(QBrush(QColor(255, 255, 255)), 1.0f, Qt::DashLine));
	addItem(&m_BoundsRect);
}

/*virtual*/ TileSetScene::~TileSetScene()
{
	for(auto iter = m_ImportTileMap.begin(); iter != m_ImportTileMap.end(); ++iter)
	{
		m_pModeImportGroup->removeFromGroup(iter.value().m_pRectItem);
		m_pModeImportGroup->removeFromGroup(iter.value().m_pPixmapItem);
		m_pModeImportGroup->removeFromGroup(iter.value().m_pOutlineItem);
		delete iter.value().m_pRectItem;
		delete iter.value().m_pPixmapItem;
		delete iter.value().m_pOutlineItem;
	}
	m_ImportTileMap.clear();
}

void TileSetScene::Initialize(AtlasTileSet *pTileSet)
{
	m_pTileSet = pTileSet;
	SyncTileSet();

	m_eMode = m_pTileSet->GetNumTiles() > 0 ? TILESETMODE_TileSet : TILESETMODE_Importing;
	SetDisplayMode(m_eMode);
}

void TileSetScene::SetDisplayMode(TileSetMode eMode)
{
	m_eMode = eMode;
	m_pModeImportGroup->setVisible(m_eMode == TILESETMODE_Importing);
	m_pModeTileSetGroup->setVisible(m_eMode == TILESETMODE_TileSet);
	update();
}

int TileSetScene::GetNumImportPixmaps() const
{
	int iNumImportTiles = 0;
	for(auto iter = m_ImportTileMap.begin(); iter != m_ImportTileMap.end(); ++iter)
	{
		if(iter.value().m_bSelected)
			iNumImportTiles++;
	}
	return iNumImportTiles;
}

QSize TileSetScene::GetImportRegionSize() const
{
	return m_vImportRegionSize;
}

QMap<QPoint, QPixmap> TileSetScene::AssembleImportMap()
{
	QMap<QPoint, QPixmap> importPixmapList;
	for(auto iter = m_ImportTileMap.begin(); iter != m_ImportTileMap.end(); ++iter)
	{
		if(iter.value().m_bSelected)
			importPixmapList.insert(iter.key(), iter.value().m_pPixmapItem->pixmap());
	}

	return importPixmapList;
}

void TileSetScene::ClearImport()
{
	for(auto iter = m_ImportTileMap.begin(); iter != m_ImportTileMap.end(); ++iter)
	{
		m_pModeImportGroup->removeFromGroup(iter.value().m_pRectItem); // removeFromGroup() leaves a dangling pointer, so delete it
		delete iter.value().m_pRectItem;
		m_pModeImportGroup->removeFromGroup(iter.value().m_pPixmapItem); // removeFromGroup() leaves a dangling pointer, so delete it
		delete iter.value().m_pPixmapItem;
		m_pModeImportGroup->removeFromGroup(iter.value().m_pOutlineItem); // removeFromGroup() leaves a dangling pointer, so delete it
		delete iter.value().m_pOutlineItem;
	}
	m_ImportTileMap.clear();
	m_vImportRegionSize = QSize(0, 0);
}

void TileSetScene::AddImport(const QPolygonF &outlinePolygon, QPoint ptGridPos, QPixmap pixmap, bool bDefaultSelected)
{
	if(m_vImportRegionSize.width() < pixmap.width())
		m_vImportRegionSize.setWidth(pixmap.width());
	if(m_vImportRegionSize.height() < pixmap.height())
		m_vImportRegionSize.setHeight(pixmap.height());

	QPoint ptCurPos(ptGridPos.x() * m_vImportRegionSize.width(), ptGridPos.y() * m_vImportRegionSize.height());

	QGraphicsRectItem *pNewGfxRectItem = new QGraphicsRectItem(0.0f, 0.0f, m_vImportRegionSize.width() + 1, m_vImportRegionSize.height() + 1);
	pNewGfxRectItem->setPos(ptCurPos.x(), ptCurPos.y());
	pNewGfxRectItem->setPen(QPen(QBrush(HyGlobal::ConvertHyColor(HyColor::Orange)), 1.0f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
	m_pModeImportGroup->addToGroup(pNewGfxRectItem);

	QGraphicsPixmapItem *pNewGfxPixmapItem = new QGraphicsPixmapItem(pixmap);
	m_pModeImportGroup->addToGroup(pNewGfxPixmapItem);

	QGraphicsPolygonItem *pNewOutlineItem = new QGraphicsPolygonItem(outlinePolygon);
	
	ImportTileItem tileItem(pNewGfxRectItem, pNewGfxPixmapItem, pNewOutlineItem);
	tileItem.SetSelected(bDefaultSelected);
	m_ImportTileMap.insert(ptGridPos, tileItem);
}

void TileSetScene::SyncImport()
{
	TileSetShape eTileType = m_pTileSet->GetTileShape();

	m_pModeImportGroup->addToGroup(&m_ImportLabel);

	HyMargins<int> borderMargins(10, 10, 10, 3);
	int iSpacingAmt = 5;
	int iTitleHeight = 30;

	QPoint ptCurPos;
	ptCurPos.setX(borderMargins.left);
	ptCurPos.setY(borderMargins.top);

	m_ImportLabel.setPos(borderMargins.left, borderMargins.top);
	m_ImportLabel.setVisible(true);
	m_ImportLabel.setFont(QFont("Arial", 12));
	m_ImportLabel.setDefaultTextColor(QColor(255, 255, 255));
	m_ImportLabel.setPlainText("Importing Tiles:");

	int minX = INT_MAX, maxX = INT_MIN, minY = INT_MAX, maxY = INT_MIN;
	for(auto iter = m_ImportTileMap.begin(); iter != m_ImportTileMap.end(); ++iter)
	{
		QPoint ptTilePos = iter.key();
		minX = HyMath::Min(minX, ptTilePos.x());
		maxX = HyMath::Max(maxX, ptTilePos.x());
		minY = HyMath::Min(minY, ptTilePos.y());
		maxY = HyMath::Max(maxY, ptTilePos.y());

		ptCurPos.setX(borderMargins.left + (ptTilePos.x() * (m_vImportRegionSize.width() + iSpacingAmt)));
		ptCurPos.setY(borderMargins.top + iTitleHeight + (ptTilePos.y() * (m_vImportRegionSize.height() + iSpacingAmt)));
		iter.value().m_pRectItem->setRect(0.0f, 0.0f, m_vImportRegionSize.width() + 2, m_vImportRegionSize.height() + 2);
		iter.value().m_pRectItem->setPos(ptCurPos.x(), ptCurPos.y());

		ptCurPos = QPoint(ptCurPos.x() + 2, ptCurPos.y() + 2); // offset by 2 pixels to avoid overlap with rect and outline (1px each)
		iter.value().m_pPixmapItem->setPos(ptCurPos);

		iter.value().m_pOutlineItem->setPos(ptCurPos.x() + 1, ptCurPos.y() + 1);
	}

	int iNumColumns = maxX - minX + 1;
	int iNumRows = maxY - minY + 1;
	m_BoundsRect.setRect(0, 0,
						 iNumColumns * m_vImportRegionSize.width() + borderMargins.left + borderMargins.right + (iNumColumns - 1) * iSpacingAmt,
						 borderMargins.top + iTitleHeight + (iNumRows * m_vImportRegionSize.height()) + ((iNumRows - 1) * iSpacingAmt) + borderMargins.bottom);

	const float fMargins = 1420.0f;
	QRectF sceneRect(-fMargins, -fMargins, m_BoundsRect.rect().width() + (fMargins * 2.0f), m_BoundsRect.rect().height() + (fMargins * 2.0f));
	setSceneRect(sceneRect);
}

void TileSetScene::SyncTileSet()
{
	if(m_pTileSet == nullptr)
	{
		HyGuiLog("TileSetScene::SyncTileSet() - m_pTileSet is null, invoke TileSetScene::Initialize() first", LOGTYPE_Error);
		return;
	}

	for(auto pTilePixmapGfxItem : m_TileSetPixmapItem)
	{
		m_pModeTileSetGroup->removeFromGroup(pTilePixmapGfxItem); // removeFromGroup() leaves a dangling pointer, so delete it
		delete pTilePixmapGfxItem;
	}
	m_TileSetPixmapItem.clear();

	QMap<QPoint, TileData *> tileDataMap = m_pTileSet->GetTileDataMap();

	for(auto it = tileDataMap.begin(); it != tileDataMap.end(); ++it)
	{
		TileData *pTileData = it.value();
		if(pTileData == nullptr)
			continue;
		TileGfxItem *pNewPixmapItem = new TileGfxItem(pTileData->GetPixmap());
		QSizeF vTileSize = pNewPixmapItem->boundingRect().size();

		pNewPixmapItem->setPos(it.key().x() * vTileSize.width(), it.key().y() * vTileSize.height());
		// Allow mouse input on graphics item
		pNewPixmapItem->setAcceptHoverEvents(true);
		pNewPixmapItem->setAcceptTouchEvents(true);
		pNewPixmapItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
		pNewPixmapItem->setFlag(QGraphicsItem::ItemIsMovable, true);
		pNewPixmapItem->setFlag(QGraphicsItem::ItemIsFocusable, true);
		pNewPixmapItem->setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

		m_pModeTileSetGroup->addToGroup(pNewPixmapItem);
		m_TileSetPixmapItem.append(pNewPixmapItem);
	}
}

void TileSetScene::OnMarqueeRelease(Qt::MouseButton eMouseBtn, QPointF ptStartDrag, QPointF ptEndDrag)
{
	if(m_eMode == TILESETMODE_Importing)
	{
		QPointF ptTopLeft, ptBotRight;
		ptTopLeft.setX(HyMath::Min(ptStartDrag.x(), ptEndDrag.x()));
		ptTopLeft.setY(HyMath::Min(ptStartDrag.y(), ptEndDrag.y()));
		ptBotRight.setX(HyMath::Max(ptStartDrag.x(), ptEndDrag.x()));
		ptBotRight.setY(HyMath::Max(ptStartDrag.y(), ptEndDrag.y()));

		// Determine which tiles were selected and toggle their import or not
		QRectF sceneRect(ptTopLeft, ptBotRight);
		for(auto iter = m_ImportTileMap.begin(); iter != m_ImportTileMap.end(); ++iter)
		{
			QRectF testRect(iter.value().m_pRectItem->pos(), QSizeF(m_pTileSet->GetAtlasRegionSize()));
			if(sceneRect.intersects(testRect))
				iter->SetSelected(eMouseBtn == Qt::LeftButton);
		}

		//TileSetUndoCmd_DropTiles *pCmd = new TileSetUndoCmd_DropTiles();
		//m_pAuxTileSet->GetTileSet()->GetUndoStack()->push(pCmd);
	}
}
