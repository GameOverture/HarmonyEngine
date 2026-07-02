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

class TileMapModel : public IEditModeModel
{
public:
	TileMapModel(EditModeType eEditModeType);
	virtual ~TileMapModel();

	virtual QJsonObject Serialize() const = 0;
	virtual void Deserialize(bool bEnabled, const QJsonObject &serializedObj) = 0;

	virtual Qt::CursorShape MouseMoveIdle() = 0;
	virtual void MouseIdleRightClick() = 0;
	virtual bool MousePressEvent(EditModeState eEditModeState, bool bShiftHeld) = 0; // Returns whether transform has begun (otherwise marquee select)
	virtual void MouseTransform(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos) = 0;
	virtual void MouseMarqueeReleased(EditModeState eEditModeState, bool bLeftClick, QPointF ptBotLeft, QPointF ptTopRight) = 0;
	virtual void MouseClickTransformReleased(glm::vec2 ptClickPos) = 0;

	virtual void OnDeleteKeyPressed() = 0;
	
	virtual QString GetActionText(EditModeState eEditModeState, QString sNodeCodeName) const = 0; // Returns undo command description (blank if no change)
	virtual void ClearAction() = 0;
};

#endif // TileMapModel_H
