/**************************************************************************
*	HyChain2d.h
*
*	Harmony Engine
*	Copyright (c) 2025 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyChain2d_h__
#define HyChain2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Physics/Fixtures/IHyFixture2d.h"

struct HyChainData
{
	glm::vec2 *					pPointList;		// Dynamically allocated
	int							iCount;
	bool						bLoop;			// If true, pPointList/iCount is guaranteed to not include the "final" point (a repeat of the first point)
};

class HyChain2d : public IHyFixture2d
{
	HyChainData									m_Data;		// NOTE: This shape is stored in pixel units like everything else. It is converted to pixel-per-meters when sent to Box2d

	b2ChainId									m_hPhysics;
	b2ChainDef *								m_pPhysicsInit;

	b2SurfaceMaterial *							m_pMaterials;		// Dynamically allocated
	int											m_iMaterialCount;

public:
	static const float							FloatSlop;

	HyChain2d(HyEntity2d *pParent = nullptr);
	HyChain2d(const HyChain2d &copyRef);
	virtual ~HyChain2d();

	const HyChain2d &operator=(const HyChain2d &rhs);

	const HyChainData &GetData() const;
	void SetData(const glm::vec2 *pVertices, uint32 uiNumVerts, bool bLoop, const b2ChainDef *pPhysicsInit = nullptr);
	void SetData(const std::vector<glm::vec2> &verticesList, bool bLoop, const b2ChainDef *pPhysicsInit = nullptr);

	virtual void TransformSelf(const glm::mat4 &mtxTransform) override;

	bool GetCentroid(glm::vec2 &ptCentroidOut) const;

	// Applies when attached to a physics body
	void Setup(const b2ChainDef &chainDefRef);
	float GetFriction() const;
	void SetFriction(float fFriction);
	float GetRestitution() const;
	void SetRestitution(float fRestitution);
	b2Filter GetFilter() const; // If this shape is a chain, it will return the filter for the first edge
	void SetFilter(const b2Filter &filter); // WARNING: This function is potentially expensive if it's apart of an active physics simulation

	virtual bool TestPoint(const glm::vec2 &ptTestPoint, const glm::mat4 &mtxSelfTransform) const override;
	virtual b2CastOutput TestRay(const glm::vec2 &ptStart, const glm::vec2 &vDirection, const glm::mat4 &mtxSelfTransform) const override;

	virtual bool ComputeAABB(b2AABB &aabbOut, const glm::mat4 &mtxTransform) const override;

protected:
	void ClearShapeData();
	void ShapeChanged();

	bool IsPhysicsRegistered() const;
	bool IsPhysicsInitialized() const;
	virtual void PhysicsAttach() override;
	virtual void PhysicsRemove(bool bUpdateBodyMass) override;

	// NOTE: Assumes 'chainDataOut' starts as zeroed-out. Will deep copy into 'chainDataOut' (dynamically allocate)
	bool AllocChainData(HyChainData &chainDataOut, const glm::mat4 &mtxTransform) const;
	bool AllocChainData(HyChainData &chainDataOut, float fPpmInverse) const;
};

#endif /* HyChain2d_h__ */
