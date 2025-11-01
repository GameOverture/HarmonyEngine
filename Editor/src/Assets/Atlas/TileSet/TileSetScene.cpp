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
#include "TileSetGfxItem.h"

const HyMargins<int> g_borderMargins(10, 10, 10, 3);
const int g_iSpacingAmt = 5;
const float g_fSceneMargins = 1420.0f;

TileSetScene::TileSetScene() :
	QGraphicsScene(),
	m_eMode(TILESETMODE_Importing),
	m_pTileSet(nullptr),
	m_pModeSetupGroup(new TileSetGfxItemGroup()),
	m_pModeImportGroup(new TileSetGfxItemGroup()),
	m_vImportRegionSize(0, 0)
{
	m_pModeSetupGroup->setAcceptHoverEvents(true);
	addItem(m_pModeSetupGroup);

	m_pModeImportGroup->setAcceptHoverEvents(true);
	addItem(m_pModeImportGroup);

	m_BorderBoundsRect.setPen(QPen(QBrush(QColor(255, 255, 255)), 1.0f, Qt::DashLine));
	m_BorderBoundsRect.setVisible(false);
	addItem(&m_BorderBoundsRect);
}

/*virtual*/ TileSetScene::~TileSetScene()
{
	ClearImportTiles();
	ClearSetupTiles();
}

void TileSetScene::Initialize(AtlasTileSet *pTileSet)
{
	ClearImportTiles();
	ClearSetupTiles();

	m_pTileSet = pTileSet;
	m_eMode = m_pTileSet->GetNumTiles() > 0 ? TILESETMODE_Setup : TILESETMODE_Importing;

	QVector<TileData*> tileDataList = m_pTileSet->GetTileDataList();
	for(int i = 0; i < tileDataList.size(); ++i)
		AddTile(TILESETMODE_Setup, tileDataList[i], m_pTileSet->GetTilePolygon(), tileDataList[i]->GetMetaGridPos(), tileDataList[i]->GetPixmap(), false);

	SetDisplayMode(m_eMode);
	RefreshTiles();
}

void TileSetScene::SetDisplayMode(TileSetMode eMode)
{
	m_eMode = eMode;
	m_pModeSetupGroup->setVisible(m_eMode == TILESETMODE_Setup); // TODO: Show setup tiles while importing, attached to the import edge (semi-transparent)
	m_pModeImportGroup->setVisible(m_eMode == TILESETMODE_Importing);
	
	update();
}

int TileSetScene::GetNumImportPixmaps() const
{
	int iNumImportTiles = 0;
	for(auto iter = m_ImportTileMap.begin(); iter != m_ImportTileMap.end(); ++iter)
	{
		if(iter.value()->IsSelected())
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
		if(iter.value()->IsSelected())
			importPixmapList.insert(iter.key(), iter.value()->GetPixmap());
	}

	return importPixmapList;
}

void TileSetScene::OnMarqueeRelease(Qt::MouseButton eMouseBtn, QPointF ptStartDrag, QPointF ptEndDrag)
{
	QPointF ptTopLeft, ptBotRight;
	ptTopLeft.setX(HyMath::Min(ptStartDrag.x(), ptEndDrag.x()));
	ptTopLeft.setY(HyMath::Min(ptStartDrag.y(), ptEndDrag.y()));
	ptBotRight.setX(HyMath::Max(ptStartDrag.x(), ptEndDrag.x()));
	ptBotRight.setY(HyMath::Max(ptStartDrag.y(), ptEndDrag.y()));
	QRectF sceneRect(ptTopLeft, ptBotRight);

	switch (m_eMode)
	{
	case TILESETMODE_Importing:
		for (auto iter = m_ImportTileMap.begin(); iter != m_ImportTileMap.end(); ++iter)
		{
			QRectF testRect(iter.value()->GetPos(), QSizeF(m_pTileSet->GetAtlasRegionSize()));
			if (sceneRect.intersects(testRect))
				iter.value()->SetSelected(eMouseBtn == Qt::LeftButton);
		}
		break;

	case TILESETMODE_Setup:
		for (auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); ++iter)
		{
			QRectF testRect(iter.value()->GetPos(), QSizeF(m_pTileSet->GetAtlasRegionSize()));
			if (sceneRect.intersects(testRect))
				iter.value()->SetSelected(eMouseBtn == Qt::LeftButton);
		}
		break;
	}
}

void TileSetScene::AddTile(TileSetMode eMode, TileData *pTileData, const QPolygonF& outlinePolygon, QPoint ptGridPos, QPixmap pixmap, bool bDefaultSelected)
{
	TileSetGfxItem* pNewTileSetGfxItem = new TileSetGfxItem(pixmap, outlinePolygon);
	pNewTileSetGfxItem->SetSelected(bDefaultSelected);

	if (eMode == TILESETMODE_Importing)
	{
		if (m_vImportRegionSize.width() < pixmap.width())
			m_vImportRegionSize.setWidth(pixmap.width());
		if (m_vImportRegionSize.height() < pixmap.height())
			m_vImportRegionSize.setHeight(pixmap.height());

		m_ImportTileMap.insert(ptGridPos, pNewTileSetGfxItem);
		m_pModeImportGroup->addToGroup(pNewTileSetGfxItem);
	}
	else
	{
		if (pixmap.width() > m_pTileSet->GetAtlasRegionSize().width() || pixmap.height() > m_pTileSet->GetAtlasRegionSize().height())
			HyGuiLog("TileSetScene::AddTile() - Setup tile being added has larger dimensions (" % QString::number(pixmap.width()) % ", " % QString::number(pixmap.height()) % " than TileSet's region size", LOGTYPE_Error);

		m_SetupTileMap.insert(pTileData, pNewTileSetGfxItem);
		m_pModeSetupGroup->addToGroup(pNewTileSetGfxItem);
	}
}

void TileSetScene::RefreshTiles()
{
	QPoint ptCurPos;
	ptCurPos.setX(g_borderMargins.left);
	ptCurPos.setY(g_borderMargins.top);

	int minX = INT_MAX, maxX = INT_MIN, minY = INT_MAX, maxY = INT_MIN;
	for (auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); ++iter)
	{
		QPoint ptGridPos = iter.key()->GetMetaGridPos();
		minX = HyMath::Min(minX, ptGridPos.x());
		maxX = HyMath::Max(maxX, ptGridPos.x());
		minY = HyMath::Min(minY, ptGridPos.y());
		maxY = HyMath::Max(maxY, ptGridPos.y());

		int iTileSpacingWidth = m_pTileSet->GetAtlasRegionSize().width() + g_iSpacingAmt;
		int iTileSpacingHeight = m_pTileSet->GetAtlasRegionSize().height() + g_iSpacingAmt;
		ptCurPos.setX(g_borderMargins.left + (ptGridPos.x() * iTileSpacingWidth));
		ptCurPos.setY(g_borderMargins.top + (ptGridPos.y() * iTileSpacingHeight));

		iter.value()->SetPos(ptCurPos, m_pTileSet->GetAtlasRegionSize(), m_pTileSet->GetTileOffset(), m_pTileSet->GetTilePolygon());
	}

	for (auto iter = m_ImportTileMap.begin(); iter != m_ImportTileMap.end(); ++iter)
	{
		QPoint ptTilePos = iter.key();
		minX = HyMath::Min(minX, ptTilePos.x());
		maxX = HyMath::Max(maxX, ptTilePos.x());
		minY = HyMath::Min(minY, ptTilePos.y());
		maxY = HyMath::Max(maxY, ptTilePos.y());

		ptCurPos.setX(g_borderMargins.left + (ptTilePos.x() * (m_vImportRegionSize.width() + g_iSpacingAmt)));
		ptCurPos.setY(g_borderMargins.top + (ptTilePos.y() * (m_vImportRegionSize.height() + g_iSpacingAmt)));

		iter.value()->SetPos(ptCurPos, m_vImportRegionSize, m_pTileSet->GetTileOffset(), m_pTileSet->GetTilePolygon());
	}

	int iNumColumns = maxX - minX + 1;
	int iNumRows = maxY - minY + 1;

	m_BorderBoundsRect.setRect(0, 0,
		iNumColumns * m_vImportRegionSize.width() + g_borderMargins.left + g_borderMargins.right + (iNumColumns - 1) * g_iSpacingAmt,
		g_borderMargins.top + (g_iSpacingAmt * 2) + (iNumRows * m_vImportRegionSize.height()) + ((iNumRows - 1) * g_iSpacingAmt) + g_borderMargins.bottom);
	m_BorderBoundsRect.setVisible(true);

	//QRectF sceneRect(-g_fSceneMargins, -g_fSceneMargins, m_ImportBoundsRect.rect().width() + (g_fSceneMargins * 2.0f), m_ImportBoundsRect.rect().height() + (g_fSceneMargins * 2.0f));
	//setSceneRect(sceneRect);
}

void TileSetScene::ClearImportTiles()
{
	for (auto iter = m_ImportTileMap.begin(); iter != m_ImportTileMap.end(); ++iter)
	{
		m_pModeImportGroup->removeFromGroup(iter.value());
		delete iter.value();
	}
	m_ImportTileMap.clear();
	m_vImportRegionSize = QSize(0, 0);
}

void TileSetScene::ClearSetupTiles()
{
	for (auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); ++iter)
	{
		m_pModeSetupGroup->removeFromGroup(iter.value());
		delete iter.value();
	}
	m_SetupTileMap.clear();
}

void TileSetScene::SetGfxItemTilePos(TileSetGfxItem* pGfxItem, QPoint ptGridPos)
{
	QPointF ptCurPos;
	ptCurPos.setX(g_borderMargins.left + (ptGridPos.x() * (m_pTileSet->GetAtlasRegionSize().width() + g_iSpacingAmt)));
	ptCurPos.setY(g_borderMargins.top + g_iSpacingAmt + (ptGridPos.y() * (m_pTileSet->GetAtlasRegionSize().height() + g_iSpacingAmt)));
	pGfxItem->SetPos(ptCurPos, m_pTileSet->GetAtlasRegionSize(), m_pTileSet->GetTileOffset(), m_pTileSet->GetTilePolygon());
}
