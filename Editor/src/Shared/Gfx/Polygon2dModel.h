/**************************************************************************
*	Polygon2dModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2025 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef POLYGON2DMODEL_H
#define POLYGON2DMODEL_H

#include "Global.h"

#define GRABPOINT_SELECT_RADIUS		6.0f	// In world units

class IPolygon2dView;

enum ShapeMouseMoveResult
{
	SHAPEMOUSEMOVE_Outside,
	SHAPEMOUSEMOVE_Inside,
	SHAPEMOUSEMOVE_Crosshair,
	SHAPEMOUSEMOVE_AddVertex,
	SHAPEMOUSEMOVE_HoverVertex,
	SHAPEMOUSEMOVE_HoverSelectedVertex
};

class GrabPointModel
{
	QPointF				m_ptPosition;
	bool				m_bIsSelected;

public:
	GrabPointModel() :
		m_ptPosition(QPointF(0.0f, 0.0f)),
		m_bIsSelected(false)
	{ }
	GrabPointModel(QPointF ptPosition) :
		m_ptPosition(ptPosition),
		m_bIsSelected(false)
	{ }
	~GrabPointModel() = default;
	QPointF GetPosition() const { return m_ptPosition; }
	bool IsSelected() const { return m_bIsSelected; }
	void SetPosition(QPointF ptPosition) { m_ptPosition = ptPosition; }
	void SetSelected(bool bIsSelected) { m_bIsSelected = bIsSelected; }
};

class Polygon2dModel
{
	HyColor								m_Color;
	EditorShape							m_eType;				// "Shape", "Type" - when serialized in property (string)

	// "Shape", "Data" - when serialized in property (QJsonArray of floats)
	QList<IHyFixture2d *>				m_FixtureList;			// This is the actual shape data used for physics/collision/rendering - usually just one fixture, but could be multiple for complex polygons
	QList<GrabPointModel>				m_GrabPointList;		// Grab Points for editing the shape - Used to serialize data when type is SHAPE_Polygon (then assembles m_FixtureList with valid sub-polygons)
	
	// Polygon validation
	bool								m_bSelfIntersecting;
	glm::vec2							m_ptSelfIntersection;
	bool								m_bReverseWindingOrder;

	int									m_iInsertVertexIndex;

	// Track Views manually since we don't inherit from QObject
	QList<IPolygon2dView *>				m_ViewList;

public:
	Polygon2dModel(HyColor color, EditorShape eShape = SHAPE_None, const QList<float> &floatList = QList<float>());
	virtual ~Polygon2dModel();

	HyColor GetColor() const;
	EditorShape GetType() const;
	QList<float> GetData() const;
	IHyFixture2d *GetFixture(int iIndex) const;
	const QList<GrabPointModel> &GetGrabPointList() const;

	bool IsValidShape() const;

	void SetColor(HyColor color);
	void SetData(HyColor color, EditorShape eShape, const QList<float> &floatList);

	void TransformSelf(glm::mat4 mtxTransform); // NOTE: Does not update m_Outline, requires a DeserializeOutline()

	ShapeMouseMoveResult OnMouseMoveEvent(QPointF ptWorldMousePos);
	bool OnMousePressEvent(QPointF ptWorldMousePos); // Returns whether transform has begun (otherwise marquee select)
	int OnMouseMarqueeReleased(QPointF ptBotLeft, QPointF ptTopRight);
	void OnMouseTransformDrag(QPointF ptWorldMousePos);
	QString OnMouseTransformReleased(QPointF ptWorldMousePos); // Returns undo command description (blank if no change)

protected:
	bool CheckIfAddVertexOnEdge(QPointF ptWorldMousePos);

	bool IsShareEdge(const std::vector<glm::vec2> &a, const std::vector<glm::vec2> &b, int &a0, int &a1, int &b0, int &b1);
	std::vector<glm::vec2> MergePolygons(const std::vector<glm::vec2> &ptA, const std::vector<glm::vec2> &ptB, int a0, int a1, int b0, int b1);
	std::vector<std::vector<glm::vec2>> MergeTriangles(const std::vector<HyTriangle2d> &triangleList);
	
	void ClearFixtures();
	void AssemblePolygonFixtures(std::vector<std::vector<glm::vec2>> subPolygonList); // Assembles m_FixtureList from properly formed sub-polygons (no self-intersections, correct winding, convex, <= 8 vertices)
};

#endif // POLYGON2DMODEL_H
