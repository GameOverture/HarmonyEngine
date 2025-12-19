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
#include "TileData.h"
#include "WgtTileSetAnimation.h"

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
	m_AuxTileSetRef.CmdSet_TileSizeWidgets(m_NewSize);
}

/*virtual*/ void TileSetUndoCmd_TileSize::undo() /*override*/
{
	m_AuxTileSetRef.CmdSet_TileSizeWidgets(m_OldSize);
}

/*virtual*/ int TileSetUndoCmd_TileSize::id() const /*override*/
{
	return MERGABLEUNDOCMD_TileSize;
}

/*virtual*/ bool TileSetUndoCmd_TileSize::mergeWith(const QUndoCommand *pOtherCmd) /*override*/
{
	//TileSetUndoCmd_TileSize *pOtherTileSizeCmd = dynamic_cast<TileSetUndoCmd_TileSize *>(const_cast<QUndoCommand *>(pOtherCmd));
	const TileSetUndoCmd_TileSize *pOtherTileSizeCmd = static_cast<const TileSetUndoCmd_TileSize *>(pOtherCmd); // This is faster than dynamic_cast, and safe as long as I always use unique MERGABLEUNDOCMD's for each ID
	if(pOtherTileSizeCmd && (&pOtherTileSizeCmd->m_TileSetRef == &m_TileSetRef))
	{
		m_NewSize = pOtherTileSizeCmd->m_NewSize;
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TileSetUndoCmd_TileOffset::TileSetUndoCmd_TileOffset(AtlasTileSet &tileSetItemRef, AuxTileSet &auxTileSetRef, QPoint newTileOffset, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_TileSetRef(tileSetItemRef),
	m_AuxTileSetRef(auxTileSetRef),
	m_OldOffset(tileSetItemRef.GetTileOffset()),
	m_NewOffset(newTileOffset)
{
	if(m_OldOffset == m_NewOffset)
		HyGuiLog("TileSetUndoCmd_TileOffset() - Old offset is the same as new offset, no need to create command.", LOGTYPE_Error);
	
	setText("Change Tile Offset");
}

/*virtual*/ TileSetUndoCmd_TileOffset::~TileSetUndoCmd_TileOffset()
{
}

/*virtual*/ void TileSetUndoCmd_TileOffset::redo() /*override*/
{
	m_AuxTileSetRef.CmdSet_TileOffsetWidgets(m_NewOffset);
}

/*virtual*/ void TileSetUndoCmd_TileOffset::undo() /*override*/
{
	m_AuxTileSetRef.CmdSet_TileOffsetWidgets(m_OldOffset);
}

/*virtual*/ int TileSetUndoCmd_TileOffset::id() const /*override*/
{
	return MERGABLEUNDOCMD_TileOffset;
}

/*virtual*/ bool TileSetUndoCmd_TileOffset::mergeWith(const QUndoCommand *pOtherCmd) /*override*/
{
	const TileSetUndoCmd_TileOffset *pOtherTileOffsetCmd = static_cast<const TileSetUndoCmd_TileOffset *>(pOtherCmd); // This is faster than dynamic_cast, and safe as long as I always use unique MERGABLEUNDOCMD's for each ID
	if(pOtherTileOffsetCmd && (&pOtherTileOffsetCmd->m_TileSetRef == &m_TileSetRef))
	{
		m_NewOffset = pOtherTileOffsetCmd->m_NewOffset;
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TileSetUndoCmd_TileShape::TileSetUndoCmd_TileShape(AuxTileSet &auxTileSetRef, TileSetShape eCurShape, TileSetShape eNewShape, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_AuxTileSetRef(auxTileSetRef),
	m_eOldShape(eCurShape),
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
	m_AuxTileSetRef.CmdSet_TileShapeWidget(m_eNewShape);
}

/*virtual*/ void TileSetUndoCmd_TileShape::undo() /*override*/
{
	m_AuxTileSetRef.CmdSet_TileShapeWidget(m_eOldShape);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TileSetUndoCmd_AppendTiles::TileSetUndoCmd_AppendTiles(AuxTileSet &auxTileSetRef, const QMap<QPoint, QPixmap> &pixmapMapRef, QSize vRegionSize, Qt::Edge eAppendEdge, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_AuxTileSetRef(auxTileSetRef),
	m_eAppendEdge(eAppendEdge),
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
		m_AppendedTilesList = m_AuxTileSetRef.GetTileSet()->Cmd_AppendNewTiles(m_RegionSize, m_PixmapMap, m_eAppendEdge);
	else
		m_AuxTileSetRef.GetTileSet()->Cmd_ReaddTiles(m_AppendedTilesList);

	if(m_AuxTileSetRef.GetCurrentPage() != TILESETPAGE_Arrange)
		m_AuxTileSetRef.SetCurrentPage(TILESETPAGE_Arrange);
}

/*virtual*/ void TileSetUndoCmd_AppendTiles::undo() /*override*/
{
	QVector<TileData *> removeTileList;
	for(const auto &pair : m_AppendedTilesList)
	{
		if(pair.second)
			removeTileList.append(pair.second);
	}

	m_AppendedTilesList = m_AuxTileSetRef.GetTileSet()->Cmd_RemoveTiles(removeTileList);

	if (m_AuxTileSetRef.GetCurrentPage() != TILESETPAGE_Arrange)
		m_AuxTileSetRef.SetCurrentPage(TILESETPAGE_Arrange);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TileSetUndoCmd_MoveTiles::TileSetUndoCmd_MoveTiles(AuxTileSet &auxTileSetRef, QList<TileData*> affectedTileList, QList<QPoint> oldGridPosList, QList<QPoint> newGridPosList, QUndoCommand* pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_AuxTileSetRef(auxTileSetRef),
	m_AffectedTileList(affectedTileList),
	m_OldGridPosList(oldGridPosList),
	m_NewGridPosList(newGridPosList)
{
	if(m_AffectedTileList.size() != m_OldGridPosList.size() || m_AffectedTileList.size() != m_NewGridPosList.size())
		HyGuiLog("TileSetUndoCmd_MoveTiles() - Affected tile list size does not match old/new grid position list size.", LOGTYPE_Error);

	setText("Move " % QString::number(m_AffectedTileList.size()) % " Tiles");
}

/*virtual*/ TileSetUndoCmd_MoveTiles::~TileSetUndoCmd_MoveTiles()
{
}

/*virtual*/ void TileSetUndoCmd_MoveTiles::redo() /*override*/
{
	m_AuxTileSetRef.GetTileSet()->Cmd_MoveTiles(m_AffectedTileList, m_NewGridPosList);
	if (m_AuxTileSetRef.GetCurrentPage() != TILESETPAGE_Arrange)
		m_AuxTileSetRef.SetCurrentPage(TILESETPAGE_Arrange);
}

/*virtual*/ void TileSetUndoCmd_MoveTiles::undo() /*override*/
{
	m_AuxTileSetRef.GetTileSet()->Cmd_MoveTiles(m_AffectedTileList, m_OldGridPosList);
	if (m_AuxTileSetRef.GetCurrentPage() != TILESETPAGE_Arrange)
		m_AuxTileSetRef.SetCurrentPage(TILESETPAGE_Arrange);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TileSetUndoCmd_RemoveTiles::TileSetUndoCmd_RemoveTiles(AuxTileSet &auxTileSetRef, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_AuxTileSetRef(auxTileSetRef)
{
	m_TilesMap = m_AuxTileSetRef.GetTileSet()->GetGfxScene()->GetSelectedSetupTiles();

	setText("Remove " % QString::number(m_TilesMap.size()) % " Tiles");
}

/*virtual*/ TileSetUndoCmd_RemoveTiles::~TileSetUndoCmd_RemoveTiles()
{ }

/*virtual*/ void TileSetUndoCmd_RemoveTiles::redo() /*override*/
{
	m_AuxTileSetRef.GetTileSet()->Cmd_RemoveTiles(m_TilesMap.keys());
	if (m_AuxTileSetRef.GetCurrentPage() != TILESETPAGE_Arrange)
		m_AuxTileSetRef.SetCurrentPage(TILESETPAGE_Arrange);
}

/*virtual*/ void TileSetUndoCmd_RemoveTiles::undo() /*override*/
{
	QList<QPair<QPoint, TileData *>> tileDataList;
	tileDataList.reserve(m_TilesMap.size());
	for (TileData *pTileData : m_TilesMap.keys())
		tileDataList.append(QPair<QPoint, TileData *>(pTileData->GetMetaGridPos(), pTileData));
	
	m_AuxTileSetRef.GetTileSet()->Cmd_ReaddTiles(tileDataList);
	if (m_AuxTileSetRef.GetCurrentPage() != TILESETPAGE_Arrange)
		m_AuxTileSetRef.SetCurrentPage(TILESETPAGE_Arrange);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TileSetUndoCmd_AddWgtItem::TileSetUndoCmd_AddWgtItem(AuxTileSet &auxTileSetRef, TileSetWgtType eType, QJsonObject itemDataObj, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_AuxTileSetRef(auxTileSetRef),
	m_eType(eType),
	m_ItemDataObj(itemDataObj)
{
	switch (m_eType)
	{
	case TILESETWGT_Animation:
		setText("Add Animation");
		break;
	case TILESETWGT_TerrainSet:
		setText("Add Terrain Set");
		break;
	case TILESETWGT_Terrain:
		setText("Add Terrain");
		break;
			
	default:
		HyGuiLog("TileSetUndoCmd_AddWgtItem() - Unknown TileSetWgtType.", LOGTYPE_Error);
		break;
	}
}

/*virtual*/ TileSetUndoCmd_AddWgtItem::~TileSetUndoCmd_AddWgtItem()
{
}

/*virtual*/ void TileSetUndoCmd_AddWgtItem::redo() /*override*/
{
	m_AuxTileSetRef.CmdSet_CreateWgtItem(m_eType, m_ItemDataObj);
	m_AuxTileSetRef.GetTileSet()->Cmd_AllocateJsonItem(m_eType, m_ItemDataObj);

	switch (m_eType)
	{
	case TILESETWGT_Animation:
		if (m_AuxTileSetRef.GetCurrentPage() != TILESETPAGE_Animation)
			m_AuxTileSetRef.SetCurrentPage(TILESETPAGE_Animation);
		break;
	case TILESETWGT_TerrainSet:
	case TILESETWGT_Terrain:
		if (m_AuxTileSetRef.GetCurrentPage() != TILESETPAGE_Autotile)
			m_AuxTileSetRef.SetCurrentPage(TILESETPAGE_Autotile);
		break;

	default:
		HyGuiLog("TileSetUndoCmd_AddWgtItem::redo() - Unknown TileSetWgtType.", LOGTYPE_Error);
		break;
	}
}

/*virtual*/ void TileSetUndoCmd_AddWgtItem::undo() /*override*/
{
	QUuid uuid(m_ItemDataObj["UUID"].toString());
	m_AuxTileSetRef.CmdSet_DeleteWgtItem(uuid);

	switch (m_eType)
	{
	case TILESETWGT_Animation:
		if (m_AuxTileSetRef.GetCurrentPage() != TILESETPAGE_Animation)
			m_AuxTileSetRef.SetCurrentPage(TILESETPAGE_Animation);
		break;
	case TILESETWGT_TerrainSet:
	case TILESETWGT_Terrain:
		if (m_AuxTileSetRef.GetCurrentPage() != TILESETPAGE_Autotile)
			m_AuxTileSetRef.SetCurrentPage(TILESETPAGE_Autotile);
		break;

	default:
		HyGuiLog("TileSetUndoCmd_AddWgtItem::redo() - Unknown TileSetWgtType.", LOGTYPE_Error);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TileSetUndoCmd_RemoveWgtItem::TileSetUndoCmd_RemoveWgtItem(AuxTileSet &auxTileSetRef, QUuid uuid, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_AuxTileSetRef(auxTileSetRef)
{
	IWgtTileSetItem *pWgtItem = m_AuxTileSetRef.FindWgtItem(uuid);
	if (pWgtItem == nullptr)
	{
		HyGuiLog("TileSetUndoCmd_RemoveWgtItem() - Could not find IWgtTileSetItem with given UUID.", LOGTYPE_Error);
		return;
	}

	m_eRemovedType = pWgtItem->GetWgtType();
	m_RemovedItemDataObj = m_AuxTileSetRef.GetTileSet()->GetJsonItem(uuid);
	switch (m_eRemovedType)
	{
	case TILESETWGT_Animation:
		setText("Remove Animation");
		break;
	case TILESETWGT_TerrainSet:
		setText("Remove Terrain Set");
		break;
	case TILESETWGT_Terrain:
		setText("Remove Terrain");
		break;

	default:
		HyGuiLog("TileSetUndoCmd_RemoveWgtItem() - Unknown TileSetWgtType.", LOGTYPE_Error);
		break;
	}
}

/*virtual*/ TileSetUndoCmd_RemoveWgtItem::~TileSetUndoCmd_RemoveWgtItem()
{
}

/*virtual*/ void TileSetUndoCmd_RemoveWgtItem::redo() /*override*/
{
	m_AuxTileSetRef.CmdSet_DeleteWgtItem(QUuid(m_RemovedItemDataObj["UUID"].toString()));

	switch (m_eRemovedType)
	{
	case TILESETWGT_Animation:
		if (m_AuxTileSetRef.GetCurrentPage() != TILESETPAGE_Animation)
			m_AuxTileSetRef.SetCurrentPage(TILESETPAGE_Animation);
		break;
	case TILESETWGT_TerrainSet:
	case TILESETWGT_Terrain:
		if (m_AuxTileSetRef.GetCurrentPage() != TILESETPAGE_Autotile)
			m_AuxTileSetRef.SetCurrentPage(TILESETPAGE_Autotile);
		break;

	default:
		HyGuiLog("TileSetUndoCmd_RemoveWgtItem::redo() - Unknown TileSetWgtType.", LOGTYPE_Error);
		break;
	}
}

/*virtual*/ void TileSetUndoCmd_RemoveWgtItem::undo() /*override*/
{
	m_AuxTileSetRef.CmdSet_CreateWgtItem(m_eRemovedType, m_RemovedItemDataObj);
	m_AuxTileSetRef.GetTileSet()->Cmd_AllocateJsonItem(m_eRemovedType, m_RemovedItemDataObj);

	switch (m_eRemovedType)
	{
	case TILESETWGT_Animation:
		if (m_AuxTileSetRef.GetCurrentPage() != TILESETPAGE_Animation)
			m_AuxTileSetRef.SetCurrentPage(TILESETPAGE_Animation);
		break;
	case TILESETWGT_TerrainSet:
	case TILESETWGT_Terrain:
		if (m_AuxTileSetRef.GetCurrentPage() != TILESETPAGE_Autotile)
			m_AuxTileSetRef.SetCurrentPage(TILESETPAGE_Autotile);
		break;

	default:
		HyGuiLog("TileSetUndoCmd_RemoveWgtItem::redo() - Unknown TileSetWgtType.", LOGTYPE_Error);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TileSetUndoCmd_OrderWgtItem::TileSetUndoCmd_OrderWgtItem(AuxTileSet &auxTileSetRef, QUuid uuid, int iOldIndex, int iNewIndex, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_AuxTileSetRef(auxTileSetRef),
	m_Uuid(uuid),
	m_iOldIndex(iOldIndex),
	m_iNewIndex(iNewIndex)
{
	if(m_iOldIndex == m_iNewIndex)
		HyGuiLog("TileSetUndoCmd_OrderWgtItem() - Old index is the same as new index, no need to create command.", LOGTYPE_Error);
	
	IWgtTileSetItem *pWgtItem = m_AuxTileSetRef.FindWgtItem(m_Uuid);
	if (pWgtItem == nullptr)
	{
		HyGuiLog("TileSetUndoCmd_OrderWgtItem() - Could not find IWgtTileSetItem with given UUID.", LOGTYPE_Error);
		return;
	}
	switch (pWgtItem->GetWgtType())
	{
	case TILESETWGT_Animation:
		setText("Reorder Animation");
		m_ePage = TILESETPAGE_Animation;
		break;
	case TILESETWGT_TerrainSet:
		setText("Reorder Terrain Set");
		m_ePage = TILESETPAGE_Autotile;
		break;
	case TILESETWGT_Terrain:
		setText("Reorder Terrain");
		m_ePage = TILESETPAGE_Autotile;
		break;
	
	default:
		HyGuiLog("TileSetUndoCmd_OrderWgtItem() - Unknown TileSetWgtType.", LOGTYPE_Error);
		break;
	}
}

/*virtual*/ TileSetUndoCmd_OrderWgtItem::~TileSetUndoCmd_OrderWgtItem()
{
}

/*virtual*/ void TileSetUndoCmd_OrderWgtItem::redo() /*override*/
{
	m_AuxTileSetRef.CmdSet_OrderWgtItem(m_Uuid, m_iNewIndex);
	m_AuxTileSetRef.SetCurrentPage(m_ePage);
}

/*virtual*/ void TileSetUndoCmd_OrderWgtItem::undo() /*override*/
{
	m_AuxTileSetRef.CmdSet_OrderWgtItem(m_Uuid, m_iOldIndex);
	m_AuxTileSetRef.SetCurrentPage(m_ePage);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TileSetUndoCmd_ModifyWgtItem::TileSetUndoCmd_ModifyWgtItem(AuxTileSet &auxTileSetRef, QString sUndoText, int iMergeId, QUuid uuid, QJsonObject oldItemDataObj, QJsonObject newItemDataObj, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_AuxTileSetRef(auxTileSetRef),
	m_iMergeId(iMergeId),
	m_Uuid(uuid),
	m_OldItemDataObj(oldItemDataObj),
	m_NewItemDataObj(newItemDataObj)
{
	setText(sUndoText);

	IWgtTileSetItem *pWgtItem = m_AuxTileSetRef.FindWgtItem(m_Uuid);
	if (pWgtItem == nullptr)
	{
		HyGuiLog("TileSetUndoCmd_ModifyWgtItem() - Could not find IWgtTileSetItem with given UUID.", LOGTYPE_Error);
		return;
	}
	switch (pWgtItem->GetWgtType())
	{
	case TILESETWGT_Animation:
		m_ePage = TILESETPAGE_Animation;
		break;
	case TILESETWGT_TerrainSet:
		m_ePage = TILESETPAGE_Autotile;
		break;
	case TILESETWGT_Terrain:
		m_ePage = TILESETPAGE_Autotile;
		break;
	
	default:
		HyGuiLog("TileSetUndoCmd_ModifyWgtItem() - Unknown TileSetWgtType.", LOGTYPE_Error);
		break;
	}
}

/*virtual*/ TileSetUndoCmd_ModifyWgtItem::~TileSetUndoCmd_ModifyWgtItem()
{
}

/*virtual*/ void TileSetUndoCmd_ModifyWgtItem::redo() /*override*/
{
	m_AuxTileSetRef.CmdSet_ModifyWgtItem(m_Uuid, m_NewItemDataObj);
	m_AuxTileSetRef.SetCurrentPage(m_ePage);
}

/*virtual*/ void TileSetUndoCmd_ModifyWgtItem::undo() /*override*/
{
	m_AuxTileSetRef.CmdSet_ModifyWgtItem(m_Uuid, m_OldItemDataObj);
	m_AuxTileSetRef.SetCurrentPage(m_ePage);
}

/*virtual*/ int TileSetUndoCmd_ModifyWgtItem::id() const /*override*/
{
	return m_iMergeId;
}

/*virtual*/ bool TileSetUndoCmd_ModifyWgtItem::mergeWith(const QUndoCommand *pOtherCmd) /*override*/
{
	const TileSetUndoCmd_ModifyWgtItem *pOtherModifyCmd = static_cast<const TileSetUndoCmd_ModifyWgtItem *>(pOtherCmd);
	if(pOtherModifyCmd && (pOtherModifyCmd->m_Uuid == m_Uuid) && (pOtherModifyCmd->m_iMergeId == m_iMergeId))
	{
		m_NewItemDataObj = pOtherModifyCmd->m_NewItemDataObj;
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TileSetUndoCmd_ApplyTerrainSet::TileSetUndoCmd_ApplyTerrainSet(AuxTileSet &auxTileSetRef, QList<TileData *> affectedTileList, QUuid newTerrainSetUuid) :
	QUndoCommand(),
	m_AuxTileSetRef(auxTileSetRef),
	m_AffectedTileList(affectedTileList),
	m_NewTerrainSetUuid(newTerrainSetUuid)
{
	setText("Apply Terrain Set to " % QString::number(m_AffectedTileList.size()) % " Tiles");

	for(TileData *pTile : m_AffectedTileList)
		m_OldTerrainSetUuidList.append(pTile->GetTerrainSet());
}

/*virtual*/ TileSetUndoCmd_ApplyTerrainSet::~TileSetUndoCmd_ApplyTerrainSet()
{
}

/*virtual*/ void TileSetUndoCmd_ApplyTerrainSet::redo() /*override*/
{
	QList<QUuid> newTerrainSetList;
	newTerrainSetList.fill(m_NewTerrainSetUuid, m_AffectedTileList.size());
	m_AuxTileSetRef.CmdSet_ApplyTerrainSet(m_AffectedTileList, newTerrainSetList);
	m_AuxTileSetRef.SetCurrentPage(TILESETPAGE_Autotile);
}

/*virtual*/ void TileSetUndoCmd_ApplyTerrainSet::undo() /*override*/
{
	m_AuxTileSetRef.CmdSet_ApplyTerrainSet(m_AffectedTileList, m_OldTerrainSetUuidList);
	m_AuxTileSetRef.SetCurrentPage(TILESETPAGE_Autotile);
}
