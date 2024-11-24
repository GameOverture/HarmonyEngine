/**************************************************************************
 *	TileSetUndoCmds.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef TILESETUNDOCMDS_H
#define TILESETUNDOCMDS_H

#include "AtlasTileSet.h"

#include <QUndoCommand>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TileSetUndoCmd_ManipTiles : public QUndoCommand
{
	AtlasTileSet &					m_TileSetRef;

	QSize							m_TileSizes;
	QVector<QPixmap>				m_PixmapList;

	QVector<int>					m_AppendedTilesAtlasIndexList;

public:
	TileSetUndoCmd_ManipTiles(AtlasTileSet &tileSetItemRef, QVector<QGraphicsPixmapItem *> pixmapList, QSize vTileSizes, QUndoCommand *pParent = nullptr);
	virtual ~TileSetUndoCmd_ManipTiles();

	virtual void redo() override;
	virtual void undo() override;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//class TileSetUndoCmd_RemoveTiles : public QUndoCommand
//{
//	AtlasTileSet &					m_TileSetRef;
//
//public:
//	TileSetUndoCmd_RemoveTiles(AtlasTileSet &tileSetItemRef, QUndoCommand *pParent = nullptr);
//	virtual ~TileSetUndoCmd_RemoveTiles();
//
//	virtual void redo() override;
//	virtual void undo() override;
//};
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//class TileSetUndoCmd_SortTiles : public QUndoCommand
//{
//	AtlasTileSet &					m_TileSetRef;
//
//public:
//	TileSetUndoCmd_SortTiles(AtlasTileSet &tileSetItemRef, QUndoCommand *pParent = nullptr);
//	virtual ~TileSetUndoCmd_SortTiles();
//
//	virtual void redo() override;
//	virtual void undo() override;
//};

#endif // TILESETUNDOCMDS_H
