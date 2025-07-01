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

TileSetUndoCmd_TileSize::TileSetUndoCmd_TileSize(AtlasTileSet &tileSetItemRef, AuxTileSet &auxTileSetRef, QSize newTileSize, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_TileSetRef(tileSetItemRef),
	m_AuxTileSetRef(auxTileSetRef),
	m_OldSize(tileSetItemRef.GetTileSize()),
	m_NewSize(newTileSize)
{
	if(m_OldSize == m_NewSize)
		HyGuiLog("TileSetUndoCmd_TileSize() - Old size is the same as new size, no need to create command.", LOGTYPE_Error);

	setText("Change Tile Size");
}

/*virtual*/ TileSetUndoCmd_TileSize::~TileSetUndoCmd_TileSize()
{
}

/*virtual*/ void TileSetUndoCmd_TileSize::redo() /*override*/
{
	m_TileSetRef.SetTileSize(m_NewSize);
	m_AuxTileSetRef.SetTileSizeWidgets(m_NewSize);
}

/*virtual*/ void TileSetUndoCmd_TileSize::undo() /*override*/
{
	m_TileSetRef.SetTileSize(m_OldSize);
	m_AuxTileSetRef.SetTileSizeWidgets(m_OldSize);
}

/*virtual*/ int TileSetUndoCmd_TileSize::id() const /*override*/
{
	return MERGABLEUNDOCMD_TileSize;
}

/*virtual*/ bool TileSetUndoCmd_TileSize::mergeWith(const QUndoCommand *pOtherCmd) /*override*/
{
	//TileSetUndoCmd_TileSize *pOtherTileSizeCmd = dynamic_cast<TileSetUndoCmd_TileSize *>(const_cast<QUndoCommand *>(pOtherCmd));
	const TileSetUndoCmd_TileSize *pOtherTileSizeCmd = static_cast<const TileSetUndoCmd_TileSize *>(pOtherCmd); // This is faster, and safe as long as I always use unique MERGABLEUNDOCMD's for each ID
	if(pOtherTileSizeCmd && (&pOtherTileSizeCmd->m_TileSetRef == &m_TileSetRef))
	{
		m_NewSize = pOtherTileSizeCmd->m_NewSize;
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TileSetUndoCmd_TileShape::TileSetUndoCmd_TileShape(AtlasTileSet &tileSetItemRef, AuxTileSet &auxTileSetRef, TileSetShape eNewShape, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_TileSetRef(tileSetItemRef),
	m_AuxTileSetRef(auxTileSetRef),
	m_eOldShape(tileSetItemRef.GetTileShape()),
	m_eNewShape(eNewShape)
{
	if(m_eOldShape == m_eNewShape)
		HyGuiLog("TileSetUndoCmd_TileShape() - Old shape is the same as new shape, no need to create command.", LOGTYPE_Error);
	
	setText("Change Tile Shape");
}

/*virtual*/ TileSetUndoCmd_TileShape::~TileSetUndoCmd_TileShape()
{
}

/*virtual*/ void TileSetUndoCmd_TileShape::redo() /*override*/
{
	m_TileSetRef.SetTileShape(m_eNewShape);
	m_AuxTileSetRef.SetTileShapeWidget(m_eNewShape);
}

/*virtual*/ void TileSetUndoCmd_TileShape::undo() /*override*/
{
	m_TileSetRef.SetTileShape(m_eOldShape);
	m_AuxTileSetRef.SetTileShapeWidget(m_eOldShape);
}

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

