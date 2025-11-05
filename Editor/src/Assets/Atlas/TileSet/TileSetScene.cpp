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
	m_vSortingStartMousePos(0, 0),
	m_vImportRegionSize(0, 0)
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

	RefreshTiles(Qt::BottomEdge);
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

void TileSetScene::RefreshTiles(Qt::Edge eImportAppendEdge)
{
	// SETUP ------------------------------------------------------------------------------------------------------------------------
	int iMinGridX = INT_MAX, iMaxGridX = INT_MIN, iMinGridY = INT_MAX, iMaxGridY = INT_MIN;
	int iTileSpacingWidth = m_pTileSet->GetAtlasRegionSize().width() + g_iSpacingAmt;
	int iTileSpacingHeight = m_pTileSet->GetAtlasRegionSize().height() + g_iSpacingAmt;
	for (auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); ++iter)
	{
		QPoint ptGridPos = iter.key()->GetMetaGridPos();
		iMinGridX = HyMath::Min(iMinGridX, ptGridPos.x());
		iMaxGridX = HyMath::Max(iMaxGridX, ptGridPos.x());
		iMinGridY = HyMath::Min(iMinGridY, ptGridPos.y());
		iMaxGridY = HyMath::Max(iMaxGridY, ptGridPos.y());

		QPoint ptCurPos;
		ptCurPos.setX(ptGridPos.x() * iTileSpacingWidth);
		ptCurPos.setY(ptGridPos.y() * iTileSpacingHeight);

		iter.value()->Setup(m_pTileSet->GetAtlasRegionSize(), m_pTileSet->GetTileOffset(), m_pTileSet->GetTilePolygon());
		iter.value()->setPos(ptCurPos);
	}

	int iNumColumns = fabs(iMaxGridX - iMinGridX + 1);
	int iNumRows = fabs(iMaxGridY - iMinGridY + 1);
	m_SetupBorderRect.setRect(iMinGridX * iTileSpacingWidth - g_borderMargins.left,
							  iMinGridY * iTileSpacingHeight - g_borderMargins.top,
							  iNumColumns * iTileSpacingWidth + g_borderMargins.left + g_borderMargins.right - g_iSpacingAmt,
							  iNumRows * iTileSpacingHeight + g_borderMargins.top + g_borderMargins.bottom - g_iSpacingAmt);
	m_SetupBorderRect.setVisible(m_SetupTileMap.empty() == false);

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

		iter.value()->Setup(m_vImportRegionSize, m_pTileSet->GetTileOffset(), m_pTileSet->GetTilePolygon());
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

		// Position the m_pModeImportGroup against the m_pModeSetupGroup's 'eImportAppendEdge'
		QPointF ptImportGroupPos;
		switch (eImportAppendEdge)
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

void TileSetScene::OnSortingTilesMousePress(QPointF ptMouseScenePos)
{
	m_vSortingStartMousePos = ptMouseScenePos;

	m_SortingInitialPosMap.clear();
	for (auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); ++iter)
	{
		if (iter.value()->IsSelected())
		{
			iter.value()->SetAsDragged(true);
			m_SortingInitialPosMap.insert(iter.value(), iter.value()->scenePos());
		}
	}

	OnSortingTilesMouseMove(ptMouseScenePos);
}

void TileSetScene::OnSortingTilesMouseMove(QPointF ptMouseScenePos)
{
	QPointF vDelta = ptMouseScenePos - m_vSortingStartMousePos;

	int iTileSpacingWidth = m_pTileSet->GetAtlasRegionSize().width() + g_iSpacingAmt;
	int iTileSpacingHeight = m_pTileSet->GetAtlasRegionSize().height() + g_iSpacingAmt;
	QPoint vGridDelta = vDelta.toPoint();
	vGridDelta.setX(vGridDelta.x() / iTileSpacingWidth);
	vGridDelta.setY(vGridDelta.y() / iTileSpacingHeight);

	for (auto iter = m_SortingInitialPosMap.begin(); iter != m_SortingInitialPosMap.end(); ++iter)
		iter.key()->setPos(iter.value() + vDelta);
}

void TileSetScene::OnSortingTilesMouseRelease(QPointF ptMouseScenePos)
{
	for (auto iter = m_SetupTileMap.begin(); iter != m_SetupTileMap.end(); ++iter)
		iter.value()->SetAsDragged(false);
}

//void TileSetScene::SetGfxItemTilePos(TileSetGfxItem* pGfxItem, QPoint ptGridPos)
//{
//	QPointF ptCurPos;
//	ptCurPos.setX(g_borderMargins.left + (ptGridPos.x() * (m_pTileSet->GetAtlasRegionSize().width() + g_iSpacingAmt)));
//	ptCurPos.setY(g_borderMargins.top + g_iSpacingAmt + (ptGridPos.y() * (m_pTileSet->GetAtlasRegionSize().height() + g_iSpacingAmt)));
//	pGfxItem->SetPos(ptCurPos, m_pTileSet->GetAtlasRegionSize(), m_pTileSet->GetTileOffset(), m_pTileSet->GetTilePolygon());
//}
