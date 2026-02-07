/**************************************************************************
*	IGfxEditModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IGfxEditModel_H
#define IGfxEditModel_H

#include "Global.h"
#include "GfxGrabPointModel.h"

class IGfxEditView;

enum EditModeState
{
	EDITMODE_Off = 0,
	EDITMODE_Idle,					// Mouse cursor determined by draw models
	EDITMODE_MouseDownOutside,		// Click started outside of the edit item's bounds
	EDITMODE_MouseDragMarquee,		// Click-dragging a marquee select box
	EDITMODE_MouseDownTransform,	// Click started on item to be manipulated
	EDITMODE_MouseDragTransform,	// Transforming (translating, rotating, scaling) the edit item
};

enum EditModeType
{
	EDITMODETYPE_None = 0,
	EDITMODETYPE_Shape,
	EDITMODETYPE_Chain,
	EDITMODETYPE_Primitive
};

enum EditModeAction
{
	EDITMODEACTION_None = 0,

	EDITMODEACTION_Creation,
	EDITMODEACTION_Outside,
	EDITMODEACTION_Inside,
	EDITMODEACTION_AppendVertex,
	EDITMODEACTION_InsertVertex,
	EDITMODEACTION_HoverGrabPoint,
	EDITMODEACTION_HoverCenter
};

class IGfxEditModel
{
protected:
	const EditModeType					m_eMODEL_TYPE;

	HyColor								m_Color;

	QList<GfxGrabPointModel>			m_GrabPointList;		// Grab Points for editing the shape - Used to serialize data when type is SHAPE_Polygon (then assembles m_FixtureList with valid sub-polygons)
	GfxGrabPointModel					m_GrabPointCenter;

	// Transform info
	EditModeAction						m_eCurAction;
	int									m_iGrabPointIndex;
	glm::vec2							m_ptGrabPointPos;

	// Track Views manually since we don't inherit from QObject
	QList<IGfxEditView *>				m_ViewList;

public:
	IGfxEditModel(EditModeType eModelType, HyColor color);
	virtual ~IGfxEditModel();

	EditModeType GetModelType() const;

	HyColor GetColor() const;
	void SetColor(HyColor color);

	virtual bool IsValidModel() const = 0;
	virtual QList<float> Serialize() const = 0;
	void Deserialize(const QList<float> &floatList);

	void AddView(IGfxEditView *pView);
	bool RemoveView(IGfxEditView *pView);
	void SyncViews(EditModeState eEditModeState, EditModeAction eResult) const;

	const QList<GfxGrabPointModel> &GetGrabPointList() const;
	const GfxGrabPointModel &GetGrabPoint(int iIndex) const;
	const GfxGrabPointModel &GetCenterGrabPoint() const;
	int GetNumGrabPointsSelected() const;
	bool IsAllGrabPointsSelected() const;
	bool IsHoverGrabPointSelected() const;
	void DeselectAllGrabPoints();

	Qt::CursorShape MouseMoveIdle(EditModeState eEditModeState, glm::vec2 ptWorldMousePos);
	bool MousePressEvent(EditModeState eEditModeState, bool bShiftHeld, Qt::MouseButtons uiButtonFlags, glm::vec2 ptWorldMousePos); // Returns whether transform has begun (otherwise marquee select)
	void MouseMarqueeReleased(EditModeState eEditModeState, bool bLeftClick, QPointF ptBotLeft, QPointF ptTopRight);
	void MouseTransform(EditModeState eEditModeState, bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos);
	virtual QString MouseTransformReleased(QString sShapeCodeName, QPointF ptWorldMousePos) = 0; // Returns undo command description (blank if no change)

protected:
	virtual bool DoDeserialize(const QList<float> &floatList) = 0;
	virtual EditModeAction DoMouseMoveIdle(glm::vec2 ptWorldMousePos) = 0;
	virtual void DoTransformCreation(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos) = 0;
};

#endif // IGfxEditModel_H
