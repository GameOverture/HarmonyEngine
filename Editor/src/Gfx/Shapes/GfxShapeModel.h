/**************************************************************************
*	GfxShapeModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2025 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef GfxShapeModel_H
#define GfxShapeModel_H

#include "Global.h"
#include "GfxGrabPointModel.h"

class IGfxShapeView;

enum ShapeMouseMoveResult
{
	SHAPEMOUSEMOVE_Outside,
	SHAPEMOUSEMOVE_Inside,
	SHAPEMOUSEMOVE_Initial,
	SHAPEMOUSEMOVE_AppendVertex,
	SHAPEMOUSEMOVE_InsertVertex,
	SHAPEMOUSEMOVE_HoverVertex,
	SHAPEMOUSEMOVE_HoverSelectedVertex,
	SHAPEMOUSEMOVE_HoverCenter
};

class GfxShapeModel
{
	HyColor								m_Color;
	EditorShape							m_eType;				// "Shape", "Type" - when serialized in property (string)

	// "Shape", "Data" - when serialized in property (QJsonArray of floats)
	QList<IHyFixture2d *>				m_FixtureList;			// This is the actual shape data used for physics/collision/rendering - usually just one fixture, but could be multiple for complex polygons
	QList<GfxGrabPointModel>			m_GrabPointList;		// Grab Points for editing the shape - Used to serialize data when type is SHAPE_Polygon (then assembles m_FixtureList with valid sub-polygons)
	GfxGrabPointModel					m_GrabPointCenter;
	
	// Extra validation used with LineChain and Polygon
	bool								m_bReverseWindingOrder;
	bool								m_bSelfIntersecting;
	glm::vec2							m_ptSelfIntersection;
	bool								m_bLoopClosed;

	// Transform info
	int									m_iGrabPointHoverIndex;
	int									m_iInsertVertexIndex;
	glm::vec2							m_ptInsertVertexPos;
	glm::vec2							m_ptTransformStartPos;
	glm::vec2							m_ptTransformDragPos;
	bool								m_bTransformShiftMod;
	enum TransformType
	{
		TRANSFORM_None = 0,
		TRANSFORM_Initial,				// Shape is malformed and still being assembled
		TRANSFORM_TranslateShape,		// Center grab point being dragged
		TRANSFORM_InsertNewVertex,		// An edge was clicked and a new vertex is being inserted
		TRANSFORM_TranslateVerts		// All selected vertices being translated
	};
	TransformType						m_eTransformType;

	// Track Views manually since we don't inherit from QObject
	QList<IGfxShapeView *>				m_ViewList;

public:
	GfxShapeModel(HyColor color, EditorShape eShape = SHAPE_None, const QList<float> &floatList = QList<float>());
	virtual ~GfxShapeModel();

	bool IsValidShape() const;

	HyColor GetColor() const;
	void SetColor(HyColor color);

	EditorShape GetType() const;
	void SetType(EditorShape eNewShape);

	QList<float> GetData() const;
	void SetData(HyColor color, EditorShape eShape, const QList<float> &floatList);

	void TransformSelf(glm::mat4 mtxTransform);

	void AddView(IGfxShapeView *pView);
	bool RemoveView(IGfxShapeView *pView);

	int GetNumFixtures() const;
	IHyFixture2d *GetFixture(int iIndex) const;
	const QList<GfxGrabPointModel> &GetGrabPointList() const;
	const GfxGrabPointModel &GetCenterGrabPoint() const;

	bool IsLoopClosed() const;

	ShapeMouseMoveResult MouseMoveIdle(QPointF ptWorldMousePos);
	bool MousePressEvent(bool bShiftHeld, Qt::MouseButtons uiButtonFlags, QPointF ptWorldMousePos); // Returns whether transform has begun (otherwise marquee select)
	void MouseMarqueeReleased(Qt::MouseButtons uiButtonFlags, QPointF ptBotLeft, QPointF ptTopRight);
	void MouseMoveTransform(bool bShiftMod, QPointF ptDragPos);
	QString MouseTransformReleased(QPointF ptWorldMousePos); // Returns undo command description (blank if no change)

protected:
	ShapeMouseMoveResult OnMouseMoveIdle(QPointF ptWorldMousePos);
	bool OnMousePressEvent(bool bShiftHeld, Qt::MouseButtons uiButtonFlags, QPointF ptWorldMousePos); // Returns whether transform has begun (otherwise marquee select)

	void DoTransformInitial();
	void DoTransformTranslateShape();
	void DoTranslateVertexTransformDrag();

	bool CheckIfAddVertexOnEdge(QPointF ptWorldMousePos);
	bool IsShareEdge(const std::vector<glm::vec2> &a, const std::vector<glm::vec2> &b, int &a0, int &a1, int &b0, int &b1);
	std::vector<glm::vec2> MergePolygons(const std::vector<glm::vec2> &ptA, const std::vector<glm::vec2> &ptB, int a0, int a1, int b0, int b1);
	std::vector<std::vector<glm::vec2>> MergeTriangles(const std::vector<HyTriangle2d> &triangleList);
	
	void ClearFixtures();
	void AssemblePolygonFixtures(std::vector<std::vector<glm::vec2>> subPolygonList); // Assembles m_FixtureList from properly formed sub-polygons (no self-intersections, correct winding, convex, <= 8 vertices)

	QList<float> ConvertedBoxData() const;
	QList<float> ConvertedCircleData() const;
	QList<float> ConvertedLineSegmentData() const;
	QList<float> ConvertedCapsuleData() const;
	QList<float> ConvertedPolygonOrLineChainData() const;
};

#endif // GfxShapeModel_H
