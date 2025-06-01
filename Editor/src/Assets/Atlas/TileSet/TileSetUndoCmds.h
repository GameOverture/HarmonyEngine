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
class TileSetUndoCmd_AppendTiles : public QUndoCommand
{
	AtlasTileSet &						m_TileSetRef;

	Qt::Edge							m_eAppendEdge;
	QSize								m_TileSize;
	QMap<QPoint, QPixmap>				m_PixmapMap;

	QList<QPair<QPoint, TileData *>>	m_AppendedTilesList;

public:
	TileSetUndoCmd_AppendTiles(AtlasTileSet &tileSetItemRef, const QMap<QPoint, QPixmap> &pixmapMapRef, QSize vTileSize, Qt::Edge eAppendEdge, QUndoCommand *pParent = nullptr);
	virtual ~TileSetUndoCmd_AppendTiles();

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
