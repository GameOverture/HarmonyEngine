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

TileSetScene::TileSetScene(AtlasTileSet &tileSetRef) :
	QGraphicsScene(),
	m_TileSetRef(tileSetRef),
	m_eMode(TILESETMODE_Importing),
	m_pModeImportGroup(new QGraphicsItemGroup()),
	m_pModeTileSetGroup(new QGraphicsItemGroup()),
	m_vImportTileSize(0, 0)
{
	addItem(m_pModeImportGroup);
	addItem(m_pModeTileSetGroup);
}

/*virtual*/ TileSetScene::~TileSetScene()
{
	for(auto iter = m_ImportTileMap.begin(); iter != m_ImportTileMap.end(); ++iter)
	{
		m_pModeImportGroup->removeFromGroup(iter.value().first);
		m_pModeImportGroup->removeFromGroup(iter.value().second);
		delete iter.value().first;
		delete iter.value().second;
	}
	m_ImportTileMap.clear();
}

void TileSetScene::SetDisplayMode(TileSetMode eMode)
{
	if(m_eMode == eMode)
		return;

	m_eMode = eMode;
	m_pModeImportGroup->setVisible(m_eMode == TILESETMODE_Importing);
	m_pModeTileSetGroup->setVisible(m_eMode == TILESETMODE_TileSet);
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
		importPixmapList.insert(iter.key(), iter.value().second->pixmap());

	return importPixmapList;
}

void TileSetScene::ClearImport()
{
	for(auto iter = m_ImportTileMap.begin(); iter != m_ImportTileMap.end(); ++iter)
	{
		m_pModeImportGroup->removeFromGroup(iter.value().first); // removeFromGroup() leaves a dangling pointer, so delete it
		delete iter.value().first;
		m_pModeImportGroup->removeFromGroup(iter.value().second); // removeFromGroup() leaves a dangling pointer, so delete it
		delete iter.value().second;
	}
	m_ImportTileMap.clear();
	m_vImportTileSize = QSize(0, 0);
}

void TileSetScene::AddImport(QPoint ptGridPos, QPixmap pixmap)
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
	ptCurPos += QPoint(1, 1);
	pNewGfxPixmapItem->setPos(ptCurPos);
	m_pModeImportGroup->addToGroup(pNewGfxPixmapItem);

	m_ImportTileMap.insert(ptGridPos, QPair<QGraphicsRectItem *, QGraphicsPixmapItem *>(pNewGfxRectItem, pNewGfxPixmapItem));
}

void TileSetScene::SyncImport()
{
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
	ptCurPos.setY(ptCurPos.y() + iTitleHeight);

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
	for(auto pTilePixmapGfxItem : m_TileSetPixmapItem)
	{
		m_pModeTileSetGroup->removeFromGroup(pTilePixmapGfxItem); // removeFromGroup() leaves a dangling pointer, so delete it
		delete pTilePixmapGfxItem;
	}
	m_TileSetPixmapItem.clear();

	QSize vTileSize = m_TileSetRef.GetTileSize();
	QMap<QPoint, TileData *> tileDataMap = m_TileSetRef.GetTileDataMap();

	for(auto it = tileDataMap.begin(); it != tileDataMap.end(); ++it)
	{
		TileData *pTileData = it.value();
		if(pTileData == nullptr)
			continue;
		QGraphicsPixmapItem *pNewPixmapItem = new QGraphicsPixmapItem(pTileData->GetPixmap());
		pNewPixmapItem->setPos(it.key().x() * vTileSize.width(), it.key().y() * vTileSize.height());
		m_pModeTileSetGroup->addToGroup(pNewPixmapItem);
		m_TileSetPixmapItem.append(pNewPixmapItem);
	}
}
