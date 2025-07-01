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
#include "AuxTileSet.h"

#include <QUndoCommand>


 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TileSetUndoCmd_TileSize : public QUndoCommand
{
	AtlasTileSet &						m_TileSetRef;
	AuxTileSet &						m_AuxTileSetRef;

	QSize								m_OldSize;
	QSize								m_NewSize;

public:
	TileSetUndoCmd_TileSize(AtlasTileSet &tileSetItemRef, AuxTileSet &auxTileSetRef, QSize newTileSize, QUndoCommand *pParent = nullptr);
	virtual ~TileSetUndoCmd_TileSize();

	virtual void redo() override;
	virtual void undo() override;
	virtual int id() const override;
	virtual bool mergeWith(const QUndoCommand *pOtherCmd) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TileSetUndoCmd_TileShape : public QUndoCommand
{
	AtlasTileSet &						m_TileSetRef;
	AuxTileSet &						m_AuxTileSetRef;
	TileSetShape							m_eOldShape;
	TileSetShape							m_eNewShape;

public:
	TileSetUndoCmd_TileShape(AtlasTileSet &tileSetItemRef, AuxTileSet &auxTileSetRef, TileSetShape eNewShape, QUndoCommand *pParent = nullptr);
	virtual ~TileSetUndoCmd_TileShape();
	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TileSetUndoCmd_AppendTiles : public QUndoCommand
{
	AtlasTileSet &						m_TileSetRef;

	Qt::Edge							m_eAppendEdge;
	QSize								m_RegionSize;
	QMap<QPoint, QPixmap>				m_PixmapMap;

	QList<QPair<QPoint, TileData *>>	m_AppendedTilesList;

public:
	TileSetUndoCmd_AppendTiles(AtlasTileSet &tileSetItemRef, const QMap<QPoint, QPixmap> &pixmapMapRef, QSize vRegionSize, Qt::Edge eAppendEdge, QUndoCommand *pParent = nullptr);
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
