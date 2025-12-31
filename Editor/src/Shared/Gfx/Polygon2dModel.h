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

struct CollisionPolygon
{
	struct Vertex
	{
		QPointF			m_ptPos;
		bool			m_bSelected;
	};
	QVector<Vertex>		m_VertexList;
};

enum PolygonClickResult
{
	POLYGONCLICKRESULT_AddVertex,
	POLYGONCLICKRESULT_SelectVertex,
	POLYGONCLICKRESULT_Outside,
	POLYGONCLICKRESULT_Inside
};

class Polygon2dModel
{
	QVector<CollisionPolygon::Vertex>	m_UserVertexList;

	// Rules
	bool								m_bIsSegments;
	bool								m_bSimplifyVertices;	// Remove collinear points
	bool								m_bEnforceConvexShapes;	// Split
	bool								m_bMaxVerticesEnabled;	// B2_MAX_POLYGON_VERTICES

	QVector<CollisionPolygon>			m_PolygonList; // Assemble() creates multiple CollisionPolygon if rules are violated, otherwise uses a single CollisionPolygon

public:
	Polygon2dModel();
	virtual ~Polygon2dModel();

	QVector<CollisionPolygon> GetPolygonList() const;

	PolygonClickResult OnMouseHover(QPointF ptMousePos);
	PolygonClickResult OnMouseClick(QPointF ptMousePos, bool bLeftClick, bool bShiftPressed);
	void OnMouseDrag(QPointF ptMousePos);

	int OnMarqueeSelect(const QRectF &rectSelection); // Returns number of vertices selected

private:
	void Assemble();
};

#endif // POLYGON2DMODEL_H
