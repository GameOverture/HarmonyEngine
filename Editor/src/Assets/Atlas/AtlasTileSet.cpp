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
		for(int i = 0; i < tileArray.size(); ++i)
		{
			QJsonObject tileObj = tileArray[i].toObject();
			MetaLocation key(tileObj["metaX"].toInt(), tileObj["metaY"].toInt());
			m_TileDataMap[key] = new TileData(tileObj);
		}
	}
}

AtlasTileSet::~AtlasTileSet()
{
}

int AtlasTileSet::GetNumTiles() const
{
	return m_TileDataMap.count();
}

QSize AtlasTileSet::GetTileSize() const
{
	return m_TileSize;
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
		autotileObj["color"] = QJsonValue(static_cast<qint64>(m_AutotileList[i].m_Color.GetAsRGBA()));
		autotileArray.append(autotileObj);
	}
	fileDataPairOut.m_Meta["autoTiles"] = autotileArray;

	QJsonArray physicsLayerArray;
	for(int i = 0; i < m_PhysicsLayerList.size(); ++i)
	{
		QJsonObject physicsLayerObj;
		physicsLayerObj["id"] = QJsonValue(static_cast<int>(m_PhysicsLayerList[i].m_hId));
		physicsLayerObj["name"] = QJsonValue(m_PhysicsLayerList[i].m_sName);
		physicsLayerObj["color"] = QJsonValue(static_cast<qint64>(m_PhysicsLayerList[i].m_Color.GetAsRGBA()));
		physicsLayerArray.append(physicsLayerObj);
	}
	fileDataPairOut.m_Meta["physicsLayers"] = physicsLayerArray;

	QJsonArray tileArray;
	for(QMap<MetaLocation, TileData *>::const_iterator iter = m_TileDataMap.begin(); iter != m_TileDataMap.end(); ++iter)
	{
		QJsonObject tileObj;
		tileObj["metaX"] = QJsonValue(iter.key().iX);
		tileObj["metaY"] = QJsonValue(iter.key().iY);
		iter.value()->GetTileData(tileObj);
	}
	fileDataPairOut.m_Meta["tileData"] = tileArray;
}

void AtlasTileSet::GetSavedFileData(FileDataPair &itemFileDataOut) const
{
	itemFileDataOut = m_TileSetDataPair;
}

bool AtlasTileSet::Save(bool bWriteToDisk)
{
	GetLatestFileData(m_TileSetDataPair);
	static_cast<AtlasModel &>(m_ModelRef).SaveTileSet(GetUuid(), m_TileSetDataPair, bWriteToDisk);

	m_bExistencePendingSave = false;
	m_pUndoStack->setClean();

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
