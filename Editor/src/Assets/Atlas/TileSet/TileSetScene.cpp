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
	m_eSceneType(SCENETYPE_Importing)
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

void TileSetScene::RemoveImportPixmaps()
{
	for(auto pPixmap : m_ImportTilePixmapList)
	{
		removeItem(pPixmap);
		delete pPixmap;
	}
	m_ImportTilePixmapList.clear();
}

void TileSetScene::AddImportPixmap(QPixmap pixmap)
{
	m_ImportTilePixmapList.append(addPixmap(pixmap));
}

void TileSetScene::ConstructImportScene(QPoint vTileSize, int iNumColumns, int iNumRows)
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
	m_ImportRect.setRect(0, 0, iNumColumns * vTileSize.x() + borderMargins.left + borderMargins.right + (iNumColumns - 1) * iSpacingAmt,
							   borderMargins.top + iTitleHeight + (iNumRows * vTileSize.y()) + ((iNumRows - 1) * iSpacingAmt) + borderMargins.bottom);
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
			m_ImportTileRectList.append(addRect(ptCurPos.x(), ptCurPos.y(), vTileSize.x() + 1, vTileSize.y() + 1, QPen(QBrush(QColor(0, 0, 0)), 1.0f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin)));
			ptCurPos += QPoint(1, 1);
			m_ImportTilePixmapList[iPixmapIndex]->setPos(ptCurPos);
			ptCurPos -= QPoint(1, 1);

			ptCurPos.setX(ptCurPos.x() + vTileSize.x() + iSpacingAmt);
		}

		ptCurPos.setX(ptTileStartPos.x());
		ptCurPos.setY(ptCurPos.y() + vTileSize.y() + iSpacingAmt);
	}
}
