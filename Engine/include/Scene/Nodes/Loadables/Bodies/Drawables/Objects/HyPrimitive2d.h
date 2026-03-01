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
#include "Scene/Physics/Fixtures/HyShape2d.h"
#include "Scene/Physics/Fixtures/HyChain2d.h"

class HyPrimitive2d : public IHyDrawable2d
{
protected:
	struct Layer
	{
		glm::vec2 *		m_pVertBuffer;
		uint32			m_uiNumVerts;
		glm::vec2		m_vOffset;
		bool			m_bVisible;
		HyColor			m_Color;

		HyFixtureType	m_eFixtureType;
		float			m_fLineThickness; // When > 0.0f, this layer will be drawn as an outline with the specified thickness in pixels
		uint32			m_uiNumSegments;

		Layer() :
			m_pVertBuffer(nullptr),
			m_uiNumVerts(0),
			m_vOffset(0.0f, 0.0f),
			m_bVisible(true),
			m_Color(HyColor::White),
			m_eFixtureType(HYFIXTURE_Nothing),
			m_fLineThickness(0.0f),
			m_uiNumSegments(16)
		{ }
		Layer(const Layer &copyRef) :
			m_pVertBuffer(nullptr),
			m_uiNumVerts(copyRef.m_uiNumVerts),
			m_vOffset(copyRef.m_vOffset),
			m_bVisible(true),
			m_Color(HyColor::White),
			m_eFixtureType(copyRef.m_eFixtureType),
			m_fLineThickness(copyRef.m_fLineThickness),
			m_uiNumSegments(copyRef.m_uiNumSegments)
		{
			if(copyRef.m_pVertBuffer)
			{
				m_pVertBuffer = HY_NEW glm::vec2[m_uiNumVerts];
				memcpy(m_pVertBuffer, copyRef.m_pVertBuffer, m_uiNumVerts * sizeof(glm::vec2));
			}
		}
		Layer(Layer &&donor) noexcept :
			m_pVertBuffer(donor.m_pVertBuffer),
			m_uiNumVerts(donor.m_uiNumVerts),
			m_vOffset(donor.m_vOffset),
			m_bVisible(donor.m_bVisible),
			m_Color(donor.m_Color),
			m_eFixtureType(donor.m_eFixtureType),
			m_fLineThickness(donor.m_fLineThickness),
			m_uiNumSegments(donor.m_uiNumSegments)
		{
			donor.m_pVertBuffer = nullptr;
			donor.m_uiNumVerts = 0;
		}
	};
	std::vector<Layer>	m_LayerList;
	bool				m_bUpdateShaderUniforms;

public:
	HyPrimitive2d(HyEntity2d *pParent = nullptr);
	HyPrimitive2d(const HyPrimitive2d &copyRef);
	virtual ~HyPrimitive2d(void);

	const HyPrimitive2d &operator=(const HyPrimitive2d &rhs);

	virtual void CalcLocalBoundingShape(HyShape2d &shapeOut) override;
	virtual float GetWidth(float fPercent = 1.0f) override;
	virtual float GetHeight(float fPercent = 1.0f) override;
	void GetCentroid(glm::vec2 &ptCentroidOut);

	int32 GetNumLayers() const;

	HyFixtureType GetLayerType(int32 iLayerIndex) const;

	int32 SetAsNothing(int32 iLayerIndex);

	// Set as an isolated edge.
	int32 SetAsLineSegment(int32 iLayerIndex, const glm::vec2 &pt1, const glm::vec2 &pt2, float fLineThickness);

	// A series of line segments chained together. 'bLoop' is whether to automatically connects last vertex to the first.
	// Passed in parameters are copied, and understood to be local coordinates.
	int32 SetAsLineChain(int32 iLayerIndex, const glm::vec2 *pVertices, uint32 uiNumVerts, bool bLoop, float fLineThickness);
	int32 SetAsLineChain(int32 iLayerIndex, const std::vector<glm::vec2> &verticesList, bool bLoop, float fLineThickness);
	int32 SetAsLineChain(int32 iLayerIndex, const HyChainData &chainData, float fLineThickness);

	int32 SetAsShape(int32 iLayerIndex, const HyShape2d &shapeRef, float fOutlineThickness = 0.0f);

	// Set as a circle with the specified center and radius
	int32 SetAsCircle(int32 iLayerIndex, float fRadius, float fOutlineThickness = 0.0f);
	int32 SetAsCircle(int32 iLayerIndex, const glm::vec2 &ptCenter, float fRadius, float fOutlineThickness = 0.0f);

	// Set as any type of polygon using counter-clockwise vertex winding array of vertices
	int32 SetAsPolygon(int32 iLayerIndex, const glm::vec2 *pVertexArray, uint32 uiCount, float fOutlineThickness = 0.0f);
	int32 SetAsPolygon(int32 iLayerIndex, const std::vector<glm::vec2> &verticesList, float fOutlineThickness = 0.0f);

	int32 SetAsBox(int32 iLayerIndex, float fWidth, float fHeight, float fOutlineThickness = 0.0f);	// Axis-aligned box, bottom left corner at 0,0
	int32 SetAsBox(int32 iLayerIndex, const HyRect &rect, float fOutlineThickness = 0.0f);			// Represent an oriented box

	int32 SetAsCapsule(int32 iLayerIndex, const glm::vec2 &pt1, const glm::vec2 &pt2, float fRadius, float fOutlineThickness = 0.0f);

	glm::vec2 GetLayerOffset(int32 iLayerIndex) const;
	int32 SetLayerOffset(int32 iLayerIndex, const glm::vec2 &vOffset);

	bool IsLayerVisible(int32 iLayerIndex) const;
	int32 SetLayerVisible(int32 iLayerIndex, bool bVisible);

	HyColor GetLayerColor(int32 iLayerIndex) const;
	int32 SetLayerColor(int32 iLayerIndex, HyColor color);

	uint32 GetNumVerts(int32 iLayerIndex) const;
	const glm::vec2 *GetVerts(int32 iLayerIndex) const;

	bool IsOutline(int32 iLayerIndex);
	float GetLineThickness(int32 iLayerIndex) const;

	virtual bool IsLoadDataValid() override;

protected:
	virtual void SetDirty(uint32 uiDirtyFlags) override;

	virtual bool OnIsValidToRender() override;

	virtual void OnUpdateUniforms(float fExtrapolatePercent) override;

	virtual void PrepRenderStage(uint32 uiStageIndex, HyRenderMode &eRenderModeOut, HyBlendMode &eBlendModeOut, uint32 &uiNumInstancesOut, uint32 &uiNumVerticesPerInstOut, bool &bIsBatchable) override;
	virtual bool WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent) override;

private:
	void ClearAllData();
	void DeleteLayerData(int32 iLayerIndex);

	void AssembleLineChain(int32 iLayerIndex, const glm::vec2 *pVertexList, uint32 uiNumVertices, bool bLoop); // Also used to do shape outlines
	void AssembleCircle(int32 iLayerIndex, glm::vec2 ptCenter, float fRadius, uint32 uiSegments);
	void AssemblePolygon(int32 iLayerIndex, const glm::vec2 *pVertexList, uint32 uiNumVertices);
	void AssembleCapsule(int32 iLayerIndex, const glm::vec2 &ptCenter1, const glm::vec2 &ptCenter2, float fRadius, uint32 uiSegments);

private: // Hide inherited functionality that doesn't exist for primitives
	using IHyLoadable::GetState;
	using IHyLoadable::SetState;
	using IHyLoadable::GetNumStates;
	using IHyLoadable::GetName;
	using IHyLoadable::GetPrefix;
	using IHyLoadable2d::Init;
	using IHyLoadable2d::Uninit;
};
typedef HyPrimitive2d HyPrimitive;

#endif /* HyPrimitive2d_h__ */
