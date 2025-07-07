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

#include <QPainter>

bool operator<(const QPoint &a, const QPoint &b)
{
	if(a.x() != b.x())
		return a.x() < b.x();
	else
		return a.y() < b.y();
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
	m_eTileShape(TILESETSHAPE_Unknown)
{
	m_ActionSave.setIcon(QIcon(":/icons16x16/file-save.png"));
	m_ActionSave.setShortcuts(QKeySequence::Save);
	m_ActionSave.setShortcutContext(Qt::WidgetShortcut);
	m_ActionSave.setToolTip("Save Tile Set and pack into Atlas manager");
	m_ActionSave.setObjectName("Save");
	QObject::connect(&m_ActionSave, &QAction::triggered, this, &AtlasTileSet::on_actionSave_triggered);

	m_pUndoStack = new QUndoStack(this);
	m_pActionUndo = m_pUndoStack->createUndoAction(nullptr, "&Undo");
	m_pActionUndo->setIcon(QIcon(":/icons16x16/edit-undo.png"));
	m_pActionUndo->setShortcuts(QKeySequence::Undo);
	m_pActionUndo->setShortcutContext(Qt::WidgetShortcut);
	m_pActionUndo->setObjectName("Undo");
	m_pActionRedo = m_pUndoStack->createRedoAction(nullptr, "&Redo");
	m_pActionRedo->setIcon(QIcon(":/icons16x16/edit-redo.png"));
	m_pActionRedo->setShortcuts(QKeySequence::Redo);
	m_pActionRedo->setShortcutContext(Qt::WidgetShortcut);
	m_pActionRedo->setObjectName("Redo");

	// Initialize AtlasTileSet members with 'm_TileSetDataPair' meta data
	if(m_TileSetDataPair.m_Meta.empty() == false)
	{
		m_eTileShape = HyGlobal::GetTileSetShapeFromString(m_TileSetDataPair.m_Meta["tileShape"].toString());

		QJsonArray atlasRegionSizeArray = m_TileSetDataPair.m_Meta["regionSize"].toArray();
		m_RegionSize = QSize(atlasRegionSizeArray[0].toInt(), atlasRegionSizeArray[1].toInt());

		QJsonArray tileSizeArray = m_TileSetDataPair.m_Meta["tileSize"].toArray();
		m_TileSize = QSize(tileSizeArray[0].toInt(), tileSizeArray[1].toInt());

		QJsonArray tileOffsetArray = m_TileSetDataPair.m_Meta["tileOffset"].toArray();
		m_TileOffset = QPoint(tileOffsetArray[0].toInt(), tileOffsetArray[1].toInt());

		UpdateTilePolygon();

		QJsonArray autotileArray = m_TileSetDataPair.m_Meta["autoTiles"].toArray();
		m_AutotileList.reserve(autotileArray.size());
		for(int i = 0; i < autotileArray.size(); ++i)
		{
			QJsonObject autotileObj = autotileArray[i].toObject();
			m_AutotileList.push_back(AutoTile(autotileObj));
		}

		QJsonArray physicsLayerArray = m_TileSetDataPair.m_Meta["physicsLayers"].toArray();
		m_PhysicsLayerList.reserve(physicsLayerArray.size());
		for(int i = 0; i < physicsLayerArray.size(); ++i)
		{
			QJsonObject physicsLayerObj = physicsLayerArray[i].toObject();
			m_PhysicsLayerList.push_back(PhysicsLayer(physicsLayerObj));
		}

		QJsonArray tileArray = m_TileSetDataPair.m_Meta["tileData"].toArray();

		// Slice the pixmaps from the sub-atlas. The row-order of the pixmaps is aligned with m_TileDataList
		QVector<QPixmap> pixmapList;
		QImage subAtlas(GetAbsMetaFilePath());
		if(subAtlas.isNull() == false && tileArray.size() > 0)
		{
			const int iNUM_COLS = NUM_COLS_TILESET(tileArray.size());
			//const int iNUM_ROWS = NUM_ROWS_TILESET(tileArray.size(), iNUM_COLS);

			for(int index = 0; index < tileArray.size(); ++index)
			{
				int iCol = index % iNUM_COLS;
				int iRow = index / iNUM_COLS;

				QRect tileRect(iCol * m_RegionSize.width(), iRow * m_RegionSize.height(), m_RegionSize.width(), m_RegionSize.height());
				QPixmap tilePixmap = QPixmap::fromImage(subAtlas.copy(tileRect));
				pixmapList.append(tilePixmap);
			}
		}

		for(int i = 0; i < tileArray.size(); ++i)
		{
			QJsonObject tileObj = tileArray[i].toObject();
			QJsonArray gridPosArray = tileObj["gridPos"].toArray();
			QPoint ptGridPos(gridPosArray[0].toInt(), gridPosArray[1].toInt());
			m_TileDataMap.insert(ptGridPos, new TileData(tileObj, pixmapList[i]));
		}
	}

	m_GfxScene.Initialize(this);
}

AtlasTileSet::~AtlasTileSet()
{
	for(auto it = m_TileDataMap.begin(); it != m_TileDataMap.end(); ++it)
		delete it.value();

	delete m_pUndoStack;
}

int AtlasTileSet::GetNumTiles() const
{
	return m_TileDataMap.size();
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
		sInfo += "Sub-Atlas ";
		sInfo += QString::number(subAtlasSize.width()) + "x" + QString::number(subAtlasSize.height()) + " | ";
		sInfo += QString::number(GetNumTiles()) + " Tiles (";
		sInfo += QString::number(m_RegionSize.width()) + "x" + QString::number(m_RegionSize.height()) + ")";
	}

	return sInfo;
}

QMap<QPoint, TileData *> AtlasTileSet::GetTileDataMap() const
{
	return m_TileDataMap;
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
	for(const QPoint &pt : m_TileDataMap.keys())
	{
		existMinX = HyMath::Min(existMinX, pt.x());
		existMaxX = HyMath::Max(existMaxX, pt.x());
		existMinY = HyMath::Min(existMinY, pt.y());
		existMaxY = HyMath::Max(existMaxY, pt.y());
	}

	QPoint vOffset;
	switch(eAppendEdge)
	{
	case Qt::TopEdge:
		vOffset = QPoint(existMinX, existMinY - importSize.height());
		break;
	case Qt::LeftEdge:
		vOffset = QPoint(existMinX - importSize.width(), existMinY);
		break;
	case Qt::RightEdge:
		vOffset = QPoint(existMaxX + 1, existMinY); // shift to right of current max X
		break;
	case Qt::BottomEdge:
		vOffset = QPoint(existMinX, existMaxY + 1);
		break;
	}

	// Insert each tile at new position
	for(auto it = importBatchMap.begin(); it != importBatchMap.end(); ++it)
	{
		TileData *pTileData = new TileData(it.value());

		QPoint newGridPos = it.key() + vOffset;
		m_TileDataMap.insert(newGridPos, pTileData);
		newTileDataList.push_back(QPair<QPoint, TileData *>(newGridPos, pTileData));
	}

	RegenerateSubAtlas();
	return newTileDataList;
}

QList<QPair<QPoint, TileData *>> AtlasTileSet::Cmd_RemoveTiles(QVector<TileData *> tileDataList)
{
	QList<QPair<QPoint, TileData *>> removedTileDataList;
	removedTileDataList.reserve(tileDataList.size());
	for(TileData *pTileData : tileDataList)
	{
		for(auto it = m_TileDataMap.begin(); it != m_TileDataMap.end(); ++it)
		{
			if(it.value() == pTileData)
			{
				removedTileDataList.push_back(QPair<QPoint, TileData *>(it.key(), pTileData));
				break;
			}
		}
	}

	// Remove tiles from map
	for(const QPair<QPoint, TileData *> &pair : removedTileDataList)
		m_TileDataMap.remove(pair.first);

	RegenerateSubAtlas();
	return removedTileDataList;
}

void AtlasTileSet::Cmd_ReaddTiles(QList<QPair<QPoint, TileData *>> tileDataList)
{
	for(const QPair<QPoint, TileData *> &pair : tileDataList)
		m_TileDataMap.insert(pair.first, pair.second);

	RegenerateSubAtlas();
}

QUndoStack *AtlasTileSet::GetUndoStack()
{
	return m_pUndoStack;
}

QAction *AtlasTileSet::GetSaveAction()
{
	return &m_ActionSave;
}

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

void AtlasTileSet::GetLatestFileData(FileDataPair &fileDataPairOut) const
{
	// Get current member data and write to fileDataPairOut
	fileDataPairOut = m_TileSetDataPair;

	fileDataPairOut.m_Meta["tileShape"] = HyGlobal::TileSetShapeName(m_eTileShape);
	fileDataPairOut.m_Meta["regionSize"] = QJsonArray() << QJsonValue(m_RegionSize.width()) << QJsonValue(m_RegionSize.height());
	fileDataPairOut.m_Meta["tileSize"] = QJsonArray() << QJsonValue(m_TileSize.width()) << QJsonValue(m_TileSize.height());
	fileDataPairOut.m_Meta["tileOffset"] = QJsonArray() << QJsonValue(m_TileOffset.x()) << QJsonValue(m_TileOffset.y());

	QJsonArray autotileArray;
	for(int i = 0; i < m_AutotileList.size(); ++i)
	{
		QJsonObject autotileObj;
		autotileObj["id"] = QJsonValue(static_cast<int>(m_AutotileList[i].m_hId));
		autotileObj["type"] = QJsonValue(m_AutotileList[i].m_iType);
		autotileObj["name"] = QJsonValue(m_AutotileList[i].m_sName);
		autotileObj["color"] = QJsonValue(static_cast<qint64>(m_AutotileList[i].m_Color.GetAsHexCode()));
		autotileArray.append(autotileObj);
	}
	fileDataPairOut.m_Meta["autoTiles"] = autotileArray;

	QJsonArray physicsLayerArray;
	for(int i = 0; i < m_PhysicsLayerList.size(); ++i)
	{
		QJsonObject physicsLayerObj;
		physicsLayerObj["id"] = QJsonValue(static_cast<int>(m_PhysicsLayerList[i].m_hId));
		physicsLayerObj["name"] = QJsonValue(m_PhysicsLayerList[i].m_sName);
		physicsLayerObj["color"] = QJsonValue(static_cast<qint64>(m_PhysicsLayerList[i].m_Color.GetAsHexCode()));
		physicsLayerArray.append(physicsLayerObj);
	}
	fileDataPairOut.m_Meta["physicsLayers"] = physicsLayerArray;

	QJsonArray tileArray;
	for(QMap<QPoint, TileData *>::const_iterator iter = m_TileDataMap.constBegin(); iter != m_TileDataMap.constEnd(); ++iter)
	{
		QJsonObject tileObj = iter.value()->GetTileData();
		tileObj["gridPos"] = QJsonArray() << QJsonValue(iter.key().x()) << QJsonValue(iter.key().y());
		tileArray.append(tileObj);
	}
	fileDataPairOut.m_Meta["tileData"] = tileArray;
}

void AtlasTileSet::GetSavedFileData(FileDataPair &itemFileDataOut) const
{
	itemFileDataOut = m_TileSetDataPair;
}

bool AtlasTileSet::Save(bool bWriteToDisk)
{
	m_bExistencePendingSave = false;
	m_pUndoStack->setClean();

	GetLatestFileData(m_TileSetDataPair);
	static_cast<AtlasModel &>(m_ModelRef).SaveTileSet(GetUuid(), m_TileSetDataPair, bWriteToDisk);

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

void AtlasTileSet::RegenerateSubAtlas()
{
	// Create a texture with a size that will accommodate all the existing, and newly appended tiles
	const int iNUM_COLS = NUM_COLS_TILESET(m_TileDataMap.size());
	const int iNUM_ROWS = NUM_ROWS_TILESET(m_TileDataMap.size(), iNUM_COLS);

	QImage newTexture(iNUM_COLS * m_RegionSize.width(), iNUM_ROWS * m_RegionSize.height(), QImage::Format_ARGB32);
	newTexture.fill(Qt::transparent);

	// Iterate through all the tiles and draw them to the blank newTexture
	QPainter p(&newTexture);
	int index = 0;
	for(auto it = m_TileDataMap.begin(); it != m_TileDataMap.end(); ++it, ++index)
	{
		const TileData *pTile = it.value();
		if(pTile == nullptr)
			continue;

		int iCol = index % iNUM_COLS;
		int iRow = index / iNUM_COLS;

		QPoint destPos(iCol * m_RegionSize.width(), iRow * m_RegionSize.height());
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
		HyGuiLog("AtlasModel::ReplaceFrame failed for tile set sub-atlas: " % GetName(), LOGTYPE_Error);

	m_GfxScene.SyncTileSet();

	// TODO: Determine if this needs to be user initiated
	Save(true);
}

void AtlasTileSet::on_actionSave_triggered()
{

}
