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

TileMapModel::TileMapModel(Project &projectRef, QUndoStack *pUndoStack) :
	IEditModeModel(EDITMODETYPE_TileMap),
	m_ProjectRef(projectRef),
	m_vGridSize(0.0f, 0.0f),
	m_eLayout(HYTILEMAPLAYOUT_Unknown),
	m_bValidHoverCoord(false)
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
	if(serializedObj.empty())
	{
		m_vGridSize = { 0.0f, 0.0f };
		m_eLayout = HYTILEMAPLAYOUT_Unknown;

		//m_TiledLayer.
		return;
	}
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
	return m_vGridSize;
}

void TileMapModel::SetGridSize(glm::ivec2 vGridSize)
{
	m_vGridSize = vGridSize;
}

HyTileMapLayout TileMapModel::GetLayout() const
{
	return m_eLayout;
}

void TileMapModel::SetLayout(HyTileMapLayout eLayout)
{
	m_eLayout = eLayout;
}

void TileMapModel::SetHoverCoordinates(bool bValidHoverCoord, QPoint ptHoverCoord)
{
	m_bValidHoverCoord = bValidHoverCoord;
	m_ptHoverCoord = ptHoverCoord;
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

HyShader *TileMapModel::GetGridShader()
{
	return m_ProjectRef.GetProjDraw()->GetTileMapGridShader(m_eLayout);
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
