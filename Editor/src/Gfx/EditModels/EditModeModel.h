/**************************************************************************
*	EditModeModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef EditModeModel_H
#define EditModeModel_H

#include "Global.h"
#include "GfxGrabPointModel.h"

class EditModeView;

enum EditModeState
{
	EDITMODE_Off = 0,
	EDITMODE_Idle,					// Mouse cursor determined by draw models
	EDITMODE_MouseDownOutside,		// Click started outside of the edit item's bounds
	EDITMODE_MouseDragMarquee,		// Click-dragging a marquee select box
	EDITMODE_MouseDownTransform,	// Click started on item to be manipulated
	EDITMODE_MouseDragTransform,	// Transforming (translating, rotating, scaling) the edit item
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

class EditModeModel
{
	HyColor								m_Color;

	// ------------------------------------------------------------------------------------------------------------------
	// "type" - when serialized in property (as a string)
	bool								m_bIsLineChain;			// Whether this is a chain model (true) or shape model (false)
	EditorShape							m_eShapeType;			// When m_bIsLineChain is false, this indicates the primitive shape type (Box, Circle, LineSegment, Polygon, Capsule). Ignored when m_bIsLineChain is true
	// "data" - when serialized in property (QJsonArray of floats)
	QList<IHyFixture2d *>				m_FixtureList;			// This is the actual shape data used for physics/collision/rendering - usually just one fixture, but could be multiple for complex polygons
	QList<GfxGrabPointModel>			m_GrabPointList;		// Grab Points for editing the shape - Used to serialize data when type is SHAPE_Polygon (then assembles m_FixtureList with valid sub-polygons)
	GfxGrabPointModel					m_GrabPointCenter;
	bool								m_bLoopClosed;
	// "outline" - when serialized in property (float)
	float								m_fOutline;				// "outline" is used with primitive layers to determine whether to render a solid (0.0f) or an outline around the shape 
	// ------------------------------------------------------------------------------------------------------------------
	
	// Extra validation used with Chain or Polygon types
	bool								m_bSelfIntersecting;
	glm::vec2							m_ptSelfIntersection;
	QString								m_sMalformedReason;		// If not empty, this edit model is considered invalid and the reason is given by this string (e.g. "Polygon has intersecting edges")

	// Transform Action info
	EditModeAction						m_eCurAction;
	glm::vec2							m_vDragDelta;
	int									m_iGrabPointIndex;
	glm::vec2							m_ptGrabPointPos;

	// Track Views manually since we don't inherit from QObject
	QList<EditModeView *>				m_ViewList;

public:
	EditModeModel(HyColor color);
	~EditModeModel();

	HyColor GetColor() const;
	void SetColor(HyColor color);

	bool IsLineChain() const;
	EditorShape GetShapeType() const;

	void ChangeToLineChain();
	void ChangeToShape(EditorShape eNewShapeType);

	bool IsLoopClosed() const;
	float GetOutline() const;

	bool IsValidModel() const;
	QJsonObject Serialize() const;
	void Deserialize(const QJsonObject &serializedObj);

	void AddView(EditModeView *pView);
	bool RemoveView(EditModeView *pView);
	void SyncViews(EditModeState eEditModeState, EditModeAction eResult) const;

	int GetNumFixtures() const;
	const IHyFixture2d *GetFixture(int iIndex) const;

	const QList<GfxGrabPointModel> &GetGrabPointList() const;
	const GfxGrabPointModel &GetGrabPoint(int iIndex) const;
	const GfxGrabPointModel *GetActiveGrabPoint() const;
	const GfxGrabPointModel &GetCenterGrabPoint() const;
	int GetActiveGrabPointIndex() const;
	int GetNumGrabPointsSelected() const;
	bool IsAllGrabPointsSelected() const;
	bool IsHoverGrabPointSelected() const;
	void DeselectAllGrabPoints();

	Qt::CursorShape MouseMoveIdle();
	bool MousePressEvent(EditModeState eEditModeState, bool bShiftHeld, Qt::MouseButtons uiButtonFlags); // Returns whether transform has begun (otherwise marquee select)
	void MouseMarqueeReleased(EditModeState eEditModeState, bool bLeftClick, QPointF ptBotLeft, QPointF ptTopRight);
	void MouseTransform(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos);

	glm::vec2 GetDragDelta() const;
	
	QString GetActionText(QString sNodeCodeName) const; // Returns undo command description (blank if no change)
	void ClearAction();

protected:
	std::vector<float> ConvertedBoxData() const;
	std::vector<float> ConvertedCircleData() const;
	std::vector<float> ConvertedLineSegmentData() const;
	std::vector<float> ConvertedCapsuleData() const;
	std::vector<float> ConvertedPolygonOrLineChainData() const;

	EditModeAction DoMouseMoveIdle();

	void TransformData(glm::mat4 mtxTransform);

	// Action Transforms - These overrides will change the model's data as they are being performed. Upon completion, Serialize() can be called
	void DoTransformCreation(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos);

	void ClearFixtures();

	std::vector<float> SerializeData() const;
	QString DeserializeData(const QJsonObject &serializedObj); // Returns empty string if successful, otherwise returns reason for failure (e.g. "Polygon has intersecting edges")

	bool CheckIfAddVertexOnEdge();
	void AssemblePolygonFixtures(std::vector<std::vector<glm::vec2>> subPolygonList); // Assembles m_FixtureList from properly formed sub-polygons (no self-intersections, correct winding, convex, <= 8 vertices)
	std::vector<glm::vec2> MergePolygons(const std::vector<glm::vec2> &ptA, const std::vector<glm::vec2> &ptB, int a0, int a1, int b0, int b1);
	std::vector<std::vector<glm::vec2>> MergeTriangles(const std::vector<HyTriangle2d> &triangleList);
	bool IsShareEdge(const std::vector<glm::vec2> &a, const std::vector<glm::vec2> &b, int &a0, int &a1, int &b0, int &b1);
};

#endif // EditModeModel_H
