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
#include "IGfxEditModel.h"

class GfxShapeModel : public IGfxEditModel
{
	friend class GfxPrimitiveModel;

	EditorShape							m_eShapeType;			// "Shape", "Type" - when serialized in property (string)

	// "Shape", "Data" - when serialized in property (QJsonArray of floats)
	QList<HyShape2d *>					m_ShapeList;			// This is the actual shape data used for physics/collision/rendering - usually just one fixture, but could be multiple for complex polygons
	
	// Extra validation used with Polygon
	bool								m_bReverseWindingOrder;
	bool								m_bSelfIntersecting;
	glm::vec2							m_ptSelfIntersection;
	bool								m_bLoopClosed;

public:
	GfxShapeModel(HyColor color);
	virtual ~GfxShapeModel();

	virtual bool IsValidModel() const override;

	EditorShape GetShapeType() const;
	void SetShapeType(EditorShape eNewShape);

	virtual QList<float> Serialize() const override;

	void TransformData(glm::mat4 mtxTransform);

	int GetNumShapeFixtures() const;
	HyShape2d *GetShapeFixture(int iIndex) const;

	bool IsLoopClosed() const;

	virtual QString MouseTransformReleased(QString sShapeCodeName, QPointF ptWorldMousePos) override; // Returns undo command description (blank if no change)

protected:
	virtual void DoDeserialize(const QList<float> &floatList) override;
	virtual EditModeAction DoMouseMoveIdle(glm::vec2 ptWorldMousePos) override;
	virtual void DoTransformCreation(glm::vec2 ptStartPos, glm::vec2 ptDragPos) override;

	bool CheckIfAddVertexOnEdge(glm::vec2 ptWorldMousePos);
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
