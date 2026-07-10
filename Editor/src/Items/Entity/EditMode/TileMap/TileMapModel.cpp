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

TileMapModel::TileMapModel() :
	IEditModeModel(EDITMODETYPE_TileMap),
	m_pTileSet(nullptr)
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

void TileMapModel::UpdateTileIds(const std::vector<std::pair<uint16, uint16>> &modifiedIndexList) // Pair<old, new>
{
}
