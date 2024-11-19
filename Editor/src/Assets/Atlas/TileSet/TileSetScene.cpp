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

	int iSpacingAmt = 5;

	QPoint ptCurPos;
	ptCurPos.setX(iSpacingAmt);
	ptCurPos.setY(iSpacingAmt);

	m_ImportLabel.setVisible(true);
	m_ImportLabel.setPlainText("Importing Tiles:");
	ptCurPos.setY(ptCurPos.y() + vTileSize.y() + iSpacingAmt);

	int iPixmapIndex = 0;
	for(int i = 0; i < iNumRows; ++i)
	{
		for(int j = 0; j < iNumColumns; ++j, ++iPixmapIndex)
		{
			m_ImportTilePixmapList[iPixmapIndex]->setPos(ptCurPos);
			ptCurPos.setX(ptCurPos.x() + vTileSize.x() + iSpacingAmt);
		}

		ptCurPos.setX(iSpacingAmt);
		ptCurPos.setY(ptCurPos.y() + vTileSize.y() + iSpacingAmt);
	}
}
