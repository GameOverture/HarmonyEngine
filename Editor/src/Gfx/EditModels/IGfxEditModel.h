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

enum EditModelType
{
	EDITMODEL_None = 0,
	EDITMODEL_Shape,
	EDITMODEL_Chain,
	EDITMODEL_Primitive
};

enum ShapeMouseMoveResult
{
	SHAPEMOUSEMOVE_None = 0,

	SHAPEMOUSEMOVE_Creation,
	SHAPEMOUSEMOVE_Outside,
	SHAPEMOUSEMOVE_Inside,
	SHAPEMOUSEMOVE_AppendVertex,
	SHAPEMOUSEMOVE_InsertVertex,
	SHAPEMOUSEMOVE_HoverGrabPoint,
	SHAPEMOUSEMOVE_HoverCenter
};

class IGfxEditModel
{
protected:
	const EditModelType					m_eMODEL_TYPE;

	HyColor								m_Color;

	QList<GfxGrabPointModel>			m_GrabPointList;		// Grab Points for editing the shape - Used to serialize data when type is SHAPE_Polygon (then assembles m_FixtureList with valid sub-polygons)
	GfxGrabPointModel					m_GrabPointCenter;

	// Transform info
	ShapeMouseMoveResult				m_eCurTransform;
	glm::mat4							m_mtxTransform;			// The current transform being applied during a mouse operation
	int									m_iVertexIndex;
	glm::vec2							m_ptVertexPos;
	bool								m_bTransformShiftMod;

	// Track Views manually since we don't inherit from QObject
	QList<IGfxEditView *>				m_ViewList;

public:
	IGfxEditModel(EditModelType eModelType, HyColor color);
	virtual ~IGfxEditModel();

	EditModelType GetModelType() const;

	HyColor GetColor() const;
	void SetColor(HyColor color);

	virtual bool IsValidModel() const = 0;
	virtual QList<float> Serialize() const = 0;
	void Deserialize(const QList<float> &floatList);

	void AddView(IGfxEditView *pView);
	bool RemoveView(IGfxEditView *pView);
	void RefreshViews(EditModeState eEditModeState, ShapeMouseMoveResult eResult) const;

	void GetTransformPreview(glm::mat4 &mtxTransformOut, int &iVertexIndexOut) const;

	const QList<GfxGrabPointModel> &GetGrabPointList() const;
	const GfxGrabPointModel &GetGrabPoint(int iIndex) const;
	const GfxGrabPointModel &GetCenterGrabPoint() const;
	int GetNumGrabPointsSelected() const;
	bool IsAllGrabPointsSelected() const;
	bool IsHoverGrabPointSelected() const;
	void DeselectAllGrabPoints();

	ShapeMouseMoveResult MouseMoveIdle(EditModeState eEditModeState, glm::vec2 ptWorldMousePos);
	ShapeMouseMoveResult MousePressEvent(EditModeState eEditModeState, bool bShiftHeld, Qt::MouseButtons uiButtonFlags, glm::vec2 ptWorldMousePos); // Returns whether transform has begun (otherwise marquee select)
	void MouseMarqueeReleased(EditModeState eEditModeState, bool bLeftClick, QPointF ptBotLeft, QPointF ptTopRight);
	void MouseMoveTransform(EditModeState eEditModeState, bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos);
	virtual QString MouseTransformReleased(QString sShapeCodeName, QPointF ptWorldMousePos) = 0; // Returns undo command description (blank if no change)

protected:
	virtual void DoDeserialize(const QList<float> &floatList) = 0;
	virtual ShapeMouseMoveResult DoMouseMoveIdle(glm::vec2 ptWorldMousePos) = 0;
	virtual void DoTransformCreation(glm::vec2 ptStartPos, glm::vec2 ptDragPos) = 0;
};

#endif // IGfxEditModel_H
