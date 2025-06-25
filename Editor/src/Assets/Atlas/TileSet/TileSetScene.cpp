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
	m_vImportTileSize(0, 0)
{
	addItem(m_pModeImportGroup);
	addItem(m_pModeTileSetGroup);
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
	return m_ImportTileMap.size();
}

QSize TileSetScene::GetImportTileSize() const
{
	return m_vImportTileSize;
}

QMap<QPoint, QPixmap> TileSetScene::AssembleImportMap()
{
	QMap<QPoint, QPixmap> importPixmapList;
	for(auto iter = m_ImportTileMap.begin(); iter != m_ImportTileMap.end(); ++iter)
		importPixmapList.insert(iter.key(), iter.value().m_pPixmapItem->pixmap());

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
	m_vImportTileSize = QSize(0, 0);
}

void TileSetScene::AddImport(const QPolygonF &outlinePolygon, QPoint ptGridPos, QPixmap pixmap)
{
	if(m_vImportTileSize.width() < pixmap.width())
		m_vImportTileSize.setWidth(pixmap.width());
	if(m_vImportTileSize.height() < pixmap.height())
		m_vImportTileSize.setHeight(pixmap.height());

	QPoint ptCurPos(ptGridPos.x() * m_vImportTileSize.width(), ptGridPos.y() * m_vImportTileSize.height());

	QGraphicsRectItem *pNewGfxRectItem = new QGraphicsRectItem(ptCurPos.x(), ptCurPos.y(), m_vImportTileSize.width() + 1, m_vImportTileSize.height() + 1);
	pNewGfxRectItem->setPen(QPen(QBrush(QColor(0, 0, 0)), 1.0f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
	m_pModeImportGroup->addToGroup(pNewGfxRectItem);

	QGraphicsPixmapItem *pNewGfxPixmapItem = new QGraphicsPixmapItem(pixmap);
	m_pModeImportGroup->addToGroup(pNewGfxPixmapItem);

	QGraphicsPolygonItem *pNewOutlineItem = new QGraphicsPolygonItem(outlinePolygon);
	m_ImportTileMap.insert(ptGridPos, ImportTileItem(pNewGfxRectItem, pNewGfxPixmapItem, pNewOutlineItem));
}

void TileSetScene::SyncImport()
{
	TileSetType eTileType = m_pTileSet->GetTileType();

	m_pModeImportGroup->addToGroup(&m_ImportLabel);

	HyMargins<int> borderMargins(10, 10, 10, 3);
	int iSpacingAmt = 5;
	int iTitleHeight = 30;

	int minX = INT_MAX, maxX = INT_MIN, minY = INT_MAX, maxY = INT_MIN;
	for(const QPoint &pt : m_ImportTileMap.keys())
	{
		minX = HyMath::Min(minX, pt.x());
		maxX = HyMath::Max(maxX, pt.x());
		minY = HyMath::Min(minY, pt.y());
		maxY = HyMath::Max(maxY, pt.y());
	}
	int iNumColumns = maxX - minX + 1;
	int iNumRows = maxY - minY + 1;

	m_ImportRect.setPen(QPen(QBrush(QColor(255, 255, 255)), 1.0f, Qt::DashLine));
	m_ImportRect.setRect(0, 0, iNumColumns * m_vImportTileSize.width() + borderMargins.left + borderMargins.right + (iNumColumns - 1) * iSpacingAmt,
							   borderMargins.top + iTitleHeight + (iNumRows * m_vImportTileSize.height()) + ((iNumRows - 1) * iSpacingAmt) + borderMargins.bottom);
	m_pModeImportGroup->addToGroup(&m_ImportRect);

	QPoint ptCurPos;
	ptCurPos.setX(borderMargins.left);
	ptCurPos.setY(borderMargins.top);

	m_ImportLabel.setPos(borderMargins.left, borderMargins.top);
	m_ImportLabel.setVisible(true);
	m_ImportLabel.setFont(QFont("Arial", 12));
	m_ImportLabel.setDefaultTextColor(QColor(255, 255, 255));
	m_ImportLabel.setPlainText("Importing Tiles:");
	//ptCurPos.setY(ptCurPos.y() + iTitleHeight);

	for(auto iter = m_ImportTileMap.begin(); iter != m_ImportTileMap.end(); ++iter)
	{
		QPoint ptTilePos = iter.key();
		ptCurPos.setX(borderMargins.left + (ptTilePos.x() * (m_vImportTileSize.width() + iSpacingAmt)));
		ptCurPos.setY(borderMargins.top + iTitleHeight + (ptTilePos.y() * (m_vImportTileSize.height() + iSpacingAmt)));
		iter.value().m_pRectItem->setRect(ptCurPos.x(), ptCurPos.y(), m_vImportTileSize.width() + 2, m_vImportTileSize.height() + 2);

		ptCurPos = QPoint(ptCurPos.x() + 2, ptCurPos.y() + 2); // offset by 2 pixels to avoid overlap with rect and outline (1px each)
		iter.value().m_pPixmapItem->setPos(ptCurPos);

		iter.value().m_pOutlineItem->setPos(ptCurPos.x() + 1, ptCurPos.y() + 1);
	}

	//QPoint ptTileStartPos = ptCurPos;
	//int iPixmapIndex = 0;
	//for(int i = 0; i < iNumRows; ++i)
	//{
	//	for(int j = 0; j < iNumColumns; ++j, ++iPixmapIndex)
	//	{
	//		QGraphicsRectItem *pNewGfxRectItem = new QGraphicsRectItem(ptCurPos.x(), ptCurPos.y(), m_vImportTileSize.width() + 1, m_vImportTileSize.height() + 1);
	//		pNewGfxRectItem->setPen(QPen(QBrush(QColor(0, 0, 0)), 1.0f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
	//		m_pModeImportGroup->addToGroup(pNewGfxRectItem);
	//		m_ImportTileRectList.append(pNewGfxRectItem);

	//		ptCurPos += QPoint(1, 1);
	//		m_ImportTilePixmapList[iPixmapIndex]->setPos(ptCurPos);
	//		ptCurPos -= QPoint(1, 1);

	//		ptCurPos.setX(ptCurPos.x() + m_vImportTileSize.width() + iSpacingAmt);
	//	}

	//	ptCurPos.setX(ptTileStartPos.x());
	//	ptCurPos.setY(ptCurPos.y() + m_vImportTileSize.height() + iSpacingAmt);
	//}
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

void TileSetScene::OnMarqueeRelease(QPoint ptStartDrag, QPoint ptEndDrag)
{
	if(m_eMode == TILESETMODE_Importing)
	{
		QPointF ptTopLeft, ptBotRight;
		ptTopLeft.setX(HyMath::Min(ptStartDrag.x(), ptEndDrag.x()));
		ptTopLeft.setY(HyMath::Min(ptStartDrag.y(), ptEndDrag.y()));
		ptBotRight.setX(HyMath::Max(ptStartDrag.x(), ptEndDrag.x()));
		ptBotRight.setY(HyMath::Max(ptStartDrag.y(), ptEndDrag.y()));

		QRectF sceneRect(ptTopLeft, ptBotRight);
		// Determine which tiles were selected and toggle their import or not
		for(auto iter = m_ImportTileMap.begin(); iter != m_ImportTileMap.end(); ++iter)
		{
			if(iter->m_pRectItem->boundingRect().contains(sceneRect))
			{
				//iter->
			}
		}
	}
}
