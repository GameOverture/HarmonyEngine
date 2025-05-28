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
	m_bExistencePendingSave(bIsPendingSave)
{
	m_pUndoStack = new QUndoStack(this);
	m_pActionUndo = m_pUndoStack->createUndoAction(nullptr, "&Undo");
	m_pActionUndo->setIcon(QIcon(":/icons16x16/edit-undo.png"));
	m_pActionUndo->setShortcuts(QKeySequence::Undo);
	m_pActionUndo->setShortcutContext(Qt::ApplicationShortcut);
	m_pActionUndo->setObjectName("Undo");

	m_pActionRedo = m_pUndoStack->createRedoAction(nullptr, "&Redo");
	m_pActionRedo->setIcon(QIcon(":/icons16x16/edit-redo.png"));
	m_pActionRedo->setShortcuts(QKeySequence::Redo);
	m_pActionRedo->setShortcutContext(Qt::ApplicationShortcut);
	m_pActionRedo->setObjectName("Redo");

	connect(m_pUndoStack, SIGNAL(cleanChanged(bool)), this, SLOT(on_undoStack_cleanChanged(bool)));
	connect(m_pUndoStack, SIGNAL(indexChanged(int)), this, SLOT(on_undoStack_indexChanged(int)));

	if(m_TileSetDataPair.m_Meta.empty() == false)
	{
		// Initialize AtlasTileSet members with 'm_TileSetDataPair' meta data
		QJsonArray tileSizeArray = m_TileSetDataPair.m_Meta["tileSize"].toArray();
		m_TileSize = QSize(tileSizeArray[0].toInt(), tileSizeArray[1].toInt());

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
			const int iNUM_ROWS = NUM_ROWS_TILESET(tileArray.size());
			
			int iIndex = 0;
			for(int iRow = 0; iRow < iNUM_ROWS; ++iRow)
			{
				for(int iCol = 0; iCol < iNUM_COLS; ++iCol)
				{
					if(iIndex >= tileArray.size())
						break; // No more tiles to process

					QRect tileRect(iCol * m_TileSize.width(), iRow * m_TileSize.height(), m_TileSize.width(), m_TileSize.height());
					QPixmap tilePixmap = QPixmap::fromImage(subAtlas.copy(tileRect));
					pixmapList.append(tilePixmap);
					++iIndex;
				}

				if(iIndex >= tileArray.size())
					break; // No more tiles to process
			}
		}

		for(int i = 0; i < tileArray.size(); ++i)
			m_TileDataList.push_back(new TileData(tileArray[i].toObject(), pixmapList[i]));
	}
}

AtlasTileSet::~AtlasTileSet()
{
}

int AtlasTileSet::GetNumTiles() const
{
	return m_TileDataList.size();
}

QSize AtlasTileSet::GetTileSize() const
{
	return m_TileSize;
}

void AtlasTileSet::SetTileSize(QSize size)
{
	m_TileSize = size;
}

QString AtlasTileSet::GetTileSetInfo() const
{
	QString sInfo;
	if(GetNumTiles() == 0)
		sInfo += "Empty";
	else
	{
		sInfo += QString::number(GetNumTiles()) + " Tiles (";
		sInfo += QString::number(m_TileSize.width()) + "x" + QString::number(m_TileSize.height()) + ")";
	}

	return sInfo;
}

TileSetScene *AtlasTileSet::GetGfxScene()
{
	return &m_GfxScene;
}

QList<QPair<int, TileData *>> AtlasTileSet::Cmd_AppendNewTiles(QSize vTileSize, const QVector<QPixmap> &newPixmapList, Qt::Edge eAppendEdge)
{
	// Update size if necessary
	if(m_TileSize != vTileSize)
	{
		if(m_TileSize.width() < vTileSize.width())
			m_TileSize.setWidth(vTileSize.width());
		if(m_TileSize.height() < vTileSize.height())
			m_TileSize.setHeight(vTileSize.height());
	}

	m_TileDataList.reserve(m_TileDataList.size() + newPixmapList.size());
	QList<QPair<int, TileData *>> newTileDataList;
	for(int i = 0; i < newPixmapList.size(); ++i)
	{
		TileData *pTileData = new TileData(newPixmapList[i]);
		int iIndex = m_TileDataList.size();
		m_TileDataList.push_back(pTileData);
		newTileDataList.push_back(QPair<int, TileData *>(iIndex, pTileData));
	}

	RegenerateSubAtlas();
	return newTileDataList;
}

QList<QPair<int, TileData *>> AtlasTileSet::Cmd_RemoveTiles(QVector<TileData *> tileDataList)
{
	QList<QPair<int, TileData *>> removedTileDataList;
	removedTileDataList.reserve(tileDataList.size());
	for(int i = 0; i < m_TileDataList.size(); ++i)
	{
		for(int j = 0; j < tileDataList.size(); ++j)
		{
			if(m_TileDataList[i] == tileDataList[j])
			{
				removedTileDataList.push_back(QPair<int, TileData *>(i, m_TileDataList[i]));
				break;
			}
		}
	}

	for(TileData *pTileData : tileDataList)
	{
		auto it = std::find(m_TileDataList.begin(), m_TileDataList.end(), pTileData);
		if(it != m_TileDataList.end())
			m_TileDataList.erase(it);
	}

	RegenerateSubAtlas();
	return removedTileDataList;
}

void AtlasTileSet::Cmd_ReaddTiles(QList<QPair<int, TileData *>> tileDataList)
{
	for(const QPair<int, TileData *> &pair : tileDataList)
	{
		m_TileDataList.insert(pair.first, pair.second);
		//m_RemovedTileDataList.removeOne(pair);
	}

	RegenerateSubAtlas();
}

QUndoStack *AtlasTileSet::GetUndoStack()
{
	return m_pUndoStack;
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
	fileDataPairOut = m_TileSetDataPair;

	// Get current member data and write to fileDataPairOut
	fileDataPairOut.m_Meta["tileSize"] = QJsonArray() << QJsonValue(m_TileSize.width()) << QJsonValue(m_TileSize.height());

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
	for(TileData *pTileData : m_TileDataList)
		tileArray.append(pTileData->GetTileData());
	
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

	//if(bWriteToDisk)
	//{
	//	if(m_pWidget)
	//		m_pWidget->update();
	//	MainWindow::GetExplorerWidget().update();
	//	MainWindow::GetAuxWidget(AUXTAB_DopeSheet)->update();
	//}

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

void AtlasTileSet::RegenerateSubAtlas()
{
	// Create a texture with a size that will accommodate all the existing, and newly appended tiles
	const int iNUM_COLS = NUM_COLS_TILESET(m_TileDataList.size());
	const int iNUM_ROWS = NUM_ROWS_TILESET(m_TileDataList.size());
	QSize textureSize(iNUM_COLS * m_TileSize.width(), iNUM_ROWS * m_TileSize.height());

	QImage newTexture(textureSize.width(), textureSize.height(), QImage::Format_ARGB32);
	newTexture.fill(Qt::transparent);

	// Iterate through all the tiles and draw them to the blank newTexture
	QPainter p(&newTexture);
	for(int i = 0; i < m_TileDataList.size(); ++i)
	{
		int iCol = i % iNUM_COLS;
		int iRow = i / iNUM_COLS;
		QPoint pos(m_TileSize.width() * iCol, m_TileSize.height() * iRow);
		p.drawPixmap(pos.x(), pos.y(), m_TileDataList[i]->GetPixmap());
	}

	if(static_cast<AtlasModel &>(m_ModelRef).ReplaceFrame(this, GetName(), newTexture, ITEM_AtlasTileSet) == false)
		HyGuiLog("AtlasModel::ReplaceFrame failed for tile set sub-atlas: " % GetName(), LOGTYPE_Error);

	// TODO: Determine if this needs to be user initiated
	Save(true);
}

void AtlasTileSet::on_undoStack_cleanChanged(bool bClean)
{
	//ProjectTabBar *pTabBar = m_pProject->GetTabBar();
	//for(int i = 0; i < pTabBar->count(); ++i)
	//{
	//	if(pTabBar->tabData(i).value<ProjectItemData *>() == this)
	//	{
	//		if(bClean)
	//		{
	//			pTabBar->setTabText(i, GetName(false));
	//			pTabBar->setTabIcon(i, GetIcon(SUBICON_None));
	//		}
	//		else
	//		{
	//			pTabBar->setTabText(i, GetName(false) + "*");
	//			pTabBar->setTabIcon(i, GetIcon(SUBICON_Dirty));
	//		}

	//		QModelIndex index = MainWindow::GetExplorerModel().FindIndex<ProjectItemData *>(this, 0);
	//		MainWindow::GetExplorerModel().dataChanged(index, index, QVector<int>() << Qt::DecorationRole);

	//		break;
	//	}
	//}

	//m_pProject->ApplySaveEnables();
}

void AtlasTileSet::on_undoStack_indexChanged(int iIndex)
{
	//if(m_pDraw == nullptr || m_pWidget == nullptr)
	//	return;

	//m_pDraw->OnUndoStackIndexChanged(iIndex);
	//m_pDraw->ApplyJsonData();

	//m_pWidget->UpdateActions();
}
