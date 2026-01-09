/**************************************************************************
 *	AtlasTileSet.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AtlasTileSet.h"
#include "AtlasModel.h"
#include "TileData.h"
#include "AtlasPacker.h"
#include "Project.h"

#include <QPainter>

bool operator<(const QPoint &a, const QPoint &b)
{
	if(a.y() != b.y())
		return a.y() < b.y();
	else
		return a.x() < b.x();
}

AtlasTileSet::AtlasTileSet(IManagerModel &modelRef,
						   QUuid uuid,
						   quint32 uiChecksum,
						   quint32 uiBankId,
						   QString sName,
						   HyTextureInfo texInfo,
						   quint16 uiW,
						   quint16 uiH,
						   quint16 uiX,
						   quint16 uiY,
						   int iTextureIndex,
						   const FileDataPair &tileSetDataPair,
						   bool bIsPendingSave,
						   uint uiErrors) :
	AtlasFrame(ITEM_AtlasTileSet, modelRef, ITEM_AtlasTileSet, uuid, uiChecksum, uiBankId, sName, 0, 0, 0, 0, texInfo, uiW, uiH, uiX, uiY, iTextureIndex, uiErrors),
	m_TileSetDataPair(tileSetDataPair),
	m_bExistencePendingSave(bIsPendingSave),
	m_bSubAtlasDirty(bIsPendingSave),
	m_eTileShape(TILESETSHAPE_Unknown)
{
	//m_ActionSave.setIcon(QIcon(":/icons16x16/file-save.png"));
	////m_ActionSave.setShortcuts(QKeySequence::Save);
	////m_ActionSave.setShortcutContext(Qt::WidgetShortcut);
	//m_ActionSave.setToolTip("Save Tile Set and pack into Atlas manager");
	//m_ActionSave.setObjectName("Save");
	//QObject::connect(&m_ActionSave, &QAction::triggered, this, &AtlasTileSet::on_actionSave_triggered);

	m_pUndoStack = new QUndoStack(this);
	m_pActionUndo = m_pUndoStack->createUndoAction(nullptr, "&Undo");
	m_pActionUndo->setIcon(QIcon(":/icons16x16/edit-undo.png"));
	//m_pActionUndo->setShortcuts(QKeySequence::Undo);
	//m_pActionUndo->setShortcutContext(Qt::WidgetShortcut);
	m_pActionUndo->setObjectName("Undo");
	m_pActionRedo = m_pUndoStack->createRedoAction(nullptr, "&Redo");
	m_pActionRedo->setIcon(QIcon(":/icons16x16/edit-redo.png"));
	//m_pActionRedo->setShortcuts(QKeySequence::Redo);
	//m_pActionRedo->setShortcutContext(Qt::WidgetShortcut);
	m_pActionRedo->setObjectName("Redo");

	// Initialize AtlasTileSet members with 'm_TileSetDataPair' meta data
	if (m_TileSetDataPair.m_Meta.empty() == false)
	{
		m_eTileShape = HyGlobal::GetTileSetShapeFromString(m_TileSetDataPair.m_Meta["tileShape"].toString());

		QJsonArray atlasRegionSizeArray = m_TileSetDataPair.m_Meta["regionSize"].toArray();
		m_RegionSize = QSize(atlasRegionSizeArray[0].toInt(), atlasRegionSizeArray[1].toInt());

		QJsonArray tileSizeArray = m_TileSetDataPair.m_Meta["tileSize"].toArray();
		m_TileSize = QSize(tileSizeArray[0].toInt(), tileSizeArray[1].toInt());

		QJsonArray tileOffsetArray = m_TileSetDataPair.m_Meta["tileOffset"].toArray();
		m_TileOffset = QPoint(tileOffsetArray[0].toInt(), tileOffsetArray[1].toInt());

		UpdateTilePolygon();

		QJsonArray animationArray = m_TileSetDataPair.m_Meta["animations"].toArray();
		m_AnimationList.reserve(animationArray.size());
		for (int i = 0; i < animationArray.size(); ++i)
		{
			QJsonObject animationObj = animationArray[i].toObject();
			m_AnimationList.push_back(Animation(animationObj));
		}

		QJsonArray terrainSetArray = m_TileSetDataPair.m_Meta["terrainSets"].toArray();
		m_TerrainSetList.reserve(terrainSetArray.size());
		for (int i = 0; i < terrainSetArray.size(); ++i)
		{
			QJsonObject terrainSetObj = terrainSetArray[i].toObject();
			m_TerrainSetList.push_back(TerrainSet(terrainSetObj));
		}

		QJsonArray collisionLayerArray = m_TileSetDataPair.m_Meta["collisionLayers"].toArray();
		m_CollisionLayerList.reserve(collisionLayerArray.size());
		for (int i = 0; i < collisionLayerArray.size(); ++i)
		{
			QJsonObject collisionObj = collisionLayerArray[i].toObject();
			m_CollisionLayerList.push_back(CollisionLayer(collisionObj));
		}

		QJsonArray tileArray = m_TileSetDataPair.m_Meta["tileData"].toArray();

		// Slice the pixmaps from the sub-atlas. The row-order of the pixmaps is aligned with m_TileDataList
		QVector<QPixmap> pixmapList;
		QImage subAtlas(GetAbsMetaFilePath());
		if (subAtlas.isNull() == false && tileArray.size() > 0)
		{
			const int iNUM_COLS = NUM_COLS_TILESET(tileArray.size());
			for (int index = 0; index < tileArray.size(); ++index)
			{
				int iCol = index % iNUM_COLS;
				int iRow = index / iNUM_COLS;

				QRect tileRect(iCol * (m_RegionSize.width() + TILESET_TILE_PADDING),
							   iRow * (m_RegionSize.height() + TILESET_TILE_PADDING),
							   m_RegionSize.width(),
							   m_RegionSize.height());
				QPixmap tilePixmap = QPixmap::fromImage(subAtlas.copy(tileRect));
				pixmapList.append(tilePixmap);
			}

			for (int i = 0; i < tileArray.size(); ++i)
			{
				QJsonObject tileObj = tileArray[i].toObject();
				m_TileDataList.append(new TileData(tileObj, pixmapList[i]));
			}
		}

		if(m_TileDataList.size() != tileArray.size())
		{
			SetError(ASSETERROR_CannotFindMetaFile);
			HyGuiLog("AtlasTileSet::AtlasTileSet() - Tile data count mismatch for TileSet: " + GetName(), LOGTYPE_Error);
		}

		m_bSubAtlasDirty = false;
	}
	else
	{
		m_eTileShape = TILESETSHAPE_Square;
		m_bSubAtlasDirty = true;
	}

	m_GfxScene.Initialize(this);
}

AtlasTileSet::~AtlasTileSet()
{
	for(auto it = m_TileDataList.begin(); it != m_TileDataList.end(); ++it)
		delete *it;

	delete m_pUndoStack;
}

int AtlasTileSet::GetNumTiles() const
{
	return m_TileDataList.size();
}

TileSetShape AtlasTileSet::GetTileShape() const
{
	return m_eTileShape;
}

void AtlasTileSet::SetTileShape(TileSetShape eTileSetShape)
{
	m_eTileShape = eTileSetShape;
	UpdateTilePolygon();
}

QSize AtlasTileSet::GetAtlasRegionSize() const
{
	return m_RegionSize;
}

void AtlasTileSet::SetAtlasRegionSize(QSize size)
{
	m_RegionSize = size;
}

QSize AtlasTileSet::GetTileSize() const
{
	return m_TileSize;
}

void AtlasTileSet::SetTileSize(QSize size)
{
	m_TileSize = size;
	UpdateTilePolygon();
}

QPoint AtlasTileSet::GetTileOffset() const
{
	return m_TileOffset;
}

void AtlasTileSet::SetTileOffset(QPoint offset)
{
	m_TileOffset = offset;
}

QPolygonF AtlasTileSet::GetTilePolygon() const
{
	return m_TilePolygon;
}

void AtlasTileSet::SetTilePolygon(const QPolygonF &polygonRef)
{
	m_TilePolygon = polygonRef;
}

QString AtlasTileSet::GetTileSetInfo() const
{
	QString sInfo;
	if(GetNumTiles() == 0)
		sInfo += "Empty";
	else
	{
		QSize subAtlasSize = GetSize();
		if(m_TileDataList.size() > 0 && (subAtlasSize.width() == 0 || subAtlasSize.height() == 0))
		{
			int iNumCols = NUM_COLS_TILESET(m_TileDataList.size());
			int iNumRows = NUM_ROWS_TILESET(m_TileDataList.size(), iNumCols);
			subAtlasSize.setWidth((m_RegionSize.width() + TILESET_TILE_PADDING) * iNumCols);
			subAtlasSize.setHeight((m_RegionSize.height() + TILESET_TILE_PADDING) * iNumRows);

			sInfo += "Sub-Atlas (unsaved) ";
			sInfo += QString::number(subAtlasSize.width()) + "x" + QString::number(subAtlasSize.height()) + " | ";
			sInfo += QString::number(GetNumTiles()) + " Tiles (";
			sInfo += QString::number(m_RegionSize.width()) + "x" + QString::number(m_RegionSize.height()) + ")";
		}
		else
		{
			sInfo += "Sub-Atlas ";
			sInfo += QString::number(subAtlasSize.width()) + "x" + QString::number(subAtlasSize.height()) + " | ";
			sInfo += QString::number(GetNumTiles()) + " Tiles (";
			sInfo += QString::number(m_RegionSize.width()) + "x" + QString::number(m_RegionSize.height()) + ")";
		}
	}

	return sInfo;
}

/*static*/ QJsonObject AtlasTileSet::GenerateNewAnimationJsonObject(QString sName, HyColor color)
{
	Animation anim(sName, color);
	return anim.ToJsonObject();
}

/*static*/ QJsonObject AtlasTileSet::GenerateNewTerrainSetJsonObject()
{
	TerrainSet terrainSet;
	return terrainSet.ToJsonObject();
}

/*static*/ QJsonObject AtlasTileSet::GenerateNewTerrainJsonObject(QUuid terrainSetUuid, QString sName, HyColor color)
{
	TerrainSet::Terrain terrain(terrainSetUuid, sName, color);
	return terrain.ToJsonObject();
}

/*static*/ QJsonObject AtlasTileSet::GenerateNewCollisionJsonObject(HyColor color)
{
	CollisionLayer collisionLayer(color);
	return collisionLayer.ToJsonObject();
}

QVector<QJsonObject> AtlasTileSet::GetAnimations() const
{
	QVector<QJsonObject> animationObjList;
	animationObjList.reserve(m_AnimationList.size());
	for (const Animation &animation : m_AnimationList)
		animationObjList.push_back(animation.ToJsonObject());
	return animationObjList;
}

QVector<QJsonObject> AtlasTileSet::GetTerrainSets() const
{
	QVector<QJsonObject> terrainSetObjList;
	terrainSetObjList.reserve(m_TerrainSetList.size());
	for (const TerrainSet &terrainSet : m_TerrainSetList)
		terrainSetObjList.push_back(terrainSet.ToJsonObject());
	return terrainSetObjList;
}

QVector<QJsonObject> AtlasTileSet::GetCollisionLayers() const
{
	QVector<QJsonObject> collisionObjList;
	collisionObjList.reserve(m_CollisionLayerList.size());
	for (const CollisionLayer &collisionLayer : m_CollisionLayerList)
		collisionObjList.push_back(collisionLayer.ToJsonObject());
	return collisionObjList;
}

QJsonObject AtlasTileSet::GetJsonItem(QUuid uuid) const
{
	for (const Animation &animation : m_AnimationList)
	{
		if (animation.m_uuid == uuid)
			return animation.ToJsonObject();
	}
	for (const TerrainSet &terrainSet : m_TerrainSetList)
	{
		if (terrainSet.m_uuid == uuid)
			return terrainSet.ToJsonObject();

		for(const TerrainSet::Terrain &terrain : terrainSet.m_TerrainList)
		{
			if(terrain.m_uuid == uuid)
				return terrain.ToJsonObject();
		}
	}
	for (const CollisionLayer &collisionLayer : m_CollisionLayerList)
	{
		if (collisionLayer.m_uuid == uuid)
			return collisionLayer.ToJsonObject();
	}
	HyGuiLog("AtlasTileSet::GetJsonItem() could not find item with UUID: " + uuid.toString(), LOGTYPE_Error);
	return QJsonObject();
}

HyColor AtlasTileSet::GetAnimationColor(QUuid animationUuid) const
{
	for(const Animation &animationRef : m_AnimationList)
	{
		if(animationRef.m_uuid == animationUuid)
			return animationRef.m_Color;
	}
	HyGuiLog("AtlasTileSet::GetAnimationColor() could not find Animation with UUID: " + animationUuid.toString(), LOGTYPE_Error);
	return HyColor::Black;
}

AutoTileType AtlasTileSet::GetTerrainSetType(QUuid terrainSetUuid) const
{
	for(const TerrainSet &terrainSetRef : m_TerrainSetList)
	{
		if(terrainSetRef.m_uuid == terrainSetUuid)
			return terrainSetRef.m_eType;
	}
	return AUTOTILETYPE_Unknown;
}

HyColor AtlasTileSet::GetTerrainColor(QUuid terrainUuid) const
{
	for(const TerrainSet &terrainSetRef : m_TerrainSetList)
	{
		for(const TerrainSet::Terrain &terrainRef : terrainSetRef.m_TerrainList)
		{
			if(terrainRef.m_uuid == terrainUuid)
				return terrainRef.m_Color;
		}
	}
	HyGuiLog("AtlasTileSet::GetTerrainColor() could not find Terrain with UUID: " + terrainUuid.toString(), LOGTYPE_Error);
	return HyColor::Black;
}

HyColor AtlasTileSet::GetCollisionLayerColor(QUuid collisionLayerUuid) const
{
	for(const CollisionLayer &collisionLayerRef : m_CollisionLayerList)
	{
		if(collisionLayerRef.m_uuid == collisionLayerUuid)
			return collisionLayerRef.m_Color;
	}
	HyGuiLog("AtlasTileSet::GetCollisionLayerColor() could not find CollisionLayer with UUID: " + collisionLayerUuid.toString(), LOGTYPE_Error);
	return HyColor::Black;
}

TileData *AtlasTileSet::FindTileData(QUuid uuid) const
{
	for (TileData *pTileData : m_TileDataList)
	{
		if (pTileData->GetUuid() == uuid)
			return pTileData;
	}
	return nullptr;
}

QVector<TileData *> AtlasTileSet::GetTileDataList() const
{
	return m_TileDataList;
}

TileSetScene *AtlasTileSet::GetGfxScene()
{
	return &m_GfxScene;
}

QList<QPair<QPoint, TileData *>> AtlasTileSet::Cmd_AppendNewTiles(QSize vRegionSize, const QMap<QPoint, QPixmap> &importBatchMap, Qt::Edge eAppendEdge)
{
	QList<QPair<QPoint, TileData *>> newTileDataList;

	if(importBatchMap.isEmpty())
		return newTileDataList;

	if(m_eTileShape == TILESETSHAPE_Unknown || m_TilePolygon.isEmpty())
		HyGuiLog("AtlasTileSet::Cmd_AppendNewTiles() was invoked while not fully initialized", LOGTYPE_Error);

	// Update region size if necessary (may only grow in size)
	if(m_RegionSize != vRegionSize)
	{
		if(m_RegionSize.width() < vRegionSize.width())
			m_RegionSize.setWidth(vRegionSize.width());
		if(m_RegionSize.height() < vRegionSize.height())
			m_RegionSize.setHeight(vRegionSize.height());
	}

	// Get bounding box of import batch
	int minX = INT_MAX, maxX = INT_MIN, minY = INT_MAX, maxY = INT_MIN;
	for(const QPoint &pt : importBatchMap.keys())
	{
		minX = HyMath::Min(minX, pt.x());
		maxX = HyMath::Max(maxX, pt.x());
		minY = HyMath::Min(minY, pt.y());
		maxY = HyMath::Max(maxY, pt.y());
	}
	QSize importSize(maxX - minX + 1, maxY - minY + 1);

	// Get bounding box of existing grid
	int existMinX = 0, existMaxX = 0, existMinY = 0, existMaxY = 0;
	for(const TileData *pTileData : m_TileDataList)
	{
		QPoint pt = pTileData->GetMetaGridPos();

		existMinX = HyMath::Min(existMinX, pt.x());
		existMaxX = HyMath::Max(existMaxX, pt.x());
		existMinY = HyMath::Min(existMinY, pt.y());
		existMaxY = HyMath::Max(existMaxY, pt.y());
	}

	QPoint vGridOffset;
	switch(eAppendEdge)
	{
	case Qt::TopEdge:
		vGridOffset = QPoint(existMinX, existMinY - importSize.height());
		break;
	case Qt::LeftEdge:
		vGridOffset = QPoint(existMinX - importSize.width(), existMinY);
		break;
	case Qt::RightEdge:
		vGridOffset = QPoint(existMaxX + 1, existMinY); // shift to right of current max X
		break;
	case Qt::BottomEdge:
		vGridOffset = QPoint(existMinX, existMaxY + 1);
		break;
	}

	// Insert each tile at new position
	for(QMap<QPoint, QPixmap>::const_iterator it = importBatchMap.begin(); it != importBatchMap.end(); ++it)
	{
		QPoint newGridPos = it.key() + vGridOffset;

		TileData *pTileData = new TileData(newGridPos, it.value());
		m_TileDataList.append(pTileData);
		newTileDataList.push_back(QPair<QPoint, TileData *>(newGridPos, pTileData));

		GetGfxScene()->AddTile(false, pTileData, GetTilePolygon(), pTileData->GetMetaGridPos(), pTileData->GetPixmap(), false);
	}

	GetGfxScene()->ClearImportTiles();
	m_bSubAtlasDirty = true;

	return newTileDataList;
}

QList<QPair<QPoint, TileData *>> AtlasTileSet::Cmd_RemoveTiles(QVector<TileData *> tileDataList)
{
	QList<QPair<QPoint, TileData *>> removedTileDataList;
	removedTileDataList.reserve(tileDataList.size());
	for(TileData *pTileData : tileDataList)
	{
		for(auto it = m_TileDataList.begin(); it != m_TileDataList.end(); ++it)
		{
			if(*it == pTileData)
			{
				removedTileDataList.push_back(QPair<QPoint, TileData *>(pTileData->GetMetaGridPos(), pTileData));
				break;
			}
		}
	}

	// Remove tiles from m_TileDataList
	for(const QPair<QPoint, TileData *> &pair : removedTileDataList)
		m_TileDataList.removeOne(pair.second);

	m_bSubAtlasDirty = true;

	return removedTileDataList;
}

void AtlasTileSet::Cmd_ReaddTiles(QList<QPair<QPoint, TileData *>> tileDataList)
{
	for(const QPair<QPoint, TileData *> &pair : tileDataList)
		m_TileDataList.append(pair.second);

	m_bSubAtlasDirty = true;
}

void AtlasTileSet::Cmd_MoveTiles(QList<TileData*> tileDataList, QList<QPoint> newGridPosList)
{
	for(int i = 0; i < tileDataList.size(); ++i)
		tileDataList[i]->SetMetaGridPos(newGridPosList[i]);

	m_bSubAtlasDirty = true;
}

void AtlasTileSet::Cmd_AllocateJsonItem(TileSetWgtType eType, QJsonObject data)
{
	switch(eType)
	{
	case TILESETWGT_Animation:
		m_AnimationList.push_back(Animation(data));
		break;
	case TILESETWGT_TerrainSet:
		m_TerrainSetList.push_back(TerrainSet(data));
		break;
	case TILESETWGT_Terrain: {
		QUuid terrainSetUuid = QUuid(data["terrainSetUUID"].toString());
		bool bFound = false;
		for(TerrainSet &terrainSet : m_TerrainSetList)
		{
			if(terrainSet.m_uuid == terrainSetUuid)
			{
				terrainSet.m_TerrainList.push_back(TerrainSet::Terrain(data));
				bFound = true;
				break;
			}
		}
		if(bFound == false)
			HyGuiLog("AtlasTileSet::Cmd_AllocateJsonItem() could not find Terrain Set with UUID: " + terrainSetUuid.toString(), LOGTYPE_Error);
		break; }
	case TILESETWGT_Collision:
		m_CollisionLayerList.append(CollisionLayer(data));
		break;

	default:
		HyGuiLog("AtlasTileSet::Cmd_AllocateJsonItem() received unknown TileSetWgtType: " + QString::number(static_cast<int>(eType)), LOGTYPE_Error);
		break;
	}
}

void AtlasTileSet::Cmd_SetJsonItem(QUuid uuid, const QJsonObject &itemDataObj)
{
	for(Animation &animationRef : m_AnimationList)
	{
		if(animationRef.m_uuid == uuid)
		{
			animationRef = Animation(itemDataObj);
			return;
		}
	}
	for(TerrainSet &terrainSetRef : m_TerrainSetList)
	{
		if(terrainSetRef.m_uuid == uuid)
		{
			terrainSetRef = TerrainSet(itemDataObj);
			return;
		}
		// Check for Terrain within TerrainSet
		for(TerrainSet::Terrain &terrainRef : terrainSetRef.m_TerrainList)
		{
			if(terrainRef.m_uuid == uuid)
			{
				terrainRef = TerrainSet::Terrain(itemDataObj);
				return;
			}
		}
	}
	for(CollisionLayer &collisionLayerRef : m_CollisionLayerList)
	{
		if(collisionLayerRef.m_uuid == uuid)
		{
			collisionLayerRef = CollisionLayer(itemDataObj);
			return;
		}
	}

	HyGuiLog("AtlasTileSet::SetJsonItem() could not find item with UUID: " + uuid.toString(), LOGTYPE_Error);
}

void AtlasTileSet::Cmd_RemoveJsonItem(QUuid uuid)
{
	for(int i = 0; i < m_AnimationList.size(); ++i)
	{
		if(m_AnimationList[i].m_uuid == uuid)
		{
			m_AnimationList.removeAt(i);
			return;
		}
	}
	for(int i = 0; i < m_TerrainSetList.size(); ++i)
	{
		if(m_TerrainSetList[i].m_uuid == uuid)
		{
			m_TerrainSetList.removeAt(i);
			return;
		}
		// Check for Terrain within TerrainSet
		for(int j = 0; j < m_TerrainSetList[i].m_TerrainList.size(); ++j)
		{
			if(m_TerrainSetList[i].m_TerrainList[j].m_uuid == uuid)
			{
				m_TerrainSetList[i].m_TerrainList.removeAt(j);
				return;
			}
		}
	}
	for(int i = 0; i < m_CollisionLayerList.size(); ++i)
	{
		if(m_CollisionLayerList[i].m_uuid == uuid)
		{
			m_CollisionLayerList.removeAt(i);
			return;
		}
	}
	HyGuiLog("AtlasTileSet::Cmd_RemoveJsonItem() could not find item with UUID: " + uuid.toString(), LOGTYPE_Error);
}

QUndoStack *AtlasTileSet::GetUndoStack()
{
	return m_pUndoStack;
}

//QAction *AtlasTileSet::GetSaveAction()
//{
//	return &m_ActionSave;
//}

QAction *AtlasTileSet::GetUndoAction()
{
	return m_pActionUndo;
}

QAction *AtlasTileSet::GetRedoAction()
{
	return m_pActionRedo;
}

QIcon AtlasTileSet::GetTileSetIcon() const
{
	SubIcon eSubIcon = SUBICON_None;
	if(m_bExistencePendingSave)
		eSubIcon = SUBICON_New;
	else if(m_uiErrors != 0)
		eSubIcon = SUBICON_Warning;
	else if(m_pUndoStack->isClean() == false)
		eSubIcon = SUBICON_Dirty;

	return GetIcon(eSubIcon);
}

void AtlasTileSet::SetSubAtlasDirty()
{
	m_bSubAtlasDirty = true;
}

void AtlasTileSet::UpdateTileSetDataPair()
{
	// Start with blank
	m_TileSetDataPair = FileDataPair();

	m_TileSetDataPair.m_Meta["tileShape"] = HyGlobal::TileSetShapeName(m_eTileShape);
	m_TileSetDataPair.m_Meta["regionSize"] = QJsonArray() << QJsonValue(m_RegionSize.width()) << QJsonValue(m_RegionSize.height());
	m_TileSetDataPair.m_Meta["tileSize"] = QJsonArray() << QJsonValue(m_TileSize.width()) << QJsonValue(m_TileSize.height());
	m_TileSetDataPair.m_Meta["tileOffset"] = QJsonArray() << QJsonValue(m_TileOffset.x()) << QJsonValue(m_TileOffset.y());

	QJsonArray animationArray;
	for(int i = 0; i < m_AnimationList.size(); ++i)
	{
		QJsonObject animationObj = m_AnimationList[i].ToJsonObject();
		animationArray.append(animationObj);
	}
	m_TileSetDataPair.m_Meta["animations"] = animationArray;

	QJsonArray terrainSetArray;
	for(int i = 0; i < m_TerrainSetList.size(); ++i)
	{
		QJsonObject terrainSetObj = m_TerrainSetList[i].ToJsonObject();
		terrainSetArray.append(terrainSetObj);
	}
	m_TileSetDataPair.m_Meta["terrainSets"] = terrainSetArray;

	QJsonArray collisionLayerArray;
	for(int i = 0; i < m_CollisionLayerList.size(); ++i)
	{
		QJsonObject collisionLayerObj = m_CollisionLayerList[i].ToJsonObject();
		collisionLayerArray.append(collisionLayerObj);
	}
	m_TileSetDataPair.m_Meta["collisionLayers"] = collisionLayerArray;

	QJsonArray tileArray;
	for(QVector<TileData*>::const_iterator iter = m_TileDataList.constBegin(); iter != m_TileDataList.constEnd(); ++iter)
	{
		QJsonObject tileObj = (*iter)->GetTileData();
		tileArray.append(tileObj);
	}
	m_TileSetDataPair.m_Meta["tileData"] = tileArray;
}

bool AtlasTileSet::Save()
{
	if(m_bSubAtlasDirty)
	{
		if(RegenerateSubAtlas() == false)
			return false;
	}

	if(m_ModelRef.GetProjOwner().IsUnsavedOpenItems())
	{
		HyGuiLog("Saving the TileSet needs to modify assets and cannot do so while there are open unsaved items. Note that the TileSet Editor will take save precedence when visible.", LOGTYPE_Warning);
		return false;
	}

	m_bSubAtlasDirty = false;
	m_bExistencePendingSave = false;
	m_pUndoStack->setClean();

	UpdateTileSetDataPair();
	static_cast<AtlasModel &>(m_ModelRef).SaveTileSet(GetUuid(), m_TileSetDataPair);

	return true;
}

bool AtlasTileSet::IsExistencePendingSave() const
{
	return m_bExistencePendingSave;
}

bool AtlasTileSet::IsSaveClean() const
{
	return m_pUndoStack->isClean();
}

void AtlasTileSet::DiscardChanges()
{
	m_pUndoStack->clear();
}

/*virtual*/ void AtlasTileSet::InsertUniqueJson(QJsonObject &frameObj) /*override*/
{
	frameObj.insert("subAtlasType", QJsonValue(HyGlobal::ItemName(m_eSubAtlasType, false)));
	frameObj.insert("width", QJsonValue(GetSize().width()));
	frameObj.insert("height", QJsonValue(GetSize().height()));
	frameObj.insert("textureIndex", QJsonValue(GetTextureIndex()));
	frameObj.insert("textureInfo", QJsonValue(static_cast<qint64>(m_TexInfo.GetBucketId())));
	frameObj.insert("x", QJsonValue(GetX()));
	frameObj.insert("y", QJsonValue(GetY()));
}

void AtlasTileSet::UpdateTilePolygon()
{
	m_TilePolygon.clear();
	
	// NOTE: m_TilePolygon is a shape centered around the origin (0, 0)
	switch(m_eTileShape)
	{
	case TILESETSHAPE_Square:
	case TILESETSHAPE_HalfOffsetSquare:
		// Center the square polygon around the origin
		m_TilePolygon << QPoint(-m_TileSize.width() / 2, -m_TileSize.height() / 2)
					  << QPoint(m_TileSize.width() / 2, -m_TileSize.height() / 2)
					  << QPoint(m_TileSize.width() / 2, m_TileSize.height() / 2)
					  << QPoint(-m_TileSize.width() / 2, m_TileSize.height() / 2);
		break;
		
	case TILESETSHAPE_Isometric:
		m_TilePolygon << QPoint(0, -m_TileSize.height() / 2)
					  << QPoint(m_TileSize.width() / 2, 0)
					  << QPoint(0, m_TileSize.height() / 2)
					  << QPoint(-m_TileSize.width() / 2, 0);
		break;

	case TILESETSHAPE_HexagonPointTop:
		m_TilePolygon << QPoint(0, -m_TileSize.height() / 2)
					  << QPoint(m_TileSize.width() / 2, -m_TileSize.height() / 4)
					  << QPoint(m_TileSize.width() / 2, m_TileSize.height() / 4)
					  << QPoint(0, m_TileSize.height() / 2)
					  << QPoint(-m_TileSize.width() / 2, m_TileSize.height() / 4)
					  << QPoint(-m_TileSize.width() / 2, -m_TileSize.height() / 4);
		break;
		
	case TILESETSHAPE_HexagonFlatTop:
		m_TilePolygon << QPoint(-m_TileSize.width() / 4, -m_TileSize.height() / 2)
					  << QPoint(m_TileSize.width() / 4, -m_TileSize.height() / 2)
					  << QPoint(m_TileSize.width() / 2, 0)
					  << QPoint(m_TileSize.width() / 4, m_TileSize.height() / 2)
					  << QPoint(-m_TileSize.width() / 4, m_TileSize.height() / 2)
					  << QPoint(-m_TileSize.width() / 2, 0);
		break;

	case TILESETSHAPE_Unknown:
	default:
		HyGuiLog("AtlasTileSet::UpdateTilePolygon() - Unknown tile shape", LOGTYPE_Error);
		return;
	}
}

bool AtlasTileSet::RegenerateSubAtlas()
{
	if(m_TileDataList.isEmpty())
		return true;

	std::sort(m_TileDataList.begin(), m_TileDataList.end(),
		[](TileData *pA, TileData *pB)
		{
			QUuid animA = pA->GetAnimation();
			QUuid animB = pB->GetAnimation();
			if(animA.isNull() == false && animB.isNull())
				return true;
			else if(animA.isNull() && animB.isNull() == false)
				return false;
			else if(animA.isNull() == false && animB.isNull() == false && animA != animB)
				return animA.toString(QUuid::WithoutBraces) < animB.toString(QUuid::WithoutBraces);

			return pA->GetMetaGridPos() < pB->GetMetaGridPos();
		});

	// Create a texture with a size that will accommodate all the existing, and newly appended tiles
	const int iNUM_COLS = NUM_COLS_TILESET(m_TileDataList.size());
	const int iNUM_ROWS = NUM_ROWS_TILESET(m_TileDataList.size(), iNUM_COLS);

	QImage newTexture(iNUM_COLS * (m_RegionSize.width() + TILESET_TILE_PADDING), iNUM_ROWS * (m_RegionSize.height() + TILESET_TILE_PADDING), QImage::Format_ARGB32);
	newTexture.fill(Qt::transparent);

	// Iterate through all the tiles and draw them to the blank newTexture
	QPainter p(&newTexture);
	int index = 0;
	for(auto it = m_TileDataList.begin(); it != m_TileDataList.end(); ++it, ++index)
	{
		const TileData* pTile = *it;
		if(pTile == nullptr)
			continue;

		int iCol = index % iNUM_COLS;
		int iRow = index / iNUM_COLS;

		QPoint destPos(iCol * (m_RegionSize.width() + TILESET_TILE_PADDING), iRow * (m_RegionSize.height() + TILESET_TILE_PADDING));
		// If the pixmap's dimensions are smaller than the tile size, we need to center it
		if(pTile->GetPixmap().width() < m_RegionSize.width() || pTile->GetPixmap().height() < m_RegionSize.height())
		{
			destPos.setX(destPos.x() + (m_RegionSize.width() - pTile->GetPixmap().width()) / 2);
			destPos.setY(destPos.y() + (m_RegionSize.height() - pTile->GetPixmap().height()) / 2);
		}
		// Then apply the user specified texture offset property
		QPoint vOffset = pTile->GetTextureOffset();
		destPos += vOffset;
		
		p.drawPixmap(destPos, pTile->GetPixmap());
	}
	p.end();

	if(static_cast<AtlasModel &>(m_ModelRef).ReplaceFrame(this, GetName(), newTexture, ITEM_AtlasTileSet) == false)
	{
		HyGuiLog("AtlasModel::ReplaceFrame failed (or can't fit in main atlases) for tile set sub-atlas: " % GetName(), LOGTYPE_Error);
		return false;
	}

	return true;
}
