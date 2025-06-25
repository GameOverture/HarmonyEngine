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

TileSetUndoCmd_AppendTiles::TileSetUndoCmd_AppendTiles(AtlasTileSet &tileSetItemRef, const QMap<QPoint, QPixmap> &pixmapMapRef, QSize vRegionSize, Qt::Edge eAppendEdge, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_eAppendEdge(eAppendEdge),
	m_TileSetRef(tileSetItemRef),
	m_RegionSize(vRegionSize),
	m_PixmapMap(pixmapMapRef)
{
	setText("Add " % QString::number(m_PixmapMap.size()) % " Tiles");
}

/*virtual*/ TileSetUndoCmd_AppendTiles::~TileSetUndoCmd_AppendTiles()
{
}

/*virtual*/ void TileSetUndoCmd_AppendTiles::redo() /*override*/
{
	if(m_AppendedTilesList.empty())
		m_AppendedTilesList = m_TileSetRef.Cmd_AppendNewTiles(m_RegionSize, m_PixmapMap, m_eAppendEdge);
	else
		m_TileSetRef.Cmd_ReaddTiles(m_AppendedTilesList);
}

/*virtual*/ void TileSetUndoCmd_AppendTiles::undo() /*override*/
{
	QVector<TileData *> removeTileList;
	for(const auto &pair : m_AppendedTilesList)
	{
		if(pair.second)
			removeTileList.append(pair.second);
	}

	m_AppendedTilesList = m_TileSetRef.Cmd_RemoveTiles(removeTileList);
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

