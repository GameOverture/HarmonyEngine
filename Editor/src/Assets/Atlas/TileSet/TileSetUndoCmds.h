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
class TileSetUndoCmd_TileShape : public QUndoCommand
{
	AuxTileSet &						m_AuxTileSetRef;
	TileSetShape						m_eOldShape;
	TileSetShape						m_eNewShape;

public:
	TileSetUndoCmd_TileShape(AuxTileSet &auxTileSetRef, TileSetShape eCurShape, TileSetShape eNewShape, QUndoCommand *pParent = nullptr);
	virtual ~TileSetUndoCmd_TileShape();
	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TileSetUndoCmd_TileSize : public QUndoCommand
{
	AtlasTileSet &m_TileSetRef;
	AuxTileSet &m_AuxTileSetRef;

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
class TileSetUndoCmd_TileOffset : public QUndoCommand
{
	AtlasTileSet &						m_TileSetRef;
	AuxTileSet &						m_AuxTileSetRef;

	QPoint								m_OldOffset;
	QPoint								m_NewOffset;

public:
	TileSetUndoCmd_TileOffset(AtlasTileSet &tileSetItemRef, AuxTileSet &auxTileSetRef, QPoint newTileOffset, QUndoCommand *pParent = nullptr);
	virtual ~TileSetUndoCmd_TileOffset();

	virtual void redo() override;
	virtual void undo() override;
	virtual int id() const override;
	virtual bool mergeWith(const QUndoCommand *pOtherCmd) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TileSetUndoCmd_AppendTiles : public QUndoCommand
{
	AuxTileSet &						m_AuxTileSetRef;

	Qt::Edge							m_eAppendEdge;
	QSize								m_RegionSize;
	QMap<QPoint, QPixmap>				m_PixmapMap;

	QList<QPair<QPoint, TileData *>>	m_AppendedTilesList;

public:
	TileSetUndoCmd_AppendTiles(AuxTileSet &auxTileSetRef, const QMap<QPoint, QPixmap> &pixmapMapRef, QSize vRegionSize, Qt::Edge eAppendEdge, QUndoCommand *pParent = nullptr);
	virtual ~TileSetUndoCmd_AppendTiles();

	virtual void redo() override;
	virtual void undo() override;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TileSetUndoCmd_MoveTiles : public QUndoCommand
{
	AuxTileSet &					m_AuxTileSetRef;

	QList<TileData*>				m_AffectedTileList;
	QList<QPoint>					m_OldGridPosList;
	QList<QPoint>					m_NewGridPosList;

public:
	TileSetUndoCmd_MoveTiles(AuxTileSet &auxTileSetRef, QList<TileData*> affectedTileList, QList<QPoint> oldGridPosList, QList<QPoint> newGridPosList, QUndoCommand *pParent = nullptr);
	virtual ~TileSetUndoCmd_MoveTiles();

	virtual void redo() override;
	virtual void undo() override;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TileSetUndoCmd_RemoveTiles : public QUndoCommand
{
	AuxTileSet &						m_AuxTileSetRef;
	QMap<TileData *, TileSetGfxItem *>	m_TilesMap;

public:
	TileSetUndoCmd_RemoveTiles(AuxTileSet &auxTileSetRef, QUndoCommand *pParent = nullptr);
	virtual ~TileSetUndoCmd_RemoveTiles();

	virtual void redo() override;
	virtual void undo() override;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TileSetUndoCmd_AddWgtItem : public QUndoCommand
{
	AuxTileSet &						m_AuxTileSetRef;
	TileSetWgtType						m_eType;
	QJsonObject							m_ItemDataObj;

public:
	TileSetUndoCmd_AddWgtItem(AuxTileSet &auxTileSetRef, TileSetWgtType eType, QJsonObject itemDataObj, QUndoCommand *pParent = nullptr);
	virtual ~TileSetUndoCmd_AddWgtItem();

	virtual void redo() override;
	virtual void undo() override;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TileSetUndoCmd_RemoveWgtItem : public QUndoCommand
{
	AuxTileSet &						m_AuxTileSetRef;
	TileSetWgtType						m_eRemovedType;
	QJsonObject							m_RemovedItemDataObj;

public:
	TileSetUndoCmd_RemoveWgtItem(AuxTileSet &auxTileSetRef, QUuid uuid, QUndoCommand *pParent = nullptr);
	virtual ~TileSetUndoCmd_RemoveWgtItem();

	virtual void redo() override;
	virtual void undo() override;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TileSetUndoCmd_OrderWgtItem : public QUndoCommand
{
	AuxTileSet &						m_AuxTileSetRef;
	QUuid								m_Uuid;
	int									m_iOldIndex;
	int									m_iNewIndex;
	TileSetPage							m_ePage;

public:
	TileSetUndoCmd_OrderWgtItem(AuxTileSet &auxTileSetRef, QUuid uuid, int iOldIndex, int iNewIndex, QUndoCommand *pParent = nullptr);
	virtual ~TileSetUndoCmd_OrderWgtItem();

	virtual void redo() override;
	virtual void undo() override;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TileSetUndoCmd_ModifyWgtItem : public QUndoCommand
{
	AuxTileSet &						m_AuxTileSetRef;
	int									m_iMergeId;
	QUuid								m_Uuid;
	QJsonObject							m_OldItemDataObj;
	QJsonObject							m_NewItemDataObj;
	TileSetPage							m_ePage;

public:
	TileSetUndoCmd_ModifyWgtItem(AuxTileSet &auxTileSetRef, QString sUndoText, int iMergeId, QUuid uuid, QJsonObject oldItemDataObj, QJsonObject newItemDataObj, QUndoCommand *pParent = nullptr);
	virtual ~TileSetUndoCmd_ModifyWgtItem();
	
	virtual void redo() override;
	virtual void undo() override;
	virtual int id() const override;
	virtual bool mergeWith(const QUndoCommand *pOtherCmd) override;
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TileSetUndoCmd_ApplyTerrainSet : public QUndoCommand
{
	AuxTileSet &						m_AuxTileSetRef;
	QList<TileData*>					m_AffectedTileList;
	QList<QUuid>						m_OldTerrainSetUuidList;
	QUuid								m_NewTerrainSetUuid;

public:
	TileSetUndoCmd_ApplyTerrainSet(AuxTileSet &auxTileSetRef, QList<TileData *> affectedTileList, QUuid newTerrainSetUuid, QUndoCommand *pParent = nullptr);
	virtual ~TileSetUndoCmd_ApplyTerrainSet();

	virtual void redo() override;
	virtual void undo() override;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TileSetUndoCmd_PaintAnimation : public QUndoCommand
{
	AuxTileSet &						m_AuxTileSetRef;
	QUuid								m_AnimationUuid;
	bool								m_bLeftClick;
	QList<TileData *>					m_PaintedMap;
	QList<QUuid>						m_OriginalAnimationMap;

public:
	TileSetUndoCmd_PaintAnimation(AuxTileSet &auxTileSetRef, bool bLeftClick, QList<TileData *> paintedTiles, QUndoCommand *pParent = nullptr);
	virtual ~TileSetUndoCmd_PaintAnimation();
	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TileSetUndoCmd_PaintAutoTileParts : public QUndoCommand
{
	AuxTileSet &										m_AuxTileSetRef;
	QUuid												m_TerrainUuid;
	bool												m_bLeftClick;
	QMap<TileData *, QBitArray>							m_PaintedPartsMap;
	QList<QPair<TileData *, QMap<QUuid, QBitArray>>>	m_OriginalTerrainMap;

public:
	TileSetUndoCmd_PaintAutoTileParts(AuxTileSet &auxTileSetRef, bool bLeftClick, QMap<TileData *, QBitArray> paintedParts, QUndoCommand *pParent = nullptr);
	virtual ~TileSetUndoCmd_PaintAutoTileParts();
	virtual void redo() override;
	virtual void undo() override;
};


#endif // TILESETUNDOCMDS_H
