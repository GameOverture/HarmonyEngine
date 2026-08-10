/**************************************************************************
*	TileMapModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "TileMapModel.h"
#include "AtlasTileSet.h"
#include "AtlasManager.h"
#include "Project.h"
#include "TileMapView.h"
#include "MainWindow.h"
#include "AuxTileMap.h"
#include "vendor/libtiled/maptovariantconverter.h"

TileMapModel::TileMapModel(Project &projectRef, QUndoStack *pUndoStack, QString sLayerCodeName) :
	IEditModeModel(EDITMODETYPE_TileMap),
	m_ProjectRef(projectRef),
	m_bValidHoverCoord(false)
{
	m_pTiledMap = std::make_unique<Tiled::Map>();
	m_pTiledMap->setInfinite(true);
	m_pTiledMap->setCompressionLevel(m_ProjectRef.GetTileMapSettings().compressionLevel);
	m_pTiledMap->setChunkSize(m_ProjectRef.GetTileMapSettings().chunkSize);
	m_pTiledMap->setLayerDataFormat(m_ProjectRef.GetTileMapSettings().layerDataFormat);
	m_pTiledMap->addLayer(new Tiled::TileLayer(sLayerCodeName));
}

/*virtual*/ TileMapModel::~TileMapModel()
{
}

/*virtual*/ QJsonObject TileMapModel::Serialize() const /*override*/
{
	QJsonObject serializedObj;

	Tiled::MapToVariantConverter tiledMapConverter;
	QVariantMap tiledMapVariantMap = tiledMapConverter.toVariant(*m_pTiledMap, QDir()).toMap();

	QJsonArray tileSetsArray;
	QVariantList tileSetsVariantList = tiledMapVariantMap["tilesets"].toList();
	for(QVariant tileSetVar : tileSetsVariantList)
	{
		QVariantMap tileSetVarMap = tileSetVar.toMap();

		QJsonObject tileSetObj;
		tileSetObj.insert("firstgid", tileSetVarMap["firstgid"].toInt());
		tileSetObj.insert("source", tileSetVarMap["source"].toString());

		tileSetsArray.append(tileSetObj);
	}
	serializedObj.insert("tilesets", tileSetsArray);

	QVariantList layersVariantList = tiledMapVariantMap["layers"].toList();
	if(layersVariantList.size() != 1)
		HyGuiLog("TileMapModel::Serialize() - internal Tiled::Map does not have 1 layer", LOGTYPE_Error);
	QVariantMap layerVarMap = layersVariantList[0].toMap();
	QJsonObject tileLayerObj;
	tileLayerObj.insert("width", layerVarMap["width"].toInt());
	tileLayerObj.insert("height", layerVarMap["height"].toInt());
	tileLayerObj.insert("startx", layerVarMap["startx"].toInt());
	tileLayerObj.insert("starty", layerVarMap["starty"].toInt());
	tileLayerObj.insert("id", layerVarMap["id"].toInt());
	tileLayerObj.insert("name", layerVarMap["name"].toString());
	tileLayerObj.insert("x", layerVarMap["x"].toInt());
    tileLayerObj.insert("y", layerVarMap["y"].toInt());
    tileLayerObj.insert("visible", layerVarMap["visible"].toBool());
	tileLayerObj.insert("compression", layerVarMap["compression"].toString());
	QString sEncoding = layerVarMap["encoding"].toString();
	tileLayerObj.insert("encoding", sEncoding);
	bool bUseCsv = sEncoding.isEmpty() || sEncoding.isNull() || sEncoding.compare("csv", Qt::CaseInsensitive) == 0;
	QJsonArray chunksArray;
	QVariantList chunkVarList = layerVarMap["chunks"].toList();
	for(QVariant chunkVar : chunkVarList)
	{
		QVariantMap chunkVarMap = chunkVar.toMap();
		QJsonObject chunkObj;
		chunkObj.insert("x", chunkVarMap["x"].toInt());
		chunkObj.insert("y", chunkVarMap["y"].toInt());
		chunkObj.insert("width", chunkVarMap["width"].toInt());
		chunkObj.insert("height", chunkVarMap["height"].toInt());
		if(bUseCsv)
		{
			QVariantList dataList = chunkVarMap["data"].toList();
			QJsonArray dataArray;
			for(QVariant var : dataList)
				dataArray.append(var.toInt());
			chunkObj.insert("data", dataArray);
		}
		else
		{
			QByteArray base64ChunkData = chunkVarMap["data"].toByteArray();
			chunkObj.insert("data", QString::fromLatin1(base64ChunkData));
		}

		chunksArray.append(chunkObj);
	}
	tileLayerObj.insert("chunks", chunksArray);
	serializedObj.insert("tileLayer", tileLayerObj);

	return serializedObj;
}
 
/*virtual*/ void TileMapModel::Deserialize(bool bEnabled, const QJsonObject &serializedObj) /*override*/
{
	while(m_pTiledMap->layerCount() > 0)
	{
		Tiled::Layer *pLayer = m_pTiledMap->takeLayerAt(0);
		delete pLayer;
	}
	while(m_pTiledMap->tilesetCount() > 0)
		m_pTiledMap->removeTilesetAt(0);

	if(serializedObj.empty())
		return;

	Tiled::GidMapper mGidMapper;

	QJsonArray tileSetsArray = serializedObj["tilesets"].toArray();
	for(QJsonValue val : tileSetsArray)
	{
		QJsonObject tileSetObj = val.toObject();
		AtlasTileSet *pTileSet = m_ProjectRef.GetAtlasModel().GetTileSetsModel().GetTileSet(tileSetObj["source"].toString());
		m_pTiledMap->addTileset(pTileSet->GetTiledTileSet());

		mGidMapper.insert(static_cast<uint32_t>(tileSetObj["firstgid"].toInteger()), pTileSet->GetTiledTileSet());
	}

	QJsonObject tileLayerObj = serializedObj["tileLayer"].toObject();
	const QString name = tileLayerObj["name"].toString();
	const int width = tileLayerObj["width"].toInt();
	const int height = tileLayerObj["height"].toInt();
	const int startX = tileLayerObj["startx"].toInt();
	const int startY = tileLayerObj["starty"].toInt();

	std::unique_ptr<Tiled::TileLayer> pTileLayer(new Tiled::TileLayer(name,
																	 tileLayerObj["x"].toInt(),
																	 tileLayerObj["y"].toInt(),
																	 width, height));

	const QString sEncoding = tileLayerObj["encoding"].toString();
	const QString sCompression = tileLayerObj["compression"].toString();
	Tiled::Map::LayerDataFormat layerDataFormat = Tiled::Map::Base64;
	if(sEncoding.isEmpty() || sEncoding == QLatin1String("csv"))
		layerDataFormat = Tiled::Map::CSV;
	else if(sEncoding == QLatin1String("base64"))
	{
		if(sCompression.isEmpty())
			layerDataFormat = Tiled::Map::Base64;
		else if(sCompression == QLatin1String("gzip"))
			layerDataFormat = Tiled::Map::Base64Gzip;
		else if(sCompression == QLatin1String("zlib"))
			layerDataFormat = Tiled::Map::Base64Zlib;
		else if(sCompression == QLatin1String("zstd"))
			layerDataFormat = Tiled::Map::Base64Zstandard;
		else
			HyGuiLog("Compression method " % sCompression % " not supported", LOGTYPE_Error);
	}
	else
		HyGuiLog("Unknown encoding: " % sEncoding, LOGTYPE_Error);

	QJsonArray chunksArray = tileLayerObj["chunks"].toArray();
	for(QJsonValue val : chunksArray)
	{
		QJsonObject chunkObj = val.toObject();
		int x = chunkObj["x"].toInt();
		int y = chunkObj["y"].toInt();
		int width = chunkObj["width"].toInt();
		int height = chunkObj["height"].toInt();
		QRect bounds(x, y, width, height);

		switch(layerDataFormat)
		{
		case Tiled::Map::XML:
		case Tiled::Map::CSV: {
			QJsonArray chunkArray = chunkObj["data"].toArray();
			if(chunkArray.size() != bounds.width() * bounds.height())
				HyGuiLog("Corrupt layer data for layer: " % name, LOGTYPE_Error);

			int x = bounds.x();
			int y = bounds.y();
			bool ok;

			for(QJsonValue val : chunkArray)
			{
				const unsigned gid = val.toInt();
				const Tiled::Cell cell = mGidMapper.gidToCell(gid, ok);

				pTileLayer->setCell(x, y, cell);

				x++;
				if (x > bounds.right()) {
					x = bounds.x();
					y++;
				}
			}
			break; }

		case Tiled::Map::Base64:
		case Tiled::Map::Base64Zlib:
		case Tiled::Map::Base64Gzip:
		case Tiled::Map::Base64Zstandard: {
			QString sBase64 = chunkObj["data"].toString();
			const QByteArray data = sBase64.toLatin1();
			Tiled::GidMapper::DecodeError error = mGidMapper.decodeLayerData(*pTileLayer,
																			 data,
																			 layerDataFormat,
																			 bounds);
			switch(error)
			{
			case Tiled::GidMapper::CorruptLayerData:
				HyGuiLog("Corrupt layer data for layer: " % pTileLayer->name(), LOGTYPE_Error);
				break;
			case Tiled::GidMapper::TileButNoTilesets:
				HyGuiLog("Tile used but no tilesets specified", LOGTYPE_Error);
				break;
			case Tiled::GidMapper::InvalidTile:
				HyGuiLog("Invalid tile: " % QString::number(mGidMapper.invalidTile()), LOGTYPE_Error);
				break;
			case Tiled::GidMapper::NoError:
				break;
			}
			break; }
		}
	}
	
	m_pTiledMap->addLayer(std::move(pTileLayer));
}

/*virtual*/ Qt::CursorShape TileMapModel::MouseMoveIdle() /*override*/
{
	SyncViews(EDITMODE_Idle);

	if(m_bValidHoverCoord)
		MainWindow::SetStatus("Edit Mode - " + QString::number(m_ptHoverCoord.x()) + ", " + QString::number(m_ptHoverCoord.y()), 0);
	else
		MainWindow::SetStatus("Edit Mode", 0);

	return Qt::ArrowCursor;
}

/*virtual*/ void TileMapModel::MouseIdleRightClick() /*override*/
{
}

/*virtual*/ bool TileMapModel::MousePressEvent(EditModeState eEditModeState, bool bShiftHeld) /*override*/
{
	AuxTileMap *pAuxTileMap = static_cast<AuxTileMap *>(MainWindow::GetAuxWidget(AUXTAB_TileMap));
	TileMapTool eCurTool = pAuxTileMap->GetSelectedTool();

	switch(eCurTool)
	{
	case TILEMAPTOOL_Select:
		break;

	case TILEMAPTOOL_Paint:
		return true;

	case TILEMAPTOOL_Rect:
	case TILEMAPTOOL_Circle:
	case TILEMAPTOOL_Fill:
	case TILEMAPTOOL_Picker:
	case TILEMAPTOOL_Eraser:
		break;
	}

	return false;
}

/*virtual*/ void TileMapModel::MouseTransform(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos) /*override*/
{
	for(IEditModeView *pView : m_ViewList)
		static_cast<TileMapView *>(pView)->SyncMouseHoverGrid();
}

/*virtual*/ void TileMapModel::MouseMarqueeReleased(EditModeState eEditModeState, bool bLeftClick, QPointF ptBotLeft, QPointF ptTopRight) /*override*/
{
}

/*virtual*/ void TileMapModel::MouseClickTransformReleased(glm::vec2 ptClickPos) /*override*/
{
}

/*virtual*/ void TileMapModel::OnDeleteKeyPressed() /*override*/
{
}

/*virtual*/ QString TileMapModel::GetActionText(EditModeState eEditModeState, QString sNodeCodeName) const /*override*/
{
	return QString();
}

/*virtual*/ void TileMapModel::ClearAction() /*override*/
{
}

glm::ivec2 TileMapModel::GetGridSize() const
{
	return glm::ivec2(m_pTiledMap->tileSize().width(), m_pTiledMap->tileSize().height());
}

void TileMapModel::SetGridSize(glm::ivec2 vGridSize)
{
	m_pTiledMap->setTileSize(QSize(vGridSize.x, vGridSize.y));
}

HyTileMapLayout TileMapModel::GetLayout() const
{
	switch(m_pTiledMap->orientation())
	{
	case Tiled::Map::Orthogonal:
		return HYTILEMAPLAYOUT_Square;

	case Tiled::Map::Isometric:
		return HYTILEMAPLAYOUT_Isometric;

	case Tiled::Map::Staggered:
		if(m_pTiledMap->staggerAxis() == Tiled::Map::StaggerX)
			return HYTILEMAPLAYOUT_IsometricStaggerX;
		else
			return HYTILEMAPLAYOUT_IsometricStaggerY;

	case Tiled::Map::Hexagonal:
		if(m_pTiledMap->staggerAxis() == Tiled::Map::StaggerX)
		{
			if(m_pTiledMap->hexSideLength() == m_pTiledMap->tileWidth())
				return HYTILEMAPLAYOUT_HalfOffsetSquare;

			return HYTILEMAPLAYOUT_HexagonFlatTop;
		}
		else
		{
			if(m_pTiledMap->hexSideLength() == m_pTiledMap->tileHeight())
				return HYTILEMAPLAYOUT_HalfOffsetSquare;

			return HYTILEMAPLAYOUT_HexagonPointTop;
		}

	case Tiled::Map::Unknown:
	case Tiled::Map::Oblique:
	default:
		HyGuiLog("TileMapModel::GetLayout() - Unhandled tiled orientation", LOGTYPE_Error);
		break;
	}

	return HYTILEMAPLAYOUT_Unknown;
}

void TileMapModel::SetLayout(HyTileMapLayout eLayout)
{
	switch(eLayout)
	{
	case HYTILEMAPLAYOUT_Square:
		m_pTiledMap->setOrientation(Tiled::Map::Orthogonal);
		break;

	case HYTILEMAPLAYOUT_HalfOffsetSquare: // NOTE: In Tiled, using hexagonal orientation and matching HexSideLength with Flat-Top (StaggerX) tile width creates "half-offset squares"
		m_pTiledMap->setOrientation(Tiled::Map::Hexagonal);
		m_pTiledMap->setStaggerAxis(Tiled::Map::StaggerX);
		m_pTiledMap->setStaggerIndex(Tiled::Map::StaggerOdd); // Harmony always uses Odd
		m_pTiledMap->setHexSideLength(m_pTiledMap->tileSize().width());
		break;

	case HYTILEMAPLAYOUT_Isometric:
		m_pTiledMap->setOrientation(Tiled::Map::Isometric);
		break;

	case HYTILEMAPLAYOUT_IsometricStaggerX:
		m_pTiledMap->setOrientation(Tiled::Map::Staggered);
		m_pTiledMap->setStaggerAxis(Tiled::Map::StaggerX);
		m_pTiledMap->setStaggerIndex(Tiled::Map::StaggerOdd); // Harmony always uses Odd
		break;

	case HYTILEMAPLAYOUT_IsometricStaggerY:
		m_pTiledMap->setOrientation(Tiled::Map::Staggered);
		m_pTiledMap->setStaggerAxis(Tiled::Map::StaggerY);
		m_pTiledMap->setStaggerIndex(Tiled::Map::StaggerOdd); // Harmony always uses Odd
		break;

	case HYTILEMAPLAYOUT_HexagonFlatTop:
		m_pTiledMap->setOrientation(Tiled::Map::Hexagonal);
		m_pTiledMap->setStaggerAxis(Tiled::Map::StaggerX);
		m_pTiledMap->setStaggerIndex(Tiled::Map::StaggerOdd); // Harmony always uses Odd
		m_pTiledMap->setHexSideLength(m_pTiledMap->tileSize().width() / 2);
		break;

	case HYTILEMAPLAYOUT_HexagonPointTop:
		m_pTiledMap->setOrientation(Tiled::Map::Hexagonal);
		m_pTiledMap->setStaggerAxis(Tiled::Map::StaggerY);
		m_pTiledMap->setStaggerIndex(Tiled::Map::StaggerOdd); // Harmony always uses Odd
		m_pTiledMap->setHexSideLength(m_pTiledMap->tileSize().height() / 2);
		break;
	}
}

void TileMapModel::SetHoverCoordinates(bool bValidHoverCoord, QPoint ptHoverCoord)
{
	m_bValidHoverCoord = bValidHoverCoord;
	m_ptHoverCoord = ptHoverCoord;
}

QList<AtlasTileSet *> TileMapModel::UsedTilesets() const
{
	QList<AtlasTileSet *> returnList;

	const AtlasManager &atlasManagerRef = m_ProjectRef.GetAtlasModel();

	QSet<Tiled::SharedTileset> tiledTileSets = m_pTiledMap->usedTilesets();
	for(Tiled::SharedTileset tileSetPtr : tiledTileSets)
	{
		QString sTileSetName = tileSetPtr->name();
		if(atlasManagerRef.GetTileSetsModel().Contains(sTileSetName) == false)
			HyGuiLog("TileMapModel::UsedTilesets - could not find TileSet in Atlas Manager with name: " % sTileSetName, LOGTYPE_Error);
		else
			returnList.push_back(atlasManagerRef.GetTileSetsModel().GetTileSet(sTileSetName));
	}

	return returnList;
}

HyShader *TileMapModel::GetGridShader()
{
	return m_ProjectRef.GetProjDraw()->GetTileMapGridShader(GetLayout());
}

void TileMapModel::SetCell(int iX, int iY, AtlasTileSet *pTileSet, int iTileId)
{
	Tiled::Layer *pLayer0 = m_pTiledMap->layerAt(0);
	if(pLayer0->isTileLayer() == false)
	{
		HyGuiLog("TileMapModel::SetCell - invalid Tiled map layer 0", LOGTYPE_Error);
		return;
	}

	Tiled::TileLayer *pTiledTileLayer = pLayer0->asTileLayer();
	Tiled::Cell cell(pTileSet->GetTiledTileSet().data(), iTileId);
	pTiledTileLayer->setCell(iX, iY, cell);

	//// NOTE: We register the tile map's dependency on the tile set. The entity will have a dependency on the tile map, which in turn has a dependency on the tile set.
	//QList<QUuid> registerList;
	//registerList.push_back(pTileSet->GetUuid());
	//m_ItemRef.GetProject().IncrementDependencies(pAddedTileMap, registerList);
	//registerList.clear();
	//registerList.push_back(pAddedTileMap->GetUuid());
	//m_ItemRef.GetProject().IncrementDependencies(&m_ItemRef, registerList);
}

void TileMapModel::UpdateTileIds(const std::vector<std::pair<uint16, uint16>> &modifiedIndexList) // Pair<old, new>
{
}
