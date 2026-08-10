/**************************************************************************
 *	TileMapGfxScene.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "TileMapGfxScene.h"
#include "AuxTileMap.h"
#include "AtlasTileSet.h"
#include "TileData.h"
#include "TileGfxItem.h"

const HyMargins<int> g_borderMargins(5, 5, 5, 5);
const int g_iTileSpacingAmt = 1;
const float g_fSceneMargins = 7000.0f;

TileMapGfxScene::TileMapGfxScene(QObject *pParent /*= nullptr*/) :
	QGraphicsScene(pParent),
	m_pAuxTileMap(static_cast<AuxTileMap *>(pParent)),
	m_pTileSet(nullptr)
{
	m_BorderRect.setPen(QPen(QBrush(QColor(255, 255, 255)), 2.0f, Qt::DashLine));
	m_BorderRect.setVisible(false);
	addItem(&m_BorderRect);

	QRectF sceneRect(-g_fSceneMargins, -g_fSceneMargins, g_fSceneMargins * 2.0f, g_fSceneMargins * 2.0f);
	setSceneRect(sceneRect);
}

/*virtual*/ TileMapGfxScene::~TileMapGfxScene()
{
}

void TileMapGfxScene::Initialize(const AtlasTileSet *pTileSet, QUuid terrainUuid)
{
	if(m_pTileSet == pTileSet && m_TerrainUuid == terrainUuid)
		return;

	m_pTileSet = pTileSet;
	m_TerrainUuid = terrainUuid;

	for(auto iter = m_TileGfxItemsMap.begin(); iter != m_TileGfxItemsMap.end(); ++iter)
		iter.value()->hide();

	if(m_TerrainUuid.isNull() == false)
	{
		m_BorderRect.hide();
		return;
	}
	
	QVector<TileData *> tileDataList = m_pTileSet->GetTileDataList();
	for(int i = 0; i < tileDataList.size(); ++i)
	{
		auto iter = m_TileGfxItemsMap.find(tileDataList[i]);
		if(iter == m_TileGfxItemsMap.end())
		{
			TileGfxItem *pTileGfxItem = new TileGfxItem(false, m_pTileSet->GetTilePixmap(tileDataList[i]), QPolygonF());
			m_TileGfxItemsMap.insert(tileDataList[i], pTileGfxItem);
			addItem(pTileGfxItem);
		}
		else
			iter.value()->show();
	}
	
	// Reposition all tiles and reset the border rectangle
	int iMinGridX = INT_MAX, iMaxGridX = INT_MIN, iMinGridY = INT_MAX, iMaxGridY = INT_MIN;
	int iTileSpacingWidth = m_pTileSet->GetAtlasRegionSize().width() + g_iTileSpacingAmt;
	int iTileSpacingHeight = m_pTileSet->GetAtlasRegionSize().height() + g_iTileSpacingAmt;
	for (auto iter = m_TileGfxItemsMap.begin(); iter != m_TileGfxItemsMap.end(); ++iter)
	{
		if(iter.value()->isVisible() == false)
			continue;

		QPoint ptGridPos = iter.key()->GetMetaGridPos();
	
		iMinGridX = HyMath::Min(iMinGridX, ptGridPos.x());
		iMaxGridX = HyMath::Max(iMaxGridX, ptGridPos.x());
		iMinGridY = HyMath::Min(iMinGridY, ptGridPos.y());
		iMaxGridY = HyMath::Max(iMaxGridY, ptGridPos.y());

		QPoint ptCurPos;
		ptCurPos.setX(ptGridPos.x() * iTileSpacingWidth);
		ptCurPos.setY(ptGridPos.y() * iTileSpacingHeight);

		iter.value()->setPos(ptCurPos);
	}
	int iNumColumns = fabs(iMaxGridX - iMinGridX + 1);
	int iNumRows = fabs(iMaxGridY - iMinGridY + 1);
	m_BorderRect.show();
	m_BorderRect.setRect(iMinGridX * iTileSpacingWidth - g_borderMargins.left - (iTileSpacingWidth / 2),
						 iMinGridY * iTileSpacingHeight - g_borderMargins.top - (iTileSpacingHeight / 2),
						 iNumColumns * iTileSpacingWidth + g_borderMargins.left + g_borderMargins.right,
						 iNumRows * iTileSpacingHeight + g_borderMargins.top + g_borderMargins.bottom);	
}

void TileMapGfxScene::OnMarqueeRelease(Qt::MouseButton eMouseBtn, bool bShiftHeld, QPointF ptStartDrag, QPointF ptEndDrag)
{
	QPointF ptTopLeft, ptBotRight;
	ptTopLeft.setX(HyMath::Min(ptStartDrag.x(), ptEndDrag.x()));
	ptTopLeft.setY(HyMath::Min(ptStartDrag.y(), ptEndDrag.y()));
	ptBotRight.setX(HyMath::Max(ptStartDrag.x(), ptEndDrag.x()));
	ptBotRight.setY(HyMath::Max(ptStartDrag.y(), ptEndDrag.y()));
	QRectF sceneRect(ptTopLeft, ptBotRight);

	QList<TileData *> brushList;
	for(auto iter = m_TileGfxItemsMap.begin(); iter != m_TileGfxItemsMap.end(); ++iter)
	{
		if(iter.value()->isVisible() == false)
			continue;

		QRectF testRect(iter.value()->boundingRect());
		testRect.translate(iter.value()->scenePos());
		if (sceneRect.intersects(testRect))
			iter.value()->SetSelected(eMouseBtn == Qt::LeftButton);
		else if(bShiftHeld == false)
			iter.value()->SetSelected(false);

		if(iter.value()->IsSelected())
			brushList.push_back(iter.key());
	}

	// Normalize the brush map so that the bottom-left tile is at (0, 0)
	int iMinGridX = INT_MAX, iMinGridY = INT_MAX;
	for(int i = 0; i < brushList.size(); ++i)
	{
		QPoint ptGridPos = brushList[i]->GetMetaGridPos();
		ptGridPos.setY(ptGridPos.y() * -1); // TileSets' meta-grid coordinates go top-to-bottom, but Harmony and TileMaps go bottom-to-top // TILETODO: make tile sets meta-grid go bottom-to-top

		iMinGridX = HyMath::Min(iMinGridX, ptGridPos.x());
		iMinGridY = HyMath::Min(iMinGridY, ptGridPos.y());
	}

	QMap<QPoint, TileData *> brushMap;
	for(int i = 0; i < brushList.size(); ++i)
	{
		QPoint ptGridPos = brushList[i]->GetMetaGridPos();
		ptGridPos.setY(ptGridPos.y() * -1); // TileSets' meta-grid coordinates go top-to-bottom, but Harmony and TileMaps go bottom-to-top // TILETODO: make tile sets meta-grid go bottom-to-top

		brushMap.insert(QPoint(ptGridPos.x() - iMinGridX, ptGridPos.y() - iMinGridY), brushList[i]);
	}
	m_pAuxTileMap->SetBrush(brushMap);
}
