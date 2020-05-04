/**************************************************************************
*	IHyNode2d.h
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyNode2d_h__
#define IHyNode2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/IHyNode.h"
#include "Scene/AnimFloats/HyAnimVec2.h"

class IHyNode2d : public IHyNode
{
	friend class HyEntity2d;

protected:
	HyEntity2d *					m_pParent;

	glm::mat4						m_mtxCached;
	float							m_fRotation;		// Reference value used in 'rot' HyAnimFloat

	b2AABB							m_WorldAABB;

public:
	HyAnimVec2						pos;
	HyAnimFloat						rot;
	HyAnimVec2						rot_pivot;
	HyAnimVec2						scale;
	HyAnimVec2						scale_pivot;

public:
	IHyNode2d(HyType eNodeType, HyEntity2d *pParent);
	IHyNode2d(const IHyNode2d &copyRef);
	IHyNode2d(IHyNode2d &&donor);
	virtual ~IHyNode2d();

	IHyNode2d &operator=(const IHyNode2d &rhs);
	IHyNode2d &operator=(IHyNode2d &&donor);

	void ParentDetach();
	HyEntity2d *ParentGet() const;

	void GetLocalTransform(glm::mat4 &outMtx) const;
	const glm::mat4 &GetWorldTransform();

	virtual const b2AABB &GetWorldAABB();

private:
	friend void _CtorSetupNewChild(HyEntity2d &parentRef, IHyNode2d &childRef);
};

#endif /* IHyNode2d_h__ */
