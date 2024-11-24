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

TileSetScene::TileSetScene() :
	QGraphicsScene(),
	m_eSceneType(SCENETYPE_Importing),
	m_vImportTileSize(0, 0)
{
	addItem(&m_ImportLabel);
}

/*virtual*/ TileSetScene::~TileSetScene()
{
}

void TileSetScene::Setup(AtlasTileSet *pTileSet)
{
	m_ImportLabel.setVisible(false);
}

int TileSetScene::GetNumImportPixmaps() const
{
	return m_ImportTilePixmapList.size();
}

QVector<QGraphicsPixmapItem *> &TileSetScene::GetImportPixmapList()
{
	return m_ImportTilePixmapList;
}

QSize TileSetScene::GetImportTileSize() const
{
	return m_vImportTileSize;
}

void TileSetScene::RemoveImportPixmaps()
{
	for(auto pPixmap : m_ImportTilePixmapList)
	{
		removeItem(pPixmap);
		delete pPixmap;
	}
	m_ImportTilePixmapList.clear();
	m_vImportTileSize = QSize(0, 0);
}

void TileSetScene::AddImportPixmap(QPixmap pixmap)
{
	m_ImportTilePixmapList.append(addPixmap(pixmap));

	if(m_vImportTileSize.width() < pixmap.width())
		m_vImportTileSize.setWidth(pixmap.width());
	if(m_vImportTileSize.height() < pixmap.height())
		m_vImportTileSize.setHeight(pixmap.height());
}

void TileSetScene::ConstructImportScene()
{
	// NOTE: TileSet atlases are always "square"
	//       COLUMNS = static_cast<int>(std::floor(std::sqrt(n)))
	//       ROWS = static_cast<int>(std::ceil(static_cast<double>(n) / columns))
	int iColumns = static_cast<int>(std::floor(std::sqrt(m_ImportTilePixmapList.size())));
	int iRows = static_cast<int>(std::ceil(static_cast<double>(m_ImportTilePixmapList.size()) / iColumns));

	ConstructImportScene(iColumns, iRows);
}

void TileSetScene::ConstructImportScene(int iNumColumns, int iNumRows)
{
	m_eSceneType = SCENETYPE_Importing;

	for(auto pTileRectGfxItem : m_ImportTileRectList)
	{
		removeItem(pTileRectGfxItem);
		delete pTileRectGfxItem;
	}
	m_ImportTileRectList.clear();

	HyMargins<int> borderMargins(10, 10, 10, 3);
	int iSpacingAmt = 5;
	int iTitleHeight = 30;

	m_ImportRect.setPen(QPen(QBrush(QColor(255, 255, 255)), 1.0f, Qt::DashLine));
	m_ImportRect.setRect(0, 0, iNumColumns * m_vImportTileSize.width() + borderMargins.left + borderMargins.right + (iNumColumns - 1) * iSpacingAmt,
							   borderMargins.top + iTitleHeight + (iNumRows * m_vImportTileSize.height()) + ((iNumRows - 1) * iSpacingAmt) + borderMargins.bottom);
	addItem(&m_ImportRect);

	QPoint ptCurPos;
	ptCurPos.setX(borderMargins.left);
	ptCurPos.setY(borderMargins.top);

	m_ImportLabel.setPos(borderMargins.left, borderMargins.top);
	m_ImportLabel.setVisible(true);
	m_ImportLabel.setFont(QFont("Arial", 12));
	m_ImportLabel.setDefaultTextColor(QColor(255, 255, 255));
	m_ImportLabel.setPlainText("Importing Tiles:");
	ptCurPos.setY(ptCurPos.y() + iTitleHeight);

	QPoint ptTileStartPos = ptCurPos;
	int iPixmapIndex = 0;
	for(int i = 0; i < iNumRows; ++i)
	{
		for(int j = 0; j < iNumColumns; ++j, ++iPixmapIndex)
		{
			m_ImportTileRectList.append(addRect(ptCurPos.x(), ptCurPos.y(), m_vImportTileSize.width() + 1, m_vImportTileSize.height() + 1, QPen(QBrush(QColor(0, 0, 0)), 1.0f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin)));
			ptCurPos += QPoint(1, 1);
			m_ImportTilePixmapList[iPixmapIndex]->setPos(ptCurPos);
			ptCurPos -= QPoint(1, 1);

			ptCurPos.setX(ptCurPos.x() + m_vImportTileSize.width() + iSpacingAmt);
		}

		ptCurPos.setX(ptTileStartPos.x());
		ptCurPos.setY(ptCurPos.y() + m_vImportTileSize.height() + iSpacingAmt);
	}
}
