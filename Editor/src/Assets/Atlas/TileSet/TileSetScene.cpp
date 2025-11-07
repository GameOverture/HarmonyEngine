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

const HyMargins<int> g_borderMargins(16, 16, 16, 16);
const int g_iSpacingAmt = 5;
const float g_fSceneMargins = 1420.0f;

TileSetScene::TileSetScene() :
	QGraphicsScene(),
	m_eMode(TILESETMODE_Importing),
	m_pTileSet(nullptr),
	m_pModeSetupGroup(new TileSetGfxItemGroup()),
	m_pModeImportGroup(new TileSetGfxItemGroup()),
	m_vDraggingStartMousePos(0, 0),
	m_vImportRegionSize(0, 0),
	m_eImportAppendEdge(Qt::BottomEdge)
{
	addItem(m_pModeSetupGroup);
	m_pModeSetupGroup->setAcceptHoverEvents(true);
	m_pModeSetupGroup->addToGroup(&m_SetupBorderRect);
	m_SetupBorderRect.setPen(QPen(QBrush(QColor(255, 255, 255)), 2.0f, Qt::DashLine));
	m_SetupBorderRect.setVisible(false);

	addItem(m_pModeImportGroup);
	m_pModeImportGroup->setAcceptHoverEvents(true);
	m_pModeImportGroup->addToGroup(&m_ImportBorderRect);
	m_ImportBorderRect.setPen(QPen(QBrush(QColor(255, 255, 255)), 2.0f, Qt::DashLine));
	m_ImportBorderRect.setVisible(false);

	QRectF sceneRect(-g_fSceneMargins, -g_fSceneMargins, g_fSceneMargins * 2.0f, g_fSceneMargins * 2.0f);
	setSceneRect(sceneRect);
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

	RefreshTiles();
	SetDisplayMode(m_eMode);
}

TileSetMode TileSetScene::GetDisplayMode() const
{
	return m_eMode;
}

void TileSetScene::SetDisplayMode(TileSetMode eMode)
{
	m_eMode = eMode;

	if (m_eMode == TILESETMODE_Setup)
	{
		m_pModeSetupGroup->setOpacity(1.0f);
		m_pModeImportGroup->setVisible(false);
	}
	else
	{
		m_pModeSetupGroup->setOpacity(0.42f);
		m_pModeImportGroup->setVisible(true);
	}
}

QPointF TileSetScene::GetFocusPt() const
{
	if (m_eMode == TILESETMODE_Setup)
		return m_SetupBorderRect.rect().center();
	else
		return m_ImportBorderRect.rect().center();
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

void TileSetScene::SetImportAppendEdge(Qt::Edge eEdge)
{
	m_eImportAppendEdge = eEdge;
}

void TileSetScene::OnMarqueeRelease(Qt::MouseButton eMouseBtn, bool bShiftHeld, QPointF ptStartDrag, QPointF ptEndDrag)
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
			QRectF testRect(iter.value()->scenePos(), QSizeF(m_pTileSet->GetAtlasRegionSize()));
			if (sceneRect.intersects(testRect))
				iter.value()->SetSelected(eMouseBtn == Qt::LeftButton);
		}
		break;

	case TILESETMODE_Setup:
		for (auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); ++iter)
		{
			QRectF testRect(iter.value()->scenePos(), QSizeF(m_pTileSet->GetAtlasRegionSize()));
			if (sceneRect.intersects(testRect))
				iter.value()->SetSelected(eMouseBtn == Qt::LeftButton);
			else if(bShiftHeld == false)
				iter.value()->SetSelected(false);
		}
		break;
	}
}

void TileSetScene::ClearSetupSelection()
{
	for (auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); ++iter)
		iter.value()->SetSelected(false);
}

TileSetGfxItem* TileSetScene::GetSetupTileAt(QPointF ptScenePos) const
{
	for (auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); ++iter)
	{
		QRectF testRect(iter.value()->scenePos(), QSizeF(m_pTileSet->GetAtlasRegionSize()));
		if (testRect.contains(ptScenePos))
			return iter.value();
	}
	return nullptr;
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

void TileSetScene::RefreshTiles(QPointF vDragDelta /*= QPointF()*/)
{
	// SETUP ------------------------------------------------------------------------------------------------------------------------
	int iMinGridX = INT_MAX, iMaxGridX = INT_MIN, iMinGridY = INT_MAX, iMaxGridY = INT_MIN;
	int iTileSpacingWidth = m_pTileSet->GetAtlasRegionSize().width() + g_iSpacingAmt;
	int iTileSpacingHeight = m_pTileSet->GetAtlasRegionSize().height() + g_iSpacingAmt;
	for (auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); ++iter)
	{
		iter.value()->Refresh(m_pTileSet->GetAtlasRegionSize(), m_pTileSet->GetTileOffset(), m_pTileSet->GetTilePolygon());

		QPoint ptGridPos;
		if(vDragDelta.isNull())
			ptGridPos = iter.key()->GetMetaGridPos();
		else // Selected tiles are being dragged
		{
			//if (iter.value()->IsSelected())
			//{
			//	iter.value()->setPos(iter.value()->GetDraggingInitialPos() + vDragDelta);
			//	continue;
			//}
			
			ptGridPos = iter.value()->GetDraggingGridPos();
		}

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
	m_SetupBorderRect.setRect(iMinGridX * iTileSpacingWidth - g_borderMargins.left,
							  iMinGridY * iTileSpacingHeight - g_borderMargins.top,
							  iNumColumns * iTileSpacingWidth + g_borderMargins.left + g_borderMargins.right - g_iSpacingAmt,
							  iNumRows * iTileSpacingHeight + g_borderMargins.top + g_borderMargins.bottom - g_iSpacingAmt);
	m_SetupBorderRect.setVisible(m_SetupTileMap.empty() == false && vDragDelta.isNull());

	// IMPORT ------------------------------------------------------------------------------------------------------------------------
	iMinGridX = INT_MAX, iMaxGridX = INT_MIN, iMinGridY = INT_MAX, iMaxGridY = INT_MIN;
	iTileSpacingWidth = m_vImportRegionSize.width() + g_iSpacingAmt;
	iTileSpacingHeight = m_vImportRegionSize.height() + g_iSpacingAmt;
	for (auto iter = m_ImportTileMap.begin(); iter != m_ImportTileMap.end(); ++iter)
	{
		QPoint ptGridPos = iter.key();
		iMinGridX = HyMath::Min(iMinGridX, ptGridPos.x());
		iMaxGridX = HyMath::Max(iMaxGridX, ptGridPos.x());
		iMinGridY = HyMath::Min(iMinGridY, ptGridPos.y());
		iMaxGridY = HyMath::Max(iMaxGridY, ptGridPos.y());

		QPoint ptCurPos;
		ptCurPos.setX(ptGridPos.x() * iTileSpacingWidth);
		ptCurPos.setY(ptGridPos.y() * iTileSpacingHeight);

		iter.value()->Refresh(m_vImportRegionSize, m_pTileSet->GetTileOffset(), m_pTileSet->GetTilePolygon());
		iter.value()->setPos(ptCurPos);
	}

	iNumColumns = fabs(iMaxGridX - iMinGridX + 1);
	iNumRows = fabs(iMaxGridY - iMinGridY + 1);
	m_ImportBorderRect.setRect(iMinGridX * iTileSpacingWidth - g_borderMargins.left,
							   iMinGridY * iTileSpacingHeight - g_borderMargins.top,
							   iNumColumns * iTileSpacingWidth + g_borderMargins.left + g_borderMargins.right - g_iSpacingAmt,
							   iNumRows * iTileSpacingHeight + g_borderMargins.top + g_borderMargins.bottom - g_iSpacingAmt);

	if(m_ImportTileMap.empty())
		m_ImportBorderRect.setVisible(false);
	else
	{
		m_ImportBorderRect.setVisible(true);

		// Position the m_pModeImportGroup against the m_pModeSetupGroup's 'm_eImportAppendEdge'
		QPointF ptImportGroupPos;
		switch (m_eImportAppendEdge)
		{
		case Qt::TopEdge:
			ptImportGroupPos.setX(m_SetupBorderRect.rect().center().x() - m_ImportBorderRect.rect().width() / 2.0f);
			ptImportGroupPos.setY(m_SetupBorderRect.rect().top() - g_borderMargins.bottom - m_ImportBorderRect.rect().height());
			break;
		case Qt::BottomEdge:
			ptImportGroupPos.setX(m_SetupBorderRect.rect().center().x() - m_ImportBorderRect.rect().width() / 2.0f);
			ptImportGroupPos.setY(m_SetupBorderRect.rect().bottom() + g_borderMargins.top);
			break;
		case Qt::LeftEdge:
			ptImportGroupPos.setX(m_SetupBorderRect.rect().left() - g_borderMargins.right - m_ImportBorderRect.rect().width());
			ptImportGroupPos.setY(m_SetupBorderRect.rect().center().y() - m_ImportBorderRect.rect().height() / 2.0f);
			break;
		case Qt::RightEdge:
			ptImportGroupPos.setX(m_SetupBorderRect.rect().right() + g_borderMargins.left);
			ptImportGroupPos.setY(m_SetupBorderRect.rect().center().y() - m_ImportBorderRect.rect().height() / 2.0f);
			break;
		}
		m_pModeImportGroup->setPos(ptImportGroupPos);
	}
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

void TileSetScene::OnDraggingTilesMousePress(QPointF ptMouseScenePos)
{
	m_vDraggingStartMousePos = ptMouseScenePos;

	for (auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); ++iter)
	{
		iter.value()->SetDraggingInitialPos(iter.value()->scenePos());
		iter.value()->SetDraggingGridPos(iter.key()->GetMetaGridPos());
		iter.value()->SetAsDragged(iter.value()->IsSelected());
	}

	OnDraggingTilesMouseMove(ptMouseScenePos);
}

void TileSetScene::OnDraggingTilesMouseMove(QPointF ptMouseScenePos)
{
	// TODO: Fix vDeltaGrid calculation to account for tile spacing properly
	//TileSetGfxItem* pHoveredSetupItem = GetSetupTileAt(m_vDraggingStartMousePos);


	QPointF vDelta = ptMouseScenePos - m_vDraggingStartMousePos;

	QPoint vDeltaGrid = vDelta.toPoint();
	vDeltaGrid.setX(vDeltaGrid.x() / (m_pTileSet->GetAtlasRegionSize().width() + g_iSpacingAmt));
	vDeltaGrid.setY(vDeltaGrid.y() / (m_pTileSet->GetAtlasRegionSize().height() + g_iSpacingAmt));
	DisplaceTiles(vDeltaGrid);

	RefreshTiles(vDelta);
}

void TileSetScene::OnDraggingTilesMouseRelease(QPointF ptMouseScenePos)
{
	for (auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); ++iter)
		iter.value()->SetAsDragged(false);
}

void TileSetScene::DisplaceTiles(QPoint vGridDelta)
{
	// Reset all tiles to their original grid positions
	for (auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); ++iter)
		iter.value()->SetDraggingGridPos(iter.key()->GetMetaGridPos());

	if (vGridDelta.isNull())
		return;

	// Gather selected tiles (the tiles being dragged)
	QSet<TileSetGfxItem*> selectedTilesSet;
	for (auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); ++iter)
	{
		if (iter.value()->IsSelected())
			selectedTilesSet.insert(iter.value());
	}

	// Determine what collisions will occur from moving selected tiles to their new destinations
	QMap<TileData*, TileSetGfxItem*> displacedTileMap;
	for (auto iter = selectedTilesSet.begin(); iter != selectedTilesSet.end(); ++iter)
	{
		QPoint ptDestinationTileGridPos = (*iter)->GetDraggingGridPos() + vGridDelta;
			
		// See if any unselected tile is colliding with this selected tile at its new position
		for (auto testIter = m_SetupTileMap.begin(); testIter != m_SetupTileMap.end(); ++testIter)
		{
			if (testIter.value()->IsSelected() == false)
			{
				if (testIter.value()->GetDraggingGridPos() == ptDestinationTileGridPos)
				{
					displacedTileMap[testIter.key()] = testIter.value();
					break;
				}
			}
		}

		// Update the selected tile's dragging grid pos
		(*iter)->SetDraggingGridPos(ptDestinationTileGridPos);
	}

	enum DisplaceDirection
	{
		DIR_Up,
		DIR_Right,
		DIR_Down,
		DIR_Left,

		NUM_DIRECTIONS
	};
	QPoint directionVectors[NUM_DIRECTIONS] = { QPoint(0, -1), QPoint(1, 0), QPoint(0, 1), QPoint(-1, 0) };
	
	HyGuiLog("Number of displaced tiles: " % QString::number(displacedTileMap.size()), LOGTYPE_Info);

	// For each tile in displacedTileMap, determine the best direction to displace/cascade that makes the least impact
	// Skip over any selected tiles and animation tile positions since they are locked in place
	for(auto iter = displacedTileMap.begin(); iter != displacedTileMap.end(); ++iter)
	{
		int iDirectionImpact[NUM_DIRECTIONS] = { 0, 0, 0, 0 };
		for(int i = 0; i < NUM_DIRECTIONS; ++i)
		{
			QPoint ptTestGridPos = iter.value()->GetDraggingGridPos();
			ptTestGridPos += directionVectors[i];

			while (true)
			{
				bool bCollisionFound = false;
				for (auto testIter = m_SetupTileMap.begin(); testIter != m_SetupTileMap.end(); ++testIter)
				{
					if (testIter.value() == iter.value())
						continue;

					if (testIter.value()->GetDraggingGridPos() == ptTestGridPos)
					{
						ptTestGridPos += directionVectors[i];

						if (false == testIter.value()->IsSelected()) // TODO: Also test for animation tiles to skip over
							iDirectionImpact[i]++;

						bCollisionFound = true;
						break;
					}
				}
				if (bCollisionFound == false)
					break;
			}
		}
		DisplaceDirection eBestDir = static_cast<DisplaceDirection>(0);
		int iImpact = iDirectionImpact[0];
		for (int i = 1; i < NUM_DIRECTIONS; ++i)
		{
			if (iDirectionImpact[i] < iImpact)
			{
				iImpact = iDirectionImpact[i];
				eBestDir = static_cast<DisplaceDirection>(i);
			}
		}

		// Move this tile in the best direction, skipping over any selected tiles and animation tile positions
		// Also cascade any newly displaced tiles in the best direction along the way while preserving their original order
		QPair<TileData*, TileSetGfxItem*> curTile(iter.key(), iter.value());
		QPoint ptTestGridPos = curTile.second->GetDraggingGridPos();
		while (true)
		{
			ptTestGridPos += directionVectors[eBestDir];

			// First check if this a skipped-over position collides with any other tile
			// TODO: Also test for animation tiles to skip over
			bool bSkipGridSpot = false;
			for (auto skipIter = selectedTilesSet.begin(); skipIter != selectedTilesSet.end(); ++skipIter)
			{
				if ((*skipIter)->GetDraggingGridPos() == ptTestGridPos)
				{
					bSkipGridSpot = true;
					break;
				}
			}
			if (bSkipGridSpot)
				continue;

			// This is not a skipped-over position, so test for collisions
			bool bCollisionFound = false;
			for (auto testIter = m_SetupTileMap.begin(); testIter != m_SetupTileMap.end(); ++testIter)
			{
				if (testIter.value() == curTile.second || testIter.value()->IsSelected()) // TODO: Also test for animation tiles to skip over
					continue;

				if (testIter.value()->GetDraggingGridPos() == ptTestGridPos)
				{
					bCollisionFound = true;
					
					// Determine which tile should continue the displacement cascade (based on original order)
					// Set the tile's DraggingGridPos that isn't continuing
					switch (eBestDir)
					{
					case DIR_Up:
						if (testIter.key()->GetMetaGridPos().y() < curTile.first->GetMetaGridPos().y())
						{
							curTile.second->SetDraggingGridPos(ptTestGridPos);
							curTile = QPair<TileData*, TileSetGfxItem*>(testIter.key(), testIter.value());
							ptTestGridPos = curTile.second->GetDraggingGridPos();
						}
						else
							testIter.value()->SetDraggingGridPos(ptTestGridPos);
						break;
					case DIR_Right:
						if (testIter.key()->GetMetaGridPos().x() > curTile.first->GetMetaGridPos().x())
						{
							curTile.second->SetDraggingGridPos(ptTestGridPos);
							curTile = QPair<TileData*, TileSetGfxItem*>(testIter.key(), testIter.value());
							ptTestGridPos = curTile.second->GetDraggingGridPos();
						}
						else
							testIter.value()->SetDraggingGridPos(ptTestGridPos);
						break;
					case DIR_Down:
						if (testIter.key()->GetMetaGridPos().y() > curTile.first->GetMetaGridPos().y())
						{
							curTile.second->SetDraggingGridPos(ptTestGridPos);
							curTile = QPair<TileData*, TileSetGfxItem*>(testIter.key(), testIter.value());
							ptTestGridPos = curTile.second->GetDraggingGridPos();
						}
						else
							testIter.value()->SetDraggingGridPos(ptTestGridPos);
						break;
					case DIR_Left:
						if (testIter.key()->GetMetaGridPos().x() < curTile.first->GetMetaGridPos().x())
						{
							curTile.second->SetDraggingGridPos(ptTestGridPos);
							curTile = QPair<TileData*, TileSetGfxItem*>(testIter.key(), testIter.value());
							ptTestGridPos = curTile.second->GetDraggingGridPos();
						}
						else
							testIter.value()->SetDraggingGridPos(ptTestGridPos);
						break;
					}

					break;
				}
			}

			if(bCollisionFound == false)
			{
				curTile.second->SetDraggingGridPos(ptTestGridPos);
				break;
			}
		}
	}
}
