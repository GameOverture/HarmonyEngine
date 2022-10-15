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
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable2d.h"

class HyAtlasGroupData;

class HyPrimitive2d : public IHyDrawable2d
{
protected:
	glm::vec2 *		m_pVertBuffer;
	uint32			m_uiNumVerts;

	bool			m_bWireframe;
	float			m_fLineThickness;
	uint32			m_uiNumSegments;

	HyRenderMode	m_eRenderMode;
	bool			m_bUpdateShaderUniforms;

public:
	HyPrimitive2d(HyEntity2d *pParent = nullptr);
	HyPrimitive2d(const HyPrimitive2d &copyRef);
	virtual ~HyPrimitive2d(void);

	const HyPrimitive2d &operator=(const HyPrimitive2d &rhs);

	uint32 GetNumVerts() const;

	bool IsWireframe();
	void SetWireframe(bool bIsWireframe);

	float GetLineThickness();
	void SetLineThickness(float fThickness);

	uint32 GetNumCircleSegments();
	void SetNumCircleSegments(uint32 uiNumSegments);

	virtual bool IsLoadDataValid() override;

protected:
	virtual void SetDirty(uint32 uiDirtyFlags) override;

	virtual void OnShapeChanged() override;
	virtual bool OnIsValidToRender() override;
	virtual void OnCalcBoundingVolume() override;

	virtual void OnUpdateUniforms(float fExtrapolatePercent) override;

	virtual void PrepRenderStage(uint32 uiStageIndex, HyRenderMode &eRenderModeOut, uint32 &uiNumInstancesOut, uint32 &uiNumVerticesPerInstOut, bool &bIsBatchable) override;
	virtual bool WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent) override;

private:
	virtual void Load() override;

	void ClearVertexData();
	void AssembleData();

	void _SetAsLineChain(b2Vec2 *pVertexList, uint32 uiNumVertices);
	void _SetAsCircle(glm::vec2 ptCenter, float fRadius, uint32 uiSegments);
	void _SetAsPolygon(const b2Vec2 *pVertexList, uint32 uiNumVertices);
};

#endif /* HyPrimitive2d_h__ */
