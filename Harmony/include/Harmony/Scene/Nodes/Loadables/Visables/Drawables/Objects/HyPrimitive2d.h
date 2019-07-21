/**************************************************************************
 *	HyPrimitive2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyPrimitive2d_h__
#define HyPrimitive2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Visables/Drawables/IHyDrawable2d.h"

class HyAtlasGroupData;

class HyPrimitive2d : public IHyDrawable2d
{
protected:
	glm::vec2 *		m_pVertBuffer;
	uint32			m_uiNumVerts;

	bool			m_bWireframe;
	float			m_fLineThickness;
	uint32			m_uiNumSegments;

public:
	HyPrimitive2d(HyEntity2d *pParent = nullptr);
	HyPrimitive2d(const HyPrimitive2d &copyRef);
	virtual ~HyPrimitive2d(void);

	const HyPrimitive2d &operator=(const HyPrimitive2d &rhs);
	virtual HyPrimitive2d *Clone() const;

	void SetAsNothing();

	// Set as an isolated edge.
	void SetAsLineSegment(const glm::vec2 &pt1, const glm::vec2 &pt2);

	// Set as a line loop. This automatically connects last vertex to the first.
	// Passed in parameters are copied, and understood to be local coordinates
	void SetAsLineLoop(const glm::vec2 *pVertices, uint32 uiNumVerts);

	// Set as a line chain with isolated end vertices. Passed in parameters are 
	// copied, and understood to be local coordinates
	void SetAsLineChain(const glm::vec2 *pVertices, uint32 uiNumVerts);

	// Set as a circle with the specified center and radius
	void SetAsCircle(float fRadius);
	void SetAsCircle(const glm::vec2 &ptCenter, float fRadius);

	// Set as a convex hull from the given array of local points.
	// uiNumVerts must be in the range [3, b2_maxPolygonVertices].
	// The points may be re-ordered, even if they form a convex polygon
	// Collinear points are handled but not removed. Collinear points
	// may lead to poor stacking behavior in physics simulation.
	void SetAsPolygon(const glm::vec2 *pVertices, uint32 uiNumVerts);

	// Build vertices to represent an axis-aligned box
	void SetAsBox(int32 iWidth, int32 iHeight);
	void SetAsBox(float fWidth, float fHeight);

	// Build vertices to represent an oriented box.
	// ptBoxCenter is the center of the box in local coordinates.
	// fRot the rotation of the box in local coordinates.
	void SetAsBox(float fHalfWidth, float fHalfHeight, const glm::vec2 &ptBoxCenter, float fRotDeg);

	uint32 GetNumVerts() const;

	bool IsWireframe();
	void SetWireframe(bool bIsWireframe);

	float GetLineThickness();
	void SetLineThickness(float fThickness);

	uint32 GetNumCircleSegments();
	void SetNumCircleSegments(uint32 uiNumSegments);

	virtual bool IsLoadDataValid() override;

protected:
	virtual bool OnIsValid() override;
	virtual void OnUpdateUniforms() override;
	virtual void OnWriteVertexData(HyVertexBuffer &vertexBufferRef) override;

private:
	void ClearData();
	void SetData();

	void SetAsLineChain(b2Vec2 *pVertexList, uint32 uiNumVertices);
	void SetAsCircle(glm::vec2 &ptCenter, float fRadius, uint32 uiSegments);
	void SetAsPolygon(b2Vec2 *pVertexList, uint32 uiNumVertices);
};

#endif /* HyPrimitive2d_h__ */
