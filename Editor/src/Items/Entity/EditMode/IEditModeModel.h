/**************************************************************************
*	IEditModeModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IEditModeModel_H
#define IEditModeModel_H

#include "Global.h"

class IEditModeView;

enum EditModeType
{
	EDITMODETYPE_Invalid = -1,

	EDITMODETYPE_PrimitiveShape = 0,
	EDITMODETYPE_PrimitiveLineChain,

	EDITMODETYPE_FixtureShape,
	EDITMODETYPE_FixtureChain,
	EDITMODETYPE_FixturePoint,

	EDITMODETYPE_TileMap,

	NUM_EDITMODETYPES
};

enum EditModeState
{
	EDITMODE_Off = 0,
	EDITMODE_Idle,					// Mouse cursor determined by draw models
	EDITMODE_MouseDownOutside,		// Click started outside of the edit item's bounds
	EDITMODE_MouseDragMarquee,		// Click-dragging a marquee select box
	EDITMODE_MouseClickTransform,	// Click started on item to be manipulated
	EDITMODE_MouseDragTransform,	// Transforming (translating, rotating, scaling) the edit item
};

class IEditModeModel
{
protected:
	EditModeType						m_eEditModeType;
	
	// Track Views manually since we don't inherit from QObject
	QList<IEditModeView *>				m_ViewList;

public:
	IEditModeModel(EditModeType eEditModeType);
	virtual ~IEditModeModel();

	EditModeType GetEditModeType() const;

	void AddView(IEditModeView *pView);
	bool RemoveView(IEditModeView *pView);
	void SyncViews(EditModeState eEditModeState) const;

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

#endif // IEditModeModel_H
