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
#include "Scene/Tweens/HyTweenVec2.h"
#include "Scene/Physics/HyShape2d.h"
#include "Utilities/HyMath.h"

#include <functional>

class HyEntity2d;
class HyStencil;

class IHyNode2d : public IHyNode
{
	friend class HyEntity2d;

protected:
	HyEntity2d *					m_pParent;

	glm::mat4						m_mtxCached;
	float							m_fRotation;		// Reference value used in 'rot' HyTweenFloat

	b2Body *						m_pPhysicsBody;
	b2AABB							m_aabbCached;

public:
	HyTweenVec2						pos;
	HyTweenFloat					rot;
	HyTweenVec2						rot_pivot;
	HyTweenVec2						scale;
	HyTweenVec2						scale_pivot;

public:
	IHyNode2d(HyType eNodeType, HyEntity2d *pParent);
	IHyNode2d(const IHyNode2d &copyRef);
	virtual ~IHyNode2d();

	const IHyNode2d &operator=(const IHyNode2d &rhs);

	void ParentDetach();
	HyEntity2d *ParentGet() const;

	void GetLocalTransform(glm::mat4 &outMtx) const;
	const glm::mat4 &GetWorldTransform();

	void PhysicsInit(b2BodyDef &bodyDefOut);
	b2Body *PhysicsBody();
	void PhysicsBodyDef(b2BodyDef &defRefOut) const;

	virtual const b2AABB &GetWorldAABB();

protected:
	virtual void PhysicsUpdate() override final;
	virtual void NodeUpdate() override = 0;
};

#endif /* IHyNode2d_h__ */
