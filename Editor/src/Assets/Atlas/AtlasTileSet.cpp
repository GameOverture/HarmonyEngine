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

void AtlasTileSet::SetTileSize(QSize size)
{
	m_TileSize = size;
}


// NOTE: TileSet atlases are always "square"
int AtlasTileSet::GetNumCols() const
{
	return static_cast<int>(std::floor(std::sqrt(GetNumTiles())));
}

// NOTE: TileSet atlases are always "square"
int AtlasTileSet::GetNumRows() const
{
	return static_cast<int>(std::ceil(static_cast<double>(GetNumTiles()) / GetNumCols()));
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

QVector<int> AtlasTileSet::Cmd_AppendTiles(QSize vTileSize, const QVector<QPixmap> &pixmapList, Qt::Edge eAppendEdge)
{
	if(m_TileSize != vTileSize)
	{
		if(m_TileSize.width() < vTileSize.width())
			m_TileSize.setWidth(vTileSize.width());
		if(m_TileSize.height() < vTileSize.height())
			m_TileSize.setHeight(vTileSize.height());
	}

	QVector<int> appendIndexList;
	
	QList<TileData *> tileListSorted = m_TileDataMap.values();
	std::sort(tileListSorted.begin(), tileListSorted.end(),
		[](const TileData *pLhs, const TileData *pRhs) -> bool
		{
			return pLhs->m_iAtlasIndex < pRhs->m_iAtlasIndex;
		});
	
	QSize textureSize(GetNumCols() * GetTileSize().width(), GetNumRows() * GetTileSize().height());

	QImage newTexture(textureSize.width(), textureSize.height(), QImage::Format_ARGB32);
	newTexture.fill(Qt::transparent);

	QPainter p(&newTexture);

	// Iterate through all the tiles and draw them to the blank newTexture
	for(int i = 0; i < tileListSorted.size(); ++i)
	{
		TileData *pTileData = tileListSorted[i];

		QPoint pos(textureSize.width() * (i / GetNumCols()), textureSize.height() * (i % GetNumCols()));
		p.drawImage(pos.x(), pos.y(), QImage(packFrameRef.path), packFrameRef.crop.x(), packFrameRef.crop.y(), packFrameRef.crop.width(), packFrameRef.crop.height());
	}

	QImage *pTexture = static_cast<QImage *>(p.device());
	QDir runtimeBankDir(pBankData->m_sAbsPath);

	switch(texInfo.GetFormat())
	{
	case HYTEXTURE_Uncompressed:
		// Param1: num channels
		// Param2: disk file type (PNG, ...)
		switch(texInfo.m_uiFormatParam2)
		{
		case HyTextureInfo::UNCOMPRESSEDFILE_PNG:
			if(false == pTexture->save(runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(iActualTextureIndex) % texInfo.GetFileExt().c_str())))
				HyGuiLog("AtlasModel::ConstructAtlasTexture failed to generate a PNG atlas", LOGTYPE_Error);
			break;

		default:
			HyGuiLog("AtlasModel::ConstructAtlasTexture unknown uncompressed file type", LOGTYPE_Error);
			break;
		}
		break;

	case HYTEXTURE_DXT: {
		// Param1: num channels
		// Param2: DXT format (1,3,5)
		QImage imgProperlyFormatted = pTexture->convertToFormat(texInfo.m_uiFormatParam1 == 4 ? QImage::Format_RGBA8888 : QImage::Format_RGB888);
		if(0 == SOIL_save_image_quality(runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(iActualTextureIndex) % texInfo.GetFileExt().c_str()).toStdString().c_str(),
			SOIL_SAVE_TYPE_DDS,
			imgProperlyFormatted.width(),
			imgProperlyFormatted.height(),
			texInfo.m_uiFormatParam1,
			imgProperlyFormatted.bits(),
			0))
		{
			HyGuiLog("AtlasModel::ConstructAtlasTexture failed to generate a DTX5 atlas", LOGTYPE_Error);
		}
		break; }

	case HYTEXTURE_ASTC: {
		// Param1: Block Size index (4x4 -> 12x12)
		// Param2: Color Profile (LDR linear, LDR sRGB, HDR RGB, HDR RGBA)
		QString sProgramPath = MainWindow::EngineSrcLocation() % HYGUIPATH_AstcEncDir;
#if defined(Q_OS_WIN)
		sProgramPath += "win/astcenc-sse2.exe";
#elif defined(Q_OS_LINUX)
		sProgramPath += "linux/astcenc-sse2";
#else
		HyGuiLog("ASTC Encoder not found for this platform", LOGTYPE_Error);
#endif

		QStringList sArgList;
		switch(texInfo.m_uiFormatParam2)
		{
		case 0: sArgList << "-cl"; break; // LDR linear
		case 1: sArgList << "-cs"; break; // LDR sRGB
		case 2: sArgList << "-ch"; break; // HDR RGB
		case 3: sArgList << "-cH"; break; // HDR RGBA
		default:
			HyGuiLog("Invalid ASTC Encoder color profile", LOGTYPE_Error);
			break;
		}

		// Create temp PNG file to be used
		QString sTempTexturePath = runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(iActualTextureIndex) % ".png");
		QString sAstcTexturePath = runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(iActualTextureIndex) % texInfo.GetFileExt().c_str());
		pTexture->save(sTempTexturePath);

		sArgList << sTempTexturePath;
		sArgList << sAstcTexturePath;

		switch(texInfo.m_uiFormatParam1)
		{
		case 0:  sArgList << "4x4"; break;   // 8.00 bpp
		case 1:  sArgList << "5x4"; break;   // 6.40 bpp
		case 2:  sArgList << "5x5"; break;   // 5.12 bpp
		case 3:  sArgList << "6x5"; break;   // 4.27 bpp
		case 4:  sArgList << "6x6"; break;   // 3.56 bpp
		case 5:  sArgList << "8x5"; break;   // 3.20 bpp
		case 6:  sArgList << "8x6"; break;   // 2.67 bpp
		case 7:  sArgList << "10x5"; break;  // 2.56 bpp
		case 8:  sArgList << "10x6"; break;  // 2.13 bpp
		case 9:  sArgList << "8x8"; break;   // 2.00 bpp
		case 10: sArgList << "10x8"; break;  // 1.60 bpp
		case 11: sArgList << "10x10"; break; // 1.28 bpp
		case 12: sArgList << "12x10"; break; // 1.07 bpp
		case 13: sArgList << "12x12"; break; // 0.89 bpp
		default:
			HyGuiLog("Invalid ASTC Encoder block footprint", LOGTYPE_Error);
			break;
		}

		sArgList << "-thorough";

		QProcess astcEncProcess;
		astcEncProcess.start(sProgramPath, sArgList);
		if(!astcEncProcess.waitForStarted())
			HyGuiLog("ASTC Encoder failed to start", LOGTYPE_Error);
		else
		{
			if(!astcEncProcess.waitForFinished(-1))
				HyGuiLog("ASTC Encoder failed while encoding", LOGTYPE_Error);
		}

		// Remove the temp texture
		if(false == QFile::remove(sTempTexturePath))
			HyGuiLog("Could not remove temp PNG texture when encoding into ASTC: " % sTempTexturePath, LOGTYPE_Warning);
		break; }

	default:
		HyGuiLog("AtlasModel::ConstructAtlasTexture tried to create an unsupported texture type: " % QString::number(texInfo.GetFormat()), LOGTYPE_Error);
		break;
	}

	return textureSize;



	return appendIndexList;
}

void AtlasTileSet::Cmd_RemoveTiles(QVector<int> atlasIndexList)
{
}

QUndoStack *AtlasTileSet::GetUndoStack()
{
	return m_pUndoStack;
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
