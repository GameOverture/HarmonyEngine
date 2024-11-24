/**************************************************************************
 *	TileSetUndoCmds.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "TileSetUndoCmds.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TileSetUndoCmd_ManipTiles::TileSetUndoCmd_ManipTiles(AtlasTileSet &tileSetItemRef, QVector<QGraphicsPixmapItem *> pixmapList, QSize vTileSizes, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_TileSetRef(tileSetItemRef),
	m_TileSizes(vTileSizes)
{
	setText("Add " % QString::number(pixmapList.size()) % " Tiles");

	m_PixmapList.reserve(pixmapList.size());
	for(auto pPixmap : pixmapList)
		m_PixmapList.append(pPixmap->pixmap());
}

/*virtual*/ TileSetUndoCmd_ManipTiles::~TileSetUndoCmd_ManipTiles()
{
}

/*virtual*/ void TileSetUndoCmd_ManipTiles::redo() /*override*/
{
	m_AppendedTilesAtlasIndexList = m_TileSetRef.Cmd_AppendTiles(m_TileSizes, m_PixmapList, Qt::BottomEdge);
}

/*virtual*/ void TileSetUndoCmd_ManipTiles::undo() /*override*/
{
	m_TileSetRef.Cmd_RemoveTiles(m_AppendedTilesAtlasIndexList);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//TileSetUndoCmd_RemoveTiles::TileSetUndoCmd_RemoveTiles(AtlasTileSet &tileSetItemRef, QUndoCommand *pParent /*= nullptr*/) :
//	QUndoCommand(pParent),
//	m_TileSetRef(tileSetItemRef)
//{
//	setText("Remove " % QString::number(m_FrameList.size()) % " Frames from State " % QString::number(iStateIndex));
//}
//
///*virtual*/ TileSetUndoCmd_RemoveTiles::~TileSetUndoCmd_RemoveTiles()
//{ }
//
///*virtual*/ void TileSetUndoCmd_RemoveTiles::redo() /*override*/
//{
//	static_cast<SpriteModel *>(m_SpriteItemRef.GetModel())->Cmd_RemoveFrames(m_iStateIndex, m_FrameList);
//	m_SpriteItemRef.FocusWidgetState(m_iStateIndex, -1);
//}
//
///*virtual*/ void TileSetUndoCmd_RemoveTiles::undo() /*override*/
//{
//	QVariant focusSubState = static_cast<SpriteModel *>(m_SpriteItemRef.GetModel())->Cmd_AddFrames(m_iStateIndex, m_FrameList);
//	m_SpriteItemRef.FocusWidgetState(m_iStateIndex, focusSubState);
//}

