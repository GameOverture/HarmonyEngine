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

TileMapModel::TileMapModel(Project &projectRef, QUndoStack *pUndoStack) :
	IEditModeModel(EDITMODETYPE_TileMap),
	m_ProjectRef(projectRef),
	m_eLayout(HYTILEMAPLAYOUT_Unknown),
	m_eStagger(HYTILEMAPSTAGGER_Odd),
	m_vTileSize(0.0f, 0.0f)
{
}

/*virtual*/ TileMapModel::~TileMapModel()
{
}

/*virtual*/ QJsonObject TileMapModel::Serialize() const /*override*/
{
	return QJsonObject();
}

/*virtual*/ void TileMapModel::Deserialize(bool bEnabled, const QJsonObject &serializedObj) /*override*/
{
	int i =0;
	++i;
}

/*virtual*/ Qt::CursorShape TileMapModel::MouseMoveIdle() /*override*/
{
	return Qt::ArrowCursor;
}

/*virtual*/ void TileMapModel::MouseIdleRightClick() /*override*/
{
}

/*virtual*/ bool TileMapModel::MousePressEvent(EditModeState eEditModeState, bool bShiftHeld) /*override*/
{
	return false;
}

/*virtual*/ void TileMapModel::MouseTransform(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos) /*override*/
{
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

HyTileMapLayout TileMapModel::GetLayout() const
{
	return m_eLayout;
}

HyTileMapStagger TileMapModel::GetStagger() const
{
	return m_eStagger;
}

glm::ivec2 TileMapModel::GetTileSize() const
{
	return m_vTileSize;
}

const Tiled::TileLayer &TileMapModel::GetTiledTileLayer() const
{
	return m_TiledLayer;
}

QList<AtlasTileSet *> TileMapModel::UsedTilesets(const AtlasManager &atlasManagerRef) const
{
	QList<AtlasTileSet *> returnList;

	QSet<Tiled::SharedTileset> tiledTileSets = m_TiledLayer.usedTilesets();
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

HyShader *TileMapModel::GetSquareShader()
{
	return m_ProjectRef.GetProjDraw()->GetTileMapSquareShader();
}

HyShader *TileMapModel::GetHexShader()
{
	return m_ProjectRef.GetProjDraw()->GetTileMapHexShader();
}

void TileMapModel::SetCell(int iX, int iY, AtlasTileSet *pTileSet, int iTileId)
{
	Tiled::Cell cell(pTileSet->GetTiledTileSet().data(), iTileId);
	m_TiledLayer.setCell(iX, iY, cell);

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
