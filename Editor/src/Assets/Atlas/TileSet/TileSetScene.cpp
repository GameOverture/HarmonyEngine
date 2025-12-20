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
#include "AuxTileSet.h"
#include "TileSetScene.h"
#include "AtlasTileSet.h"
#include "TileData.h"
#include "TileSetGfxItem.h"
#include "TileSetUndoCmds.h"

#include <QBitArray>

const HyMargins<int> g_borderMargins(5, 5, 5, 5);
const int g_iSpacingAmt = 5;
const float g_fSceneMargins = 7000.0f;

TileSetScene::TileSetScene() :
	QGraphicsScene(),
	m_pTileSet(nullptr),
	m_pModeSetupGroup(new TileSetGfxItemGroup()),
	m_pModeImportGroup(new TileSetGfxItemGroup()),
	m_vArrangingStartMousePos(0, 0),
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
	m_ImportBorderRect.setPen(QPen(QBrush(QColor(0, 255, 0)), 2.0f, Qt::DashLine));
	m_ImportBorderRect.setVisible(false);

	m_pHoverAutoTilePartItem = new QGraphicsPolygonItem();
	m_pModeSetupGroup->addToGroup(m_pHoverAutoTilePartItem);
	m_pHoverAutoTilePartItem->setBrush(QBrush(QColor(255, 255, 255)));
	m_pHoverAutoTilePartItem->setPen(QPen(Qt::NoPen));
	m_pHoverAutoTilePartItem->setOpacity(0.5f);
	m_pHoverAutoTilePartItem->setZValue(9999.0f);
	m_pHoverAutoTilePartItem->hide();


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

	QVector<TileData*> tileDataList = m_pTileSet->GetTileDataList();
	for(int i = 0; i < tileDataList.size(); ++i)
		AddTile(false, tileDataList[i], m_pTileSet->GetTilePolygon(), tileDataList[i]->GetMetaGridPos(), tileDataList[i]->GetPixmap(), false);

	OnTileSetPageChange(m_pTileSet->GetNumTiles() > 0 ? TILESETPAGE_Arrange : TILESETPAGE_Import);
}

QGraphicsRectItem &TileSetScene::GetGfxBorderRect()
{
	return m_SetupBorderRect;
}

QGraphicsRectItem &TileSetScene::GetGfxImportBorderRect()
{
	return m_ImportBorderRect;
}

void TileSetScene::OnTileSetPageChange(TileSetPage ePage)
{
	if(ePage == TILESETPAGE_Import)
	{
		m_pModeSetupGroup->setOpacity(0.42f);
		m_pModeImportGroup->setVisible(true);
	}
	else
	{
		m_pModeSetupGroup->setOpacity(1.0f);
		m_pModeImportGroup->setVisible(false);
	}

	RefreshSetupTiles(ePage);
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

TileSetGfxItem *TileSetScene::GetGfxTile(TileData *pTileData) const
{
	if (m_SetupTileMap.contains(pTileData))
		return m_SetupTileMap.value(pTileData);
	return nullptr;
}

int TileSetScene::GetNumSetupSelected() const
{
	int iNumSetupTiles = 0;
	for (auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); ++iter)
	{
		if (iter.value()->IsSelected())
			iNumSetupTiles++;
	}
	return iNumSetupTiles;
}

QMap<TileData *, TileSetGfxItem *> TileSetScene::GetSelectedSetupTiles() const
{
	QMap<TileData*, TileSetGfxItem*> selectedTileMap;
	for (auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); ++iter)
	{
		if (iter.value()->IsSelected())
			selectedTileMap.insert(iter.key(), iter.value());
	}
	return selectedTileMap;
}

TileData *TileSetScene::IsPointInTile(QPointF ptScenePos) const
{
	for(auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); ++iter)
	{
		QRectF testRect(iter.value()->boundingRect());
		testRect.translate(iter.value()->scenePos());
		if(testRect.contains(ptScenePos))
			return iter.key();
	}
	return nullptr;
}

void TileSetScene::OnMarqueeRelease(AuxTileSet &auxTileSetRef, Qt::MouseButton eMouseBtn, bool bShiftHeld, QPointF ptStartDrag, QPointF ptEndDrag)
{
	QPointF ptTopLeft, ptBotRight;
	ptTopLeft.setX(HyMath::Min(ptStartDrag.x(), ptEndDrag.x()));
	ptTopLeft.setY(HyMath::Min(ptStartDrag.y(), ptEndDrag.y()));
	ptBotRight.setX(HyMath::Max(ptStartDrag.x(), ptEndDrag.x()));
	ptBotRight.setY(HyMath::Max(ptStartDrag.y(), ptEndDrag.y()));
	QRectF sceneRect(ptTopLeft, ptBotRight);

	switch (auxTileSetRef.GetCurrentPage())
	{
	case TILESETPAGE_Import:
		for (auto iter = m_ImportTileMap.begin(); iter != m_ImportTileMap.end(); ++iter)
		{
			QRectF testRect(iter.value()->boundingRect());
			testRect.translate(iter.value()->scenePos());
			if (sceneRect.intersects(testRect))
				iter.value()->SetSelected(eMouseBtn == Qt::LeftButton);
		}
		break;

	case TILESETPAGE_Arrange:
		for (auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); ++iter)
		{
			QRectF testRect(iter.value()->boundingRect());
			testRect.translate(iter.value()->scenePos());
			if (sceneRect.intersects(testRect))
				iter.value()->SetSelected(eMouseBtn == Qt::LeftButton);
			else if(bShiftHeld == false)
				iter.value()->SetSelected(false);
		}
		break;

	case TILESETPAGE_Animation:
		for (auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); ++iter)
		{
			QRectF testRect(iter.value()->boundingRect());
			testRect.translate(iter.value()->scenePos());
			if (sceneRect.intersects(testRect))
				m_PaintStrokeAnimationList.append(iter.key());
		}
		break;

	case TILESETPAGE_Autotile: {
		QUuid selectedTerrainSetUuid;
		if(eMouseBtn == Qt::LeftButton)
			selectedTerrainSetUuid = auxTileSetRef.GetSelectedTerrainSet();
	
		QList<TileData *> affectedTileList;
		for(auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); ++iter)
		{
			QRectF testRect(iter.value()->boundingRect());
			testRect.translate(iter.value()->scenePos());
			if(sceneRect.intersects(testRect))
			{
				if(iter.key()->GetTerrainSet() != selectedTerrainSetUuid)
					affectedTileList.append(iter.key());
			}
		}

		TileSetUndoCmd_ApplyTerrainSet *pTerrainSetCmd = new TileSetUndoCmd_ApplyTerrainSet(auxTileSetRef, affectedTileList, selectedTerrainSetUuid);
		m_pTileSet->GetUndoStack()->push(pTerrainSetCmd);
		break; }

	case TILESETPAGE_Collision:
	case TILESETPAGE_CustomData:
		break;
	}
}

void TileSetScene::ClearSetupSelection()
{
	for (auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); ++iter)
		iter.value()->SetSelected(false);
}

void TileSetScene::AddTile(bool bImportTile, TileData *pTileData, const QPolygonF& outlinePolygon, QPoint ptGridPos, QPixmap pixmap, bool bDefaultSelected)
{
	TileSetGfxItem* pNewTileSetGfxItem = new TileSetGfxItem(pixmap, outlinePolygon);
	pNewTileSetGfxItem->SetSelected(bDefaultSelected);

	if (bImportTile)
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

void TileSetScene::RefreshImportTiles()
{
	int iMinGridX = INT_MAX, iMaxGridX = INT_MIN, iMinGridY = INT_MAX, iMaxGridY = INT_MIN;
	int iTileSpacingWidth = m_vImportRegionSize.width() + g_iSpacingAmt;
	int iTileSpacingHeight = m_vImportRegionSize.height() + g_iSpacingAmt;
	for(auto iter = m_ImportTileMap.begin(); iter != m_ImportTileMap.end(); ++iter)
	{
		QPoint ptGridPos = iter.key();
		iMinGridX = HyMath::Min(iMinGridX, ptGridPos.x());
		iMaxGridX = HyMath::Max(iMaxGridX, ptGridPos.x());
		iMinGridY = HyMath::Min(iMinGridY, ptGridPos.y());
		iMaxGridY = HyMath::Max(iMaxGridY, ptGridPos.y());

		QPoint ptCurPos;
		ptCurPos.setX(ptGridPos.x() * iTileSpacingWidth);
		ptCurPos.setY(ptGridPos.y() * iTileSpacingHeight);

		iter.value()->Refresh(m_vImportRegionSize, m_pTileSet, TILESETPAGE_Import, nullptr);
		iter.value()->setPos(ptCurPos);
	}

	int iNumColumns = fabs(iMaxGridX - iMinGridX + 1);
	int iNumRows = fabs(iMaxGridY - iMinGridY + 1);
	m_ImportBorderRect.setRect(iMinGridX * iTileSpacingWidth - g_borderMargins.left - (iTileSpacingWidth / 2),
							   iMinGridY * iTileSpacingHeight - g_borderMargins.top - (iTileSpacingHeight / 2),
							   iNumColumns * iTileSpacingWidth + g_borderMargins.left + g_borderMargins.right - g_iSpacingAmt,
							   iNumRows * iTileSpacingHeight + g_borderMargins.top + g_borderMargins.bottom - g_iSpacingAmt);

	if(m_ImportTileMap.empty())
		m_ImportBorderRect.setVisible(false);
	else
	{
		m_ImportBorderRect.setVisible(true);

		if(m_SetupBorderRect.isVisible())
		{
			// Position the m_pModeImportGroup against the m_pModeSetupGroup's 'm_eImportAppendEdge'
			QPointF ptImportGroupPos;
			switch(m_eImportAppendEdge)
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
}

void TileSetScene::RefreshSetupTiles(TileSetPage ePage, QPointF vDragDelta /*= QPointF()*/)
{
	// Sync m_pTileSet's 'AtlasTileSet::m_TileDataList' with m_SetupTileMap
	// Either add or remove TileData entries as needed
	QVector<TileData *> tileDataList = m_pTileSet->GetTileDataList();
	for (int i = 0; i < tileDataList.size(); ++i)
	{
		if (m_SetupTileMap.contains(tileDataList[i]) == false)
		{
			// TileData exists in TileSet but not in Scene, so add it
			AddTile(false, tileDataList[i], m_pTileSet->GetTilePolygon(), tileDataList[i]->GetMetaGridPos(), tileDataList[i]->GetPixmap(), false);
		}
	}
	for (auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); )
	{
		if (tileDataList.contains(iter.key()) == false)
		{
			// TileData exists in Scene but not in TileSet, so remove it
			m_pModeSetupGroup->removeFromGroup(iter.value());
			delete iter.value();
			iter = m_SetupTileMap.erase(iter);
		}
		else
			++iter;
	}

	// Refresh all tiles and reposition them
	int iMinGridX = INT_MAX, iMaxGridX = INT_MIN, iMinGridY = INT_MAX, iMaxGridY = INT_MIN;
	int iTileSpacingWidth = m_pTileSet->GetAtlasRegionSize().width() + g_iSpacingAmt;
	int iTileSpacingHeight = m_pTileSet->GetAtlasRegionSize().height() + g_iSpacingAmt;
	for (auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); ++iter)
	{
		iter.value()->Refresh(m_pTileSet->GetAtlasRegionSize(), m_pTileSet, ePage, iter.key());

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
	m_SetupBorderRect.setRect(iMinGridX * iTileSpacingWidth - g_borderMargins.left - (iTileSpacingWidth / 2),
							  iMinGridY * iTileSpacingHeight - g_borderMargins.top - (iTileSpacingHeight / 2),
							  iNumColumns * iTileSpacingWidth + g_borderMargins.left + g_borderMargins.right - g_iSpacingAmt,
							  iNumRows * iTileSpacingHeight + g_borderMargins.top + g_borderMargins.bottom - g_iSpacingAmt);
	m_SetupBorderRect.setVisible(m_SetupTileMap.empty() == false && vDragDelta.isNull());
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

void TileSetScene::OnArrangingTilesMousePress(QPointF ptMouseScenePos)
{
	m_vArrangingStartMousePos = ptMouseScenePos;

	for (auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); ++iter)
	{
		iter.value()->SetDraggingInitialPos(iter.value()->scenePos());
		iter.value()->SetDraggingGridPos(iter.key()->GetMetaGridPos());
		iter.value()->SetAsDragged(iter.value()->IsSelected());
	}

	OnArrangingTilesMouseMove(ptMouseScenePos);
}

void TileSetScene::OnArrangingTilesMouseMove(QPointF ptMouseScenePos)
{
	QPointF vDelta = ptMouseScenePos - m_vArrangingStartMousePos;

	QPoint vDeltaGrid = vDelta.toPoint();
	vDeltaGrid.setX(vDeltaGrid.x() / (m_pTileSet->GetAtlasRegionSize().width() + g_iSpacingAmt));
	vDeltaGrid.setY(vDeltaGrid.y() / (m_pTileSet->GetAtlasRegionSize().height() + g_iSpacingAmt));
	DisplaceTiles(vDeltaGrid);

	RefreshSetupTiles(TILESETPAGE_Arrange, vDelta);
}

void TileSetScene::OnArrangingTilesMouseRelease(AuxTileSet &auxTileSetRef, QPointF ptMouseScenePos)
{
	QList<TileData*> affectedTileList;
	QList<QPoint> oldGridPosList;
	QList<QPoint> newGridPosList;
	for (auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); ++iter)
	{
		iter.value()->SetAsDragged(false);

		if(iter.value()->GetDraggingGridPos() != iter.key()->GetMetaGridPos())
		{
			affectedTileList.append(iter.key());
			oldGridPosList.append(iter.key()->GetMetaGridPos());
			newGridPosList.append(iter.value()->GetDraggingGridPos());
		}
	}

	TileSetUndoCmd_MoveTiles* pMoveCmd = new TileSetUndoCmd_MoveTiles(auxTileSetRef, affectedTileList, oldGridPosList, newGridPosList);
	m_pTileSet->GetUndoStack()->push(pMoveCmd);
}

void TileSetScene::HoverAutoTilePartAt(QPointF ptScenePos)
{
	m_pHoverAutoTilePartItem->hide();

	// Highlight auto-tile parts under mouse (if it's an enabled tile)
	TileData *pTile = IsPointInTile(ptScenePos);
	if(pTile == nullptr || pTile->GetTerrainSet().isNull())
		return;

	TileSetGfxItem *pGfxTile = m_SetupTileMap[pTile];
	QPointF ptTileLocalPos = ptScenePos - pGfxTile->scenePos();

	TileSetAutoTilePart ePart = AUTOTILEPART_Unknown;
	QGraphicsPolygonItem *pGfxAutoTilePart = pGfxTile->GetAutoTilePartAt(ptTileLocalPos, ePart);
	if(pGfxAutoTilePart)
	{
		m_pHoverAutoTilePartItem->setPolygon(pGfxAutoTilePart->polygon());
		m_pHoverAutoTilePartItem->setPos(pGfxAutoTilePart->scenePos());
		m_pHoverAutoTilePartItem->show();
	}
}

void TileSetScene::OnTerrainSetApplied(TileData *pTileData)
{
	if(m_SetupTileMap.contains(pTileData))
	{
		m_SetupTileMap[pTileData]->AllocateAutoTileParts(m_pTileSet, m_pTileSet->GetTerrainSetType(pTileData->GetTerrainSet()), m_pTileSet->GetTileShape());
		m_SetupTileMap[pTileData]->Refresh(m_pTileSet->GetAtlasRegionSize(), m_pTileSet, TILESETPAGE_Autotile, pTileData);
	}
}

void TileSetScene::StartPaintStroke()
{
	m_pHoverAutoTilePartItem->hide();

	m_PaintStrokeAnimationList.clear();
	m_PaintStrokeAutoTilePartMap.clear();
}

void TileSetScene::OnPaintingStroke(AuxTileSet &auxTileSetRef, QPointF ptScenePos, Qt::MouseButtons uiMouseFlags)
{
	TileData *pTile = IsPointInTile(ptScenePos);
	if(pTile == nullptr)
		return;

	if(auxTileSetRef.GetCurrentPage() == TILESETPAGE_Animation)
	{
		QUuid animUuid = auxTileSetRef.GetSelectedAnimation();
		if(animUuid.isNull()) // Only paint if an animation is selected
			return;

		TileSetGfxItem *pGfxTile = m_SetupTileMap[pTile];
		pGfxTile->SetAnimation(uiMouseFlags & Qt::LeftButton, m_pTileSet->GetAnimationColor(animUuid));

		m_PaintStrokeAnimationList.push_back(pTile);
	}
	else if(auxTileSetRef.GetCurrentPage() == TILESETPAGE_Autotile)
	{
		if(pTile->GetTerrainSet().isNull()) // Only paint if tile is enabled for auto-tiling
			return;

		QUuid terrainUuid = auxTileSetRef.GetSelectedTerrain();
		if(terrainUuid.isNull())
			return;
		
		TileSetGfxItem *pGfxTile = m_SetupTileMap[pTile];
		QPointF ptTileLocalPos = ptScenePos - pGfxTile->scenePos();

		TileSetAutoTilePart ePart = AUTOTILEPART_Unknown;
		QGraphicsPolygonItem *pGfxAutoTilePart = pGfxTile->GetAutoTilePartAt(ptTileLocalPos, ePart);
		if(pGfxAutoTilePart)
		{
			if(uiMouseFlags & Qt::LeftButton)
			{
				pGfxAutoTilePart->setBrush(QBrush(HyGlobal::ConvertHyColor(m_pTileSet->GetTerrainColor(terrainUuid))));
				pGfxAutoTilePart->show();
			}
			else if(uiMouseFlags & Qt::RightButton)
				pGfxAutoTilePart->hide();
			
			if(m_PaintStrokeAutoTilePartMap.contains(pTile) == false)
				m_PaintStrokeAutoTilePartMap[pTile] = QBitArray(NUM_AUTOTILEPARTS);
			m_PaintStrokeAutoTilePartMap[pTile].setBit(static_cast<int>(ePart));
		}
	}
	else
		HyGuiLog("TileSetScene::OnPaintingStroke() - Painting stroke on unsupported TileSetPage", LOGTYPE_Error);
}

void TileSetScene::OnPaintStrokeRelease(AuxTileSet &auxTileSetRef, Qt::MouseButton eMouseBtn)
{
	if(auxTileSetRef.GetCurrentPage() == TILESETPAGE_Animation)
	{
		if(m_PaintStrokeAnimationList.empty())
			return;

		TileSetUndoCmd_PaintAnimation *pPaintAnimCmd = new TileSetUndoCmd_PaintAnimation(auxTileSetRef, eMouseBtn == Qt::LeftButton, m_PaintStrokeAnimationList);
		m_pTileSet->GetUndoStack()->push(pPaintAnimCmd);
	}
	else if(auxTileSetRef.GetCurrentPage() == TILESETPAGE_Autotile)
	{
		if(m_PaintStrokeAutoTilePartMap.empty())
			return;

		TileSetUndoCmd_PaintAutoTileParts *pPaintAutoTilePartsCmd = new TileSetUndoCmd_PaintAutoTileParts(auxTileSetRef, eMouseBtn == Qt::LeftButton, m_PaintStrokeAutoTilePartMap);
		m_pTileSet->GetUndoStack()->push(pPaintAutoTilePartsCmd);
	}
	else
		HyGuiLog("TileSetScene::OnPaintStrokeRelease() - Releasing paint stroke on unsupported TileSetPage", LOGTYPE_Error);
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
