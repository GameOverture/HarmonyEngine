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
#include "IEditModeModel.h"
#include "GfxGrabPointModel.h"

enum EditModeAction
{
	EDITMODEACTION_None = 0,

	EDITMODEACTION_Creation,
	EDITMODEACTION_Outside,
	EDITMODEACTION_Inside,
	EDITMODEACTION_AppendVertex,
	EDITMODEACTION_InsertVertex,
	EDITMODEACTION_HoverGrabPoint,
	EDITMODEACTION_HoverCenter,
	EDITMODEACTION_CloseLoop			// For polygon or line chain, when an end vertex is selected and the user clicks on the opposite end vertex to close the shape
};

class EditModeModel : public IEditModeModel
{
	HyColor								m_Color;
	glm::vec2							m_vOffset;				// Only used for primitive layer
	bool								m_bVisible;				// Only used for primitive layer
	float								m_fAlpha;				// Only used for primitive layer

	int32								m_iDisplayOrder;

	// ------------------------------------------------------------------------------------------------------------------
	// "type" - when serialized in property (as a string)
	EditorShape							m_eShapeType;			// When m_eEditModeType == EDITMODETYPE_PrimitiveShape or EDITMODETYPE_FixtureShape (None, Box, Circle, LineSegment, Polygon, Capsule). Not used otherwise
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

public:
	EditModeModel(EditModeType eEditModeType, HyColor color);
	virtual ~EditModeModel();

	virtual QJsonObject Serialize() const override;
	virtual void Deserialize(bool bEnabled, const QJsonObject &serializedObj) override;

	virtual Qt::CursorShape MouseMoveIdle() override;
	virtual void MouseIdleRightClick() override;
	virtual bool MousePressEvent(EditModeState eEditModeState, bool bShiftHeld) override; // Returns whether transform has begun (otherwise marquee select)
	virtual void MouseTransform(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos) override;
	virtual void MouseMarqueeReleased(EditModeState eEditModeState, bool bLeftClick, QPointF ptBotLeft, QPointF ptTopRight) override;
	virtual void MouseClickTransformReleased(glm::vec2 ptClickPos) override;

	bool IsFixture() const;
	bool IsLineChain() const;
	void SetEditModeType(EditModeType eEditModeType);
	EditorShape GetShapeType() const;

	HyColor GetColor() const;
	void SetColor(HyColor color);

	glm::vec2 GetOffset() const;
	void SetOffset(glm::vec2 vOffset);

	bool IsVisible() const;
	void SetVisible(bool bVisible);

	float GetAlpha() const;
	void SetAlpha(float fAlpha);

	int GetDisplayOrder() const;
	void SetDisplayOrder(int iDisplayOrder);

	void ChangeToLineChain(bool bIsActiveEditModeItem, bool bAsFixture);
	void ChangeToPoint(bool bIsActiveEditModeItem);
	void ChangeToShape(bool bIsActiveEditModeItem, EditorShape eNewShapeType, bool bAsFixture);

	bool IsLoopClosed() const;
	float GetOutline() const;
	void SetOutline(float fOutline);

	bool IsValidModel() const;

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

	void OnDeleteKeyPressed();

	glm::vec2 GetDragDelta() const;
	EditModeAction GetCurrentAction() const;
	
	QString GetActionText(EditModeState eEditModeState, QString sNodeCodeName) const; // Returns undo command description (blank if no change)
	void ClearAction();

protected:
	std::vector<float> ConvertedPointData() const;
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
