/**************************************************************************
*	TileMapModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef TileMapModel_H
#define TileMapModel_H

#include "Global.h"
#include "IEditModeModel.h"

class AtlasTileSet;

class TileMapModel : public IEditModeModel
{
	AtlasTileSet *			m_pTileSet;

public:
	TileMapModel();
	virtual ~TileMapModel();

	virtual QJsonObject Serialize() const override;
	virtual void Deserialize(bool bEnabled, const QJsonObject &serializedObj) override;

	virtual Qt::CursorShape MouseMoveIdle() override;
	virtual void MouseIdleRightClick() override;
	virtual bool MousePressEvent(EditModeState eEditModeState, bool bShiftHeld) override; // Returns whether transform has begun (otherwise marquee select)
	virtual void MouseTransform(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos) override;
	virtual void MouseMarqueeReleased(EditModeState eEditModeState, bool bLeftClick, QPointF ptBotLeft, QPointF ptTopRight) override;
	virtual void MouseClickTransformReleased(glm::vec2 ptClickPos) override;

	virtual void OnDeleteKeyPressed() override;
	
	virtual QString GetActionText(EditModeState eEditModeState, QString sNodeCodeName) const override; // Returns undo command description (blank if no change)
	virtual void ClearAction() override;

	void UpdateAtlasIndices(const std::vector<std::pair<uint16, uint16>> &modifiedIndexList);
};

#endif // TileMapModel_H
